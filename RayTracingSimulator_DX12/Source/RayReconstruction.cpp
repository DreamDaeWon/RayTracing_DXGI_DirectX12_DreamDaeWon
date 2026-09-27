#include "RayReconstruction.h"
#include "nvsdk_ngx_helpers_dlssd_d3d.h"
#include <mutex>
using namespace DirectX;

namespace {
void NVSDK_CONV NgxLog(const char* message, NVSDK_NGX_Logging_Level, NVSDK_NGX_Feature) {
    static std::mutex mutex; std::lock_guard<std::mutex> lock(mutex);
    std::ofstream log(ExecutableDirectory() / L"ngx.log", std::ios::app); log << message << '\n';
}
}
bool RayReconstruction::Fail(NVSDK_NGX_Result result, const char* action) {
    char code[40]; snprintf(code, sizeof(code), " (NGX 0x%08X)", static_cast<unsigned>(result));
    status = std::string(action) + code; available = active = false;
    NgxLog(status.c_str(), NVSDK_NGX_LOGGING_LEVEL_ON, NVSDK_NGX_Feature_RayReconstruction); return false;
}
void RayReconstruction::Initialize(D3D12Context& context) {
    srv = context.AllocateDescriptor();
    auto folder = ExecutableDirectory(); auto cache = folder / L"NGX"; std::filesystem::create_directories(cache);
    std::wstring search = folder.wstring(); const wchar_t* paths[] = {search.c_str()};
    NVSDK_NGX_FeatureCommonInfo info{}; info.PathListInfo.Path = paths; info.PathListInfo.Length = 1;
    info.LoggingInfo = {NgxLog, NVSDK_NGX_LOGGING_LEVEL_ON, false};
    auto result = NVSDK_NGX_D3D12_Init_with_ProjectID("c37e72a2-d831-4b76-934c-a6b746552bd6", NVSDK_NGX_ENGINE_TYPE_CUSTOM, "1.0.0", cache.c_str(), context.Device(), &info);
    if (NVSDK_NGX_FAILED(result)) { Fail(result, "RR initialization unavailable"); return; }
    initialized = true;
    result = NVSDK_NGX_D3D12_GetCapabilityParameters(&parameters);
    if (NVSDK_NGX_FAILED(result)) { Fail(result, "RR capability query failed"); return; }
    int supported = 0, update = 0;
    parameters->Get(NVSDK_NGX_Parameter_SuperSamplingDenoising_Available, &supported);
    parameters->Get(NVSDK_NGX_Parameter_SuperSamplingDenoising_NeedsUpdatedDriver, &update);
    if (!supported || update) {
        unsigned major = 0, minor = 0;
        parameters->Get(NVSDK_NGX_Parameter_SuperSamplingDenoising_MinDriverVersionMajor, &major);
        parameters->Get(NVSDK_NGX_Parameter_SuperSamplingDenoising_MinDriverVersionMinor, &minor);
        status = update ? "RR requires NVIDIA driver " + std::to_string(major) + "." + std::to_string(minor) : "DLSS RR unavailable on this GPU/runtime";
        return;
    }
    available = true; status = "DLSS Ray Reconstruction ready";
}
void RayReconstruction::Shutdown(D3D12Context& context) {
    if (!initialized) return;
    context.WaitIdle();
    if (handle) { NVSDK_NGX_D3D12_ReleaseFeature(handle); handle = nullptr; }
    if (parameters) { NVSDK_NGX_D3D12_DestroyParameters(parameters); parameters = nullptr; }
    NVSDK_NGX_D3D12_Shutdown1(context.Device()); initialized = available = active = false;
}
bool RayReconstruction::Evaluate(D3D12Context& context, const PathTracer& tracer, const Camera& camera, float aspect, float dt) {
    if (!available) return false;
    if (!handle || width != tracer.Width() || height != tracer.Height()) {
        // Submit all commands referencing the old instance before recreating it.
        context.FlushAndContinue();
        if (handle) { NVSDK_NGX_D3D12_ReleaseFeature(handle); handle = nullptr; }
        width = tracer.Width(); height = tracer.Height(); output.Reset(); Reset();
        D3D12_HEAP_PROPERTIES heap{}; heap.Type = D3D12_HEAP_TYPE_DEFAULT;
        D3D12_RESOURCE_DESC desc{}; desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; desc.Width = width; desc.Height = height;
        desc.DepthOrArraySize = desc.MipLevels = 1; desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; desc.SampleDesc.Count = 1; desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        Check(context.Device()->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(&output)), "Create RR output");
        output->SetName(L"NVIDIA DLSS RR output");
        D3D12_SHADER_RESOURCE_VIEW_DESC view{}; view.Format = desc.Format; view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; view.Texture2D.MipLevels = 1;
        context.Device()->CreateShaderResourceView(output.Get(), &view, context.Cpu(srv));
        NVSDK_NGX_DLSSD_Create_Params create{};
        create.InWidth = create.InTargetWidth = width; create.InHeight = create.InTargetHeight = height;
        create.InPerfQualityValue = NVSDK_NGX_PerfQuality_Value_DLAA;
        create.InFeatureCreateFlags = NVSDK_NGX_DLSS_Feature_Flags_IsHDR | NVSDK_NGX_DLSS_Feature_Flags_MVLowRes;
        create.InDenoiseMode = NVSDK_NGX_DLSS_Denoise_Mode_DLUnified;
        create.InRoughnessMode = NVSDK_NGX_DLSS_Roughness_Mode_Packed; create.InUseHWDepth = NVSDK_NGX_DLSS_Depth_Type_Linear;
        auto result = NGX_D3D12_CREATE_DLSSD_EXT(context.List(), 1, 1, &handle, parameters, &create);
        if (NVSDK_NGX_FAILED(result)) return Fail(result, "RR feature creation failed");
        context.FlushAndContinue();
    }
    context.Transition(output.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    NVSDK_NGX_D3D12_DLSSD_Eval_Params eval{};
    eval.pInColor = tracer.GuideResource(PathTracer::NoisyColor); eval.pInOutput = output.Get();
    eval.pInDiffuseAlbedo = tracer.GuideResource(PathTracer::DiffuseAlbedo); eval.pInSpecularAlbedo = tracer.GuideResource(PathTracer::SpecularAlbedo);
    eval.pInNormals = tracer.GuideResource(PathTracer::NormalRoughness); eval.pInDepth = tracer.GuideResource(PathTracer::Depth);
    eval.pInMotionVectors = tracer.GuideResource(PathTracer::Motion); eval.pInSpecularHitDistance = tracer.GuideResource(PathTracer::SpecularDistance);
    eval.InRenderSubrectDimensions = {width, height}; eval.InReset = reset ? 1 : 0;
    eval.InJitterOffsetX = tracer.Jitter().x; eval.InJitterOffsetY = tracer.Jitter().y;
    eval.InMVScaleX = eval.InMVScaleY = 1; eval.InPreExposure = eval.InExposureScale = 1;
    eval.InFrameTimeDeltaInMsec = dt * 1000;
    // NGX uses row-major storage and vectors multiplied on the left, like DirectXMath.
    XMFLOAT4X4 view, projection;
    XMStoreFloat4x4(&view, camera.View()); XMStoreFloat4x4(&projection, camera.Projection(aspect));
    eval.pInWorldToViewMatrix = &view._11; eval.pInViewToClipMatrix = &projection._11;
    auto result = NGX_D3D12_EVALUATE_DLSSD_EXT(context.List(), handle, parameters, &eval);
    context.Transition(output.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    // NGX modifies descriptor heaps and pipeline bindings; restore app state explicitly.
    ID3D12DescriptorHeap* heaps[] = {context.Heap()}; context.List()->SetDescriptorHeaps(1, heaps);
    auto rtv = context.CurrentRTV(); context.List()->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
    if (NVSDK_NGX_FAILED(result)) return Fail(result, "RR evaluation failed");
    reset = false; active = true; ++historyFrames; ++evaluations; status = "DLSS RR active / native resolution"; return true;
}
bool RayReconstruction::ValidateOutput(D3D12Context& context, const PathTracer& tracer, std::ostream& log) {
    if (!active || !output) return false;
    auto desc = output->GetDesc(); D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout{}; UINT64 bytes = 0;
    context.Device()->GetCopyableFootprints(&desc, 0, 1, 0, &layout, nullptr, nullptr, &bytes);
    auto rr = CreateBuffer(context.Device(), bytes, D3D12_HEAP_TYPE_READBACK), raw = CreateBuffer(context.Device(), bytes, D3D12_HEAP_TYPE_READBACK);
    auto copy = [&](ID3D12Resource* source, ID3D12Resource* target, D3D12_RESOURCE_STATES state) {
        context.Transition(source, state, D3D12_RESOURCE_STATE_COPY_SOURCE);
        D3D12_TEXTURE_COPY_LOCATION from{}; from.pResource = source; from.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        D3D12_TEXTURE_COPY_LOCATION to{}; to.pResource = target; to.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT; to.PlacedFootprint = layout;
        context.List()->CopyTextureRegion(&to, 0, 0, 0, &from, nullptr); context.Transition(source, D3D12_RESOURCE_STATE_COPY_SOURCE, state);
    };
    copy(output.Get(), rr.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE); copy(tracer.GuideResource(PathTracer::NoisyColor), raw.Get(), PathTracer::GuideState);
    context.FlushAndContinue();
    uint8_t* a = nullptr; uint8_t* b = nullptr; D3D12_RANGE range{0, SIZE_T(bytes)};
    Check(rr->Map(0, &range, reinterpret_cast<void**>(&a)), "Map RR readback"); Check(raw->Map(0, &range, reinterpret_cast<void**>(&b)), "Map raw readback");
    double mean = 0, difference = 0, rrVariation = 0, rawVariation = 0; UINT invalid = 0;
    for (UINT y = 0; y < height; ++y) {
        const float* ar = reinterpret_cast<float*>(a + layout.Offset + y * layout.Footprint.RowPitch);
        const float* br = reinterpret_cast<float*>(b + layout.Offset + y * layout.Footprint.RowPitch);
        for (UINT x = 0; x < width; ++x) for (UINT c = 0; c < 3; ++c) {
            float v = ar[x * 4 + c]; if (!std::isfinite(v)) ++invalid; else { mean += v; difference += fabs(v - br[x * 4 + c]); }
            if (x > width / 4 && x < width / 2 && y > height / 4 && y < height / 3) {
                rrVariation += fabs(v - ar[(x - 1) * 4 + c]); rawVariation += fabs(br[x * 4 + c] - br[(x - 1) * 4 + c]);
            }
        }
    }
    D3D12_RANGE noWrite{0, 0}; rr->Unmap(0, &noWrite); raw->Unmap(0, &noWrite);
    const double channels = double(width) * height * 3;
    log << "RR readback: frames=" << historyFrames << ", evaluations=" << evaluations << ", mean=" << mean / channels << ", raw difference=" << difference / channels << ", invalid=" << invalid << ", wall variation raw=" << rawVariation << " RR=" << rrVariation << '\n';
    return invalid == 0 && mean / channels > 0.001 && difference / channels > 0.0001;
}

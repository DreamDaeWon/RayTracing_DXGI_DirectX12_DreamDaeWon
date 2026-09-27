#include "PathTracer.h"
using namespace DirectX;

void PathTracer::Initialize(D3D12Context& context) {
    D3D12_DESCRIPTOR_RANGE ranges[3]{};
    ranges[0] = {D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, 0};
    ranges[1] = {D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, 0};
    ranges[2] = {D3D12_DESCRIPTOR_RANGE_TYPE_UAV, GuideCount, 1, 0, 0};
    D3D12_ROOT_PARAMETER parameters[6]{};
    parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; parameters[0].Descriptor.ShaderRegister = 0;
    for (UINT i = 0; i < 2; ++i) { parameters[i + 1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; parameters[i + 1].DescriptorTable = {1, &ranges[i]}; }
    parameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV; parameters[3].Descriptor.ShaderRegister = 1;
    parameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV; parameters[4].Descriptor.ShaderRegister = 2;
    parameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; parameters[5].DescriptorTable = {1, &ranges[2]};
    D3D12_ROOT_SIGNATURE_DESC desc{}; desc.NumParameters = 6; desc.pParameters = parameters;
    root = CreateRootSignature(context.Device(), desc);
    auto shader = LoadShader(L"PathTracing.cso"); D3D12_COMPUTE_PIPELINE_STATE_DESC pso{};
    pso.pRootSignature = root.Get(); pso.CS = {shader->GetBufferPointer(), shader->GetBufferSize()};
    Check(context.Device()->CreateComputePipelineState(&pso, IID_PPV_ARGS(&pipeline)), "Create path tracing pipeline");
    for (UINT i = 0; i < 2; ++i) {
        srvIndices[i] = context.AllocateDescriptor(); uavIndices[i] = context.AllocateDescriptor();
        constants[i] = CreateBuffer(context.Device(), 256, D3D12_HEAP_TYPE_UPLOAD);
    }
    guideUavBase = context.AllocateDescriptor();
    for (UINT i = 1; i < GuideCount; ++i) if (context.AllocateDescriptor() != guideUavBase + i) throw std::runtime_error("RR guide descriptors must be contiguous");
    rawSrv = context.AllocateDescriptor();
}
void PathTracer::Resize(D3D12Context& context, UINT w, UINT h) {
    w = std::max(w, 1u); h = std::max(h, 1u);
    if (width == w && height == h) return;
    context.WaitIdle(); width = w; height = h; Reset(); readIndex = 0; previousCameraValid = false;
    D3D12_RESOURCE_DESC desc{}; desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = w; desc.Height = h; desc.DepthOrArraySize = 1; desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; desc.SampleDesc.Count = 1; desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    D3D12_HEAP_PROPERTIES heap{}; heap.Type = D3D12_HEAP_TYPE_DEFAULT;
    for (UINT i = 0; i < 2; ++i) {
        accumulation[i].Reset();
        Check(context.Device()->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, ReadState, nullptr, IID_PPV_ARGS(&accumulation[i])), "Create float accumulation texture");
        accumulation[i]->SetName(i == 0 ? L"Accumulation A" : L"Accumulation B");
        D3D12_SHADER_RESOURCE_VIEW_DESC srv{}; srv.Format = desc.Format; srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; srv.Texture2D.MipLevels = 1;
        context.Device()->CreateShaderResourceView(accumulation[i].Get(), &srv, context.Cpu(srvIndices[i]));
        D3D12_UNORDERED_ACCESS_VIEW_DESC uav{}; uav.Format = desc.Format; uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        context.Device()->CreateUnorderedAccessView(accumulation[i].Get(), nullptr, &uav, context.Cpu(uavIndices[i]));
    }
    for (UINT i = 0; i < GuideCount; ++i) {
        desc.Format = i == Depth || i == SpecularDistance ? DXGI_FORMAT_R32_FLOAT : i == Motion ? DXGI_FORMAT_R32G32_FLOAT : DXGI_FORMAT_R32G32B32A32_FLOAT;
        guides[i].Reset();
        Check(context.Device()->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, GuideState, nullptr, IID_PPV_ARGS(&guides[i])), "Create RR guide texture");
        D3D12_UNORDERED_ACCESS_VIEW_DESC uav{}; uav.Format = desc.Format; uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        context.Device()->CreateUnorderedAccessView(guides[i].Get(), nullptr, &uav, context.Cpu(guideUavBase + i));
    }
    D3D12_SHADER_RESOURCE_VIEW_DESC raw{}; raw.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; raw.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    raw.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; raw.Texture2D.MipLevels = 1;
    context.Device()->CreateShaderResourceView(guides[NoisyColor].Get(), &raw, context.Cpu(rawSrv));
}
void PathTracer::Render(D3D12Context& context, const Scene& scene, const RendererSettings& settings, float aspect) {
    if ((!settings.accumulation && settings.comparison != 6) || accumulatedFrames >= 1000000) Reset();
    XMFLOAT3 f, r, u; scene.camera.Basis(f, r, u);
    Constants data{};
    data.cameraPosition = {scene.camera.position.x, scene.camera.position.y, scene.camera.position.z, 0};
    data.forward = {f.x, f.y, f.z, 0}; data.right = {r.x, r.y, r.z, 0}; data.up = {u.x, u.y, u.z, 0};
    data.dimensions = {width, height, accumulatedFrames, sequence++};
    data.options = {static_cast<UINT>(scene.objects.size()), static_cast<UINT>(settings.maxBounces), static_cast<UINT>(settings.spp), static_cast<UINT>(settings.comparison)};
    data.settings = {settings.split, settings.skyIntensity, aspect, tanf(XMConvertToRadians(scene.camera.fov) * 0.5f)};
    auto halton = [](UINT index, UINT base) { float f = 1, r = 0; while (index) { f /= float(base); r += f * float(index % base); index /= base; } return r; };
    jitter = {halton((sequence - 1) % 64 + 1, 2) - 0.5f, halton((sequence - 1) % 64 + 1, 3) - 0.5f};
    data.jitter = {jitter.x, jitter.y, settings.comparison >= 3 ? 1.0f : 0.0f, 0};
    XMStoreFloat4x4(&data.viewProjection, scene.camera.View() * scene.camera.Projection(aspect));
    data.previousViewProjection = previousCameraValid ? previousViewProjection : data.viewProjection;
    previousViewProjection = data.viewProjection; previousCameraValid = true;
    void* ptr = nullptr; D3D12_RANGE noRead{0, 0};
    Check(constants[context.FrameIndex()]->Map(0, &noRead, &ptr), "Map path tracing constants"); memcpy(ptr, &data, sizeof(data)); constants[context.FrameIndex()]->Unmap(0, nullptr);
    UINT writeIndex = 1 - readIndex;
    context.Transition(accumulation[writeIndex].Get(), ReadState, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    for (auto& guide : guides) context.Transition(guide.Get(), GuideState, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    auto* list = context.List(); list->SetPipelineState(pipeline.Get()); list->SetComputeRootSignature(root.Get());
    list->SetComputeRootConstantBufferView(0, constants[context.FrameIndex()]->GetGPUVirtualAddress());
    list->SetComputeRootDescriptorTable(1, context.Gpu(srvIndices[readIndex]));
    list->SetComputeRootDescriptorTable(2, context.Gpu(uavIndices[writeIndex]));
    list->SetComputeRootShaderResourceView(3, scene.ObjectAddress(context.FrameIndex()));
    list->SetComputeRootShaderResourceView(4, scene.MaterialAddress(context.FrameIndex()));
    list->SetComputeRootDescriptorTable(5, context.Gpu(guideUavBase));
    list->Dispatch((width + 7) / 8, (height + 7) / 8, 1);
    // Transition barriers order UAV writes before both post-processing and next frame's read.
    context.Transition(accumulation[writeIndex].Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, ReadState);
    for (auto& guide : guides) context.Transition(guide.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, GuideState);
    readIndex = writeIndex; ++accumulatedFrames;
}
bool PathTracer::ValidateOutput(D3D12Context& context, std::ostream& log) {
    auto desc = accumulation[readIndex]->GetDesc(); D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout{}; UINT64 bytes = 0;
    context.Device()->GetCopyableFootprints(&desc, 0, 1, 0, &layout, nullptr, nullptr, &bytes);
    auto readback = CreateBuffer(context.Device(), bytes, D3D12_HEAP_TYPE_READBACK);
    context.Transition(accumulation[readIndex].Get(), ReadState, D3D12_RESOURCE_STATE_COPY_SOURCE);
    D3D12_TEXTURE_COPY_LOCATION dst{}; dst.pResource = readback.Get(); dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT; dst.PlacedFootprint = layout;
    D3D12_TEXTURE_COPY_LOCATION src{}; src.pResource = accumulation[readIndex].Get(); src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    context.List()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
    context.Transition(accumulation[readIndex].Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, ReadState);
    context.FlushAndContinue();
    uint8_t* pixels = nullptr; D3D12_RANGE read{0, static_cast<SIZE_T>(bytes)};
    Check(readback->Map(0, &read, reinterpret_cast<void**>(&pixels)), "Map validation pixels");
    double mean = 0; float peak = 0; UINT invalid = 0;
    for (UINT y = 0; y < height; ++y) {
        auto* row = reinterpret_cast<float*>(pixels + layout.Offset + y * layout.Footprint.RowPitch);
        for (UINT x = 0; x < width; ++x) for (UINT c = 0; c < 3; ++c) {
            float value = row[x * 4 + c]; if (!std::isfinite(value) || value < 0) ++invalid;
            else { mean += value; peak = std::max(peak, value); }
        }
    }
    D3D12_RANGE noWrite{0, 0}; readback->Unmap(0, &noWrite); mean /= double(width) * height * 3;
    log << "Float readback: " << width << 'x' << height << ", history=" << accumulatedFrames << ", mean=" << mean << ", peak=" << peak << ", invalid=" << invalid << '\n';
    return invalid == 0 && mean > 0.0001 && peak > 0.01;
}

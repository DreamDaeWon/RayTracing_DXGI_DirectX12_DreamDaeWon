#include "PostProcessor.h"
void PostProcessor::Initialize(D3D12Context& context) {
    D3D12_DESCRIPTOR_RANGE ranges[3]{};
    D3D12_ROOT_PARAMETER params[4]{};
    for (UINT i = 0; i < 3; ++i) {
        ranges[i] = {D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i, 0, 0};
        params[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; params[i].DescriptorTable = {1, &ranges[i]}; params[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    }
    params[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS; params[3].Constants = {0, 0, 4}; params[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    D3D12_STATIC_SAMPLER_DESC sampler{}; sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = sampler.AddressV = sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS; sampler.MaxLOD = D3D12_FLOAT32_MAX; sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    D3D12_ROOT_SIGNATURE_DESC desc{}; desc.NumParameters = 4; desc.pParameters = params; desc.NumStaticSamplers = 1; desc.pStaticSamplers = &sampler; desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    root = CreateRootSignature(context.Device(), desc);
    auto vs = LoadShader(L"PostProcessVS.cso"), ps = LoadShader(L"PostProcessPS.cso");
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{}; pso.pRootSignature = root.Get();
    pso.VS = {vs->GetBufferPointer(), vs->GetBufferSize()}; pso.PS = {ps->GetBufferPointer(), ps->GetBufferSize()};
    pso.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    pso.BlendState.RenderTarget[0].SrcBlend = pso.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    pso.BlendState.RenderTarget[0].DestBlend = pso.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    pso.BlendState.RenderTarget[0].BlendOp = pso.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    pso.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    pso.SampleMask = UINT_MAX; pso.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; pso.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; pso.RasterizerState.DepthClipEnable = TRUE;
    pso.DepthStencilState.DepthEnable = FALSE; pso.DepthStencilState.StencilEnable = FALSE;
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; pso.NumRenderTargets = 1; pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; pso.SampleDesc.Count = 1;
    Check(context.Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipeline)), "Create ACES pipeline");
}
void PostProcessor::Render(D3D12Context& context, const PathTracer& tracer, const RayReconstruction& rr, const ViewportRect& rect, const RendererSettings& settings) {
    auto* list = context.List(); D3D12_VIEWPORT viewport{rect.x, rect.y, rect.width, rect.height, 0, 1};
    ID3D12DescriptorHeap* heaps[] = {context.Heap()}; list->SetDescriptorHeaps(1, heaps);
    auto rtv = context.CurrentRTV(); list->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
    context.Transition(tracer.GuideResource(PathTracer::NoisyColor), PathTracer::GuideState, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    D3D12_RECT scissor{LONG(rect.x), LONG(rect.y), LONG(rect.x + rect.width), LONG(rect.y + rect.height)};
    list->RSSetViewports(1, &viewport); list->RSSetScissorRects(1, &scissor);
    list->SetPipelineState(pipeline.Get()); list->SetGraphicsRootSignature(root.Get());
    list->SetGraphicsRootDescriptorTable(0, tracer.Output(context));
    list->SetGraphicsRootDescriptorTable(1, tracer.RawOutput(context));
    list->SetGraphicsRootDescriptorTable(2, rr.Active() ? rr.Output(context) : tracer.RawOutput(context));
    const float constants[] = {settings.exposure, settings.split, float(settings.comparison), rr.Active() ? 1.0f : 0.0f};
    list->SetGraphicsRoot32BitConstants(3, 4, constants, 0);
    list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); list->DrawInstanced(3, 1, 0, 0);
    context.Transition(tracer.GuideResource(PathTracer::NoisyColor), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, PathTracer::GuideState);
}

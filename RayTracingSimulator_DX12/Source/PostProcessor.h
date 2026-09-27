#pragma once
#include "PathTracer.h"
#include "RayReconstruction.h"
class PostProcessor {
public:
    void Initialize(D3D12Context& context);
    void Render(D3D12Context& context, const PathTracer& tracer, const RayReconstruction& rr, const ViewportRect& viewport, const RendererSettings& settings);
private:
    ComPtr<ID3D12RootSignature> root;
    ComPtr<ID3D12PipelineState> pipeline;
};

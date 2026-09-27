#pragma once
#include "PathTracer.h"
#include "nvsdk_ngx.h"

class RayReconstruction {
public:
    void Initialize(D3D12Context& context);
    void Shutdown(D3D12Context& context);
    void Reset() { reset = true; historyFrames = 0; }
    bool Evaluate(D3D12Context& context, const PathTracer& tracer, const Camera& camera, float aspect, float dt);
    bool Available() const { return available; }
    bool Active() const { return active; }
    void Deactivate() { if (active) { active = false; Reset(); if (available) status = "DLSS RR ready / inactive"; } }
    const std::string& Status() const { return status; }
    UINT Frames() const { return historyFrames; }
    UINT Evaluations() const { return evaluations; }
    D3D12_GPU_DESCRIPTOR_HANDLE Output(const D3D12Context& context) const { return context.Gpu(srv); }
    ID3D12Resource* OutputResource() const { return output.Get(); }
    bool ValidateOutput(D3D12Context& context, const PathTracer& tracer, std::ostream& log);
private:
    bool Fail(NVSDK_NGX_Result result, const char* action);
    NVSDK_NGX_Handle* handle = nullptr;
    NVSDK_NGX_Parameter* parameters = nullptr;
    ComPtr<ID3D12Resource> output;
    UINT width = 0, height = 0, srv = 0, historyFrames = 0, evaluations = 0;
    bool initialized = false, available = false, active = false, reset = true;
    std::string status = "Not initialized";
};

#pragma once
#include "PathTracer.h"
#include "RayReconstruction.h"
class EditorUI {
public:
    void Initialize(HWND window, D3D12Context& context);
    void Shutdown();
    bool Build(D3D12Context& context, Scene& scene, RendererSettings& settings, const PathTracer& tracer, const RayReconstruction& rr, float dt);
    bool RRResetRequested() const { return rrResetRequested; }
    void Render(D3D12Context& context);
    ViewportRect Viewport(UINT width, UINT height) const;
private:
    int selected = 1, operation = 0;
    bool initialized = false;
    bool rrResetRequested = false;
};

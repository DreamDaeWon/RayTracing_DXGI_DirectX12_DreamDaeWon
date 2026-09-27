#pragma once
#include "EditorUI.h"
#include "PostProcessor.h"

class Application {
public:
    int Run(HINSTANCE instance);
    ~Application();
private:
    static LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
    void ParseArguments();
    void SelfTestStep(UINT frame, std::ostream& log);
    HWND window = nullptr;
    D3D12Context context;
    Scene scene;
    PathTracer tracer;
    RayReconstruction reconstruction;
    PostProcessor post;
    EditorUI editor;
    RendererSettings settings;
    UINT pendingWidth = 1280, pendingHeight = 720, frameLimit = 0;
    bool minimized = false, selfTest = false, debug = false, warp = false, running = true;
    bool rrTest = false;
    std::filesystem::path capturePath;
};

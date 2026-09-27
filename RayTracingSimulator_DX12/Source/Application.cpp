#include "Application.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include <shellapi.h>
#include <chrono>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

Application::~Application() {
    try { if (context.Device()) context.WaitIdle(); } catch (...) {}
    try { if (context.Device()) reconstruction.Shutdown(context); } catch (...) {}
    editor.Shutdown(); if (window && IsWindow(window)) DestroyWindow(window);
}
LRESULT CALLBACK Application::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* app = reinterpret_cast<Application*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        app = static_cast<Application*>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
    }
    if (ImGui::GetCurrentContext() && ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam)) return 1;
    if (app) switch (message) {
    case WM_SIZE:
        app->minimized = wParam == SIZE_MINIMIZED;
        if (!app->minimized) { app->pendingWidth = LOWORD(lParam); app->pendingHeight = HIWORD(lParam); } return 0;
    case WM_GETMINMAXINFO: reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize = {900, 560}; return 0;
    case WM_ERASEBKGND: return 1;
    case WM_CLOSE: app->running = false; return 0;
    case WM_DESTROY: app->running = false; PostQuitMessage(0); return 0;
    case WM_SYSCOMMAND: if ((wParam & 0xfff0) == SC_KEYMENU) return 0; break;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}
void Application::ParseArguments() {
    int count = 0; auto args = CommandLineToArgvW(GetCommandLineW(), &count);
    if (!args) throw std::runtime_error("Parse command line failed");
    try {
        for (int i = 1; i < count; ++i) {
            std::wstring arg(args[i]);
            auto value = [&]() -> const wchar_t* { if (++i >= count) throw std::runtime_error("Missing command line value"); return args[i]; };
            if (arg == L"--self-test" || arg == L"--rr-test") { selfTest = true; rrTest = arg == L"--rr-test"; debug = true; frameLimit = 96; settings.vsync = false; pendingWidth = 960; pendingHeight = 600; settings.resolutionScale = 0.75f; settings.comparison = rrTest ? 3 : 2; }
            else if (arg == L"--debug") debug = true;
            else if (arg == L"--warp") warp = true;
            else if (arg == L"--frames") frameLimit = std::max(1, _wtoi(value()));
            else if (arg == L"--width") pendingWidth = std::clamp(_wtoi(value()), 900, 3840);
            else if (arg == L"--height") pendingHeight = std::clamp(_wtoi(value()), 560, 2160);
            else if (arg == L"--mode") settings.comparison = std::clamp(_wtoi(value()), 0, 6);
            else if (arg == L"--spp") settings.spp = std::clamp(_wtoi(value()), 1, 8);
            else if (arg == L"--bounces") settings.maxBounces = std::clamp(_wtoi(value()), 1, 16);
            else if (arg == L"--scale") settings.resolutionScale = std::clamp(float(_wtof(value())), 0.25f, 1.0f);
            else if (arg == L"--capture") capturePath = value();
            else throw std::runtime_error("Unknown command line option");
        }
    } catch (...) { LocalFree(args); throw; }
    LocalFree(args);
}
void Application::SelfTestStep(UINT frame, std::ostream& log) {
    if (rrTest) {
        if (frame >= 16 && frame < 32) { scene.camera.position.x += 0.01f; tracer.Reset(); }
        if (frame == 32) {
            if (reconstruction.Frames() != 32) throw std::runtime_error("Camera movement must preserve RR history");
            scene.materials[1].parameters.x = 0.5f; tracer.Reset(); reconstruction.Reset(); log << "RR material reset\n";
        }
        if (frame == 40) { settings.split = 0.3f; log << "RR split change preserves history\n"; }
        if (frame == 48) {
            if (reconstruction.Frames() != 16) throw std::runtime_error("Split slider must preserve RR history");
            SetWindowPos(window, nullptr, 0, 0, 1050, 690, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE); log << "RR resize\n";
        }
        if (frame == 64) { settings.comparison = 2; tracer.Reset(); log << "Legacy comparison\n"; }
        if (frame == 72) { settings.comparison = 6; settings.accumulation = false; settings.split = 0.5f; tracer.Reset(); log << "Accumulated GI / RR comparison activated\n"; }
        if (frame == 73 && !settings.accumulation) throw std::runtime_error("GI / RR comparison must force accumulation on");
        if (frame == 80) { reconstruction.Reset(); log << "Manual RR reset\n"; }
        return;
    }
    // Exercise real GPU resource lifetimes and history invalidation, including boundary settings.
    if (frame == 8) { scene.camera.position.x += 0.3f; tracer.Reset(); log << "Camera edit -> reset\n"; }
    if (frame == 16) { scene.materials[1].parameters.x = 0.65f; tracer.Reset(); log << "Material edit -> reset\n"; }
    if (frame == 24) { scene.objects[3].position.y += 0.3f; scene.objects[3].rotation.y += 25; tracer.Reset(); log << "Transform edit -> reset\n"; }
    if (frame == 32) { settings.accumulation = false; log << "Accumulation off\n"; }
    if (frame == 36) { if (tracer.AccumulatedFrames() != 1) throw std::runtime_error("Accumulation-off invariant failed"); settings.accumulation = true; tracer.Reset(); }
    if (frame == 40) { settings.comparison = 1; settings.maxBounces = 1; tracer.Reset(); log << "Direct-only mode\n"; }
    if (frame == 48) { settings.comparison = 0; settings.maxBounces = 16; settings.spp = 8; tracer.Reset(); log << "GI: 16 bounces / 8 spp\n"; }
    if (frame == 52) { settings.spp = 1; settings.maxBounces = 6; settings.comparison = 2; settings.split = 0.35f; tracer.Reset(); }
    if (frame == 56) { SetWindowPos(window, nullptr, 0, 0, 1050, 690, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE); log << "Resize window and accumulation textures\n"; }
    if (frame == 64) { settings.resolutionScale = 0.65f; tracer.Reset(); log << "Resolution scale change\n"; }
    if (frame == 72) { scene.Reset(); settings.split = 0.5f; tracer.Reset(); log << "Restore scene\n"; }
    if (frame == 80) { settings.exposure = 0.5f; log << "Exposure only / keep history\n"; }
}
int Application::Run(HINSTANCE instance) {
    ParseArguments();
    std::ofstream log(ExecutableDirectory() / (rrTest ? L"rr-test.log" : selfTest ? L"self-test.log" : L"runtime.log"));
    ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc{sizeof(wc)}; wc.style = CS_HREDRAW | CS_VREDRAW; wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = instance; wc.hCursor = LoadCursorW(nullptr, IDC_ARROW); wc.lpszClassName = L"PathTracingSimulatorDX12";
    if (!RegisterClassExW(&wc)) throw std::runtime_error("Register window class failed");
    RECT size{0, 0, LONG(pendingWidth), LONG(pendingHeight)}; AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, FALSE);
    window = CreateWindowExW(0, wc.lpszClassName, L"LUMEN | DirectX 12 Path Tracing Simulator", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, size.right - size.left, size.bottom - size.top, nullptr, nullptr, instance, this);
    if (!window) throw std::runtime_error("Create window failed");
    context.Initialize(window, pendingWidth, pendingHeight, debug, warp);
    scene.Initialize(context.Device()); tracer.Initialize(context); post.Initialize(context); editor.Initialize(window, context);
    reconstruction.Initialize(context);
    log << reconstruction.Status() << '\n';
    if (rrTest && !reconstruction.Available()) throw std::runtime_error(reconstruction.Status());
    log << "Adapter: " << context.AdapterName() << "\nGPU validation: " << context.DebugActive() << '\n'; log.flush();
    // Automated tests can render off-screen; ordinary launches display the interactive editor.
    if (!selfTest) { ShowWindow(window, SW_SHOW); UpdateWindow(window); }
    auto last = std::chrono::steady_clock::now(); UINT rendered = 0; unsigned validationErrors = 0;
    while (running) {
        MSG message{};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) { if (message.message == WM_QUIT) running = false; TranslateMessage(&message); DispatchMessageW(&message); }
        if (!running) break;
        if (minimized) { WaitMessage(); last = std::chrono::steady_clock::now(); continue; }
        if (selfTest) SelfTestStep(rendered, log);
        context.Resize(pendingWidth, pendingHeight);
        context.BeginFrame();
        auto now = std::chrono::steady_clock::now(); float dt = std::clamp(std::chrono::duration<float>(now - last).count(), 0.0001f, 0.05f); last = now;
        if (editor.Build(context, scene, settings, tracer, reconstruction, dt)) tracer.Reset();
        if (editor.RRResetRequested()) reconstruction.Reset();
        auto rect = editor.Viewport(context.Width(), context.Height());
        tracer.Resize(context, std::max(1u, UINT(rect.width * settings.resolutionScale)), std::max(1u, UINT(rect.height * settings.resolutionScale)));
        scene.Upload(context.FrameIndex());
        tracer.Render(context, scene, settings, rect.width / rect.height);
        if (settings.comparison == 3 || settings.comparison == 4 || settings.comparison == 6) {
            bool result = reconstruction.Evaluate(context, tracer, scene.camera, rect.width / rect.height, dt);
            if (rrTest && !result) throw std::runtime_error(reconstruction.Status());
        } else reconstruction.Deactivate();
        bool finalFrame = frameLimit && rendered + 1 >= frameLimit;
        if (selfTest && (rendered == 7 || rendered == 35 || rendered == 47 || rendered == 51 || finalFrame)) {
            if (!tracer.ValidateOutput(context, log)) throw std::runtime_error("GPU float image validation failed");
            if (rrTest && !reconstruction.ValidateOutput(context, tracer, log)) throw std::runtime_error("RR float image validation failed");
        }
        post.Render(context, tracer, reconstruction, rect, settings); editor.Render(context);
        if (finalFrame && !capturePath.empty()) context.Capture(capturePath);
        context.EndFrame(settings.vsync); ++rendered;
        if (selfTest && rendered % 8 == 0) { context.WaitIdle(); validationErrors += context.ValidateDebugMessages(log); log.flush(); }
        if (finalFrame) running = false;
    }
    context.WaitIdle(); validationErrors += context.ValidateDebugMessages(log);
    log << "Rendered frames: " << rendered << "\nD3D12 warnings/errors: " << validationErrors << '\n';
    log << "RR status: " << reconstruction.Status() << "\nRR evaluations: " << reconstruction.Evaluations() << '\n';
    if (selfTest && !rrTest && tracer.AccumulatedFrames() != 24) throw std::runtime_error("Final accumulation history should contain 24 frames");
    if (rrTest && (reconstruction.Frames() != 16 || reconstruction.Evaluations() != 88 || tracer.AccumulatedFrames() != 24)) throw std::runtime_error("RR/GI history/evaluation count mismatch");
    bool success = validationErrors == 0 && (!selfTest || rendered == 96);
    log << (success ? "PASS" : "FAIL") << '\n'; log.flush();
    return success ? 0 : 2;
}
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    try { Application application; return application.Run(instance); }
    catch (const std::exception& error) {
        std::ofstream log(ExecutableDirectory() / L"error.log", std::ios::app); log << error.what() << '\n';
        OutputDebugStringA(error.what());
        if (!wcsstr(GetCommandLineW(), L"--self-test") && !wcsstr(GetCommandLineW(), L"--rr-test") && !wcsstr(GetCommandLineW(), L"--frames")) MessageBoxA(nullptr, error.what(), "DX12 Path Tracer - startup/runtime error", MB_OK | MB_ICONERROR);
        return 1;
    }
}

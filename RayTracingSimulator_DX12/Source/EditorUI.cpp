#include "EditorUI.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "ImGuizmo.h"
using namespace DirectX;

void EditorUI::Initialize(HWND window, D3D12Context& context) {
    IMGUI_CHECKVERSION(); ImGui::CreateContext();
    auto& io = ImGui::GetIO(); io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; io.IniFilename = nullptr;
    const char* font = "C:/Windows/Fonts/segoeui.ttf";
    if (std::filesystem::exists(font)) io.Fonts->AddFontFromFileTTF(font, 16.0f);
    ImGui::StyleColorsDark(); auto& style = ImGui::GetStyle();
    style.WindowRounding = 0; style.FrameRounding = 4; style.GrabRounding = 4;
    style.WindowPadding = {16, 14}; style.FramePadding = {8, 5}; style.ItemSpacing = {8, 9};
    style.Colors[ImGuiCol_WindowBg] = {0.045f, 0.055f, 0.075f, 1};
    style.Colors[ImGuiCol_FrameBg] = {0.09f, 0.11f, 0.145f, 1};
    style.Colors[ImGuiCol_Header] = {0.11f, 0.21f, 0.25f, 1};
    style.Colors[ImGuiCol_HeaderHovered] = {0.13f, 0.30f, 0.33f, 1};
    style.Colors[ImGuiCol_Button] = {0.10f, 0.25f, 0.28f, 1};
    style.Colors[ImGuiCol_ButtonHovered] = {0.15f, 0.4f, 0.42f, 1};
    style.Colors[ImGuiCol_SliderGrab] = style.Colors[ImGuiCol_CheckMark] = {0.27f, 0.85f, 0.73f, 1};
    if (!ImGui_ImplWin32_Init(window)) throw std::runtime_error("Initialize ImGui Win32 failed");
    ImGui_ImplDX12_InitInfo info{}; info.Device = context.Device(); info.CommandQueue = context.Queue();
    info.NumFramesInFlight = D3D12Context::FrameCount; info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    info.SrvDescriptorHeap = context.Heap(); info.UserData = &context;
    info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* data, D3D12_CPU_DESCRIPTOR_HANDLE* cpu, D3D12_GPU_DESCRIPTOR_HANDLE* gpu) {
        auto& ctx = *static_cast<D3D12Context*>(data->UserData); UINT index = ctx.AllocateDescriptor(); *cpu = ctx.Cpu(index); *gpu = ctx.Gpu(index);
    };
    info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* data, D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE) {
        auto& ctx = *static_cast<D3D12Context*>(data->UserData);
        UINT stride = ctx.Device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        ctx.FreeDescriptor(static_cast<UINT>((cpu.ptr - ctx.Cpu(0).ptr) / stride));
    };
    if (!ImGui_ImplDX12_Init(&info)) throw std::runtime_error("Initialize ImGui DX12 failed");
    initialized = true;
}
void EditorUI::Shutdown() {
    if (initialized) { ImGui_ImplDX12_Shutdown(); ImGui_ImplWin32_Shutdown(); ImGui::DestroyContext(); initialized = false; }
}
ViewportRect EditorUI::Viewport(UINT width, UINT height) const { return {0, 84, std::max(1.0f, float(width) - 326), std::max(1.0f, float(height) - 116)}; }
bool EditorUI::Build(D3D12Context& context, Scene& scene, RendererSettings& settings, const PathTracer& tracer, const RayReconstruction& rr, float dt) {
    ImGui_ImplDX12_NewFrame(); ImGui_ImplWin32_NewFrame(); ImGui::NewFrame(); ImGuizmo::BeginFrame();
    bool changed = false, rrOnlyReset = false; auto& io = ImGui::GetIO();
    const ImGuiWindowFlags fixed = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
    auto rect = Viewport(context.Width(), context.Height());
    ImGui::SetNextWindowPos({0, 0}); ImGui::SetNextWindowSize({float(context.Width()), 84});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {16, 9});
    ImGui::Begin("Top bar", nullptr, fixed);
    ImGui::TextColored({0.32f, 0.88f, 0.77f, 1}, "LUMEN / DX12"); ImGui::SameLine(160);
    ImGui::TextDisabled("PATH TRACING LAB"); ImGui::SameLine(350);
    ImGui::SetNextItemWidth(240);
    changed |= ImGui::Combo("##mode", &settings.comparison, "Full global illumination\0Direct light baseline\0Split: Direct / GI\0Split: Raw / DLSS RR\0DLSS RR only\0Raw path tracing only\0Split: Full GI / DLSS RR\0");
    ImGui::SameLine(); if (ImGui::Button("Reset history")) changed = true;
    if (ImGui::BeginTabBar("Comparison tabs")) {
        // A clicked tab may become visible only on the next frame: inspect clicks
        // even when BeginTabItem returns false, or forced selection would undo them.
        bool visible = ImGui::BeginTabItem("Lighting comparison", nullptr, settings.comparison < 3 ? ImGuiTabItemFlags_SetSelected : 0);
        if (ImGui::IsItemClicked()) { settings.comparison = 2; changed = true; }
        if (visible) ImGui::EndTabItem();
        visible = ImGui::BeginTabItem("Raw / DLSS Ray Reconstruction", nullptr, settings.comparison >= 3 && settings.comparison <= 5 ? ImGuiTabItemFlags_SetSelected : 0);
        if (ImGui::IsItemClicked()) { settings.comparison = 3; changed = true; }
        if (visible) ImGui::EndTabItem();
        visible = ImGui::BeginTabItem("Full GI / DLSS RR", nullptr, settings.comparison == 6 ? ImGuiTabItemFlags_SetSelected : 0);
        if (ImGui::IsItemClicked()) { settings.comparison = 6; changed = true; }
        if (visible) ImGui::EndTabItem();
        ImGui::EndTabBar();
    }
    if (settings.comparison == 6 && !settings.accumulation) { settings.accumulation = true; changed = true; }
    ImGui::End(); ImGui::PopStyleVar();

    ImGui::SetNextWindowPos({rect.width, rect.y}); ImGui::SetNextWindowSize({326, rect.height});
    ImGui::Begin("Inspector", nullptr, fixed);
    if (ImGui::BeginTabBar("InspectorTabs")) {
        if (ImGui::BeginTabItem("Scene")) {
            ImGui::TextDisabled("OBJECTS / %d", static_cast<int>(scene.objects.size()));
            if (ImGui::BeginListBox("##objects", {-1, 120})) {
                for (int i = 0; i < static_cast<int>(scene.objects.size()); ++i)
                    if (ImGui::Selectable(scene.objects[i].name.c_str(), selected == i)) selected = i;
                ImGui::EndListBox();
            }
            selected = std::clamp(selected, 0, static_cast<int>(scene.objects.size()) - 1);
            auto& object = scene.objects[selected]; auto& material = scene.materials[object.material];
            ImGui::SeparatorText("Transform");
            ImGui::RadioButton("Move", &operation, 0); ImGui::SameLine(); ImGui::RadioButton("Rotate", &operation, 1); ImGui::SameLine(); ImGui::RadioButton("Scale", &operation, 2);
            ImGui::SetNextItemWidth(205); changed |= ImGui::DragFloat3("Position", &object.position.x, 0.02f);
            ImGui::SetNextItemWidth(205);
            if (object.box) changed |= ImGui::DragFloat3("Rotation", &object.rotation.x, 0.5f);
            else { float radius = object.scale.x; if (ImGui::DragFloat("Radius", &radius, 0.01f, 0.05f, 10, "%.2f", ImGuiSliderFlags_AlwaysClamp)) { object.scale = {radius, radius, radius}; changed = true; } }
            if (object.box) { ImGui::SetNextItemWidth(205); changed |= ImGui::DragFloat3("Extents", &object.scale.x, 0.02f, 0.05f, 20, "%.2f", ImGuiSliderFlags_AlwaysClamp); }
            ImGui::SeparatorText("Surface / PBR");
            ImGui::SetNextItemWidth(200); changed |= ImGui::ColorEdit3("Color", &material.color.x, ImGuiColorEditFlags_Float);
            ImGui::SetNextItemWidth(200); changed |= ImGui::SliderFloat("Roughness", &material.parameters.x, 0.05f, 1, "%.2f");
            ImGui::SetNextItemWidth(200); changed |= ImGui::SliderFloat("Metallic", &material.parameters.y, 0, 1, "%.2f");
            ImGui::SetNextItemWidth(200); changed |= ImGui::SliderFloat("Transmission", &material.parameters.w, 0, 1, "%.2f");
            ImGui::SetNextItemWidth(200); changed |= ImGui::SliderFloat("IOR", &material.parameters.z, 1.01f, 2.5f, "%.2f");
            ImGui::SetNextItemWidth(200); changed |= ImGui::ColorEdit3("Light tint", &material.emission.x, ImGuiColorEditFlags_Float);
            ImGui::SetNextItemWidth(200); changed |= ImGui::SliderFloat("Emission", &material.emission.w, 0, 40, "%.1f");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Renderer")) {
            ImGui::SeparatorText("Integration");
            changed |= ImGui::SliderInt("Max bounces", &settings.maxBounces, 1, 16);
            changed |= ImGui::SliderInt("SPP", &settings.spp, 1, 8);
            ImGui::BeginDisabled(settings.comparison >= 3);
            changed |= ImGui::Checkbox("Accumulate samples", &settings.accumulation);
            ImGui::EndDisabled();
            if (settings.comparison == 6) ImGui::TextWrapped("Left: accumulated GI (forced on). Right: NVIDIA RR from the current noisy frame. Same SPP and resolution; different temporal methods.");
            else if (settings.comparison >= 3) ImGui::TextWrapped("Raw side: current frame only. RR side: NVIDIA temporal reconstruction. Same samples and resolution.");
            if (ImGui::Button("Clear accumulation", {-1, 0})) changed = true;
            ImGui::SeparatorText("Comparison");
            if (ImGui::SliderFloat("Split", &settings.split, 0, 1, "%.2f") && settings.comparison < 3) changed = true;
            ImGui::TextWrapped("%s", settings.comparison < 3 ? "Left: direct-light baseline. Right: multi-bounce DX12 GI." : settings.comparison == 6 ? "Left: accumulated full GI. Right: DLSS Ray Reconstruction." : "Left: raw path tracing. Right: DLSS Ray Reconstruction.");
            ImGui::SeparatorText("DLSS Ray Reconstruction");
            ImGui::TextWrapped("%s", rr.Status().c_str());
            ImGui::Text("RR history: %u frames", rr.Frames());
            if (ImGui::Button("Reset RR history")) rrOnlyReset = true;
            ImGui::SeparatorText("Output");
            ImGui::SliderFloat("Exposure / EV", &settings.exposure, -4, 4, "%.1f");
            changed |= ImGui::SliderFloat("Render scale", &settings.resolutionScale, 0.25f, 1, "%.2f");
            changed |= ImGui::SliderFloat("Sky intensity", &settings.skyIntensity, 0, 2, "%.2f");
            ImGui::Checkbox("VSync", &settings.vsync);
            ImGui::Text("Internal: %u x %u", tracer.Width(), tracer.Height());
            ImGui::SeparatorText("Camera");
            changed |= ImGui::SliderFloat("FOV", &scene.camera.fov, 25, 90, "%.0f deg");
            ImGui::SliderFloat("Move speed", &scene.camera.speed, 0.5f, 12, "%.1f");
            if (ImGui::Button("Reset camera")) { scene.camera = Camera{}; changed = true; }
            if (ImGui::Button("Restore studio scene")) { scene.Reset(); selected = 1; changed = true; }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Info")) {
            ImGui::SeparatorText("Controls");
            ImGui::TextWrapped("Hold RMB in viewport: look around\nW A S D: move while holding RMB\nQ / E: down / up\nShift: faster movement\nT / R / S: move / rotate / scale gizmo\nDrag the split line to compare");
            ImGui::SeparatorText("Pipeline");
            ImGui::TextWrapped("Compute / HLSL 5.1\nRGBA32F ping-pong accumulation\nPCG + subpixel jitter\nGGX metals + Lambert diffuse\nSnell refraction + Fresnel glass\nEmissive area light sampling\nRussian roulette\nACES + gamma 2.2");
            ImGui::SeparatorText("Device");
            ImGui::TextWrapped("%s", context.AdapterName().c_str());
            ImGui::Text("D3D12 validation: %s", context.DebugActive() ? "GPU enabled" : "off");
            ImGui::TextWrapped("Compute intersection renderer + NVIDIA NGX DLSS Ray Reconstruction. RR uses native input/output resolution. The direct-light baseline is reconstructed; this app does not execute OpenGL.");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();

    ImGui::SetNextWindowPos({rect.x, rect.y}); ImGui::SetNextWindowSize({rect.width, rect.height});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
    ImGui::Begin("Viewport", nullptr, fixed | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    bool hovered = ImGui::IsWindowHovered();
    auto* draw = ImGui::GetWindowDrawList();
    if (settings.comparison == 2 || settings.comparison == 3 || settings.comparison == 6) {
        float x = rect.x + rect.width * settings.split;
        draw->AddLine({x, rect.y}, {x, rect.y + rect.height}, IM_COL32(113, 226, 203, 235), 1.5f);
        draw->AddCircleFilled({x, rect.y + rect.height * 0.5f}, 12, IM_COL32(35, 68, 70, 240));
        draw->AddText({x - 8, rect.y + rect.height * 0.5f - 9}, IM_COL32_WHITE, "<>");
    }
    auto badge = [&](ImVec2 p, const char* text, ImU32 color) {
        auto size = ImGui::CalcTextSize(text); draw->AddRectFilled({p.x - 8, p.y - 5}, {p.x + size.x + 8, p.y + size.y + 5}, IM_COL32(12, 19, 26, 220), 4);
        draw->AddText(p, color, text);
    };
    if (settings.comparison < 3) {
        if (settings.comparison != 0) badge({16, rect.y + 16}, "01 / DIRECT LIGHT", IM_COL32(230, 206, 153, 255));
        if (settings.comparison != 1) badge({settings.comparison == 2 ? std::max(rect.width * settings.split + 16, rect.width - 205) : 16, rect.y + 16}, "02 / PATH TRACED GI", IM_COL32(106, 230, 202, 255));
    } else {
        if (settings.comparison != 4) badge({16, rect.y + 16}, settings.comparison == 6 ? "01 / ACCUMULATED GI" : "01 / RAW PATH TRACING", IM_COL32(230, 206, 153, 255));
        if (settings.comparison != 5) badge({settings.comparison == 3 || settings.comparison == 6 ? std::max(rect.width * settings.split + 16, rect.width - 220) : 16, rect.y + 16}, rr.Active() ? "02 / DLSS RR ON" : "02 / RR NOT ACTIVE", IM_COL32(106, 230, 202, 255));
        if (!rr.Available()) badge({16, rect.y + rect.height - 32}, rr.Status().c_str(), IM_COL32(255, 185, 120, 255));
    }
    if (!io.WantTextInput && !ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        if (ImGui::IsKeyPressed(ImGuiKey_T)) operation = 0;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) operation = 1;
        if (ImGui::IsKeyPressed(ImGuiKey_S)) operation = 2;
    }
    ImGuizmo::SetDrawlist(draw); ImGuizmo::SetRect(rect.x, rect.y, rect.width, rect.height); ImGuizmo::SetOrthographic(false);
    XMFLOAT4X4 view, projection, model;
    XMStoreFloat4x4(&view, scene.camera.View()); XMStoreFloat4x4(&projection, scene.camera.Projection(rect.width / rect.height));
    auto& object = scene.objects[selected]; XMStoreFloat4x4(&model, object.Matrix());
    ImGuizmo::OPERATION gizmoOperation = operation == 0 ? ImGuizmo::TRANSLATE : operation == 1 ? ImGuizmo::ROTATE : ImGuizmo::SCALE;
    ImGuizmo::Enable(!ImGui::IsMouseDown(ImGuiMouseButton_Right));
    if (ImGuizmo::Manipulate(&view._11, &projection._11, gizmoOperation, ImGuizmo::LOCAL, &model._11)) {
        XMFLOAT3 previousScale = object.scale;
        ImGuizmo::DecomposeMatrixToComponents(&model._11, &object.position.x, &object.rotation.x, &object.scale.x);
        object.scale.x = std::max(0.05f, object.scale.x); object.scale.y = std::max(0.05f, object.scale.y); object.scale.z = std::max(0.05f, object.scale.z);
        if (!object.box) {
            float radius = object.scale.x;
            if (fabsf(object.scale.y - previousScale.y) > fabsf(radius - previousScale.x)) radius = object.scale.y;
            if (fabsf(object.scale.z - previousScale.z) > fabsf(radius - previousScale.x)) radius = object.scale.z;
            object.scale = {radius, radius, radius};
        }
        changed = true;
    }
    // Test the gizmo first so the comparison divider cannot steal an overlapping axis drag.
    if ((settings.comparison == 2 || settings.comparison == 3 || settings.comparison == 6) && !ImGuizmo::IsOver() && !ImGuizmo::IsUsing()) {
        float x = rect.x + rect.width * settings.split;
        ImGui::SetCursorScreenPos({std::clamp(x - 6, rect.x, rect.x + rect.width - 12), rect.y + 40});
        ImGui::InvisibleButton("Split divider", {12, rect.height - 40});
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            settings.split = std::clamp((io.MousePos.x - rect.x) / rect.width, 0.0f, 1.0f);
            if (settings.comparison == 2) changed = true;
        }
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }
    rrResetRequested = changed || rrOnlyReset;
    bool cameraActive = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Right) && !ImGuizmo::IsUsing();
    changed |= scene.camera.Update(dt, cameraActive, io.MouseDelta.x, io.MouseDelta.y);
    if (cameraActive) ImGui::SetMouseCursor(ImGuiMouseCursor_None);
    ImGui::End(); ImGui::PopStyleVar();
    ImGui::SetNextWindowPos({0, float(context.Height()) - 32}); ImGui::SetNextWindowSize({float(context.Width()), 32});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {16, 6});
    ImGui::Begin("Status", nullptr, fixed);
    ImGui::TextColored({0.32f, 0.88f, 0.77f, 1}, "%.0f FPS", io.Framerate); ImGui::SameLine();
    if (settings.comparison == 6) ImGui::Text(" / %.2f ms | GPU %.2f ms | GI %u frames / %llu spp | RR %s / %u frames", 1000.0f / std::max(io.Framerate, 1.0f), context.GpuMilliseconds(), tracer.AccumulatedFrames(), static_cast<unsigned long long>(tracer.AccumulatedFrames()) * settings.spp, rr.Active() ? "ON" : "OFF", rr.Frames());
    else if (settings.comparison >= 3) ImGui::Text(" / %.2f ms | GPU %.2f ms | Raw %d SPP | RR %s / %u frames", 1000.0f / std::max(io.Framerate, 1.0f), context.GpuMilliseconds(), settings.spp, rr.Active() ? "ON" : "OFF", rr.Frames());
    else ImGui::Text(" / %.2f ms   |   GPU %.2f ms   |   %u frames / %llu spp", 1000.0f / std::max(io.Framerate, 1.0f), context.GpuMilliseconds(), tracer.AccumulatedFrames(), static_cast<unsigned long long>(tracer.AccumulatedFrames()) * settings.spp);
    ImGui::SameLine(); ImGui::TextDisabled("   RMB + WASD / navigate");
    ImGui::End(); ImGui::PopStyleVar();
    return changed;
}
void EditorUI::Render(D3D12Context& context) { ImGui::Render(); ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), context.List()); }

cmake_minimum_required(VERSION 3.24)
project(RayTracingSimulator_DX12 LANGUAGES CXX)
if(NOT WIN32)
    message(FATAL_ERROR "This project requires Windows and the DirectX 12 SDK.")
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN UNICODE _UNICODE)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/External/imgui/imgui.cpp")
    message(FATAL_ERROR "Run Setup.ps1 first to download ImGui and ImGuizmo.")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/External/dlss/include/nvsdk_ngx.h")
    message(FATAL_ERROR "Run Setup.ps1 to download the NVIDIA DLSS SDK.")
endif()
# New ImGuizmo revisions put sources in src/. Normalize the official files to
# the requested External/imguizmo/ImGuizmo.cpp path without hand-written copies.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/External/imguizmo/src/ImGuizmo.cpp")
    configure_file(External/imguizmo/src/ImGuizmo.cpp "${CMAKE_CURRENT_SOURCE_DIR}/External/imguizmo/ImGuizmo.cpp" COPYONLY)
    configure_file(External/imguizmo/src/ImGuizmo.h "${CMAKE_CURRENT_SOURCE_DIR}/External/imguizmo/ImGuizmo.h" COPYONLY)
endif()
add_library(EditorDependencies STATIC
    External/imgui/imgui.cpp External/imgui/imgui_draw.cpp
    External/imgui/imgui_tables.cpp External/imgui/imgui_widgets.cpp
    External/imgui/backends/imgui_impl_win32.cpp
    External/imgui/backends/imgui_impl_dx12.cpp
    External/imguizmo/ImGuizmo.cpp)
target_include_directories(EditorDependencies PUBLIC
    External/imgui External/imgui/backends External/imguizmo
    External/imguizmo/src)
target_compile_options(EditorDependencies PRIVATE /utf-8 /MP)

add_executable(ShaderCompiler Source/ShaderCompiler.cpp)
target_link_libraries(ShaderCompiler PRIVATE d3dcompiler)
target_compile_options(ShaderCompiler PRIVATE /W4 /utf-8)
set(SHADER_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/CompiledShaders")
file(MAKE_DIRECTORY "${SHADER_OUTPUT_DIR}")
function(compile_shader name source entry profile)
    add_custom_command(OUTPUT "${SHADER_OUTPUT_DIR}/${name}.cso"
        COMMAND $<TARGET_FILE:ShaderCompiler> "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/${source}"
            "${SHADER_OUTPUT_DIR}/${name}.cso" ${entry} ${profile}
        DEPENDS ShaderCompiler "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/${source}"
        COMMENT "Compiling ${name} (${profile})" VERBATIM)
endfunction()
compile_shader(PathTracing PathTracing.hlsl CSMain cs_5_1)
compile_shader(PostProcessVS PostProcess.hlsl VSMain vs_5_1)
compile_shader(PostProcessPS PostProcess.hlsl PSMain ps_5_1)
add_custom_target(Shaders ALL DEPENDS "${SHADER_OUTPUT_DIR}/PathTracing.cso"
    "${SHADER_OUTPUT_DIR}/PostProcessVS.cso" "${SHADER_OUTPUT_DIR}/PostProcessPS.cso")

add_executable(RayTracingSimulator_DX12 WIN32
    Source/Application.cpp Source/D3D12Context.cpp Source/PathTracer.cpp
    Source/Scene.cpp Source/PostProcessor.cpp Source/EditorUI.cpp Source/RayReconstruction.cpp)
target_include_directories(RayTracingSimulator_DX12 PRIVATE
    Source External/imgui External/imgui/backends External/imguizmo External/imguizmo/src External/dlss/include)
target_compile_options(RayTracingSimulator_DX12 PRIVATE /W4 /utf-8 /MP /permissive-)
target_link_libraries(RayTracingSimulator_DX12 PRIVATE EditorDependencies
    d3d12.lib dxgi.lib d3dcompiler.lib dxguid.lib shell32.lib dwmapi.lib)
target_link_libraries(RayTracingSimulator_DX12 PRIVATE
    optimized "${CMAKE_CURRENT_SOURCE_DIR}/External/dlss/lib/Windows_x86_64/x64/nvsdk_ngx_d.lib"
    debug "${CMAKE_CURRENT_SOURCE_DIR}/External/dlss/lib/Windows_x86_64/x64/nvsdk_ngx_d_dbg.lib")
add_dependencies(RayTracingSimulator_DX12 Shaders)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
# An always-run target refreshes both source and bytecode even on shader-only edits.
add_custom_target(DeployShaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Shaders" "$<TARGET_FILE_DIR:RayTracingSimulator_DX12>/Shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SHADER_OUTPUT_DIR}" "$<TARGET_FILE_DIR:RayTracingSimulator_DX12>/Shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/External/dlss/lib/Windows_x86_64/rel/nvngx_dlssd.dll" "$<TARGET_FILE_DIR:RayTracingSimulator_DX12>/nvngx_dlssd.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/External/dlss/LICENSE.txt" "$<TARGET_FILE_DIR:RayTracingSimulator_DX12>/NVIDIA-DLSS-LICENSE.txt"
    DEPENDS RayTracingSimulator_DX12 Shaders VERBATIM)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT RayTracingSimulator_DX12)
set_property(TARGET RayTracingSimulator_DX12 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:RayTracingSimulator_DX12>")
include(CTest)
if(BUILD_TESTING)
    add_test(NAME DX12_GPU_Smoke COMMAND RayTracingSimulator_DX12 --self-test)
    set_tests_properties(DX12_GPU_Smoke PROPERTIES TIMEOUT 180)
    add_test(NAME DX12_DLSS_RR COMMAND RayTracingSimulator_DX12 --rr-test)
    set_tests_properties(DX12_DLSS_RR PROPERTIES TIMEOUT 240 LABELS "NVIDIA_RTX")
endif()

param([switch]$Run, [switch]$Test, [switch]$TestRR)
$ErrorActionPreference = 'Stop'
$vsPath = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath
if ($vsPath) {
    $env:PATH += ";$vsPath\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin"
    $env:PATH += ";$vsPath\MSBuild\Current\Bin\amd64"
}
Push-Location $PSScriptRoot
try {
    New-Item -ItemType Directory -Force -Path External | Out-Null
    $dependencyLock = Get-Content -LiteralPath 'dependencies.lock.json' -Raw | ConvertFrom-Json
    if (!(Test-Path External/imgui/imgui.cpp)) {
        git clone --depth 1 -b docking https://github.com/ocornut/imgui.git External/imgui
        if ($LASTEXITCODE) { throw 'ImGui download failed.' }
        git -C External/imgui fetch --depth 1 origin $dependencyLock.imgui.commit
        if ($LASTEXITCODE) { throw 'Pinned ImGui revision download failed.' }
        git -C External/imgui checkout --detach $dependencyLock.imgui.commit
        if ($LASTEXITCODE) { throw 'Pinned ImGui checkout failed.' }
    }
    if (!(Test-Path External/imguizmo/CMakeLists.txt)) {
        git clone --depth 1 https://github.com/CedricGuillemet/ImGuizmo.git External/imguizmo
        if ($LASTEXITCODE) { throw 'ImGuizmo download failed.' }
        git -C External/imguizmo fetch --depth 1 origin $dependencyLock.imguizmo.commit
        if ($LASTEXITCODE) { throw 'Pinned ImGuizmo revision download failed.' }
        git -C External/imguizmo checkout --detach $dependencyLock.imguizmo.commit
        if ($LASTEXITCODE) { throw 'Pinned ImGuizmo checkout failed.' }
    }
    if (!(Test-Path External/dlss/include/nvsdk_ngx.h)) {
        git clone --depth 1 --filter=blob:none --sparse https://github.com/NVIDIA/DLSS.git External/dlss
        if ($LASTEXITCODE) { throw 'NVIDIA DLSS SDK download failed.' }
        git -C External/dlss sparse-checkout set include doc lib/Windows_x86_64/rel lib/Windows_x86_64/x64
        if ($LASTEXITCODE) { throw 'NVIDIA DLSS SDK sparse checkout failed.' }
        git -C External/dlss fetch --depth 1 origin $dependencyLock.dlss.commit
        if ($LASTEXITCODE) { throw 'Pinned NVIDIA DLSS SDK download failed.' }
        git -C External/dlss checkout --detach $dependencyLock.dlss.commit
        if ($LASTEXITCODE) { throw 'Pinned NVIDIA DLSS SDK checkout failed.' }
    }
    cmake -B Build -S . -A x64
    if ($LASTEXITCODE) { throw 'CMake configure failed.' }
    cmake --build Build --config Release --parallel
    if ($LASTEXITCODE) { throw 'Release build failed.' }
    if ($Test) {
        ctest --test-dir Build -C Release -R '^DX12_GPU_Smoke$' --output-on-failure
        if ($LASTEXITCODE) { throw 'GPU smoke test failed.' }
    }
    if ($TestRR) {
        ctest --test-dir Build -C Release -R '^DX12_DLSS_RR$' --output-on-failure
        if ($LASTEXITCODE) { throw 'DLSS RR GPU test failed (requires a supported NVIDIA RTX GPU).' }
    }
    if ($Run) { & ./Build/Release/RayTracingSimulator_DX12.exe }
} finally { Pop-Location }

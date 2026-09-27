# DirectX 12 Path Tracing Simulator

C++20 / Win32 / DirectX 12 Compute / HLSL / Dear ImGui docking / ImGuizmo / NVIDIA DLSS Ray Reconstruction.
기존 `최종/` 엔진과 독립적인 프로젝트입니다. 매니저, CBase, CGameInstance를 사용하지 않습니다.

**수식·기본값·DLSS/RR 차이·전체 파일 해설과 실제 비교 화면:** [한국어 기술 해설서](Docs/기술해설서.md).

## 실행과 빌드

빌드된 실행 파일: `Build/Release/RayTracingSimulator_DX12.exe`

실행 파일과 같은 디렉터리에 `Shaders/`, `nvngx_dlssd.dll`, `NVIDIA-DLSS-LICENSE.txt`를 함께 둡니다.
작업 디렉터리에 관계없이 실행됩니다. NGX 로그/캐시를 쓰므로 실행 디렉터리에 쓰기 권한이 필요합니다.
개발 도구: Visual Studio의 **Desktop development with C++**, Windows SDK, Git.
DirectX 12 Feature Level 11_0 이상에서 동작하며 NVIDIA RTX/DXR 지원은 필수가 아닙니다.
**RR 기능**에는 지원되는 NVIDIA GeForce RTX GPU와 드라이버가 필요합니다.
동봉 SDK 310.9.1의 Windows 최소 드라이버 요구는 580.00이며 실제 사용 가능 여부는 NGX로 조회합니다.
미지원 환경에서는 RR을 적용했다고 표시하지 않고 원본 영상과 원인을 표시합니다.

프로젝트 루트에서:

```powershell
.\Setup.ps1 -Test -Run
# NVIDIA RTX에서 RR 검증까지 실행
.\Setup.ps1 -Test -TestRR -Run
```

스크립트가 vswhere로 CMake/MSBuild 경로를 추가하고, 누락된 외부 라이브러리를 다운로드하며,
`dependencies.lock.json`의 검증한 커밋으로 새 다운로드를 고정합니다. 기존 External 체크아웃은 보존합니다.
ImGui/ImGuizmo는 각 저장소의 MIT 라이선스, NVIDIA DLSS는 별도의 NVIDIA SDK 라이선스를 따릅니다.
DLSS SDK는 필요한 Windows 파일을 sparse checkout하며, 공식 서명된 RR DLL을 빌드 폴더로 복사합니다.
ImGuizmo의 새 `src/` 구조는 CMake가 원본 파일을 루트 위치로 복사해 호환합니다.

수동 빌드(저장소 루트에서):

```powershell
$vsPath = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath
if ($vsPath) {
    $env:PATH += ";$vsPath\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin"
    $env:PATH += ";$vsPath\MSBuild\Current\Bin\amd64"
}
cmake -B RayTracingSimulator_DX12/Build -S RayTracingSimulator_DX12 -A x64
cmake --build RayTracingSimulator_DX12/Build --config Release --parallel
ctest --test-dir RayTracingSimulator_DX12/Build -C Release --output-on-failure
```

HLSL도 빌드 시 `D3DCompile`로 검사합니다. 셰이더 오류/경고는 빌드 실패로 처리합니다.
HLSL 소스와 컴파일된 `.cso`를 실행 디렉터리의 `Shaders/`로 자동 배치합니다.
셰이더만 수정해도 다음 빌드에서 다시 컴파일하고 배치합니다.

## 조작

| 조작 | 동작 |
|---|---|
| 뷰포트 우클릭 유지 + 마우스 | 카메라 yaw/pitch |
| 우클릭 유지 + WASD | 카메라 앞/뒤/좌/우 |
| 우클릭 유지 + Q / E | 아래 / 위 |
| Shift | 이동 속도 3배 |
| Scene 목록 | 편집할 오브젝트 선택 |
| T / R / S | 이동 / 회전 / 크기 기즈모 |
| 화면 중앙 구분선 드래그 | 현재 비교의 분할 위치 |
| Lighting comparison 탭 | 기존 직접광 / 다중 바운스 GI 비교 |
| Raw / DLSS Ray Reconstruction 탭 | RR 없는 Path Tracing / 실제 DLSS RR 비교 |
| Full GI / DLSS RR 탭 | 누적된 Full GI / 실제 DLSS RR 비교 |
| 상단 모드 메뉴 | 두 종류의 Split 또는 각 영상 단독 표시 |
| Reset history | 수동 누적 초기화 |

구체는 반지름을 유지하는 균일 스케일, 박스는 비균일 스케일과 회전을 지원합니다.
카메라, 기즈모, 재질, FOV, 바운스, SPP, 조명, 비교 모드 변경은 즉시 누적을 초기화합니다.
직접광/GI 분할선은 픽셀의 렌더 방식도 바꾸므로 누적을 초기화하고,
Raw/RR 또는 누적 GI/RR 분할선은 표시 영역만 바꾸므로 누적과 RR 이력을 유지합니다.
Exposure는 누적 후 처리이므로 누적을 유지합니다.

RR 비교는 **동일 프레임·동일 입력 해상도·동일 SPP**의 원본을 양쪽에서 사용합니다.
왼쪽은 프레임 누적 전 원본, 오른쪽은 그 원본에 NVIDIA RR을 적용한 결과입니다.
RR은 DLAA 모드로 입력과 출력 해상도가 같으며 두 영상에 같은 후처리/노출을 적용합니다.
렌더 스케일을 낮추면 두 영상 모두 같은 배율로 화면에 확대됩니다.
RR 탭에서는 기존 평균 누적 설정을 비활성화하고 RR 자체의 시간 이력을 사용합니다.
카메라 이동은 motion vector로 RR 이력을 유지하며, 재질/기즈모/FOV/렌더 설정 변경은 RR 이력을 초기화합니다.
RR 비교의 Split 슬라이더와 Exposure 변경은 RR 이력을 초기화하지 않습니다.
새 Full GI / DLSS RR 탭에서는 왼쪽 1/N 프레임 누적을 강제로 켜고,
오른쪽 RR은 현재 프레임 noisy HDR와 가이드 버퍼를 입력으로 사용합니다.
동일한 장면·SPP·내부 해상도를 공유하지만 시간적 처리 방식과 이력 길이가 달라
같은 연산량의 화질 벤치마크로 해석하지 않습니다.

**Scene**: 위치/회전/크기, 선형 RGB Color, Roughness, Metallic, Transmission, IOR, 발광색/강도.
**Renderer**: Max Bounces 1–16, SPP 1–8, Accumulation ON/OFF, 노출, 렌더 스케일, 하늘, VSync.
**하단 상태**: FPS, CPU 프레임 간격, GPU 명령 실행 시간, 누적 프레임/픽셀당 샘플 수.
GPU 시간은 timestamp query로 측정하며 디스플레이 대기 시간은 포함하지 않습니다.

## 렌더링 구현

| 모듈 | 역할 |
|---|---|
| Application | Win32 메시지, 최소화/리사이즈, 오류 로그, 실행 옵션, GPU 통합 테스트 |
| D3D12Context | 고성능 어댑터 선택, Direct Queue, 프레임별 allocator/fence, swap chain, RTV/SRV heap, GPU timestamp |
| PathTracer | Compute PSO/root signature, RGBA32F 핑퐁, 프레임별 상수, float 출력 검증 |
| Scene | 카메라, 구체/회전 박스, StructuredBuffer 지오메트리/재질, 프레임별 업로드 |
| PostProcessor | 풀스크린 삼각형, ACES filmic curve, gamma 2.2 |
| EditorUI | ImGui/ImGuizmo, 재질·렌더 설정, 비교 슬라이더, 프로파일러 |
| RayReconstruction | NVIDIA NGX 초기화/지원 조회, RR 인스턴스·리소스 수명, 추론 실행, readback 검증 |

- PCG 해시, 프레임별 독립 시드, 서브픽셀 AA 지터.
- Lambert diffuse + GGX/Smith/Schlick rough metal BRDF, 혼합 PDF 중요도 샘플링.
- 이상적인 dielectric glass: Snell 굴절, Schlick Fresnel, 전반사, 굴절률에 따른 radiance throughput.
- 구체 면광원의 solid-angle sampling, 회전 박스 면광원의 면적 샘플링, 그림자 광선.
- Next-event estimation(NEE). 불투명 BSDF 경로의 광원 도달을 중복 가산하지 않고,
  카메라 또는 유리 delta 경로를 통한 광원 도달은 가산합니다.
- 4번째 산란 이후 Russian Roulette.
- `lerp(CurrentColor, PrevAccumColor, FrameIndex / (FrameIndex + 1.0f))`로 누적.
  FrameIndex=0은 과거 텍스처를 읽지 않아 새 리소스나 오래된 값이 섞이지 않습니다.

RR용 별도 입력: noisy HDR color, diffuse/specular albedo, world-space normal + roughness,
linear depth, current-to-previous pixel motion vector, specular hit distance.
64위치 Halton 카메라 지터를 사용하고 경로 산란은 PCG 난수로 샘플링합니다.
`NGX_D3D12_CREATE_DLSSD_EXT` / `NGX_D3D12_EVALUATE_DLSSD_EXT`로 실제 NVIDIA RR 모델을 실행합니다.
입력은 NON_PIXEL_SHADER_RESOURCE, 출력은 UAV로 전달하고 NGX 호출 후 앱의 descriptor heap과 RTV를 다시 바인딩합니다.
RR 출력은 PIXEL_SHADER_RESOURCE로 전환하여 ACES 패스에서 원본과 합성합니다.
리사이즈 시 GPU 작업을 완료하고 RR 인스턴스 및 출력 텍스처를 재생성합니다.

리소스 상태:

```text
Accumulation A/B:
  combined pixel/non-pixel SRV -> UAV -> combined pixel/non-pixel SRV
  한 프레임은 A를 읽고 B를 기록, 다음 프레임은 B를 읽고 A를 기록

Back buffer:
  PRESENT -> RENDER_TARGET -> PRESENT

검증/캡처:
  SRV 또는 RENDER_TARGET -> COPY_SOURCE -> 원래 상태
```

UAV에서 SRV로 전환하는 transition barrier가 쓰기 완료 및 후속 읽기를 동기화합니다.
동일한 Direct Queue에 순서대로 제출하므로 프레임 간 핑퐁 접근도 정렬됩니다.
프레임별 allocator/상수/StructuredBuffer는 해당 fence 완료 후 재사용합니다.
리사이즈/descriptor 재작성/종료는 GPU idle을 기다립니다.

## 비교 기준과 범위

포트폴리오의 RayTracingSimulator 설명에 나온 **프레임 난수, 1/N 누적, 카메라 변경 시 리셋,
오브젝트 속성 편집**을 재구현했습니다. 저장소에는 OpenGL 원본 셰이더가 없어 비교 모드의
직접광은 **단일 표면 직접광 + 단순 환경 반사/주변광의 재현 기준**입니다.
동일한 장면/카메라/노출에서 GI의 간접광, 색 번짐, 다중 반사/굴절 차이를 보여줍니다.
실제 OpenGL과 DX12 API 성능을 비교하는 벤치마크가 아닙니다.

이 구현은 analytic sphere/OBB를 순회하는 **Compute 기반** path tracer입니다.
NVIDIA DLSS Ray Reconstruction을 포함합니다. DXR 가속 구조, 메시 로더/BVH, DLSS Frame Generation은 포함하지 않습니다.
기본 10개 오브젝트 장면에 맞췄으며 작은 광원의 유리 caustics는 수렴에 많은 샘플이 필요합니다.
유리는 매끈한 dielectric 모델이고 Roughness는 불투명 BRDF에 적용됩니다.
서로 겹친 유리의 매질 스택, 광학적 분산, 흡수 매질은 모델링하지 않습니다.
RR의 유리 guide는 첫 표면 기준이며 굴절된 배경 전체의 움직임을 정확히 추적하는 모델은 아닙니다.
복잡한 굴절/caustics나 빠른 움직임에서는 RR도 잔상 또는 세부 손실이 생길 수 있습니다.
오브젝트 편집 중에는 RR을 리셋하며 애니메이션용 오브젝트 motion vector는 구현하지 않습니다.

## 검증 및 진단

```powershell
# 96프레임 통합 테스트: 창을 표시하지 않음, D3D12 GPU validation 요청
.\Build\Release\RayTracingSimulator_DX12.exe --self-test

# NVIDIA RTX에서 실제 RR 추론과 GPU 입력/출력 검증
.\Build\Release\RayTracingSimulator_DX12.exe --rr-test

# 디버그 레이어를 켠 일반 편집기
.\Build\Release\RayTracingSimulator_DX12.exe --debug

# 지정 프레임 렌더 후 PPM 캡처 및 종료 (출력 폴더는 미리 생성)
.\Build\Release\RayTracingSimulator_DX12.exe --frames 160 --mode 3 --spp 1 --scale 1 --capture rr-comparison.ppm
```

그 밖의 옵션: `--width`, `--height`, `--mode 0..6`, `--bounces 1..16`, `--warp`.
모드: 0 GI, 1 직접광, 2 직접광/GI Split, 3 Raw/RR Split, 4 RR 단독, 5 Raw 단독, 6 누적 GI/RR Split.
`--self-test`/`--rr-test`는 96프레임으로 고정하며 `--frames`와 혼용하지 않습니다.
WARP는 소프트웨어 호환 경로로, 실시간 성능은 기대하지 않습니다.

실행 폴더의 `runtime.log`, `self-test.log`, `rr-test.log`, 예외 발생 시 `error.log`에 기록합니다.
NVIDIA SDK 진단은 `ngx.log` 및 `NGX/`에 기록합니다.
GPU 디버그 레이어가 설치되지 않은 환경은 `GPU validation: 0`으로 명시합니다.
자동 테스트는 이미지가 실제로 유효한지 GPU readback으로 검사하고, 경고도 실패로 처리합니다.

2026-09-28 검증 환경: VS 2026 / MSVC 19.51 / Windows SDK 10.0.26100.0 /
NVIDIA GeForce RTX 4070 Laptop GPU.

- Release C++/HLSL 빌드: 성공.
- CTest `DX12_GPU_Smoke`: 96프레임 통과, GPU validation 활성, D3D12 경고/오류 0.
- 카메라/재질/변환 수정, Accumulation OFF, 직접광/GI/분할 전환, 16 bounces/8 SPP,
  창 리사이즈, 렌더 스케일 변경, 장면 복원, Exposure 변경 검증.
- 5회 RGBA32F readback: 유한값/비음수 검사 통과, 검은 화면 아님 확인.
- CTest `DX12_DLSS_RR`: 96프레임, 실제 RR 평가 88회, GPU validation 활성, 경고/오류 0.
- RR 카메라 이동 이력 유지, 재질 리셋, Split 변경 이력 유지, 리사이즈,
  기존 비교 전환 후 누적 GI/RR Split 재활성화, 수동 리셋 검증.
- 5회 RR readback: 유한값, 비검정 출력, 원본과 RR의 실제 픽셀 차이 확인.
- 기본 크기 1280×720, 내부 해상도 954×604, 1 SPP × 160프레임 RR Split 렌더/캡처 성공.
- 새 누적 GI/RR Split(모드 6) 160프레임 실행/캡처: 실제 RR 평가 160회, 경고/오류 0, PASS.

공식 통합 문서: [NVIDIA DLSS SDK](https://github.com/NVIDIA/DLSS),
[Ray Reconstruction 가이드](https://github.com/NVIDIA/DLSS/blob/main/doc/DLSS-RR%20Integration%20Guide.pdf).

`External/`, `Build/`, `Captures/`는 이 하위 프로젝트의 `.gitignore`로 제외됩니다.
새 체크아웃에서는 `Setup.ps1`로 외부 소스와 빌드 결과를 복구할 수 있습니다.

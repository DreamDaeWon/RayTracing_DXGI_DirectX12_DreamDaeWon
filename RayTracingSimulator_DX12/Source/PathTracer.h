#pragma once
#include "D3D12Context.h"
#include "Scene.h"

class PathTracer {
public:
    void Initialize(D3D12Context& context);
    void Resize(D3D12Context& context, UINT width, UINT height);
    void Render(D3D12Context& context, const Scene& scene, const RendererSettings& settings, float aspect);
    void Reset() { accumulatedFrames = 0; }
    UINT AccumulatedFrames() const { return accumulatedFrames; }
    UINT Width() const { return width; }
    UINT Height() const { return height; }
    D3D12_GPU_DESCRIPTOR_HANDLE Output(const D3D12Context& context) const { return context.Gpu(srvIndices[readIndex]); }
    bool ValidateOutput(D3D12Context& context, std::ostream& log);
    enum Guide { NoisyColor, DiffuseAlbedo, SpecularAlbedo, NormalRoughness, Depth, Motion, SpecularDistance, GuideCount };
    ID3D12Resource* GuideResource(Guide guide) const { return guides[guide].Get(); }
    D3D12_GPU_DESCRIPTOR_HANDLE RawOutput(const D3D12Context& context) const { return context.Gpu(rawSrv); }
    DirectX::XMFLOAT2 Jitter() const { return jitter; }
    static constexpr D3D12_RESOURCE_STATES GuideState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
private:
    struct Constants {
        DirectX::XMFLOAT4 cameraPosition, forward, right, up;
        DirectX::XMUINT4 dimensions; // width, height, history frames, random sequence
        DirectX::XMUINT4 options; // object count, bounces, spp, comparison
        DirectX::XMFLOAT4 settings; // split, sky intensity, aspect, tan(fov/2)
        DirectX::XMFLOAT4 jitter; // common subpixel offset, RR mode, reserved
        DirectX::XMFLOAT4X4 previousViewProjection, viewProjection;
    };
    static_assert(sizeof(Constants) == 256);
    ComPtr<ID3D12RootSignature> root;
    ComPtr<ID3D12PipelineState> pipeline;
    std::array<ComPtr<ID3D12Resource>, 2> accumulation, constants;
    std::array<UINT, 2> srvIndices{}, uavIndices{};
    std::array<ComPtr<ID3D12Resource>, GuideCount> guides;
    UINT guideUavBase = 0, rawSrv = 0;
    DirectX::XMFLOAT2 jitter{};
    DirectX::XMFLOAT4X4 previousViewProjection{};
    bool previousCameraValid = false;
    UINT width = 0, height = 0, readIndex = 0, accumulatedFrames = 0, sequence = 0;
    static constexpr D3D12_RESOURCE_STATES ReadState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
};

#pragma once
#include "Common.h"

struct Material {
    DirectX::XMFLOAT4 color{0.7f, 0.7f, 0.7f, 1};
    DirectX::XMFLOAT4 emission{1, 1, 1, 0}; // rgb tint, w intensity
    DirectX::XMFLOAT4 parameters{0.45f, 0, 1.5f, 0}; // roughness, metallic, IOR, transmission
};
struct GPUObject {
    DirectX::XMFLOAT4 centerRadius;
    DirectX::XMFLOAT4 extentsType;
    DirectX::XMFLOAT4 rotation;
    DirectX::XMUINT4 material;
};
static_assert(sizeof(Material) == 48 && sizeof(GPUObject) == 64);
struct SceneObject {
    std::string name;
    DirectX::XMFLOAT3 position{}, rotation{}, scale{1, 1, 1};
    bool box = false;
    UINT material = 0;
    DirectX::XMMATRIX Matrix() const;
};
struct Camera {
    DirectX::XMFLOAT3 position{0, 2.1f, 8.5f};
    float yaw = 0, pitch = -0.055f, fov = 48, speed = 3;
    void Basis(DirectX::XMFLOAT3& forward, DirectX::XMFLOAT3& right, DirectX::XMFLOAT3& up) const;
    DirectX::XMMATRIX View() const;
    DirectX::XMMATRIX Projection(float aspect) const;
    bool Update(float dt, bool active, float mouseX, float mouseY);
};
struct RendererSettings {
    int maxBounces = 6, spp = 1, comparison = 3; // 0 GI, 1 direct, 2 light split, 3 raw/RR, 4 RR, 5 raw, 6 accumulated GI/RR
    float split = 0.5f, exposure = 0, resolutionScale = 0.75f, skyIntensity = 0.25f;
    bool accumulation = true, vsync = true;
};
class Scene {
public:
    static constexpr UINT Capacity = 64;
    Scene();
    void Reset();
    void Initialize(ID3D12Device* device);
    void Upload(UINT frame);
    D3D12_GPU_VIRTUAL_ADDRESS ObjectAddress(UINT frame) const { return objectBuffers[frame]->GetGPUVirtualAddress(); }
    D3D12_GPU_VIRTUAL_ADDRESS MaterialAddress(UINT frame) const { return materialBuffers[frame]->GetGPUVirtualAddress(); }
    std::vector<SceneObject> objects;
    std::vector<Material> materials;
    Camera camera;
private:
    std::array<ComPtr<ID3D12Resource>, 2> objectBuffers, materialBuffers;
};

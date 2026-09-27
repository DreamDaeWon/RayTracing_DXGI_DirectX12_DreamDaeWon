#include "Scene.h"
using namespace DirectX;

XMMATRIX SceneObject::Matrix() const {
    return XMMatrixScaling(scale.x, scale.y, scale.z) * XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z)) * XMMatrixTranslation(position.x, position.y, position.z);
}
void Camera::Basis(XMFLOAT3& forward, XMFLOAT3& right, XMFLOAT3& up) const {
    XMVECTOR f = XMVectorSet(sinf(yaw) * cosf(pitch), sinf(pitch), -cosf(yaw) * cosf(pitch), 0);
    XMVECTOR r = XMVector3Normalize(XMVector3Cross(f, XMVectorSet(0, 1, 0, 0)));
    XMStoreFloat3(&forward, f); XMStoreFloat3(&right, r); XMStoreFloat3(&up, XMVector3Cross(r, f));
}
XMMATRIX Camera::View() const { XMFLOAT3 f, r, u; Basis(f, r, u); return XMMatrixLookToRH(XMLoadFloat3(&position), XMLoadFloat3(&f), XMLoadFloat3(&u)); }
XMMATRIX Camera::Projection(float aspect) const { return XMMatrixPerspectiveFovRH(XMConvertToRadians(fov), aspect, 0.05f, 200.0f); }
bool Camera::Update(float dt, bool active, float mouseX, float mouseY) {
    if (!active) return false;
    bool changed = mouseX != 0 || mouseY != 0;
    yaw += mouseX * 0.003f; pitch = std::clamp(pitch - mouseY * 0.003f, -1.52f, 1.52f);
    XMFLOAT3 f, r, u; Basis(f, r, u);
    XMVECTOR motion = XMVectorZero();
    auto down = [](int key) { return (GetAsyncKeyState(key) & 0x8000) != 0; };
    if (down('W')) motion += XMLoadFloat3(&f);
    if (down('S')) motion -= XMLoadFloat3(&f);
    if (down('D')) motion += XMLoadFloat3(&r);
    if (down('A')) motion -= XMLoadFloat3(&r);
    if (down('E')) motion += XMVectorSet(0, 1, 0, 0);
    if (down('Q')) motion -= XMVectorSet(0, 1, 0, 0);
    if (XMVectorGetX(XMVector3LengthSq(motion)) > 0) {
        motion = XMVector3Normalize(motion) * dt * speed * (down(VK_SHIFT) ? 3.0f : 1.0f);
        XMStoreFloat3(&position, XMLoadFloat3(&position) + motion); changed = true;
    }
    return changed;
}
Scene::Scene() { Reset(); }
void Scene::Reset() {
    objects.clear(); materials.clear(); camera = Camera{};
    auto add = [&](const char* name, XMFLOAT3 pos, XMFLOAT3 scale, bool box, XMFLOAT3 color, float rough, float metal, float transmission = 0.0f, float emission = 0.0f) {
        Material m; m.color = {color.x, color.y, color.z, 1}; m.parameters = {rough, metal, 1.5f, transmission}; m.emission = {1.0f, 0.90f, 0.72f, emission};
        objects.push_back({name, pos, {}, scale, box, static_cast<UINT>(materials.size())}); materials.push_back(m);
    };
    add("01 / Porcelain", {-1.8f, 0.85f, 0.1f}, {0.85f, 0.85f, 0.85f}, false, {0.72f, 0.76f, 0.8f}, 0.6f, 0);
    add("02 / Brushed gold", {0.05f, 0.85f, -0.45f}, {0.85f, 0.85f, 0.85f}, false, {0.92f, 0.62f, 0.22f}, 0.22f, 1);
    add("03 / Optical glass", {1.8f, 0.95f, 0.45f}, {0.95f, 0.95f, 0.95f}, false, {0.96f, 0.99f, 1}, 0.02f, 0, 1);
    add("04 / Coral block", {-1.45f, 0.55f, -2.0f}, {0.65f, 0.55f, 0.65f}, true, {0.82f, 0.15f, 0.075f}, 0.55f, 0);
    objects.back().rotation.y = 24;
    add("05 / Teal block", {1.6f, 1.05f, -2.05f}, {0.55f, 1.05f, 0.55f}, true, {0.055f, 0.38f, 0.36f}, 0.35f, 0.1f);
    objects.back().rotation.y = -18;
    add("Floor", {0, -0.12f, 0}, {4.1f, 0.12f, 4.2f}, true, {0.64f, 0.66f, 0.69f}, 0.8f, 0);
    add("Back wall", {0, 2.5f, -3.3f}, {4.1f, 2.5f, 0.12f}, true, {0.65f, 0.68f, 0.72f}, 0.8f, 0);
    add("Red bounce wall", {-4, 2.5f, 0}, {0.12f, 2.5f, 3.3f}, true, {0.65f, 0.055f, 0.045f}, 0.8f, 0);
    add("Teal bounce wall", {4, 2.5f, 0}, {0.12f, 2.5f, 3.3f}, true, {0.045f, 0.45f, 0.38f}, 0.8f, 0);
    add("Area light / sphere", {0, 4.0f, -0.5f}, {0.65f, 0.65f, 0.65f}, false, {1, 1, 1}, 0.5f, 0, 0, 18);
}
void Scene::Initialize(ID3D12Device* device) {
    for (UINT i = 0; i < 2; ++i) {
        objectBuffers[i] = CreateBuffer(device, Capacity * sizeof(GPUObject), D3D12_HEAP_TYPE_UPLOAD);
        materialBuffers[i] = CreateBuffer(device, Capacity * sizeof(Material), D3D12_HEAP_TYPE_UPLOAD);
    }
}
void Scene::Upload(UINT frame) {
    if (objects.size() > Capacity || materials.size() > Capacity) throw std::runtime_error("Scene capacity exceeded");
    std::vector<GPUObject> data; data.reserve(objects.size());
    for (const auto& object : objects) {
        XMFLOAT4 q; XMStoreFloat4(&q, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(object.rotation.x), XMConvertToRadians(object.rotation.y), XMConvertToRadians(object.rotation.z)));
        const float radius = std::max({object.scale.x, object.scale.y, object.scale.z});
        data.push_back({{object.position.x, object.position.y, object.position.z, radius}, {object.scale.x, object.scale.y, object.scale.z, object.box ? 1.0f : 0.0f}, q, {object.material, 0, 0, 0}});
    }
    void* ptr = nullptr; D3D12_RANGE noRead{0, 0};
    Check(objectBuffers[frame]->Map(0, &noRead, &ptr), "Map scene objects"); memcpy(ptr, data.data(), data.size() * sizeof(GPUObject)); objectBuffers[frame]->Unmap(0, nullptr);
    Check(materialBuffers[frame]->Map(0, &noRead, &ptr), "Map scene materials"); memcpy(ptr, materials.data(), materials.size() * sizeof(Material)); materialBuffers[frame]->Unmap(0, nullptr);
}

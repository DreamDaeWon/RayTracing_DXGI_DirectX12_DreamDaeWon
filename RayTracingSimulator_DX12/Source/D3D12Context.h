#pragma once
#include "Common.h"

class D3D12Context {
public:
    static constexpr UINT FrameCount = 2;
    static constexpr UINT DescriptorCount = 256;
    void Initialize(HWND window, UINT width, UINT height, bool debug, bool warp = false);
    ~D3D12Context();
    void Resize(UINT width, UINT height);
    void BeginFrame();
    void EndFrame(bool vsync);
    void WaitIdle();
    void Transition(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
    UINT AllocateDescriptor();
    void FreeDescriptor(UINT index);
    D3D12_CPU_DESCRIPTOR_HANDLE Cpu(UINT index) const;
    D3D12_GPU_DESCRIPTOR_HANDLE Gpu(UINT index) const;
    D3D12_CPU_DESCRIPTOR_HANDLE CurrentRTV() const;
    void Capture(const std::filesystem::path& path);
    void FlushAndContinue();
    unsigned ValidateDebugMessages(std::ostream& out);
    ID3D12Device* Device() const { return device.Get(); }
    ID3D12CommandQueue* Queue() const { return queue.Get(); }
    ID3D12GraphicsCommandList* List() const { return list.Get(); }
    ID3D12DescriptorHeap* Heap() const { return srvHeap.Get(); }
    UINT FrameIndex() const { return frameIndex; }
    UINT Width() const { return width; }
    UINT Height() const { return height; }
    const std::string& AdapterName() const { return adapterName; }
    bool DebugActive() const { return debugActive; }
    double GpuMilliseconds() const { return gpuMilliseconds; }
private:
    void CreateBackBuffers();
    void WaitFence(UINT64 value);
    ComPtr<ID3D12Device> device;
    ComPtr<ID3D12CommandQueue> queue;
    ComPtr<IDXGISwapChain3> swapchain;
    ComPtr<ID3D12GraphicsCommandList> list;
    std::array<ComPtr<ID3D12CommandAllocator>, FrameCount> allocators;
    std::array<ComPtr<ID3D12Resource>, FrameCount> backBuffers;
    ComPtr<ID3D12DescriptorHeap> rtvHeap, srvHeap;
    ComPtr<ID3D12Fence> fence;
    ComPtr<ID3D12QueryHeap> timestamps;
    ComPtr<ID3D12Resource> timestampReadback;
    std::array<UINT64, FrameCount> frameFences{};
    std::array<bool, FrameCount> timestampValid{};
    std::array<bool, DescriptorCount> descriptorUsed{};
    UINT64 fenceValue = 0, timestampFrequency = 1;
    HANDLE fenceEvent = nullptr;
    UINT frameIndex = 0, width = 0, height = 0, rtvStride = 0, srvStride = 0;
    bool debugActive = false, tearingSupported = false;
    double gpuMilliseconds = 0;
    std::string adapterName;
};

#include "D3D12Context.h"
#include <d3d12sdklayers.h>

void D3D12Context::Initialize(HWND window, UINT w, UINT h, bool debug, bool warp) {
    width = w; height = h;
    ComPtr<ID3D12Debug> debugLayer;
    if (debug && SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer)))) {
        debugLayer->EnableDebugLayer(); debugActive = true;
        ComPtr<ID3D12Debug1> validation;
        if (SUCCEEDED(debugLayer.As(&validation))) validation->SetEnableGPUBasedValidation(TRUE);
    }
    ComPtr<IDXGIFactory6> factory;
    Check(CreateDXGIFactory2(debugActive ? DXGI_CREATE_FACTORY_DEBUG : 0, IID_PPV_ARGS(&factory)), "Create DXGI factory");
    BOOL allowTearing = FALSE;
    factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
    tearingSupported = allowTearing != FALSE;
    ComPtr<IDXGIAdapter1> adapter;
    if (!warp) {
        for (UINT i = 0; factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
            DXGI_ADAPTER_DESC1 desc{}; adapter->GetDesc1(&desc);
            if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) && SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)))) break;
            adapter.Reset();
        }
    }
    if (!device) {
        Check(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)), "Find WARP adapter");
        Check(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)), "Create D3D12 device");
    }
    DXGI_ADAPTER_DESC1 adapterDesc{}; adapter->GetDesc1(&adapterDesc);
    char name[512]{}; WideCharToMultiByte(CP_UTF8, 0, adapterDesc.Description, -1, name, sizeof(name), nullptr, nullptr); adapterName = name;
    D3D12_COMMAND_QUEUE_DESC queueDesc{}; queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    Check(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&queue)), "Create direct queue");
    DXGI_SWAP_CHAIN_DESC1 swapDesc{};
    swapDesc.Width = width; swapDesc.Height = height; swapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.SampleDesc.Count = 1; swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.BufferCount = FrameCount; swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapDesc.Flags = tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
    ComPtr<IDXGISwapChain1> initialSwap;
    Check(factory->CreateSwapChainForHwnd(queue.Get(), window, &swapDesc, nullptr, nullptr, &initialSwap), "Create swap chain");
    Check(initialSwap.As(&swapchain), "Query swap chain");
    Check(factory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER), "Configure window association");
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{}; heapDesc.NumDescriptors = FrameCount; heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    Check(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeap)), "Create RTV heap");
    heapDesc.NumDescriptors = DescriptorCount; heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    Check(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&srvHeap)), "Create shader heap");
    rtvStride = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    srvStride = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    for (auto& allocator : allocators) Check(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)), "Create allocator");
    Check(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocators[0].Get(), nullptr, IID_PPV_ARGS(&list)), "Create command list");
    Check(list->Close(), "Close initial list");
    Check(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "Create fence");
    fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!fenceEvent) throw std::runtime_error("Create fence event failed");
    D3D12_QUERY_HEAP_DESC queryDesc{}; queryDesc.Count = FrameCount * 2; queryDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
    Check(device->CreateQueryHeap(&queryDesc, IID_PPV_ARGS(&timestamps)), "Create timestamp heap");
    timestampReadback = CreateBuffer(device.Get(), FrameCount * 2 * sizeof(UINT64), D3D12_HEAP_TYPE_READBACK);
    Check(queue->GetTimestampFrequency(&timestampFrequency), "Get GPU clock frequency");
    CreateBackBuffers();
}
D3D12Context::~D3D12Context() {
    if (queue && fence && fenceEvent) { try { WaitIdle(); } catch (...) {} }
    if (fenceEvent) CloseHandle(fenceEvent);
}
void D3D12Context::CreateBackBuffers() {
    auto handle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < FrameCount; ++i) {
        Check(swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i])), "Get back buffer");
        device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, handle); handle.ptr += rtvStride;
    }
    frameIndex = swapchain->GetCurrentBackBufferIndex();
}
void D3D12Context::WaitFence(UINT64 value) {
    if (fence->GetCompletedValue() < value) {
        Check(fence->SetEventOnCompletion(value, fenceEvent), "Wait GPU fence");
        DWORD result = WaitForSingleObject(fenceEvent, 30000);
        if (result != WAIT_OBJECT_0) { Check(device->GetDeviceRemovedReason(), "GPU device removed"); throw std::runtime_error("GPU fence timed out after 30 seconds"); }
    }
    Check(device->GetDeviceRemovedReason(), "GPU device status");
}
void D3D12Context::WaitIdle() {
    Check(queue->Signal(fence.Get(), ++fenceValue), "Signal idle fence"); WaitFence(fenceValue);
}
void D3D12Context::Resize(UINT w, UINT h) {
    if (!w || !h || (width == w && height == h)) return;
    WaitIdle(); for (auto& buffer : backBuffers) buffer.Reset();
    Check(swapchain->ResizeBuffers(FrameCount, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0), "Resize swap chain");
    width = w; height = h; frameFences.fill(0); timestampValid.fill(false); CreateBackBuffers();
}
void D3D12Context::BeginFrame() {
    frameIndex = swapchain->GetCurrentBackBufferIndex(); WaitFence(frameFences[frameIndex]);
    if (timestampValid[frameIndex]) {
        UINT64* data = nullptr;
        D3D12_RANGE read{ frameIndex * 2 * sizeof(UINT64), (frameIndex * 2 + 2) * sizeof(UINT64) };
        Check(timestampReadback->Map(0, &read, reinterpret_cast<void**>(&data)), "Map timestamps");
        gpuMilliseconds = double(data[frameIndex * 2 + 1] - data[frameIndex * 2]) * 1000.0 / double(timestampFrequency);
        D3D12_RANGE noWrite{0, 0}; timestampReadback->Unmap(0, &noWrite);
    }
    Check(allocators[frameIndex]->Reset(), "Reset allocator");
    Check(list->Reset(allocators[frameIndex].Get(), nullptr), "Reset command list");
    ID3D12DescriptorHeap* heaps[] = {srvHeap.Get()}; list->SetDescriptorHeaps(1, heaps);
    list->EndQuery(timestamps.Get(), D3D12_QUERY_TYPE_TIMESTAMP, frameIndex * 2);
    Transition(backBuffers[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    auto rtv = CurrentRTV(); const float clear[] = {0.025f, 0.031f, 0.047f, 1};
    list->OMSetRenderTargets(1, &rtv, FALSE, nullptr); list->ClearRenderTargetView(rtv, clear, 0, nullptr);
}
void D3D12Context::EndFrame(bool vsync) {
    Transition(backBuffers[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    list->EndQuery(timestamps.Get(), D3D12_QUERY_TYPE_TIMESTAMP, frameIndex * 2 + 1);
    list->ResolveQueryData(timestamps.Get(), D3D12_QUERY_TYPE_TIMESTAMP, frameIndex * 2, 2, timestampReadback.Get(), frameIndex * 2 * sizeof(UINT64));
    Check(list->Close(), "Close frame list"); ID3D12CommandList* lists[] = {list.Get()}; queue->ExecuteCommandLists(1, lists);
    HRESULT present = swapchain->Present(vsync ? 1 : 0, !vsync && tearingSupported ? DXGI_PRESENT_ALLOW_TEARING : 0);
    Check(queue->Signal(fence.Get(), ++fenceValue), "Signal frame fence");
    frameFences[frameIndex] = fenceValue; timestampValid[frameIndex] = true;
    if (FAILED(present)) { Check(device->GetDeviceRemovedReason(), "Present / device removed"); Check(present, "Present"); }
}
void D3D12Context::Transition(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
    if (before == after) return;
    D3D12_RESOURCE_BARRIER barrier{}; barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition = {resource, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, before, after}; list->ResourceBarrier(1, &barrier);
}
UINT D3D12Context::AllocateDescriptor() {
    for (UINT i = 0; i < DescriptorCount; ++i) if (!descriptorUsed[i]) { descriptorUsed[i] = true; return i; }
    throw std::runtime_error("Shader descriptor heap exhausted");
}
void D3D12Context::FreeDescriptor(UINT index) { if (index < DescriptorCount) descriptorUsed[index] = false; }
D3D12_CPU_DESCRIPTOR_HANDLE D3D12Context::Cpu(UINT index) const { auto h = srvHeap->GetCPUDescriptorHandleForHeapStart(); h.ptr += SIZE_T(index) * srvStride; return h; }
D3D12_GPU_DESCRIPTOR_HANDLE D3D12Context::Gpu(UINT index) const { auto h = srvHeap->GetGPUDescriptorHandleForHeapStart(); h.ptr += UINT64(index) * srvStride; return h; }
D3D12_CPU_DESCRIPTOR_HANDLE D3D12Context::CurrentRTV() const { auto h = rtvHeap->GetCPUDescriptorHandleForHeapStart(); h.ptr += SIZE_T(frameIndex) * rtvStride; return h; }
unsigned D3D12Context::ValidateDebugMessages(std::ostream& out) {
    ComPtr<ID3D12InfoQueue> info; unsigned count = 0;
    if (SUCCEEDED(device.As(&info))) {
        for (UINT64 i = 0; i < info->GetNumStoredMessages(); ++i) {
            SIZE_T size = 0; info->GetMessage(i, nullptr, &size); std::vector<uint8_t> storage(size);
            auto* message = reinterpret_cast<D3D12_MESSAGE*>(storage.data());
            Check(info->GetMessage(i, message, &size), "Read validation message");
            if (message->Severity <= D3D12_MESSAGE_SEVERITY_WARNING) { out << message->pDescription << '\n'; ++count; }
        }
        info->ClearStoredMessages();
    }
    return count;
}
void D3D12Context::Capture(const std::filesystem::path& path) {
    // Called while the current back buffer is an RTV, before EndFrame.
    auto desc = backBuffers[frameIndex]->GetDesc(); D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout{}; UINT64 bytes = 0;
    device->GetCopyableFootprints(&desc, 0, 1, 0, &layout, nullptr, nullptr, &bytes);
    auto readback = CreateBuffer(device.Get(), bytes, D3D12_HEAP_TYPE_READBACK);
    Transition(backBuffers[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE);
    D3D12_TEXTURE_COPY_LOCATION dst{}; dst.pResource = readback.Get(); dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT; dst.PlacedFootprint = layout;
    D3D12_TEXTURE_COPY_LOCATION src{}; src.pResource = backBuffers[frameIndex].Get(); src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    list->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
    Transition(backBuffers[frameIndex].Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
    Check(list->Close(), "Close capture list"); ID3D12CommandList* lists[] = {list.Get()}; queue->ExecuteCommandLists(1, lists); WaitIdle();
    uint8_t* pixels = nullptr; D3D12_RANGE read{0, static_cast<SIZE_T>(bytes)};
    Check(readback->Map(0, &read, reinterpret_cast<void**>(&pixels)), "Map capture");
    std::ofstream file(path, std::ios::binary); file << "P6\n" << width << ' ' << height << "\n255\n";
    for (UINT y = 0; y < height; ++y) for (UINT x = 0; x < width; ++x) file.write(reinterpret_cast<char*>(pixels + layout.Offset + y * layout.Footprint.RowPitch + x * 4), 3);
    D3D12_RANGE noWrite{0, 0}; readback->Unmap(0, &noWrite);
    if (!file) throw std::runtime_error("Could not write capture file");
    Check(list->Reset(allocators[frameIndex].Get(), nullptr), "Continue after capture");
}
void D3D12Context::FlushAndContinue() {
    Check(list->Close(), "Close readback list"); ID3D12CommandList* lists[] = {list.Get()}; queue->ExecuteCommandLists(1, lists); WaitIdle();
    Check(list->Reset(allocators[frameIndex].Get(), nullptr), "Continue after readback");
    ID3D12DescriptorHeap* heaps[] = {srvHeap.Get()}; list->SetDescriptorHeaps(1, heaps);
    auto rtv = CurrentRTV(); list->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
}

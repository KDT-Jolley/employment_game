#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
#include <atlimage.h>

#include "Game.h"
// コンストラクタ
Game::Game(uint32_t width, uint32_t height)
//　メンバ変数の初期化
    :m_hInst(nullptr), m_hWnd(nullptr), m_Width(width), m_Height(height) {

}
//  デストラクタ
Game::~Game()
{
}

//　ゲームの初期化処理
bool Game::Initialize(HINSTANCE hInstance)
{
    if (!CreateGameWindow(hInstance))
    {
        return false;
    }

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;

    std::string err;
    std::string warn;

    bool result = loader.LoadASCIIFromFile(
        &model,
        &err,
        &warn,
        "Gltf/gltf/soccer_ ball.gltf"
    );

    if (!InitD3D12())
    {
        return false;
    }

    return true;
}

//　ゲーム画面の作成
bool Game::CreateGameWindow(HINSTANCE hInstance)
{
    WNDCLASS wc{};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"DX12WindowClass";
    wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    RegisterClass(&wc);

    m_hWnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        L"DirectX12",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        960,
        540,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (m_hWnd == nullptr)
    {
        return false;
    }

    ShowWindow(m_hWnd, SW_SHOW);

    return true;
}

//　ゲームの実行処理
void Game::Run()
{
    MSG msg{};

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Render();
    }
}
bool Game::InitD3D12()
{
    UINT dxgiFactoryFlags = 0;

    CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));

    D3D12CreateDevice(
        nullptr,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&device)
    );

    D3D12_COMMAND_QUEUE_DESC queueDesc{};

    device->CreateCommandQueue(
        &queueDesc,
        IID_PPV_ARGS(&commandQueue)
    );
    Vertex vertices[] =
    {
        {{ 0.0f,  0.25f, 0.0f }, {1,0,0,1}},
        {{ 0.25f,-0.25f, 0.0f }, {0,1,0,1}},
        {{-0.25f,-0.25f, 0.0f }, {0,0,1,1}},
    };
    const UINT vertexBufferSize = sizeof(vertices);

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = vertexBufferSize;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertexBuffer)
    );

    void* mappedData = nullptr;

    vertexBuffer->Map(0, nullptr, &mappedData);

    memcpy(mappedData, vertices, sizeof(vertices));

    vertexBuffer->Unmap(0, nullptr);

    vbView.BufferLocation =
        vertexBuffer->GetGPUVirtualAddress();

    vbView.SizeInBytes = vertexBufferSize;

    vbView.StrideInBytes = sizeof(Vertex);

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

    swapChainDesc.BufferCount = 2;
    swapChainDesc.Width = m_Width;
    swapChainDesc.Height = m_Height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> tempSwapChain;

    factory->CreateSwapChainForHwnd(
        commandQueue.Get(),
        m_hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &tempSwapChain
    );

    tempSwapChain.As(&swapChain);

    frameIndex = swapChain->GetCurrentBackBufferIndex();

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};

    rtvHeapDesc.NumDescriptors = 2;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    device->CreateDescriptorHeap(
        &rtvHeapDesc,
        IID_PPV_ARGS(&rtvHeap)
    );

    rtvDescriptorSize =
        device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV
        );

    D3D12_CPU_DESCRIPTOR_HANDLE handle =
        rtvHeap->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < 2; i++)
    {
        swapChain->GetBuffer(
            i,
            IID_PPV_ARGS(&backBuffers[i])
        );

        device->CreateRenderTargetView(
            backBuffers[i].Get(),
            nullptr,
            handle
        );

        handle.ptr += rtvDescriptorSize;
    }

    device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&commandAllocator)
    );

    device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        commandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(&commandList)
    );

    return true;
}
void Game::Render()
{
    commandAllocator->Reset();
    commandList->Reset(
        commandAllocator.Get(),
        pipelineState.Get()
    );

    D3D12_RESOURCE_BARRIER barrier{};

    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = backBuffers[frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource =
        D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    commandList->ResourceBarrier(1, &barrier);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
        rtvHeap->GetCPUDescriptorHandleForHeapStart();

    rtvHandle.ptr += frameIndex * rtvDescriptorSize;

    commandList->OMSetRenderTargets(
        1,
        &rtvHandle,
        FALSE,
        nullptr
    );

    FLOAT clearColor[] = { 0.0f, 0.2f, 0.8f, 1.0f };

    commandList->ClearRenderTargetView(
        rtvHandle,
        clearColor,
        0,
        nullptr
    );
    commandList->SetGraphicsRootSignature(rootSignature.Get());

    commandList->IASetPrimitiveTopology(
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    commandList->IASetVertexBuffers(0, 1, &vbView);

    commandList->DrawInstanced(3, 1, 0, 0);


    barrier.Transition.StateBefore =
        D3D12_RESOURCE_STATE_RENDER_TARGET;

    barrier.Transition.StateAfter =
        D3D12_RESOURCE_STATE_PRESENT;

    commandList->ResourceBarrier(1, &barrier);

    commandList->Close();

    ID3D12CommandList* lists[] =
    {
        commandList.Get()
    };

    commandQueue->ExecuteCommandLists(1, lists);

    swapChain->Present(1, 0);

    frameIndex = swapChain->GetCurrentBackBufferIndex();
}
//　ウィンドウプロシージャ
LRESULT CALLBACK Game::WindowProc(
    HWND hwnd,
    UINT msg,
    WPARAM wparam,
    LPARAM lparam
)
{
    switch (msg)
    {
    case WM_DESTROY:

        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}
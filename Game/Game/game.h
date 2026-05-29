#pragma once


#include <Windows.h>
#include <cstdint>
#include <d3d12.h>    // ★追加
#include <dxgi1_4.h>  // ★追加
#include <DirectXMath.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

class Game
{
public:

    Game(uint32_t width, uint32_t height);
    ~Game();

    bool Initialize(HINSTANCE hInstance);

    void Run();

    ComPtr<ID3D12Device> device;
    ComPtr<IDXGISwapChain3> swapChain;
    ComPtr<ID3D12CommandQueue> commandQueue;
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> commandList;
    ComPtr<ID3D12DescriptorHeap> rtvHeap;
    ComPtr<ID3D12Resource> backBuffers[2];
    bool InitD3D12();
    void Render();
    UINT rtvDescriptorSize = 0;
    UINT frameIndex = 0;
    ComPtr<IDXGIFactory4> factory;
    ComPtr<ID3D12PipelineState> pipelineState;
    ComPtr<ID3D12RootSignature> rootSignature;
    ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vbView{};
private:


    HINSTANCE   m_hInst;        // インスタンスハンドルです.
    HWND        m_hWnd;         // ウィンドウハンドルです.
    uint32_t    m_Width;        // ウィンドウの横幅です.
    uint32_t    m_Height;       // ウィンドウの縦幅です.
    static LRESULT CALLBACK WindowProc(
        HWND hwnd,
        UINT msg,
        WPARAM wparam,
        LPARAM lparam
    );

    bool CreateGameWindow(HINSTANCE hInstance);
    struct Vertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT4 color;
    };
};
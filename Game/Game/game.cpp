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

    hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        L"DirectX12",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1280,
        720,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (hwnd == nullptr)
    {
        return false;
    }

    ShowWindow(hwnd, SW_SHOW);

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

        HBITMAP hBitmap = (HBITMAP)LoadImage(
            nullptr,
            L"Gltf/gltf/01.jpg",
            IMAGE_BITMAP,
            100,
            100,
            LR_LOADFROMFILE
        );
       
    }
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
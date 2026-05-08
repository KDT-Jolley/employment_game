#include "Game.h"

Game::Game(uint32_t width, uint32_t height)
    :m_hInst(nullptr), m_hWnd(nullptr), m_Width(width), m_Height(height) {

}
Game::~Game()
{
}

bool Game::Initialize(HINSTANCE hInstance)
{
    return CreateGameWindow(hInstance);
}

bool Game::CreateGameWindow(HINSTANCE hInstance)
{
    WNDCLASS wc{};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"DX12WindowClass";

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

        // Ç±Ç±Ç…ï`âÊèàóù
    }
}

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
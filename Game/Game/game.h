#pragma once

#include <Windows.h>
#include <cstdint>
#include <d3d12.h>    // ★追加
#include <dxgi1_4.h>  // ★追加
#include <DirectXMath.h>
#include <wrl/client.h>

class Game
{
public:

    Game(uint32_t width, uint32_t height);
    ~Game();

    bool Initialize(HINSTANCE hInstance);

    void Run();

private:

    HWND hwnd = nullptr;
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
};
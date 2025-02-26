

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <string>
// Data
static ID3D11Device*            g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

  
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window being minimized or screen locked
        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();











































        {
            ImGui::Begin("Dagger UI");

            // Constants for better readability
            const int NUM_DAGGERS = 6;
            const int NUM_BLITZES = 3;
            const int CHANNELS_PER_BLITZ = 8;
            const int CHANNELS_PER_DAGGER = 4;
            const int TOTAL_CHANNELS = 24;

            static bool isDaggerPressed[NUM_DAGGERS] = { false };
            static bool isBlitzPressed[NUM_BLITZES] = { false };
            static bool buttonColors[TOTAL_CHANNELS] = { false };

            // Base functions
            const char* base_functions[] = {
                "Open Load", "Short Vbat", "Short GND", "PWM Generation", "Low Current Generation",
                "Voltage Measurement", "Current Measurement (Low Current)", "PWM Measurement"
            };

            // Display base functions
            ImGui::Text("Base Functions:");
            for (const auto& func : base_functions) {
                if (ImGui::Button(func)) {
                    // Implement function behavior here
                }
            }

            // Blitz buttons
            for (int blitzIndex = 0; blitzIndex < NUM_BLITZES; blitzIndex++) {
                if (ImGui::Button(("BLITZ" + std::to_string(blitzIndex + 1)).c_str(), ImVec2(100, 30))) {
                    isBlitzPressed[blitzIndex] = !isBlitzPressed[blitzIndex]; // Toggle state
                }

                // Change color if Blitz is pressed
                if (isBlitzPressed[blitzIndex]) {
                    for (int i = blitzIndex * CHANNELS_PER_BLITZ; i < (blitzIndex + 1) * CHANNELS_PER_BLITZ; i++) {
                        buttonColors[i] = true;
                    }
                }

                ImGui::SameLine();
            }

            ImGui::NewLine();

            // Display Blitz-controlled functions and channels
            for (int blitzIndex = 0; blitzIndex < NUM_BLITZES; blitzIndex++) {
                if (isBlitzPressed[blitzIndex]) {
                    const char* blitz_functions[] = {
                        "Current Measurement (High Current)",
                        "Quiescent Current Measurement",
                        "Short to GND (High Current)",
                        "Load Simulation (High Current)"
                    };

                    ImGui::Text("Blitz %d Controls Functions:", blitzIndex + 1);
                    for (const auto& func : blitz_functions) {
                        if (ImGui::Button(func)) {
                            // Implement function behavior here
                        }
                    }

                    ImGui::Text("Channels controlled by Blitz %d:", blitzIndex + 1);
                    for (int i = blitzIndex * CHANNELS_PER_BLITZ; i < (blitzIndex + 1) * CHANNELS_PER_BLITZ; i++) {
                        ImGui::Text("- Channel %d", i + 1);
                    }
                    ImGui::NewLine();
                }
            }

            // Dagger buttons
            for (int daggerIndex = 0; daggerIndex < NUM_DAGGERS; daggerIndex++) {
                if (ImGui::Button(("DAGGER" + std::to_string(daggerIndex + 1)).c_str(), ImVec2(100, 30))) {
                    isDaggerPressed[daggerIndex] = !isDaggerPressed[daggerIndex]; // Toggle state
                }

                // Change color if Dagger is pressed
                if (isDaggerPressed[daggerIndex]) {
                    for (int i = daggerIndex * CHANNELS_PER_DAGGER; i < (daggerIndex + 1) * CHANNELS_PER_DAGGER; i++) {
                        buttonColors[i] = true;
                    }
                }

                ImGui::SameLine();
            }

            ImGui::NewLine();

            // Display the 24 buttons at the bottom
            for (int i = 0; i < TOTAL_CHANNELS; i++) {
                if (buttonColors[i]) {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green color
                }

                ImGui::Button(std::to_string(i + 1).c_str(), ImVec2(50, 30));

                if (buttonColors[i]) {
                    ImGui::PopStyleColor();
                    buttonColors[i] = false; // Reset after displaying
                }

                if (i % 8 < 7) {
                    ImGui::SameLine();
                }
            }

            // Load Simulation (Resistor) function controlled by active daggers
            bool anyDaggerActive = false;
            for (int i = 0; i < NUM_DAGGERS; i++) {
                if (isDaggerPressed[i]) {
                    anyDaggerActive = true;
                    break;
                }
            }

            if (anyDaggerActive) {
                ImGui::NewLine();
                if (ImGui::Button("Load Simulation (Resistor)")) {
                    // Implement the behavior for Load Simulation (Resistor) here
                }

                // Display channels controlled by active daggers
                ImGui::Text("Channels Controlled by Active Daggers:");
                for (int daggerIndex = 0; daggerIndex < NUM_DAGGERS; daggerIndex++) {
                    if (isDaggerPressed[daggerIndex]) {
                        ImGui::Text("- DAGGER %d controls channels %d to %d", daggerIndex + 1, daggerIndex * CHANNELS_PER_DAGGER + 1, (daggerIndex + 1) * CHANNELS_PER_DAGGER);
                    }
                }
            }

            ImGui::End();
        }


































































        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present
        HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
        //HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

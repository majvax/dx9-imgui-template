#include "overlay.h"
#include "byte.h"
#include "menu.h"
#include "fonts.h"

LPDIRECT3DDEVICE9 Overlay::pD3DDevice = nullptr;
LPDIRECT3D9 Overlay::pD3D = nullptr;
D3DPRESENT_PARAMETERS Overlay::D3Dpp = {};
UINT Overlay::g_ResizeHeight = 0;
UINT Overlay::g_ResizeWidth = 0;
bool Overlay::bInit = false;
bool Overlay::bDone = false;
HWND Overlay::hTargetWindow = nullptr;
BOOL Overlay::bTargetSet = FALSE;
DWORD Overlay::dTargetPID = 0;


/**
    @brief : Function that create a D3D9 device.
    @param  hWnd : HWND of the created window.
    @retval : true if the function succeed else false.
**/
bool Overlay::CreateDeviceD3D(const HWND hWnd)
{
    if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&D3Dpp, sizeof(D3Dpp));
    D3Dpp.Windowed = TRUE;
    D3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    D3Dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    D3Dpp.EnableAutoDepthStencil = TRUE;
    D3Dpp.AutoDepthStencilFormat = D3DFMT_D16;
    D3Dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &D3Dpp, &pD3DDevice) < 0)
        return false;

    return true;
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

/**
    @brief : Window message handler (https://learn.microsoft.com/en-us/windows/win32/api/winuser/nc-winuser-wndproc).
**/
LRESULT WINAPI Overlay::WndProc(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam)
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
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;

    default:
        break;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

/**
    @brief : Function that create the overlay window and more.
**/
void Overlay::Render()
{
    ImGui_ImplWin32_EnableDpiAwareness();

    // Get the main window of the process when overlay as DLL

    if (hTargetWindow == nullptr)
        throw std::exception("Target window is not set");


    WNDCLASSEX wc;

    wc.cbClsExtra = NULL;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = NULL;
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = _T("Overlay");
    wc.lpszMenuName = nullptr;
    wc.style = CS_VREDRAW | CS_HREDRAW;

    ::RegisterClassEx(&wc);
    const HWND hwnd = ::CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, wc.lpszClassName, _T("Overlay"), WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, wc.hInstance, nullptr);

    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    const MARGINS margin = { -1, 0, 0, 0 };
    DwmExtendFrameIntoClientArea(hwnd, &margin);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // Scale the font size depending of the screen size.
    const HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info = {};
    info.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(monitor, &info);
    const int monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;

    /*if (monitor_height > 1080)
    {
        const float fScale = 2.0f;
        ImFontConfig cfg;
        cfg.SizePixels = 13 * fScale;
        ImGui::GetIO().Fonts->AddFontDefault(&cfg);
    }*/

    ImGui::GetIO().IniFilename = nullptr;

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(pD3DDevice);


    // Load Fonts
    ImFontConfig font_config;
    font_config.PixelSnapH = false;
    font_config.OversampleH = 5;
    font_config.OversampleV = 5;
    font_config.RasterizerMultiply = 1.2f;

    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
        0xE000, 0xE226, // icons
        0,
    };

    font_config.GlyphRanges = ranges;
    font_config.FontDataOwnedByAtlas = false;

    fonts::medium = io.Fonts->AddFontFromMemoryTTF(InterMedium, sizeof(InterMedium), 15.0f, &font_config, ranges);
    fonts::semibold = io.Fonts->AddFontFromMemoryTTF(InterSemiBold, sizeof(InterSemiBold), 17.0f, &font_config, ranges);
    fonts::logo = io.Fonts->AddFontFromMemoryTTF(logo, sizeof(logo), 17.0f, &font_config, ranges);
    // load your custom font here



    const ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    bInit = true;

    bDone = false;

    while (!bDone)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                bDone = true;
        }

        if (GetAsyncKeyState(VK_END) & 1)
            bDone = true;

        // Check if the targeted window is still up.
        if (!IsWindowAlive() && bTargetSet)
            bDone = true;

        if (bDone)
            break;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            D3Dpp.BackBufferWidth = g_ResizeWidth;
            D3Dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }


        if (hTargetWindow != nullptr && bTargetSet)
            MoveWindow(hwnd);

        // Clear overlay when the targeted window is not focus
        if (!IsWindowFocus(hwnd) && bTargetSet)
        {
            pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
            pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
            pD3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
            const D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
            pD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);

            if (pD3DDevice->BeginScene() >= 0)
                pD3DDevice->EndScene();

            const HRESULT result = pD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);

            // Handle loss of D3D9 device
            if (result == D3DERR_DEVICELOST && pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
                ResetDevice();

            continue;
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            Menu::Draw();
        }
        ImGui::EndFrame();

        // Overlay handle inputs when menu is showed.
        if (Menu::isActive())
            SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
        else
            SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);

        ImGui::EndFrame();

        pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        pD3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        const D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        pD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);


        // Draw figure with DirectX
        if (IsWindowTargeted())
            Menu::DXDraw(pD3DDevice);

        if (pD3DDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            pD3DDevice->EndScene();
        }

        const HRESULT result = pD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    bInit = false;


    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);
    return;
}

/**
    @brief : Reset the current D3D9 device.
**/
void Overlay::ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    const HRESULT hr = pD3DDevice->Reset(&D3Dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

/**
    @brief : Release the D3D9 device and object.
**/
void Overlay::CleanupDeviceD3D()
{
    if (pD3DDevice) { pD3DDevice->Release(); pD3DDevice = nullptr; }
    if (pD3D) { pD3D->Release(); pD3D = nullptr; }
}


/**
    @brief : Function that retrieve the main window of the process.
             This function is only called when the overlay is build as DLL.
**/
void Overlay::GetWindow()
{
    EnumWindows(EnumWind, NULL);
}

/**
    @brief : Callback function that retrive the main window of the process.
             This function is only called when the overlay is build as DLL.
             (https://learn.microsoft.com/fr-fr/windows/win32/api/winuser/nf-winuser-enumwindows)

**/
BOOL CALLBACK Overlay::EnumWind(const HWND hWindow, const LPARAM lPrams)
{
    DWORD procID;
    GetWindowThreadProcessId(hWindow, &procID);
    if (GetCurrentProcessId() != procID)
        return TRUE;

    if (!IsWindowValid(hWindow))
        return TRUE;

    SetTargetWindow(hWindow);
    return FALSE;
}

/**
    @brief : Function that move the overlay on top of the targeted window.
    @param hCurrentProcessWindow : Window of the overlay.
**/
void Overlay::MoveWindow(const HWND hCurrentProcessWindow)
{
    RECT rect;
    if (hTargetWindow == nullptr)
        return;

    GetWindowRect(hTargetWindow, &rect);

    int lWindowWidth = rect.right - rect.left;
    int lWindowHeight = rect.bottom - rect.top;

    lWindowWidth -= 5;
    lWindowHeight -= 29;

    SetWindowPos(hCurrentProcessWindow, nullptr, rect.left, rect.top, lWindowWidth, lWindowHeight, SWP_SHOWWINDOW);
}

/**
    @brief : Function that check if the overlay window or the targeted window is focus.
    @param  hCurrentProcessWindow : Window of the overlay.
    @retval : TRUE if one of the window is focus else FALSE.
**/
BOOL Overlay::IsWindowFocus(const HWND hCurrentProcessWindow)
{
    char lpCurrentWindowUsedClass[125];
    char lpCurrentWindowClass[125];
    char lpOverlayWindowClass[125];

    const HWND hCurrentWindowUsed = GetForegroundWindow();
    if (GetClassNameA(hCurrentWindowUsed, lpCurrentWindowUsedClass, sizeof(lpCurrentWindowUsedClass)) == 0)
        return FALSE;

    if (GetClassNameA(hTargetWindow, lpCurrentWindowClass, sizeof(lpCurrentWindowClass)) == 0)
        return FALSE;

    if (GetClassNameA(hCurrentProcessWindow, lpOverlayWindowClass, sizeof(lpOverlayWindowClass)) == 0)
        return FALSE;

    if (strcmp(lpCurrentWindowUsedClass, lpCurrentWindowClass) != 0 && strcmp(lpCurrentWindowUsedClass, lpOverlayWindowClass) != 0)
    {
        SetWindowLong(hCurrentProcessWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
        return FALSE;
    }

    return TRUE;
}

/**
    @brief : function that check if a target window is set.
    @retval : TRUE if a target window has been setted else FALSE.
**/
BOOL Overlay::IsWindowTargeted()
{
    return bTargetSet;
}

/**
    @brief : Function that clear the current window list and enumerate all windows.
    @param vWindowList : pointer of the WindowItem vector.
**/
void Overlay::GetAllWindow(std::vector<WindowItem>* vWindowList)
{
    vWindowList->clear();
    EnumWindows(EnumAllWind, (LPARAM)vWindowList);
}

/**
    @brief : Callback function that retrive all the valid window and get processname, pid and window title.
             (https://learn.microsoft.com/fr-fr/windows/win32/api/winuser/nf-winuser-enumwindows)
**/
BOOL CALLBACK Overlay::EnumAllWind(const HWND hWindow, const LPARAM lPrams)
{
    if (!IsWindowValid(hWindow))
        return TRUE;

    WindowItem CurrentWindowItem = { hWindow, 0, 0 };
    DWORD procID;

    GetWindowTextA(hWindow, CurrentWindowItem.CurrentWindowTitle, sizeof(CurrentWindowItem.CurrentWindowTitle));

    if (strlen(CurrentWindowItem.CurrentWindowTitle) == 0)
        return TRUE;

    GetWindowThreadProcessId(hWindow, &procID);
    GetProcessName(CurrentWindowItem.CurrentProcessName, procID);

    const auto vWindowList = (std::vector<WindowItem>*)lPrams;

    vWindowList->push_back(CurrentWindowItem);

    return TRUE;
}


/**
    @brief : Function that retrieve the process name from the PID.
    @param lpProcessName : pointer to the string that store the process name.
    @param dPID : PID of the process.
**/
void Overlay::GetProcessName(const LPSTR lpProcessName, const DWORD dPID)
{
    char lpCurrentProcessName[125];

    PROCESSENTRY32 ProcList{};
    ProcList.dwSize = sizeof(ProcList);

    const HANDLE hProcList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcList == INVALID_HANDLE_VALUE)
        return;

    if (!Process32First(hProcList, &ProcList))
        return;


    if (ProcList.th32ProcessID == dPID)
    {
        wcstombs_s(nullptr, lpCurrentProcessName, ProcList.szExeFile, sizeof(lpCurrentProcessName));
        strncpy_s(lpProcessName, sizeof(lpCurrentProcessName), lpCurrentProcessName, sizeof(lpCurrentProcessName));
        return;
    }

    while (Process32Next(hProcList, &ProcList))
    {
        if (ProcList.th32ProcessID == dPID)
        {
            wcstombs_s(nullptr, lpCurrentProcessName, ProcList.szExeFile, sizeof(lpCurrentProcessName));
            strncpy_s(lpProcessName, sizeof(lpCurrentProcessName), lpCurrentProcessName, sizeof(lpCurrentProcessName));
            return;
        }
    }
}

/**
    @brief : Function that check if a window is valid.
    @param  hCurrentWindow : window to be tested.
    @retval : TRUE if the window is valid else FALSE.
**/
BOOL Overlay::IsWindowValid(const HWND hCurrentWindow)
{
    DWORD styles, ex_styles;
    RECT rect;

    if (!IsWindowVisible(hCurrentWindow) ||
        (IsIconic(hCurrentWindow) || IsWindowCloaked(hCurrentWindow)))
        return FALSE;

    GetClientRect(hCurrentWindow, &rect);
    styles = (DWORD)GetWindowLongPtr(hCurrentWindow, GWL_STYLE);
    ex_styles = (DWORD)GetWindowLongPtr(hCurrentWindow, GWL_EXSTYLE);

    if (ex_styles & WS_EX_TOOLWINDOW)
        return FALSE;
    if (styles & WS_CHILD)
        return FALSE;
    if (rect.bottom == 0 || rect.right == 0)
        return FALSE;

    return TRUE;
}

/**
    @brief : Function that check if a window is cloacked.
    @param  hCurrentWindow : window to be tested.
    @retval : TRUE if the window is cloacked else FALSE.
**/
BOOL Overlay::IsWindowCloaked(const HWND hCurrentWindow)
{
    DWORD cloaked;
    const HRESULT hr = DwmGetWindowAttribute(hCurrentWindow, DWMWA_CLOAKED, &cloaked,
        sizeof(cloaked));
    return SUCCEEDED(hr) && cloaked;
}

/**
    @brief : Setter function used to define the target window from the window picker.
             This is used only when the overlay is build as an EXE.
    @param hWindow : target window.
**/
void Overlay::SetTargetWindow(const HWND hWindow)
{
    hTargetWindow = hWindow;
    SetForegroundWindow(hTargetWindow);
    GetWindowThreadProcessId(hTargetWindow, &dTargetPID);
    bTargetSet = TRUE;
}

/**
    @brief : Function that look if the targeted window has been closed.
    @retval : TRUE if the function is still up else FALSE.
**/
BOOL Overlay::IsWindowAlive()
{
    DWORD dCurrentPID;

    if (hTargetWindow == nullptr)
        return FALSE;

    if (!IsWindow(hTargetWindow))
        return FALSE;

    GetWindowThreadProcessId(hTargetWindow, &dCurrentPID);

    if (dCurrentPID != dTargetPID)
        return FALSE;

    return TRUE;
}


/**
	@brief : Function that retrieve the RECT of the targeted window.
    @retval : RECT of the targeted window.
**/
RECT Overlay::GetTargetWindowRect()
{
	RECT rect;
	GetWindowRect(hTargetWindow, &rect);
	return rect;
}
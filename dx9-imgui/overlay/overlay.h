#ifndef OVERLAY_H
#define OVERLAY_H

#include "../imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

#include <d3d9.h>
#include <d3dx9.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <dwmapi.h>


#include <tchar.h>
#include <vector>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



class Overlay {
private:
	static LPDIRECT3D9 pD3D;
	static LPDIRECT3DDEVICE9 pD3DDevice;
	static D3DPRESENT_PARAMETERS D3Dpp;
	static bool bInit;
	static UINT g_ResizeWidth, g_ResizeHeight;
	static HWND hTargetWindow;
	static BOOL bTargetSet;
	static DWORD dTargetPID;

	static bool CreateDeviceD3D(HWND hWnd);
	static void CleanupDeviceD3D();
	static void ResetDevice();
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static BOOL EnumWind(HWND hWindow, LPARAM lPrams);
	static void GetWindow();
	static void MoveWindow(HWND hCurrentProcessWindow);
	static BOOL IsWindowFocus(HWND hCurrentProcessWindow);
	static BOOL EnumAllWind(HWND hWindow, LPARAM lPrams);
	static void GetProcessName(LPSTR lpProcessName, DWORD dPID);
	static BOOL IsWindowCloaked(HWND hCurrentWindow);
	static BOOL IsWindowAlive();
public:
	struct WindowItem
	{
		HWND CurrentWindow;
		char CurrentWindowTitle[125];
		char CurrentProcessName[125];
	};
	static bool bDone;
	static void Render();
	static BOOL IsWindowTargeted();
	static void GetAllWindow(std::vector<WindowItem>* vWindowList);
	static void SetTargetWindow(HWND hWindow);
	static BOOL IsWindowValid(HWND hCurrentWindow);
	static HWND GetTargetWindow() { return hTargetWindow; }
	static RECT GetTargetWindowRect();
};


#endif // OVERLAY_H

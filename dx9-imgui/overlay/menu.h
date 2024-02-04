#ifndef MENU_H
#define MENU_H

#include "overlay.h"


class Menu
{
private:
	static LPCSTR lpWindowName;
	static ImVec2 vWindowSize;
	static ImGuiWindowFlags WindowFlags;
	static bool bDraw;
	static Overlay::WindowItem lpSelectedWindow;
	static LPDIRECT3DDEVICE9 pD3DDevice;

public:
	static bool isActive();
	static void Draw();
	static void DXDraw(LPDIRECT3DDEVICE9 pCurrentD3DDevice);
};

#endif
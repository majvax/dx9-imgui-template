#ifndef DRAWING_H
#define DRAWING_H
#define D3DFVF_TL (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define RGB D3DCOLOR_XRGB
#include "overlay.h"

struct D3DTLVERTEX
{
	float fX;
	float fY;
	float fZ;
	float fRHW;
	D3DCOLOR Color;
	float fU;
	float fV;
};

class Drawing {
private:
	static LPDIRECT3DDEVICE9 pD3DDevice;
	static D3DTLVERTEX CreateD3DTLVERTEX(float X, float Y, float Z, float RHW, D3DCOLOR color, float U, float V);
public:
	static void SetDevice(LPDIRECT3DDEVICE9 pDevice);
	static void DrawFilledRectangle(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b);


	static void DrawBox(int x, int y, int width, int height, D3DCOLOR color);
	static void DrawFilledBox(int x, int y, int w, int h, D3DCOLOR one, D3DCOLOR two);
	static void DrawLine(float x1, float y1, float x2, float y2, D3DCOLOR color);
	static void DrawCircle(int x, int y, int radius, D3DCOLOR color);
	static void DrawFilledCircle(int x, int y, int radius, D3DCOLOR color);
};

#endif
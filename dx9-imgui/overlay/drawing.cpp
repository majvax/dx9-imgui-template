#include "drawing.h"



LPDIRECT3DDEVICE9 Drawing::pD3DDevice = nullptr;


D3DTLVERTEX Drawing::CreateD3DTLVERTEX(float X, float Y, float Z, float RHW, D3DCOLOR color, float U, float V)
{
	D3DTLVERTEX v =
	{
		X,
		Y,
		Z,
		RHW,
		color,
		U,
		V
	};

	return v;
}


void Drawing::SetDevice(LPDIRECT3DDEVICE9 pDevice)
{
	pD3DDevice = pDevice;
}


void Drawing::DrawFilledRectangle(const int x, const int y, const int w, const int h, const unsigned char r, const unsigned char g, const unsigned char b)
{
	const D3DCOLOR rectColor = D3DCOLOR_XRGB(r, g, b);	//No point in using alpha because clear & alpha dont work!
	const D3DRECT BarRect = { x, y, x + w, y + h };

	pD3DDevice->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, rectColor, 0, 0);
}


void Drawing::DrawBox(int x, int y, int width, int height, D3DCOLOR color)
{
	D3DTLVERTEX Line[5];

	Line[0] = CreateD3DTLVERTEX(x, y, 0.0f, 1.0f, color, 0.0f, 0.0f);
	Line[1] = CreateD3DTLVERTEX(x + width, y, 0.0f, 1.0f, color, 0.0f, 0.0f);
	Line[2] = CreateD3DTLVERTEX(x + width, y + height, 0.0f, 1.0f, color, 0.0f, 0.0f);
	Line[3] = CreateD3DTLVERTEX(x, y + height, 0.0f, 1.0f, color, 0.0f, 0.0f);
	Line[4] = CreateD3DTLVERTEX(x, y, 0.0f, 1.0f, color, 0.0f, 0.0f);

	pD3DDevice->SetFVF(D3DFVF_TL);
	pD3DDevice->SetTexture(0, NULL);
	pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &Line[0], sizeof(Line[0]));
}

void Drawing::DrawFilledBox(int x, int y, int w, int h, D3DCOLOR one, D3DCOLOR two)
{
	D3DTLVERTEX vertices[4];
	vertices[0] = CreateD3DTLVERTEX(x, y, 0.0f, 1.0f, one, 0.0f, 0.0f);
	vertices[1] = CreateD3DTLVERTEX(x + w, y, 0.0f, 1.0f, one, 0.0f, 0.0f);
	vertices[2] = CreateD3DTLVERTEX(x, y + h, 0.0f, 1.0f, two, 0.0f, 0.0f);
	vertices[3] = CreateD3DTLVERTEX(x + w, y + h, 0.0f, 1.0f, two, 0.0f, 0.0f);

	pD3DDevice->SetTexture(0, NULL);
	pD3DDevice->SetPixelShader(0);
	pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(D3DTLVERTEX));
}


void Drawing::DrawLine(float x1, float y1, float x2, float y2, D3DCOLOR color)
{
	D3DTLVERTEX Line[2];

	Line[0] = CreateD3DTLVERTEX(x1, y1, 0.0f, 0.0f, color, 0.0f, 0.0f);
	Line[1] = CreateD3DTLVERTEX(x2, y2, 0.0f, 0.0f, color, 0.0f, 0.0f);

	pD3DDevice->SetFVF(D3DFVF_TL);
	pD3DDevice->SetTexture(0, NULL);
	pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, &Line[0], sizeof(Line[0]));
}

void Drawing::DrawCircle(const int x, const int y, const int radius, D3DCOLOR color)
{

	const int NUMPOINTS = 200;
	D3DTLVERTEX Circle[NUMPOINTS + 1];
	int i;
	float X;
	float Y;
	float Theta;
	float WedgeAngle = (float)((2 * D3DX_PI) / NUMPOINTS);

	for (i = 0; i <= NUMPOINTS; i++)
	{
		Theta = i * WedgeAngle;

		//Compute X and Y locations
		X = (float)(x + radius * cos(Theta));
		Y = (float)(y - radius * sin(Theta));

		Circle[i] = CreateD3DTLVERTEX(X, Y, 0.0f, 1.0f, color, 0.0f, 0.0f);
	}

	//Now draw the circle
	pD3DDevice->SetFVF(D3DFVF_TL);
	pD3DDevice->SetTexture(0, NULL);
	pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, NUMPOINTS, &Circle[0], sizeof(Circle[0]));
}


void Drawing::DrawFilledCircle(int xPos, int yPos, int Radius, D3DCOLOR color)
{
	D3DTLVERTEX Vtex[400];

	float x1 = xPos;
	float y1 = yPos;

	for (int i = 0; i <= 363; i += 3)
	{
		float angle = (i / 57.3f);
		float x2 = xPos + (Radius * sin(angle));
		float y2 = yPos + (Radius * cos(angle));
		Vtex[i] = CreateD3DTLVERTEX(xPos, yPos, 0, 1, color, 0, 0);
		Vtex[i + 1] = CreateD3DTLVERTEX(x1, y1, 0, 1, color, 0, 0);
		Vtex[i + 2] = CreateD3DTLVERTEX(x2, y2, 0, 1, color, 0, 0);

		y1 = y2;
		x1 = x2;
	}

	pD3DDevice->SetFVF(D3DFVF_TL);
	pD3DDevice->SetTexture(0, NULL);
	pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 363, Vtex, sizeof(D3DTLVERTEX));
}
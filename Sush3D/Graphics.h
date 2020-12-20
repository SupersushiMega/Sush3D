#pragma once

#include <Windows.h>
#include <d2d1.h>
#include <d2d1_1.h>


class Graphics
{
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* rendertarget;
	ID2D1SolidColorBrush* Solidbrush;

public:
	Graphics();
	~Graphics();

	bool Init(HWND windowHandle);

	void BeginDraw()
	{
		rendertarget->BeginDraw();
	}

	void EndDraw()
	{
		rendertarget->EndDraw();
	}

	void ClearScreen(float r, float g, float b);
	void DrawTriangle(float *x1, float *y1, float *x2, float *y2, float *x3, float *y3, float *r, float *g, float *b, float *a);
};

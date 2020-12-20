#include "Graphics.h"

Graphics::Graphics()
{
	factory = NULL;
	rendertarget = NULL;
	Solidbrush = NULL;
}

Graphics::~Graphics()
{
	if (factory)
	{
		factory->Release();
	}

	if (rendertarget)
	{
		rendertarget->Release();
	}

	if (Solidbrush)
	{
		Solidbrush->Release();
	}
}

bool Graphics::Init(HWND windowHandle)
{
	HRESULT res = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);
	if (res != S_OK)
	{
		return false;
	}

	RECT rect;
	GetClientRect(windowHandle, &rect);

	res = factory->CreateHwndRenderTarget
	(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			windowHandle,
			D2D1::SizeU(rect.right, rect.bottom)),
		&rendertarget
	);

	if (res != S_OK)
	{
		return false;
	}

	res = rendertarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0), &Solidbrush);

	if (res != S_OK)
	{
		return false;
	}

	return true;
}

void Graphics::ClearScreen(float r, float g, float b)
{
	rendertarget->Clear(D2D1::ColorF(r, g, b));
};

void Graphics::DrawTriangle(float *x1, float *y1, float *x2, float *y2, float *x3, float *y3, float *r, float *g, float *b, float *a)
{
	Solidbrush->SetColor(D2D1::ColorF(*r, *g, *b, *a));
	rendertarget->DrawLine(D2D1::Point2F(*x1, *y1), D2D1::Point2F(*x2, *y2), Solidbrush);
	rendertarget->DrawLine(D2D1::Point2F(*x2, *y2), D2D1::Point2F(*x3, *y3), Solidbrush);
	rendertarget->DrawLine(D2D1::Point2F(*x3, *y3), D2D1::Point2F(*x1, *y1), Solidbrush);
};

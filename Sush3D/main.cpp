/*Sush3D

Made by SupersushiMega
*/

#include <Windows.h>
#include "Graphics.h"

Graphics* graphics;


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmd, int nCmdShow)
{
	//Window Setup
	//=========================================================================================
	WNDCLASSEX windowclass;
	ZeroMemory(&windowclass, sizeof(WNDCLASSEX));

	windowclass.cbSize = sizeof(WNDCLASSEX);
	windowclass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	windowclass.hInstance = hInstance;
	windowclass.lpfnWndProc = WindowProc;
	windowclass.lpszClassName = L"MainWindow";
	windowclass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&windowclass);

	RECT rect = {0, 0, 800, 600}; //Drawing area
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);	//Calculate window Size

	HWND windowhandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MainWindow", L"Sush3D", WS_OVERLAPPEDWINDOW, 100, 100, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, 0);
	//=========================================================================================

	if (!windowhandle)
	{
		return -1;
	}

	graphics = new Graphics();

	if (!graphics->Init(windowhandle))
	{
		delete graphics;
		return -1;
	}

	ShowWindow(windowhandle, nCmdShow);

	float y = 0.0f;
	float ySpeed = 0.0f;


	MSG message;
	message.message = WM_NULL;

	float x1 = 0.0f;
	float y1 = 0.0f;

	float x2 = 0.0f;
	float y2 = 0.0f;

	float x3 = 0.0f;
	float y3 = 0.0f;

	float r = 1.0f;

	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, windowhandle, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}
		else
		{
			//update

			y = 1.0f;

			//render
			graphics->BeginDraw();

			//graphics->ClearScreen(0.0f, 0.0f, 0.5f);

			r = 0.0f;

			graphics->DrawTriangle(&x1, &y1, &x2, &y2, &x3, &y3, &r, &r, &r, &y);

			r = 1.0f;

			x3 = x2;
			x2 = x1;
			x1 = rand() % 800;

			y3 = y2;
			y2 = y1;
			y1 = rand() % 600;

			graphics->DrawTriangle(&x1, &y1, &x2, &y2, &x3, &y3, &r, &r, &r, &r);

			graphics->EndDraw();
		}
	}

	delete graphics;

	return 0;
}
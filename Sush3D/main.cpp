/*Sush3D

Made by SupersushiMega with help of javidx9 code-It-Yourself 3D Graphics Engine youtube series
*/

#include <Windows.h>
#include "Graphics.h"
#include <algorithm>

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

	RECT resolution = { 0, 0, 800, 600 };

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

	RECT rect = resolution; //Drawing area
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);	//Calculate window Size

	HWND windowhandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MainWindow", L"Sush3D", WS_OVERLAPPEDWINDOW, 100, 100, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, 0);
	//=========================================================================================

	if (!windowhandle)
	{
		return -1;
	}

	graphics = new Graphics();

	if (!graphics->Init(windowhandle, 90.0f, 0.1f, 1000.0f))
	{
		delete graphics;
		return -1;
	}

	ShowWindow(windowhandle, nCmdShow);

	float y = 0.0f;
	float ySpeed = 0.0f;


	MSG message;
	message.message = WM_NULL;

	Graphics::mesh CubeMesh;
	Graphics::mesh TestMesh;

	TestMesh.LoadFromObj("axis.obj");

	CubeMesh.tri = {

		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST                                                      
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// NORTH                                                     
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

		// WEST                                                      
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

		// TOP                                                       
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		// BOTTOM                                                    
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

	};

	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, windowhandle, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}
		else
		{
			//update

			if (GetAsyncKeyState(L'A'))	//a
			{
				graphics->camera.LocalPosDelta.x = -0.1f;
			}
			else if (GetAsyncKeyState(L'D'))	//d
			{
				graphics->camera.LocalPosDelta.x = 0.1f;
			}
			else
			{
				graphics->camera.LocalPosDelta.x = 0.0f;
			}

			if (GetAsyncKeyState(L'W'))	//w
			{
				graphics->camera.LocalPosDelta.z = 0.1f;
			}
			else if (GetAsyncKeyState(L'S'))	//s
			{
				graphics->camera.LocalPosDelta.z = -0.1f;
			}
			else
			{
				graphics->camera.LocalPosDelta.z = 0.0f;
			}

			if (GetAsyncKeyState(VK_LEFT))	//Left rotation
			{
				graphics->camera.TargetRot.y += 0.1f;
			}
			else if (GetAsyncKeyState(VK_RIGHT))	//Right rotation
			{
				graphics->camera.TargetRot.y += -0.1f;
			}

			//render
			graphics->BeginDraw();

			Graphics::Color color;

			graphics->ClearScreen(0.0f, 0.0f, 0.0f);

			graphics->DrawMesh(TestMesh, color);

			graphics->EndDraw();
		}
	}

	delete graphics;

	return 0;
}
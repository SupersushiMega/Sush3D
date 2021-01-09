/*Sush3D

Made by SupersushiMega with help of javidx9 code-It-Yourself 3D Graphics Engine youtube series
*/

#include <Windows.h>
#include "Graphics.h"
#include <algorithm>

Graphics* graphics;
bool closeWindow = false;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);
		closeWindow = true;
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmd, int nCmdShow)
{
	RECT resolution = { 0, 0, 1024, 800 };

	Graphics::ImageBuff imageBuffer = Graphics::ImageBuff(resolution.right, resolution.bottom);
	Graphics::Alpha_DepthBuff AlphaDepthBuffer = Graphics::Alpha_DepthBuff(resolution.right, resolution.bottom);

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

	if (!graphics->Init(windowhandle, resolution.right, resolution.bottom, 90.0f, 0.1f, 1000.0f))
	{
		delete graphics;
		return -1;
	}

	ShowWindow(windowhandle, nCmdShow);

	float y = 0.0f;
	float ySpeed = 0.0f;


	MSG message;
	message.message = WM_NULL;

	Graphics::mesh Logo;
	Graphics::mesh Suzzane;
	Graphics::mesh terrain;

	Logo.LoadFromObj("Sush3DLogo.obj");
	Logo.WorldPos = { 0.0f, -50.0f, 25.0f };

	Suzzane.LoadFromObj("Suzzane.obj");
	Suzzane.WorldPos = { 10.0f, 10.0f, 10.0f };

	terrain.LoadFromObj("Terrain.obj");


	Graphics::BitMap SuzzaneBMP;
	SuzzaneBMP.LoadBitmapRGB("Suzzane.bmp");
	SuzzaneBMP.LoadBitmapAlpha("SuzzaneAlpha.bmp");

	Graphics::BitMap terrainBMP;
	terrainBMP.LoadBitmapRGB("TerrainUV.bmp");

	Graphics::BitMap LogoBMP;
	LogoBMP.LoadBitmapRGB("Sush3DLogo_RGB.bmp");

	uint8_t frame = 0;
	while (!closeWindow)
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

			if (GetAsyncKeyState(VK_UP))	//Left rotation
			{
				graphics->camera.LocalPosDelta.y = 0.1f;
			}
			else if (GetAsyncKeyState(VK_DOWN))	//Right rotation
			{
				graphics->camera.LocalPosDelta.y = -0.1f;
			}
			else
			{
				graphics->camera.LocalPosDelta.y = 0.0f;
			}

			if (GetAsyncKeyState(VK_LEFT))	//Left rotation
			{
				graphics->camera.TargetRot.y += -0.1f;
			}
			else if (GetAsyncKeyState(VK_RIGHT))	//Right rotation
			{
				graphics->camera.TargetRot.y += 0.1f;
			}

			//render
			Graphics::Color color = { 0.5f, 0.5f, 0.5f, 0.5f };


			graphics->ClearScreen(1, 1, 1, imageBuffer, AlphaDepthBuffer);
			/*for (uint16_t Y = 0; Y < bitmap.Resolution[0]; Y++)
			{
				for (uint16_t X = 0; X < bitmap.Resolution[1]; X++)
				{
					float tmp = 0.0f;
					imageBuffer.PutPix(X, Y, bitmap.Pixels[Y][X]);
				}
			}*/


			//graphics->BeginDraw();
			//graphics->DrawMesh(TestMesh, color, imageBuffer);

			Logo.rotation.y += 0.01f;
			
			graphics->DrawMeshTextured(Logo, LogoBMP, imageBuffer, AlphaDepthBuffer);
			//graphics->DrawMesh(Logo, color, imageBuffer);
			graphics->DrawMeshFilled(Suzzane, color, imageBuffer, AlphaDepthBuffer);
			graphics->DrawMeshTextured(terrain, terrainBMP, imageBuffer, AlphaDepthBuffer);

			//graphics->DrawMesh(Logo, color, imageBuffer);
			//graphics->DrawMesh(Suzzane, color, imageBuffer);
			//graphics->DrawMesh(terrain, color, imageBuffer);

			uint16_t w = imageBuffer.width / 2;
			uint16_t h = imageBuffer.height / 2;

			//graphics->DrawMeshTextured(TestMesh, bitmap, imageBuffer, AlphaDepthBuffer);
			//graphics->EndDraw();
			//graphics->DrawLine(p1, p2, color, imageBuffer);
			if (frame)
			{
				graphics->refresh(imageBuffer);
			}
			frame++;
		}
	}

	delete graphics;

	return 0;
}
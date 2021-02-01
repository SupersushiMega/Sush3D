/*Sush3D

Made by SupersushiMega with help of javidx9 code-It-Yourself 3D Graphics Engine youtube series
*/

#include <Windows.h>
#include "Graphics.h"
#include <algorithm>
#include <time.h>

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

	Graphics::BitMap BMPDrawTest;
	BMPDrawTest.LoadBitmapRGB("BMPDrawTest.bmp");
	BMPDrawTest.LoadBitmapAlpha("BMPDrawTestAlpha.bmp");

	Graphics::BitMap RobotSprite;
	RobotSprite.LoadBitmapRGB("RobotSpriteTest.bmp");
	RobotSprite.LoadBitmapAlpha("RobotSpriteTestAlpha.bmp");

	Graphics::vec3D ZeroPos = { 0.0f, 0.0f, 0.0f };

	uint8_t frame = 0;

	uint16_t FPS = 0;

	time_t StartTime = time(NULL);
	time_t CurTime = time(NULL);

	float X = graphics->camera.GlobalPos.x;
	float Y = graphics->camera.GlobalPos.y;
	float Z = graphics->camera.GlobalPos.z;

	char Buffer[500] = { 0 };

	string String;

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
			Graphics::Color color = { 0.5f, 0.5f, 0.5f, 1.0f };


			graphics->ClearScreen(1, 1, 1, imageBuffer, AlphaDepthBuffer);

			Logo.rotation.y += 0.01f;
			
			X = graphics->camera.GlobalPos.x;
			Y = graphics->camera.GlobalPos.y;
			Z = graphics->camera.GlobalPos.z;

			

			graphics->DrawMeshTextured(Logo, LogoBMP, imageBuffer, AlphaDepthBuffer);
			//graphics->DrawMesh(Logo, color, imageBuffer);
			//graphics->DrawMeshFilled(Suzzane, color, imageBuffer, AlphaDepthBuffer);
			graphics->DrawMeshTextured(Suzzane, SuzzaneBMP, imageBuffer, AlphaDepthBuffer);
			graphics->DrawMeshTextured(terrain, terrainBMP, imageBuffer, AlphaDepthBuffer);
			graphics->DrawBMP(BMPDrawTest, 600, 780, imageBuffer, AlphaDepthBuffer);
			graphics->DrawSprite3D(RobotSprite, ZeroPos, imageBuffer, AlphaDepthBuffer, 20, 20);

			sprintf_s(Buffer, "Sush3D \nVersion : Development_2Din3D_Sprite\nX = %f \nY = %f \nZ = %f \n~FPS = %d", X, Y, Z, FPS);

			graphics->DrawString(Buffer, 0, 0, color, imageBuffer);

			//graphics->DrawMesh(Logo, color, imageBuffer);
			//graphics->DrawMesh(Suzzane, color, imageBuffer);
			//graphics->DrawMesh(terrain, color, imageBuffer);

			//graphics->DrawMeshTextured(TestMesh, bitmap, imageBuffer, AlphaDepthBuffer);
			//graphics->EndDraw();
			//graphics->DrawLine(p1, p2, color, imageBuffer);

			graphics->refresh(imageBuffer);

			if (difftime(CurTime, StartTime) > 1)
			{
				time(&StartTime);
				FPS = frame;
				frame = 0;
			}
			time(&CurTime);
			frame++;
		}
	}

	delete graphics;

	return 0;
}
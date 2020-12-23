#pragma once

#include <Windows.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <vector>
#include <fstream>
#include <strstream>

using namespace std;

class Graphics
{
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* rendertarget;
	ID2D1SolidColorBrush* Solidbrush;

public:

	uint16_t Resolution[2] = { 0 };
	float fov = 90.0f;
	float ViewingDist = 1000.0f;
	float DistfromScreen = 0.5f;
	float aspect = 0.0f;
	float XYcoef = 0.0f;

	Graphics();
	~Graphics();

	struct matrix4x4
	{
		float mat[4][4] = { 0 };
	};

	matrix4x4 ProjMatrix;

	struct Color
	{
		float r = 1.0f;
		float g = 1.0f;
		float b = 1.0f;
		float a = 1.0f;
	};

	struct vec3D
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 1.0f;
	};

	struct triangle
	{
		vec3D vectors[3];
		Color color = {1};
	};

	struct mesh
	{
		vector<triangle> tri;
		
		bool LoadFromObj(string filename);
	};

	struct GlobalLight
	{
		vec3D Direction = { 0.0f, 0.0f, -1.0f };
		Color color = { 1.0f, 1.0f, 1.0f };
		float strength = 1.0f;
	};

	GlobalLight globalLight;
	vec3D camera;

private:
	//Calculation functions
	//==========================================================================================================================
	vec3D MatrixVectorMultiplication(vec3D& inputVec, matrix4x4& matrix);
	matrix4x4 MatrixMatrixMultiplication(matrix4x4& matrix1, matrix4x4& matrix2);
	
	matrix4x4 MakeIdentityMarix();
	matrix4x4 MakeZrotationMatrix(float RadAngle);
	matrix4x4 MakeXrotationMatrix(float RadAngle);
	matrix4x4 MakeYrotationMatrix(float RadAngle);
	matrix4x4 MakeTranslationMatrix(float x, float y, float z);
	matrix4x4 MakeProjectionMatrix(float FovDeg, float Aspect, float DistFromScrn, float viewDist);

	vec3D AddVectors(vec3D& vec1, vec3D& vec2);
	vec3D SubVectors(vec3D& vec1, vec3D& vec2);
	vec3D MultVector(vec3D& vec1, float& mult);
	vec3D DivVector(vec3D& vec1, float& div);

	float DotProduct(vec3D& vec1, vec3D& vec2);
	
	float VectorLength(vec3D& vec);
	vec3D Normalise(vec3D& vec);
	vec3D CrossProd(vec3D& vec1, vec3D& vec2);
	//==========================================================================================================================

	//Draw functions
	//==========================================================================================================================
private:
	void DrawFlatTop(vec3D& point0, vec3D& point1, vec3D& point2);	//draw triangle with flattop
	void DrawFlatBottom(vec3D& point0, vec3D& point1, vec3D& point2);	//draw triangle with flatbottom

public:
	void BeginDraw()
	{
		rendertarget->BeginDraw();
	}

	void EndDraw()
	{
		rendertarget->EndDraw();
	}

	void ClearScreen(float r, float g, float b);
	void DrawTriangle(float& x1, float& y1, float& x2, float& y2, float& x3, float& y3, float& r, float& g, float& b, float& a);
	void DrawTriangle2(triangle Triangle, Color color);
	void DrawTriangle2filled(triangle &Triangle, Color &color);
	void DrawMesh(mesh mesh, Color color);
	//==========================================================================================================================

	//Other functions
	//==========================================================================================================================
	bool Init(HWND windowHandle, float FOV, float DistancefromScreen, float ViewingDistance);
	//==========================================================================================================================
};

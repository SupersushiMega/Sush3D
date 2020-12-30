#pragma once

/*Sush3D

Made by SupersushiMega with help of javidx9 code-It-Yourself 3D Graphics Engine youtube series

Textures must be in a 24Bit Bitmap format (Bitmaps of textures made with blenders material editor must have the principled shader node before matrial output node)
*/

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
	ID2D1Bitmap* BufferBmp;

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

	class ImageBuff
	{
		public:
			ImageBuff(uint16_t Width, uint16_t Height);
			~ImageBuff();
			void PutPix(uint16_t x, uint16_t y, Color col);

			uint32_t* PixelsPtr = nullptr;
			uint16_t width = 0;
			uint16_t height = 0;
	};

	struct vec2D
	{
		float u = 0.0f;
		float v = 0.0f;
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
		vec2D texCoord[3];
		Color color = {1.0f, 1.0f, 0.0f, 1.0f};
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

	struct Camera
	{
		vec3D LocalPosDelta = { 0.0f };
		vec3D GlobalPos = {0.0f};
		vec3D Rotation = {0.0f, 0.0f, 1.0f};
		vec3D TargetRot = { 0.0f, 0.0f, 1.0f };
	};

	struct BitMap
	{
		uint32_t Resolution[2];
		vector<vector<Color>> Pixels;

		bool LoadBitmap(const char* filename);
	};

	GlobalLight globalLight;

	Camera camera;

	vec3D UpVec = { 0.0f, -1.0f, 0.0f };

public:
	//Calculation functions
	//==========================================================================================================================
	vec3D MatrixVectorMultiplication(vec3D& inputVec, matrix4x4& matrix);
	matrix4x4 MatrixMatrixMultiplication(matrix4x4& matrix1, matrix4x4& matrix2);
	matrix4x4 MatrixInvertQuick(matrix4x4& matrixIn);	//Only Rotation and Translation Matrices
	
	matrix4x4 MakeIdentityMarix();
	matrix4x4 MakeZrotationMatrix(float RadAngle);
	matrix4x4 MakeXrotationMatrix(float RadAngle);
	matrix4x4 MakeYrotationMatrix(float RadAngle);
	matrix4x4 MakeTranslationMatrix(float x, float y, float z);
	matrix4x4 MakeProjectionMatrix(float FovDeg, float Aspect, float DistFromScrn, float viewDist);
	matrix4x4 MakePointAtMatrix(vec3D position, vec3D target, vec3D UPvec);

	vec3D AddVectors(vec3D& vec1, vec3D& vec2);
	vec3D SubVectors(vec3D& vec1, vec3D& vec2);
	vec3D MultVectorFloat(vec3D& vec1, float& mult);	//Multiplies a vector by a float
	vec3D MultVectorVector(vec3D& vec1, vec3D& mult);	//Multiplies two vectors
	vec3D DivVector(vec3D& vec1, float& div);

	float DotProduct(vec3D& vec1, vec3D& vec2);
	
	float VectorLength(vec3D& vec);
	vec3D Normalise(vec3D& vec);
	vec3D CrossProd(vec3D& vec1, vec3D& vec2);

	vec3D PlaneIntersect(vec3D& PlanePoint, vec3D& PlaneNormal, vec3D& StartOfLine, vec3D& EndOfLine, float& t);

	uint16_t TrianglePlaneClip(vec3D PlanePoint, vec3D PlaneNormal, triangle& InputTriangle, triangle& OutputTriangle1, triangle& OutputTriangle2);
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
	void DrawPixel(float &x, float &y, Color &col);
	void DrawTriangle(float& x1, float& y1, float& x2, float& y2, float& x3, float& y3, float& r, float& g, float& b, float& a);
	void DrawTriangle2(triangle Triangle, Color color);
	void DrawTriangle2filled(triangle &Triangle, Color &color);
	void DrawMesh(mesh mesh, Color color);

	void refresh(ImageBuff& Buffer);
	//==========================================================================================================================

	//Other functions
	//==========================================================================================================================
	bool Init(HWND windowHandle, uint16_t width, uint16_t height, float FOV, float DistancefromScreen, float ViewingDistance);
	//==========================================================================================================================
};

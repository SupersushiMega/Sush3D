#include "Graphics.h"
#include <iostream>
#include <string>
float theta = 0;

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

bool Graphics::Init(HWND windowHandle, float FOV, float DistancefromScreen, float ViewingDistance)
{
	HRESULT res = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);
	if (res != S_OK)
	{
		return false;
	}

	RECT resolution;
	GetClientRect(windowHandle, &resolution);

	res = factory->CreateHwndRenderTarget
	(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			windowHandle,
			D2D1::SizeU(resolution.right, resolution.bottom)),
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

	//Projection Matrix
	//==========================================================================================================================
	Graphics::Resolution[0] = resolution.right;
	Graphics::Resolution[1] = resolution.bottom;
	Graphics::fov = FOV;
	Graphics::DistfromScreen = DistancefromScreen;
	Graphics::ViewingDist = ViewingDistance;
	Graphics::aspect = (float)resolution.bottom / (float)resolution.right;
	Graphics::XYcoef = 1.0f / tanf(Graphics::fov * 0.5f / 180.0f * 3.14159f);

	Graphics::ProjMatrix.mat[0][0] = Graphics::aspect * Graphics::XYcoef;;
	Graphics::ProjMatrix.mat[1][1] = Graphics::XYcoef;
	Graphics::ProjMatrix.mat[2][2] = Graphics::ViewingDist / (Graphics::ViewingDist - Graphics::DistfromScreen);
	Graphics::ProjMatrix.mat[3][2] = (-Graphics::ViewingDist * Graphics::DistfromScreen) / (Graphics::ViewingDist - Graphics::DistfromScreen);
	Graphics::ProjMatrix.mat[2][3] = 1.0f;
	Graphics::ProjMatrix.mat[3][3] = 0.0f;
	//==========================================================================================================================


	return true;
}

void Graphics::MatrixVectorMultiplication(vec3D& inputVec, vec3D& outputVec, matrix4x4& matrix)
{
	outputVec.x = inputVec.x * matrix.mat[0][0] + inputVec.y * matrix.mat[1][0] + inputVec.z * matrix.mat[2][0] + matrix.mat[3][0];
	outputVec.y = inputVec.x * matrix.mat[0][1] + inputVec.y * matrix.mat[1][1] + inputVec.z * matrix.mat[2][1] + matrix.mat[3][1];
	outputVec.z = inputVec.x * matrix.mat[0][2] + inputVec.y * matrix.mat[1][2] + inputVec.z * matrix.mat[2][2] + matrix.mat[3][2];
	float w = inputVec.x * matrix.mat[0][3] + inputVec.y * matrix.mat[1][3] + inputVec.z * matrix.mat[2][3] + matrix.mat[3][3];

	if (w != 0.0f)
	{
		outputVec.x /= w;
		outputVec.y /= w;
		outputVec.z /= w;
	}
}

void Graphics::ClearScreen(float r, float g, float b)
{
	rendertarget->Clear(D2D1::ColorF(r, g, b));
};

void Graphics::DrawFlatTop(vec3D& point0, vec3D& point1, vec3D& point2)
{
	const float slope0 = (point2.x - point0.x) / (point2.y - point0.y);
	const float slope1 = (point2.x - point1.x) / (point2.y - point1.y);

	const int32_t StartY = (int16_t)ceil(point0.y - 0.5f);
	const int32_t EndY = (int16_t)ceil(point2.y - 0.5f);

	for (int32_t y = StartY; y < EndY; y++)
	{
		vec3D Start;
		vec3D End;

		Start.y = y;
		End.y = y;

		const float pointx0 = slope0 * (float(y) + 0.5f - point0.y) + point0.x;
		const float pointx1 = slope1 * (float(y) + 0.5f - point1.y) + point1.x;

		Start.x = (int32_t)ceil(pointx0 - 0.05f);
		End.x = (int32_t)ceil(pointx1 - 0.05f);

		rendertarget->DrawLine(D2D1::Point2F(Start.x, Start.y), D2D1::Point2F(End.x, End.y), Solidbrush);
	}
};

void Graphics::DrawFlatBottom(vec3D& point0, vec3D& point1, vec3D& point2)
{
	const float slope0 = (point1.x - point0.x) / (point1.y - point0.y);
	const float slope1 = (point2.x - point0.x) / (point2.y - point0.y);

	const int32_t StartY = (int16_t)ceil(point0.y - 0.5f);
	const int32_t EndY = (int16_t)ceil(point2.y - 0.5f);

	for (int32_t y = StartY; y < EndY; y++)
	{
		vec3D Start;
		vec3D End;

		Start.y = y;
		End.y = y;

		const float pointx0 = slope0 * (float(y) + 0.5f - point0.y) + point0.x;
		const float pointx1 = slope1 * (float(y) + 0.5f - point0.y) + point0.x;

		Start.x = (int32_t)ceil(pointx0 - 0.05f);
		End.x = (int32_t)ceil(pointx1 - 0.05f);

		rendertarget->DrawLine(D2D1::Point2F(Start.x, Start.y), D2D1::Point2F(End.x, End.y), Solidbrush);
	}
};

void Graphics::DrawTriangle(float& x1, float& y1, float& x2, float& y2, float& x3, float& y3, float& r, float& g, float& b, float& a)
{
	Solidbrush->SetColor(D2D1::ColorF(r, g, b, a));
	rendertarget->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), Solidbrush);
	rendertarget->DrawLine(D2D1::Point2F(x2, y2), D2D1::Point2F(x3, y3), Solidbrush);
	rendertarget->DrawLine(D2D1::Point2F(x3, y3), D2D1::Point2F(x1, y1), Solidbrush);
};

void Graphics::DrawTriangle2(triangle Triangle, Color color)
{
	Solidbrush->SetColor(D2D1::ColorF(color.r, color.g, color.b, color.a));
	for (char i = 0; i < 3; i++)
	{
		rendertarget->DrawLine(D2D1::Point2F(Triangle.vectors[i].x, Triangle.vectors[i].y), D2D1::Point2F(Triangle.vectors[(i + 1) % 3].x, Triangle.vectors[(i + 1) % 3].y), Solidbrush);
	}
};

void Graphics::DrawTriangle2filled(triangle &Triangle, Color &color)
{
	Solidbrush->SetColor(D2D1::ColorF(color.r, color.g, color.b, color.a));

	vec3D* vec0 = &Triangle.vectors[0];
	vec3D* vec1 = &Triangle.vectors[1];
	vec3D* vec2 = &Triangle.vectors[2];

	//vector Sort by y value
	//==========================================================================================================================
	if (vec1->y < vec0->y)
	{
		std::swap(vec0, vec1);
	}
	if (vec2->y < vec1->y)
	{
		std::swap(vec1, vec2);
	}
	if (vec1->y < vec0->y)
	{
		std::swap(vec0, vec1);
	}
	//==========================================================================================================================

	//Check for natural flattop or bottom or neither
	//==========================================================================================================================
	if (vec0->y == vec1->y)	//flattop
	{
		if (vec1->x < vec0->x)	//vector sort by x value
		{
			std::swap(vec0, vec1);
		}
		DrawFlatTop(*vec0, *vec1, *vec2);
	}

	else if (vec1->y == vec2->y)	//flatbottom
	{
		if (vec2->x < vec1->x)	//vector sort by x value
		{
			std::swap(vec1, vec2);
		}
		DrawFlatBottom(*vec0, *vec1, *vec2);
	}

	else //neither
	{
		float alpha = ((vec1->y - vec0->y) / (vec2->y - vec0->y)); //spliting point

		//Splitting vector
		vec3D vecSplt;
		vecSplt.x = vec0->x + (vec2->x - vec0->x) * alpha;
		vecSplt.y = vec0->y + (vec2->y - vec0->y) * alpha;

		if (vec1->x < vecSplt.x)	//check if triangle is major right
		{
			DrawFlatBottom(*vec0, *vec1, vecSplt);
			DrawFlatTop(*vec1, vecSplt, *vec2);
		}

		else	//triangle is major left
		{
			DrawFlatBottom(*vec0, vecSplt, *vec1);
			DrawFlatTop(vecSplt, *vec1, *vec2);
		}

	}
	//==========================================================================================================================

};

void Graphics::DrawMesh(mesh mesh, Color color)
{
	theta += 0.01f;
	//Z Rotation Matrix
	//==========================================================================================================================
	Graphics::RotZMatrix.mat[0][0] = cosf(theta);
	Graphics::RotZMatrix.mat[0][1] = sinf(theta);
	Graphics::RotZMatrix.mat[1][0] = -sinf(theta);
	Graphics::RotZMatrix.mat[1][1] = cosf(theta);
	Graphics::RotZMatrix.mat[2][2] = 1;
	Graphics::RotZMatrix.mat[3][3] = 1;
	//==========================================================================================================================

	//X Rotation Matrix
	//==========================================================================================================================
	Graphics::RotXMatrix.mat[0][0] = 1;
	Graphics::RotXMatrix.mat[1][1] = cosf(theta * 0.5f);
	Graphics::RotXMatrix.mat[1][2] = sinf(theta * 0.5f);
	Graphics::RotXMatrix.mat[2][1] = -sinf(theta * 0.5f);
	Graphics::RotXMatrix.mat[2][2] = cosf(theta * 0.5f);
	Graphics::RotXMatrix.mat[3][3] = 1;
	//==========================================================================================================================

	for (auto tri : mesh.tri)
	{
		triangle ZrotadetTri;
		triangle ZXrotadetTri;
		triangle TranslatedTri;
		triangle ProjectedTri;

		vec3D normal;
		vec3D line1;
		vec3D line2;

		float NormalLength = 0;

		MatrixVectorMultiplication(tri.vectors[0], ZrotadetTri.vectors[0], Graphics::RotZMatrix);
		MatrixVectorMultiplication(tri.vectors[1], ZrotadetTri.vectors[1], Graphics::RotZMatrix);
		MatrixVectorMultiplication(tri.vectors[2], ZrotadetTri.vectors[2], Graphics::RotZMatrix);

		MatrixVectorMultiplication(ZrotadetTri.vectors[0], ZXrotadetTri.vectors[0], Graphics::RotXMatrix);
		MatrixVectorMultiplication(ZrotadetTri.vectors[1], ZXrotadetTri.vectors[1], Graphics::RotXMatrix);
		MatrixVectorMultiplication(ZrotadetTri.vectors[2], ZXrotadetTri.vectors[2], Graphics::RotXMatrix);

		//z translation
		//==========================================================================================================================
		TranslatedTri = ZXrotadetTri;
		TranslatedTri.vectors[0].z = ZXrotadetTri.vectors[0].z + 3.0f;
		TranslatedTri.vectors[1].z = ZXrotadetTri.vectors[1].z + 3.0f;
		TranslatedTri.vectors[2].z = ZXrotadetTri.vectors[2].z + 3.0f;
		//==========================================================================================================================

		//Normal Calculation
		//==========================================================================================================================
		line1.x = TranslatedTri.vectors[1].x - TranslatedTri.vectors[0].x;
		line1.y = TranslatedTri.vectors[1].y - TranslatedTri.vectors[0].y;
		line1.z = TranslatedTri.vectors[1].z - TranslatedTri.vectors[0].z;

		line2.x = TranslatedTri.vectors[2].x - TranslatedTri.vectors[0].x;
		line2.y = TranslatedTri.vectors[2].y - TranslatedTri.vectors[0].y;
		line2.z = TranslatedTri.vectors[2].z - TranslatedTri.vectors[0].z;

		normal.x = line1.y * line2.z - line1.z * line2.y;
		normal.y = line1.z * line2.x - line1.x * line2.z;
		normal.z = line1.x * line2.y - line1.y * line2.x;

		NormalLength = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

		normal.x /= NormalLength;
		normal.y /= NormalLength;
		normal.z /= NormalLength;
		//==========================================================================================================================



		if (normal.x * (TranslatedTri.vectors[0].x - Graphics::camera.x) + normal.y * (TranslatedTri.vectors[0].y - Graphics::camera.y) + normal.z * (TranslatedTri.vectors[0].z - Graphics::camera.z) < 0.0f)
		{
			//Projection Matrix Multiplication
			//==========================================================================================================================
			MatrixVectorMultiplication(TranslatedTri.vectors[0], ProjectedTri.vectors[0], Graphics::ProjMatrix);
			MatrixVectorMultiplication(TranslatedTri.vectors[1], ProjectedTri.vectors[1], Graphics::ProjMatrix);
			MatrixVectorMultiplication(TranslatedTri.vectors[2], ProjectedTri.vectors[2], Graphics::ProjMatrix);
			//==========================================================================================================================

			//Scaling
			//==========================================================================================================================
			ProjectedTri.vectors[0].x += 1.0f;
			ProjectedTri.vectors[0].y += 1.0f;

			ProjectedTri.vectors[1].x += 1.0f;
			ProjectedTri.vectors[1].y += 1.0f;

			ProjectedTri.vectors[2].x += 1.0f;
			ProjectedTri.vectors[2].y += 1.0f;


			ProjectedTri.vectors[0].x *= 0.5f * (float)Graphics::Resolution[0];
			ProjectedTri.vectors[0].y *= 0.5f * (float)Graphics::Resolution[1];

			ProjectedTri.vectors[1].x *= 0.5f * (float)Graphics::Resolution[0];
			ProjectedTri.vectors[1].y *= 0.5f * (float)Graphics::Resolution[1];

			ProjectedTri.vectors[2].x *= 0.5f * (float)Graphics::Resolution[0];
			ProjectedTri.vectors[2].y *= 0.5f * (float)Graphics::Resolution[1];
			//==========================================================================================================================

			DrawTriangle2filled(ProjectedTri, color);
			DrawTriangle2(ProjectedTri, color);
		}
	}
};

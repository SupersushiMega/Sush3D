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
	Graphics::Resolution[0] = 800;
	Graphics::Resolution[1] = 600;
	Graphics::fov = FOV;
	Graphics::DistfromScreen = DistancefromScreen;
	Graphics::ViewingDist = ViewingDistance;
	Graphics::aspect = 800.0f / 600.0f;
	Graphics::XYcoef = 1.0f / tanf(Graphics::fov * 0.5f / 180.0f * 3.14159f);

	Graphics::ProjMatrix;

	Graphics::ProjMatrix.mat[0][0] = Graphics::aspect * Graphics::XYcoef;;
	Graphics::ProjMatrix.mat[1][1] = Graphics::XYcoef;
	Graphics::ProjMatrix.mat[2][2] = Graphics::ViewingDist / (Graphics::ViewingDist - Graphics::DistfromScreen);
	Graphics::ProjMatrix.mat[3][2] = (-Graphics::ViewingDist * Graphics::DistfromScreen) / (Graphics::ViewingDist - Graphics::DistfromScreen);
	Graphics::ProjMatrix.mat[2][3] = 1.0f;
	Graphics::ProjMatrix.mat[3][3] = 0.0f;
	//==========================================================================================================================

	return true;
}

void Graphics::MatrixVectorMultiplication(vec3D &inputVec, vec3D &outputVec, matrix4x4 &matrix)
{
	outputVec.x =	inputVec.x * matrix.mat[0][0] + inputVec.y * matrix.mat[1][0] + inputVec.z * matrix.mat[2][0] + inputVec.x + matrix.mat[3][0];
	outputVec.y =	inputVec.x * matrix.mat[0][1] + inputVec.y * matrix.mat[1][1] + inputVec.z * matrix.mat[2][1] + inputVec.x + matrix.mat[3][1];
	outputVec.z =	inputVec.x * matrix.mat[0][2] + inputVec.y * matrix.mat[1][2] + inputVec.z * matrix.mat[2][2] + inputVec.x + matrix.mat[3][2];
	float w		=	inputVec.x * matrix.mat[0][3] + inputVec.y * matrix.mat[1][3] + inputVec.z * matrix.mat[2][3] + inputVec.x + matrix.mat[3][3];

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

void Graphics::DrawTriangle(float &x1, float &y1, float &x2, float &y2, float &x3, float &y3, float &r, float &g, float &b, float &a)
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

void Graphics::DrawMesh(mesh mesh, Color color)
{
	for (auto tri : mesh.tri)
	{
		//Projection Matrix Multiplication
		//==========================================================================================================================
		triangle ProjectedTri;
		MatrixVectorMultiplication(tri.vectors[0], ProjectedTri.vectors[0], Graphics::ProjMatrix);
		MatrixVectorMultiplication(tri.vectors[1], ProjectedTri.vectors[1], Graphics::ProjMatrix);
		MatrixVectorMultiplication(tri.vectors[2], ProjectedTri.vectors[2], Graphics::ProjMatrix);
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

		DrawTriangle2(ProjectedTri, color);
		
	}
};

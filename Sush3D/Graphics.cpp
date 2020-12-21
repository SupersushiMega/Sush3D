#include "Graphics.h"

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

		DrawTriangle2(ProjectedTri, color);

	}
};

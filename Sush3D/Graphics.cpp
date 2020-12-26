#include "Graphics.h"
#include <iostream>
#include <string>
#include <fstream>
#include <strstream>
#include <algorithm>
#include <list>

/*Sush3D

Made by SupersushiMega with help of javidx9 code-It-Yourself 3D Graphics Engine youtube series
*/

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

	Graphics::Resolution[0] = resolution.bottom;
	Graphics::Resolution[1] = resolution.right;

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

	rendertarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	res = rendertarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0), &Solidbrush);

	if (res != S_OK)
	{
		return false;
	}

	Graphics::ProjMatrix = MakeProjectionMatrix(90.0f, (float)Graphics::Resolution[0] / (float)Graphics::Resolution[1], 0.1f, 1000.0f);

	printf("test");

	return true;
}

bool Graphics::LoadBitmap(const char *filename)
{
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	FILE *bitmap;
	
	unsigned char* pointer;

	fopen_s(&bitmap, filename, "r");

	fread(&fileHeader, sizeof(fileHeader), 1, bitmap);	//get fileheader data
	fread(&infoHeader, sizeof(infoHeader), 1, bitmap);	//get infoheader data
	return true;
}

bool Graphics::mesh::LoadFromObj(string filename)
{
	ifstream f(filename);
	if (!f.is_open())
	{
		return false;
	}

	vector<vec3D> verts; //vertice pool

	while (!f.eof())
	{
		char line[128];
		f.getline(line, 128);

		strstream stream;
		stream << line;

		char Waste;

		if (line[0] == 'v')
		{
			vec3D TempVert;
			stream >> Waste >> TempVert.x >> TempVert.y >> TempVert.z;
			verts.push_back(TempVert);
		}

		else if (line[0] == 'f')
		{
			uint64_t face[3] = {0};
			stream >> Waste >> face[0] >> face[1] >> face[2];
			tri.push_back({ verts[face[0] - 1], verts[face[1] - 1] , verts[face[2] - 1] });
		}
	}

	return true;
};

Graphics::vec3D Graphics::MatrixVectorMultiplication(vec3D& inputVec, matrix4x4& matrix)
{
	vec3D outputVec;
	outputVec.x = inputVec.x * matrix.mat[0][0] + inputVec.y * matrix.mat[1][0] + inputVec.z * matrix.mat[2][0] + inputVec.w * matrix.mat[3][0];
	outputVec.y = inputVec.x * matrix.mat[0][1] + inputVec.y * matrix.mat[1][1] + inputVec.z * matrix.mat[2][1] + inputVec.w * matrix.mat[3][1];
	outputVec.z = inputVec.x * matrix.mat[0][2] + inputVec.y * matrix.mat[1][2] + inputVec.z * matrix.mat[2][2] + inputVec.w * matrix.mat[3][2];
	outputVec.w = inputVec.x * matrix.mat[0][3] + inputVec.y * matrix.mat[1][3] + inputVec.z * matrix.mat[2][3] + inputVec.w * matrix.mat[3][3];

	return outputVec;
}

Graphics::matrix4x4 Graphics::MatrixMatrixMultiplication(matrix4x4& matrix1, matrix4x4& matrix2)
{
	matrix4x4 matrix;
	for (uint8_t x = 0; x < 4; x++)
	{
		for (uint8_t y = 0; y < 4; y++)
		{
			matrix.mat[y][x] = matrix1.mat[y][0] * matrix2.mat[0][x] + matrix1.mat[y][1] * matrix2.mat[1][x] + matrix1.mat[y][2] * matrix2.mat[2][x] + matrix1.mat[y][3] * matrix2.mat[3][x];
		}
	}
	return matrix;
}

Graphics::matrix4x4 Graphics::MatrixInvertQuick(matrix4x4& matrixIn)	//Only Rotation and Translation Matrices
{
	matrix4x4 matrix;
	matrix.mat[0][0] = matrixIn.mat[0][0];
	matrix.mat[0][1] = matrixIn.mat[1][0];
	matrix.mat[0][2] = matrixIn.mat[2][0];
	matrix.mat[0][3] = 0.0f;

	matrix.mat[1][0] = matrixIn.mat[0][1];
	matrix.mat[1][1] = matrixIn.mat[1][1];
	matrix.mat[1][2] = matrixIn.mat[2][1];
	matrix.mat[1][3] = 0.0f;

	matrix.mat[2][0] = matrixIn.mat[0][2];
	matrix.mat[2][1] = matrixIn.mat[1][2];
	matrix.mat[2][2] = matrixIn.mat[2][2];
	matrix.mat[2][3] = 0.0f;

	matrix.mat[3][0] = -(matrixIn.mat[3][0] * matrix.mat[0][0] + matrixIn.mat[3][1] * matrix.mat[1][0] + matrixIn.mat[3][2] * matrix.mat[2][0]);
	matrix.mat[3][1] = -(matrixIn.mat[3][0] * matrix.mat[0][1] + matrixIn.mat[3][1] * matrix.mat[1][1] + matrixIn.mat[3][2] * matrix.mat[2][1]);
	matrix.mat[3][2] = -(matrixIn.mat[3][0] * matrix.mat[0][2] + matrixIn.mat[3][1] * matrix.mat[1][2] + matrixIn.mat[3][2] * matrix.mat[2][2]);
	matrix.mat[3][3] = 1.0f;
	return matrix;
}

Graphics::matrix4x4 Graphics::MakeIdentityMarix()
{
	matrix4x4 matrix;
	matrix.mat[0][0] = 1.0f;
	matrix.mat[1][1] = 1.0f;
	matrix.mat[2][2] = 1.0f;
	matrix.mat[3][3] = 1.0f;
	return matrix;
}

Graphics::matrix4x4 Graphics::MakeZrotationMatrix(float RadAngle)
{
	matrix4x4 matrix;
	matrix.mat[0][0] = cosf(RadAngle);
	matrix.mat[0][1] = sinf(RadAngle);
	matrix.mat[1][0] = -sinf(RadAngle);
	matrix.mat[1][1] = cosf(RadAngle);
	matrix.mat[2][2] = 1.0f;
	matrix.mat[3][3] = 1.0f;
	return matrix;
}

Graphics::matrix4x4 Graphics::MakeXrotationMatrix(float RadAngle)
{
	matrix4x4 matrix;
	matrix.mat[0][0] = 1.0f;
	matrix.mat[1][1] = cosf(RadAngle);
	matrix.mat[1][2] = sinf(RadAngle);
	matrix.mat[2][1] = -sinf(RadAngle);
	matrix.mat[2][2] = cosf(RadAngle);
	matrix.mat[3][3] = 1.0f;
	return matrix;
}

Graphics::matrix4x4 Graphics::MakeYrotationMatrix(float RadAngle)
{
	matrix4x4 matrix;
	matrix.mat[0][0] = cosf(RadAngle);
	matrix.mat[0][2] = sinf(RadAngle);
	matrix.mat[2][0] = -sinf(RadAngle);
	matrix.mat[1][1] = 1.0f;
	matrix.mat[2][2] = cosf(RadAngle);
	matrix.mat[3][3] = 1.0f;
	return matrix;
}

Graphics::matrix4x4 Graphics::MakeTranslationMatrix(float x, float y, float z)
{
	matrix4x4 matrix;
	matrix.mat[0][0] = 1.0f;
	matrix.mat[1][1] = 1.0f;
	matrix.mat[2][2] = 1.0f;
	matrix.mat[3][3] = 1.0f;
	matrix.mat[3][0] = x;
	matrix.mat[3][1] = y;
	matrix.mat[3][2] = z;
	return matrix;
}

Graphics::matrix4x4 Graphics::MakeProjectionMatrix(float FovDeg, float Aspect, float DistFromScrn, float viewDist)
{
	float FovRadian = 1.0f / tanf(FovDeg * 0.5f / 180.0f * 3.14159f);
	matrix4x4 matrix;
	matrix.mat[0][0] = Aspect * FovRadian;
	matrix.mat[1][1] = FovRadian;
	matrix.mat[2][2] = viewDist / (viewDist - DistFromScrn);
	matrix.mat[3][2] = (-viewDist * DistFromScrn) / (viewDist - DistFromScrn);
	matrix.mat[2][3] = 1.0f;
	matrix.mat[3][3] = 0.0f;
	return matrix;
}

Graphics::matrix4x4 Graphics::MakePointAtMatrix(vec3D position, vec3D target, vec3D UPvec)
{
	vec3D ForwardNew = SubVectors(target, position);
	ForwardNew = Normalise(ForwardNew);

	float DotProd = DotProduct(UPvec, ForwardNew);

	vec3D a = MultVectorFloat(ForwardNew, DotProd);
	vec3D UPvecNew = SubVectors(UPvec, a);
	UPvecNew = Normalise(UPvecNew);

	vec3D RightNew = CrossProd(UPvecNew, ForwardNew);

	matrix4x4 matrix;
	matrix.mat[0][0] = RightNew.x;
	matrix.mat[0][1] = RightNew.y;
	matrix.mat[0][2] = RightNew.z;
	matrix.mat[0][3] = 0.0f;

	matrix.mat[1][0] = UPvecNew.x;
	matrix.mat[1][1] = UPvecNew.y;
	matrix.mat[1][2] = UPvecNew.z;
	matrix.mat[1][3] = 0.0f;

	matrix.mat[2][0] = ForwardNew.x;
	matrix.mat[2][1] = ForwardNew.y;
	matrix.mat[2][2] = ForwardNew.z;
	matrix.mat[2][3] = 0.0f;

	matrix.mat[3][0] = position.x;
	matrix.mat[3][1] = position.y;
	matrix.mat[3][2] = position.z;
	matrix.mat[3][3] = 1.0f;
	return matrix;
}

Graphics::vec3D Graphics::AddVectors(vec3D& vec1, vec3D& vec2)
{
	return { vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z };
}

Graphics::vec3D Graphics::SubVectors(vec3D& vec1, vec3D& vec2)
{
	return { vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z };
}

Graphics::vec3D Graphics::MultVectorFloat(vec3D& vec1, float& mult)
{
	return { vec1.x * mult, vec1.y * mult, vec1.z * mult };
}

Graphics::vec3D Graphics::MultVectorVector(vec3D& vec1, vec3D& vec2)
{
	return { vec1.x * vec2.x, vec1.y * vec2.y, vec1.z * vec2.z };
}

Graphics::vec3D Graphics::DivVector(vec3D& vec1, float& div)
{
	return { vec1.x / div, vec1.y / div, vec1.z / div };
}

float Graphics::DotProduct(vec3D& vec1, vec3D &vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

float Graphics::VectorLength(vec3D& vec)
{
	return sqrtf(DotProduct(vec, vec));
}

Graphics::vec3D Graphics::Normalise(vec3D& vec)
{
	float length = VectorLength(vec);
	return { vec.x / length, vec.y / length, vec.z / length, };
}

Graphics::vec3D Graphics::CrossProd(vec3D& vec1, vec3D& vec2)
{
	vec3D vec;
	vec.x = vec1.y * vec2.z - vec1.z * vec2.y;
	vec.y = vec1.z * vec2.x - vec1.x * vec2.z;
	vec.z = vec1.x * vec2.y - vec1.y * vec2.x;
	return vec;
}

Graphics::vec3D Graphics::PlaneIntersect(vec3D& PlanePoint, vec3D& PlaneNormal, vec3D& StartOfLine, vec3D& EndOfLine, float& t)
{
	PlaneNormal = Normalise(PlaneNormal);
	float PlaneDotProd = -DotProduct(PlaneNormal, PlanePoint);
	float ad = DotProduct(StartOfLine, PlaneNormal);
	float bd = DotProduct(EndOfLine, PlaneNormal);
	t = (-PlaneDotProd - ad) / (bd - ad);
	vec3D StartOfLinetoEnd = SubVectors(EndOfLine, StartOfLine);
	vec3D IntersectToLine = MultVectorFloat(StartOfLinetoEnd, t);
	return AddVectors(StartOfLine, IntersectToLine);
}

uint16_t Graphics::TrianglePlaneClip(vec3D PlanePoint, vec3D PlaneNormal, triangle& InputTriangle, triangle& OutputTriangle1, triangle& OutputTriangle2)
{
	PlaneNormal = Normalise(PlaneNormal);

	auto distance = [&](vec3D& point)
	{
		vec3D normal = Normalise(point);
		return(PlaneNormal.x * point.x + PlaneNormal.y * point.y + PlaneNormal.z * point.z - DotProduct(PlaneNormal, PlanePoint));
	};

	vec3D* InPoints[3];
	uint8_t InPointCount = 0;

	vec3D* OutPoints[3];
	uint8_t OutPointCount = 0;

	vec2D* InTexPoints[3];
	uint8_t InTexPointCount = 0;

	vec2D* OutTexPoints[3];
	uint8_t OutTexPointCount = 0;

	float dist0 = distance(InputTriangle.vectors[0]);
	float dist1 = distance(InputTriangle.vectors[1]);
	float dist2 = distance(InputTriangle.vectors[2]);

	if (dist0 >= 0)
	{
		InPoints[InPointCount++] = &InputTriangle.vectors[0];
		InTexPoints[InTexPointCount++] = &InputTriangle.texCoord[0];
	}
	else
	{
		OutPoints[OutPointCount++] = &InputTriangle.vectors[0];
		OutTexPoints[OutTexPointCount++] = &InputTriangle.texCoord[0];
	}
	
	if (dist1 >= 0)
	{
		InPoints[InPointCount++] = &InputTriangle.vectors[1];
		InTexPoints[InTexPointCount++] = &InputTriangle.texCoord[1];
	}
	else
	{
		OutPoints[OutPointCount++] = &InputTriangle.vectors[1];
		OutTexPoints[OutTexPointCount++] = &InputTriangle.texCoord[1];
	}
	
	if (dist2 >= 0)
	{
		InPoints[InPointCount++] = &InputTriangle.vectors[2];
		InTexPoints[InTexPointCount++] = &InputTriangle.texCoord[2];
	}
	else
	{
		OutPoints[OutPointCount++] = &InputTriangle.vectors[2];
		OutTexPoints[OutTexPointCount++] = &InputTriangle.texCoord[2];
	}

	float t;

	if (InPointCount == 0)
	{
		return 0;
	}

	else if (InPointCount == 3)
	{
		OutputTriangle1 = InputTriangle;
		return 1;
	}

	else if (InPointCount == 1 && OutPointCount == 2)
	{
		//OutputTriangle1.color = InputTriangle.color;
		OutputTriangle1.color = { 0.0f, 0.0f, 1.0f };

		OutputTriangle1.vectors[0] = *InPoints[0];
		OutputTriangle1.texCoord[0] = *InTexPoints[0];

		OutputTriangle1.vectors[1] = PlaneIntersect(PlanePoint, PlaneNormal, *InPoints[0], *OutPoints[0], t);

		OutputTriangle1.texCoord[1].u = t * (OutTexPoints[0]->u - InTexPoints[0]->u) + InTexPoints[0]->u;
		OutputTriangle1.texCoord[1].v = t * (OutTexPoints[0]->v - InTexPoints[0]->v) + InTexPoints[0]->v;
		//OutputTriangle1.texCoord[1].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;

		OutputTriangle1.vectors[2] = PlaneIntersect(PlanePoint, PlaneNormal, *InPoints[0], *OutPoints[1], t);
		OutputTriangle1.texCoord[2].u = t * (OutTexPoints[1]->u - InTexPoints[0]->u) + InTexPoints[0]->u;
		OutputTriangle1.texCoord[2].v = t * (OutTexPoints[1]->v - InTexPoints[0]->v) + InTexPoints[0]->v;
		//out_tri1.t[2].w = t * (outside_tex[1]->w - inside_tex[0]->w) + inside_tex[0]->w;

		return 1;
	}

	else if (InPointCount == 2 && OutPointCount == 1)
	{
		//OutputTriangle1.color = InputTriangle.color;
		//OutputTriangle2.color = InputTriangle.color;

		OutputTriangle1.color.r = 1.0f;
		OutputTriangle1.color.g = 0.0f;
		OutputTriangle1.color.b = 0.0f;

		OutputTriangle2.color.r = 0.0f;
		OutputTriangle2.color.g = 1.0f;
		OutputTriangle2.color.b = 0.0f;

		OutputTriangle1.vectors[0] = *InPoints[0];
		OutputTriangle1.vectors[1] = *InPoints[1];
		OutputTriangle1.texCoord[0] = *InTexPoints[0];
		OutputTriangle1.texCoord[1] = *InTexPoints[1];
		OutputTriangle1.vectors[2] = PlaneIntersect(PlanePoint, PlaneNormal, *InPoints[0], *OutPoints[0], t);
		OutputTriangle1.texCoord[2].u = t * (OutTexPoints[0]->u - InTexPoints[0]->u) + InTexPoints[0]->u;
		OutputTriangle1.texCoord[2].v = t * (OutTexPoints[0]->v - InTexPoints[0]->v) + InTexPoints[0]->v;
		
		
		OutputTriangle2.vectors[0] = *InPoints[1];
		OutputTriangle2.texCoord[0] = *InTexPoints[1];
		OutputTriangle2.vectors[1] = OutputTriangle1.vectors[2];
		OutputTriangle2.texCoord[1] = OutputTriangle1.texCoord[2];

		OutputTriangle2.vectors[2] = PlaneIntersect(PlanePoint, PlaneNormal, *InPoints[1], *OutPoints[0], t);
		OutputTriangle2.texCoord[2].u = t * (OutTexPoints[0]->u - InTexPoints[1]->u) + InTexPoints[1]->u;
		OutputTriangle2.texCoord[2].v = t * (OutTexPoints[0]->v - InTexPoints[1]->v) + InTexPoints[1]->v;
		
		return 2;
	}
}

void Graphics::ClearScreen(float r, float g, float b)
{
	rendertarget->Clear(D2D1::ColorF(r, g, b));
};


void Graphics::DrawFlatTop(vec3D& point0, vec3D& point1, vec3D& point2)
{
	//Calculate Slopes
	//==========================================================================================================================
	const float slope0 = (point2.x - point0.x) / (point2.y - point0.y);
	const float slope1 = (point2.x - point1.x) / (point2.y - point1.y);
	//==========================================================================================================================

	//Calculate Start and End Y
	//==========================================================================================================================
	const int32_t StartY = (int16_t)ceil(point0.y - 0.5f);
	const int32_t EndY = (int16_t)ceil(point2.y - 0.5f);
	//==========================================================================================================================


	for (int32_t y = StartY; y < EndY; y++)
	{
		vec3D Start;
		vec3D End;

		Start.y = y;
		End.y = y;

		const float pointx0 = slope0 * (float(y) + 0.5f - point0.y) + point0.x;
		const float pointx1 = slope1 * (float(y) + 0.5f - point1.y) + point1.x;

		//Calculate Start and End X
		//==========================================================================================================================
		Start.x = (int32_t)ceil(pointx0 - 0.05f);
		End.x = (int32_t)ceil(pointx1 - 0.05f);
		//==========================================================================================================================

		rendertarget->DrawLine(D2D1::Point2F(Start.x, Start.y), D2D1::Point2F(End.x, End.y), Solidbrush, 2.0f);	//Draw Lines
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

		rendertarget->DrawLine(D2D1::Point2F(Start.x, Start.y), D2D1::Point2F(End.x, End.y), Solidbrush, 2.0f);
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
	triangle TransformedTri;
	triangle ViewedTri;
	triangle ProjectedTri;

	vec3D normal;
	vec3D line1;
	vec3D line2;
	
	matrix4x4 RotZMatrix = MakeZrotationMatrix(theta);

	matrix4x4 RotXMatrix = MakeXrotationMatrix(0.0f);

	matrix4x4 TransMatrix = MakeTranslationMatrix(0.0f, 0.0f, 8.0f);

	matrix4x4 WorldMatrix = MakeIdentityMarix();
	WorldMatrix = MatrixMatrixMultiplication(RotZMatrix, RotXMatrix);
	WorldMatrix = MatrixMatrixMultiplication(WorldMatrix, TransMatrix);

	vec3D target = { 0.0f,0.0f,1.0f };

	matrix4x4 CamRotYMatrix = MakeYrotationMatrix(camera.TargetRot.y);
	vec3D lookDir = MatrixVectorMultiplication(target, CamRotYMatrix);
	target = AddVectors(Graphics::camera.GlobalPos, lookDir);
	matrix4x4 CamMatrix = MakePointAtMatrix(Graphics::camera.GlobalPos, target, Graphics::UpVec);


	float temp = 0.1f;

	vec3D tempVec = MultVectorFloat(lookDir, Graphics::camera.LocalPosDelta.z);

	Graphics::camera.GlobalPos = Graphics::AddVectors(Graphics::camera.GlobalPos, tempVec);
	Graphics::camera.GlobalPos.x += Graphics::camera.LocalPosDelta.x;

	matrix4x4 ViewMatrix = MatrixInvertQuick(CamMatrix);

	vector<triangle> TriangleToRasterVector;

	//Draw Triangles
	//==========================================================================================================================
	for (auto tri : mesh.tri)
	{
		TransformedTri.vectors[0] = MatrixVectorMultiplication(tri.vectors[0], WorldMatrix);
		TransformedTri.vectors[1] = MatrixVectorMultiplication(tri.vectors[1], WorldMatrix);
		TransformedTri.vectors[2] = MatrixVectorMultiplication(tri.vectors[2], WorldMatrix);

		TransformedTri.texCoord[0] = tri.texCoord[0];
		TransformedTri.texCoord[1] = tri.texCoord[1];
		TransformedTri.texCoord[2] = tri.texCoord[2];

		//Normal Calculation
		//==========================================================================================================================
		line1 = SubVectors(TransformedTri.vectors[1], TransformedTri.vectors[0]);
		line2 = SubVectors(TransformedTri.vectors[2], TransformedTri.vectors[0]);

		normal = CrossProd(line1, line2);

		normal = Normalise(normal);
		//==========================================================================================================================

		vec3D CameraRay = SubVectors(TransformedTri.vectors[0], camera.GlobalPos);	//Get a ray from triangle to camera

		if (DotProduct(normal, CameraRay) < 0.0f)
		{
			//World to viewspace
			//==========================================================================================================================
			ViewedTri.vectors[1] = MatrixVectorMultiplication(TransformedTri.vectors[1], ViewMatrix);
			ViewedTri.vectors[0] = MatrixVectorMultiplication(TransformedTri.vectors[0], ViewMatrix);
			ViewedTri.vectors[2] = MatrixVectorMultiplication(TransformedTri.vectors[2], ViewMatrix);

			ViewedTri.texCoord[0] = TransformedTri.texCoord[0];
			ViewedTri.texCoord[1] = TransformedTri.texCoord[1];
			ViewedTri.texCoord[2] = TransformedTri.texCoord[2];

			//==========================================================================================================================

			//Clipping
			//==========================================================================================================================
			uint8_t ClippedTriCount = 0;
			triangle ClippedTri[2];

			ClippedTriCount = TrianglePlaneClip({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, ViewedTri, ClippedTri[0], ClippedTri[1]);
			//==========================================================================================================================

			for (uint8_t n = 0; n < ClippedTriCount; n++)
			{

				//lighting
				//==========================================================================================================================
				//Normalize light
				vec3D LightDirGlobal = { Graphics::globalLight.Direction.x, Graphics::globalLight.Direction.y, Graphics::globalLight.Direction.z };
				LightDirGlobal = Normalise(LightDirGlobal);

				float DotProduct = max(0.1f, Graphics::DotProduct(LightDirGlobal, normal));

				ProjectedTri.color = { ClippedTri[n].color.r * DotProduct, ClippedTri[n].color.g * DotProduct, ClippedTri[n].color.b * DotProduct, ClippedTri[n].color.a};
				//==========================================================================================================================

				//Projection Matrix Multiplication
				//==========================================================================================================================
				ProjectedTri.vectors[0] = MatrixVectorMultiplication(ClippedTri[n].vectors[0], Graphics::ProjMatrix);
				ProjectedTri.vectors[1] = MatrixVectorMultiplication(ClippedTri[n].vectors[1], Graphics::ProjMatrix);
				ProjectedTri.vectors[2] = MatrixVectorMultiplication(ClippedTri[n].vectors[2], Graphics::ProjMatrix);

				ProjectedTri.texCoord[0] = ClippedTri[n].texCoord[0];
				ProjectedTri.texCoord[1] = ClippedTri[n].texCoord[1];
				ProjectedTri.texCoord[2] = ClippedTri[n].texCoord[2];
				//==========================================================================================================================

				ProjectedTri.vectors[0] = DivVector(ProjectedTri.vectors[0], ProjectedTri.vectors[0].w);
				ProjectedTri.vectors[1] = DivVector(ProjectedTri.vectors[1], ProjectedTri.vectors[1].w);
				ProjectedTri.vectors[2] = DivVector(ProjectedTri.vectors[2], ProjectedTri.vectors[2].w);

				//Scaling
				//==========================================================================================================================
				vec3D ViewOffset = { 1.0f, 1.0f, 0.0f };

				ProjectedTri.vectors[0] = AddVectors(ProjectedTri.vectors[0], ViewOffset);
				ProjectedTri.vectors[1] = AddVectors(ProjectedTri.vectors[1], ViewOffset);
				ProjectedTri.vectors[2] = AddVectors(ProjectedTri.vectors[2], ViewOffset);

				ProjectedTri.vectors[0].x *= 0.5f * (float)Graphics::Resolution[1];
				ProjectedTri.vectors[0].y *= 0.5f * (float)Graphics::Resolution[0];

				ProjectedTri.vectors[1].x *= 0.5f * (float)Graphics::Resolution[1];
				ProjectedTri.vectors[1].y *= 0.5f * (float)Graphics::Resolution[0];

				ProjectedTri.vectors[2].x *= 0.5f * (float)Graphics::Resolution[1];
				ProjectedTri.vectors[2].y *= 0.5f * (float)Graphics::Resolution[0];
				//==========================================================================================================================

				TriangleToRasterVector.push_back(ProjectedTri);

				//DrawTriangle2filled(ProjectedTri, color);
			}
		}
	}

	sort(TriangleToRasterVector.begin(), TriangleToRasterVector.end(), [](triangle& tri1, triangle& tri2)
		{
			float z1 = (tri1.vectors[0].z, tri1.vectors[1].z, tri1.vectors[2].z) / 3.0f;
			float z2 = (tri2.vectors[0].z, tri2.vectors[1].z, tri2.vectors[2].z) / 3.0f;
			return z1 > z2;
		});

	for (auto& TriToRast : TriangleToRasterVector)
	{
		triangle clippedTri[2];
		list<triangle> TriangleList;
		TriangleList.push_back(TriToRast);
		uint8_t newTriCount = 1;
		uint8_t TriAddCount = 0;

		for (uint8_t Case = 0; Case < 4; Case++)
		{
			while (newTriCount > 0)
			{
				triangle testTri = TriangleList.front();
				TriangleList.pop_front();
				newTriCount--;

				switch (Case)
				{
					case 0:
					{
						TriAddCount = TrianglePlaneClip({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, testTri, clippedTri[0], clippedTri[1]);
						break;
					}
					case 1:
					{
						TriAddCount = TrianglePlaneClip({ 0.0f, (float)Resolution[0] - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, testTri, clippedTri[0], clippedTri[1]);
						break;
					}
					case 2:
					{
						TriAddCount = TrianglePlaneClip({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, testTri, clippedTri[0], clippedTri[1]);
						break;
					}
					case 3:
					{
						TriAddCount = TrianglePlaneClip({ (float)Resolution[1] - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, testTri, clippedTri[0], clippedTri[1]);
						break;
					}
				}

				for (uint8_t w = 0; w < TriAddCount; w++)
					TriangleList.push_back(clippedTri[w]);
			}
			newTriCount = TriangleList.size();
		}
		for (auto& Tri : TriangleList)
		{
			DrawTriangle2filled(Tri, Tri.color);
		}
	}
	//==========================================================================================================================
};

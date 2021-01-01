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

Graphics::Alpha_DepthBuff::Alpha_DepthBuff(uint16_t Width, uint16_t Height)
{
	DepthPtr = new float[Width * Height];
	AlphaPtr = new float[Width * Height];

	width = Width;
	height = Height;
}

Graphics::Alpha_DepthBuff::~Alpha_DepthBuff()
{
	delete[] DepthPtr;
	delete[] AlphaPtr;

	DepthPtr = nullptr;
	AlphaPtr = nullptr;
}

void Graphics::Alpha_DepthBuff::putDepth(uint16_t& x, uint16_t& y, float& Depth)
{
	DepthPtr[y * width + x] = Depth;
}

void Graphics::Alpha_DepthBuff::putAlpha(uint16_t& x, uint16_t& y, float& Alpha)
{
	DepthPtr[y * width + x] = Alpha;
}

float Graphics::Alpha_DepthBuff::getDepth(uint16_t& x, uint16_t& y)
{
	return DepthPtr[y * width + x];
}

float Graphics::Alpha_DepthBuff::getAlpha(uint16_t& x, uint16_t& y)
{
	return DepthPtr[y * width + x];
}

Graphics::Graphics()
{
	factory = NULL;
	rendertarget = NULL;
	Solidbrush = NULL;
	BufferBmp = NULL;
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


bool Graphics::Init(HWND windowHandle, uint16_t width, uint16_t height, float FOV, float DistancefromScreen, float ViewingDistance)
{
	HRESULT res = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);
	if (res != S_OK)
	{
		return false;
	}

	RECT resolution;
	GetClientRect(windowHandle, &resolution);

	/*Graphics::Resolution[0] = resolution.bottom;
	Graphics::Resolution[1] = resolution.right;*/

	Graphics::Resolution.height = resolution.bottom;
	Graphics::Resolution.width = resolution.right;

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

	Graphics::ProjMatrix = MakeProjectionMatrix(90.0f, (float)Graphics::Resolution.height / (float)Graphics::Resolution.width, 0.1f, 1000.0f);

	return true;
}

bool Graphics::BitMap::LoadBitmapRGB(const char *filename, bool invert)
{
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	FILE *bitmap;

	unsigned char* pointer;

	struct TempRGB
	{
		uint8_t b = 0;
		uint8_t g = 0;
		uint8_t r = 0;
	};

	TempRGB tempRGB;

	unsigned char *PixPoint;

	fopen_s(&bitmap, filename, "rb");

	if (bitmap == 0)
	{
		return false;
	}

	fread(&fileHeader, sizeof(fileHeader), 1, bitmap);	//get fileheader data
	fread(&infoHeader, sizeof(infoHeader), 1, bitmap);	//get infoheader data

	MapResolution[0] = infoHeader.biWidth;
	MapResolution[1] = infoHeader.biHeight;

	uint64_t size = infoHeader.biWidth * infoHeader.biHeight;

	PixPoint = new unsigned char[size * 3];

	fseek(bitmap, fileHeader.bfOffBits, 0);

	uint16_t sizeRGB = sizeof(TempRGB);

	Color tempCol;
	vector<Color> TempVecX;
	vector<vector<Color>> TempVecY;

	for (uint16_t Y = 0; Y < infoHeader.biHeight; Y++)
	{
		TempVecX.clear();
		for (uint16_t X = 0; X < infoHeader.biWidth; X++)
		{
			fread(&tempRGB, sizeRGB, 1, bitmap);
			if (tempRGB.r == 0.0f && tempRGB.g == 0 && tempRGB.b == 0)
			{
				while(0);
			}
			tempCol = { (float)tempRGB.r / 255.0f, (float)tempRGB.g / 255.0f, (float)tempRGB.b / 255.0f, 1.0f };
			TempVecX.push_back(tempCol);
		}
		TempVecY.push_back(TempVecX);
	}

	if (invert)
	{
		for (uint16_t Y = 0; Y < infoHeader.biHeight; Y++)
		{
			Pixels.push_back(TempVecY[Y]);	//Invert Y of Image to make it upright
		}
	}
	else
	{
		for (uint16_t Y = infoHeader.biHeight; Y > 0; Y--)
		{
			Pixels.push_back(TempVecY[Y - 1]);	//Invert Y of Image to make it upright
		}
	}

	fclose(bitmap);
	//while (1);
	return true;
}

bool Graphics::BitMap::LoadBitmapAlpha(const char* filename, bool invert)
{
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	FILE* bitmap;

	unsigned char* pointer;

	struct TempRGB
	{
		uint8_t b = 0;
		uint8_t g = 0;
		uint8_t r = 0;
	};

	TempRGB tempRGB;

	unsigned char* PixPoint;

	fopen_s(&bitmap, filename, "rb");

	if (bitmap == 0)
	{
		return false;
	}

	fread(&fileHeader, sizeof(fileHeader), 1, bitmap);	//get fileheader data
	fread(&infoHeader, sizeof(infoHeader), 1, bitmap);	//get infoheader data

	MapResolution[0] = infoHeader.biWidth;
	MapResolution[1] = infoHeader.biHeight;

	uint64_t size = infoHeader.biWidth * infoHeader.biHeight;

	PixPoint = new unsigned char[size];

	fseek(bitmap, fileHeader.bfOffBits, 0);

	uint8_t Alpha;

	uint8_t sizeAlpha = sizeof(Alpha);

	Color tempCol;
	vector<Color> TempVecX;
	vector<vector<Color>> TempVecY;

	for (uint16_t Y = 0; Y < infoHeader.biHeight; Y++)
	{
		TempVecX.clear();
		for (uint16_t X = 0; X < infoHeader.biWidth; X++)
		{
			fread(&Alpha, sizeAlpha, 1, bitmap);
			if (tempRGB.r == 0.0f && tempRGB.g == 0 && tempRGB.b == 0)
			{
				while (0);
			}
			Pixels[Y][X].a = (float)Alpha / 255;
			TempVecX.push_back(tempCol);
		}
		TempVecY.push_back(TempVecX);
	}

	if (invert)
	{
		for (uint16_t Y = 0; Y < infoHeader.biHeight; Y++)
		{
			Pixels.push_back(TempVecY[Y]);	//Invert Y of Image to make it upright
		}
	}
	else
	{
		for (uint16_t Y = infoHeader.biHeight; Y > 0; Y--)
		{
			Pixels.push_back(TempVecY[Y - 1]);	//Invert Y of Image to make it upright
		}
	}

	fclose(bitmap);
	//while (1);
	return true;
}

Graphics::ImageBuff::ImageBuff(uint16_t Width, uint16_t Height)
{
	width = Width;
	height = Height;
	PixelsPtr = new uint32_t[width * height];
}

Graphics::ImageBuff::~ImageBuff()
{
	delete[] PixelsPtr;
	PixelsPtr = nullptr;
}

void Graphics::ImageBuff::PutPix(uint16_t& x, uint16_t& y, Color& col)
{
	//Convert Color
	//==========================================================================================================================
	uint32_t buffer = 0;
	buffer |= (uint32_t)(col.b * 255);
	buffer |= ((uint32_t)(col.g * 255)<<8);
	buffer |= ((uint32_t)(col.r * 255)<<16);
	//==========================================================================================================================
	PixelsPtr[(y * width) + x] = buffer;
}

Graphics::Color Graphics::ImageBuff::GetPix(uint16_t& x, uint16_t& y)
{
	//Convert Color
	//==========================================================================================================================
	Color ColOut;
	uint32_t buffer = PixelsPtr[(y * width) + x];
	uint32_t mask = 0x0000ff;

	if (buffer != 0)
	{
		while (0);
	}

	ColOut.b = (float)(buffer & 0x0000ff) / 255;
	ColOut.g = (float)((buffer & 0x00ff00)>>8) / 255;
	ColOut.r = (float)((buffer & 0xff0000)>>16) / 255;
	//==========================================================================================================================
	return ColOut;
}

bool Graphics::mesh::LoadFromObj(string filename, bool hasTexture)
{
	ifstream f(filename);
	if (!f.is_open())
	{
		return false;
	}

	vector<vec3D> verts; //vertice pool
	vector<vec2D> TexCoords; //Textur vertice pool

	while (!f.eof())
	{
		char line[128];
		f.getline(line, 128);

		strstream stream;
		stream << line;

		char Waste;

		if (line[0] == 'v')
		{
			if (line[1] == 't')
			{
				vec2D TempVert;
				stream >> Waste >> Waste >> TempVert.u >> TempVert.v;
				TexCoords.push_back(TempVert);
			}
			else
			{
				vec3D TempVert;
				stream >> Waste >> TempVert.x >> TempVert.y >> TempVert.z;
				verts.push_back(TempVert);
			}
		}

		if (!hasTexture)
		{
			if (line[0] == 'f')
			{
				uint64_t face[3] = { 0 };
				stream >> Waste >> face[0] >> face[1] >> face[2];
				tri.push_back({ verts[face[0] - 1], verts[face[1] - 1] , verts[face[2] - 1] });
			}
		}
		else
		{
			if (line[0] == 'f')
			{
				stream >> Waste;

				string tokens[6];
				int32_t nTokenCount = -1;


				while (!stream.eof())
				{
					char c = stream.get();
					if (c == ' ' || c == '/')
						nTokenCount++;
					else
						tokens[nTokenCount].append(1, c);
				}

				tokens[nTokenCount].pop_back();

				tri.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1], TexCoords[stoi(tokens[1]) - 1], TexCoords[stoi(tokens[3]) - 1], TexCoords[stoi(tokens[5]) - 1] });
			}
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
		OutputTriangle1.texCoord[1].w = t * (OutTexPoints[0]->w - InTexPoints[0]->w) + InTexPoints[0]->w;

		OutputTriangle1.vectors[2] = PlaneIntersect(PlanePoint, PlaneNormal, *InPoints[0], *OutPoints[1], t);
		OutputTriangle1.texCoord[2].u = t * (OutTexPoints[1]->u - InTexPoints[0]->u) + InTexPoints[0]->u;
		OutputTriangle1.texCoord[2].v = t * (OutTexPoints[1]->v - InTexPoints[0]->v) + InTexPoints[0]->v;
		OutputTriangle1.texCoord[2].w = t * (OutTexPoints[1]->w - InTexPoints[0]->w) + InTexPoints[0]->w;

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
		OutputTriangle1.texCoord[2].w = t * (OutTexPoints[0]->w - InTexPoints[0]->w) + InTexPoints[0]->w;
		
		
		OutputTriangle2.vectors[0] = *InPoints[1];
		OutputTriangle2.texCoord[0] = *InTexPoints[1];
		OutputTriangle2.vectors[1] = OutputTriangle1.vectors[2];
		OutputTriangle2.texCoord[1] = OutputTriangle1.texCoord[2];

		OutputTriangle2.vectors[2] = PlaneIntersect(PlanePoint, PlaneNormal, *InPoints[1], *OutPoints[0], t);
		OutputTriangle2.texCoord[2].u = t * (OutTexPoints[0]->u - InTexPoints[1]->u) + InTexPoints[1]->u;
		OutputTriangle2.texCoord[2].v = t * (OutTexPoints[0]->v - InTexPoints[1]->v) + InTexPoints[1]->v;
		OutputTriangle2.texCoord[2].w = t * (OutTexPoints[0]->w - InTexPoints[1]->w) + InTexPoints[1]->w;
		
		return 2;
	}
}

void Graphics::ClearScreen(float r, float g, float b, ImageBuff& imageBuff, Alpha_DepthBuff& AlphaDepthBuff)
{
	for (uint16_t Y = 0; Y < Resolution.height; Y++)
	{
		for (uint16_t X = 0; X < Resolution.width; X++)
		{
			float tmp = 0.0f;
			Color col = { r, g, b, 1.0f };
			imageBuff.PutPix(X, Y, col);
			AlphaDepthBuff.putDepth(X, Y, tmp);
			AlphaDepthBuff.putAlpha(X, Y, tmp);
		}
	}
	while (0);
};

void Graphics::DrawPixel(uint16_t &x, uint16_t&y, Color &col, ImageBuff& DepthBuffer)
{
	DepthBuffer.PutPix(x, y, col);
}

void Graphics::DrawLine(Point& p1, Point& p2, Color& col, ImageBuff& DepthBuffer)
{
	float Xmod = 0.0f;
	float Ymod = 0.0f;

	float CurX = 0.0f;
	uint16_t CurY = 0.0f;

	float endX = 0;
	uint16_t endY = 0;

	if (p1.y < p2.y)
	{
		endY = p2.y;
		CurY = p1.y;
		CurX = p1.x;
	}
	else
	{
		endY = p1.y;
		CurY = p2.y;
		CurX = p2.x;
	}

	int32_t DeltaX = p1.x - p2.x;
	uint16_t DeltaY = abs(p1.y - p2.y);

	uint16_t CurXu = 0;

	if (DeltaY != 0)
	{
		Xmod = (float)DeltaX / (float)DeltaY;
		do
		{
			CurY++;
			endX = CurX + Xmod;
			do
			{
				CurXu = CurX;
				if ((CurX < DepthBuffer.width) && (CurY < DepthBuffer.height))
				{
					DepthBuffer.PutPix(CurXu, CurY, col);
				}
				CurX += Xmod;
			} while (CurX != endX && CurX >= 0);
		} while (CurY < endY && CurY >= 0);
	}
	else
	{
		for (CurXu = CurX; CurXu != (CurX + DeltaX); CurXu += (abs(DeltaX) / DeltaX))
		{
			if ((CurX < DepthBuffer.width) && (CurY < DepthBuffer.height))
			{
				DepthBuffer.PutPix(CurXu, CurY, col);
			}
		}
	}
}

void Graphics::DrawFlatTop(vec3D& point0, vec3D& point1, vec3D& point2, Color col, ImageBuff& imageBuff)
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


	for (uint16_t y = StartY; y < EndY; y++)
	{
		vec3D Start;
		vec3D End;

		Start.y = y;
		End.y = y;

		const float pointx0 = slope0 * (float(y) + 0.5f - point0.y) + point0.x;
		const float pointx1 = slope1 * (float(y) + 0.5f - point1.y) + point1.x;

		//Calculate Start and End X
		//==========================================================================================================================
		Start.x = (uint16_t)ceil(pointx0 - 0.05f);
		End.x = (uint16_t)ceil(pointx1 - 0.05f);
		//==========================================================================================================================
		for (uint16_t x = Start.x; x < End.x; x++)
		{
			imageBuff.PutPix(x, y, col);
		}
		//rendertarget->DrawLine(D2D1::Point2F(Start.x, Start.y), D2D1::Point2F(End.x, End.y), Solidbrush, 2.0f);	//Draw Lines
	}
};

void Graphics::DrawFlatBottom(vec3D& point0, vec3D& point1, vec3D& point2, Color col, ImageBuff& imageBuff)
{
	const float slope0 = (point1.x - point0.x) / (point1.y - point0.y);
	const float slope1 = (point2.x - point0.x) / (point2.y - point0.y);

	const int32_t StartY = (int16_t)ceil(point0.y - 0.5f);
	const int32_t EndY = (int16_t)ceil(point2.y - 0.5f);

	for (uint16_t y = StartY; y < EndY; y++)
	{
		vec3D Start;
		vec3D End;

		Start.y = y;
		End.y = y;

		const float pointx0 = slope0 * (float(y) + 0.5f - point0.y) + point0.x;
		const float pointx1 = slope1 * (float(y) + 0.5f - point0.y) + point0.x;

		Start.x = (uint16_t)ceil(pointx0 - 0.05f);
		End.x = (uint16_t)ceil(pointx1 - 0.05f);

		for (uint16_t x = Start.x; x < End.x; x++)
		{
			imageBuff.PutPix(x, y, col);
		}
		//rendertarget->DrawLine(D2D1::Point2F(Start.x, Start.y), D2D1::Point2F(End.x, End.y), Solidbrush, 2.0f);
	}
};

void Graphics::DrawTriangle(uint16_t& x1, uint16_t& y1, uint16_t& x2, uint16_t& y2, uint16_t& x3, uint16_t& y3, uint16_t& r, uint16_t& g, uint16_t& b, uint16_t& a, ImageBuff& imageBuff)
{
	Solidbrush->SetColor(D2D1::ColorF(r, g, b, a));
	rendertarget->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), Solidbrush);
	rendertarget->DrawLine(D2D1::Point2F(x2, y2), D2D1::Point2F(x3, y3), Solidbrush);
	rendertarget->DrawLine(D2D1::Point2F(x3, y3), D2D1::Point2F(x1, y1), Solidbrush);
};


void Graphics::DrawTriangle2(triangle Triangle, Color color, ImageBuff& imageBuff)
{
	for (char i = 0; i < 3; i++)
	{
		Point p1 = { Triangle.vectors[i].x, Triangle.vectors[i].y };
		Point p2 = { Triangle.vectors[(i + 1) % 3].x, Triangle.vectors[(i + 1) % 3].y };
		DrawLine(p1, p2, color, imageBuff);
	}
};


void Graphics::DrawTriangle2filled(triangle &Triangle, Color &color, ImageBuff& imageBuff, Alpha_DepthBuff& AlphaDepthBuff)
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
		DrawFlatTop(*vec0, *vec1, *vec2, color, imageBuff);
	}

	else if (vec1->y == vec2->y)	//flatbottom
	{
		if (vec2->x < vec1->x)	//vector sort by x value
		{
			std::swap(vec1, vec2);
		}
		DrawFlatBottom(*vec0, *vec1, *vec2, color, imageBuff);
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
			DrawFlatBottom(*vec0, *vec1, vecSplt, color, imageBuff);
			DrawFlatTop(*vec1, vecSplt, *vec2, color, imageBuff);
		}

		else	//triangle is major left
		{
			DrawFlatBottom(*vec0, vecSplt, *vec1, color, imageBuff);
			DrawFlatTop(vecSplt, *vec1, *vec2, color, imageBuff);
		}

	}
	//==========================================================================================================================

};

void Graphics::DrawTriangle2textured(triangle& Triangle, BitMap& texture, ImageBuff& imageBuff, Alpha_DepthBuff& AlphaDepthBuff)
{
	vec3D* vec0 = &Triangle.vectors[0];
	vec3D* vec1 = &Triangle.vectors[1];
	vec3D* vec2 = &Triangle.vectors[2];

	vec2D* tex0 = &Triangle.texCoord[0];
	vec2D* tex1 = &Triangle.texCoord[1];
	vec2D* tex2 = &Triangle.texCoord[2];

	//vector Sort by y value
	//==========================================================================================================================
	if (vec1->y < vec0->y)
	{
		std::swap(vec0, vec1);
		std::swap(tex0, tex1);
	}
	if (vec2->y < vec0->y)
	{
		std::swap(vec0, vec2);
		std::swap(tex0, tex2);
	}
	if (vec2->y < vec1->y)
	{
		std::swap(vec1, vec2);
		std::swap(tex1, tex2);
	}
	//==========================================================================================================================

	int32_t dy1 = (uint16_t)vec1->y - (uint16_t)vec0->y;
	int32_t dx1 = (uint16_t)vec1->x - (uint16_t)vec0->x;

	float dv1 = tex1->v - tex0->v;
	float du1 = tex1->u - tex0->u;
	float dw1 = tex1->w - tex0->w;


	int32_t dy2 = vec2->y - vec0->y;
	int32_t dx2 = vec2->x - vec0->x;

	float dv2 = tex2->v - tex0->v;
	float du2 = tex2->u - tex0->u;
	float dw2 = tex2->w - tex0->w;

	float Xstep1 = 0.0f;
	float Xstep2 = 0.0f;

	float Ustep1 = 0.0f;
	float Ustep2 = 0.0f;

	float Vstep1 = 0.0f;
	float Vstep2 = 0.0f;

	float Wstep1 = 0.0f;
	float Wstep2 = 0.0f;

	float TextureU = 0.0f;
	float TextureV = 0.0f;
	float TextureW = 0.0f;

	if (dy1)
	{
		Xstep1 = dx1 / (float)abs(dy1);
	}
	if (dy2)
	{
		Xstep2 = dx2 / (float)abs(dy2);
	}

	if (dy1)
	{
		Ustep1 = du1 / (float)abs(dy1);
	}
	if (dy1)
	{
		Vstep1 = dv1 / (float)abs(dy1);
	}
	if (dy1)
	{
		Wstep1 = dw1 / (float)abs(dy1);
	}

	if (dy2)
	{
		Ustep2 = du2 / (float)abs(dy2);
	}
	if (dy2)
	{
		Vstep2 = dv2 / (float)abs(dy2);
	}
	if (dy2)
	{
		Wstep2 = dw2 / (float)abs(dy2);
	}

	Color color;

	if (dy1)
	{
		for (uint16_t y = vec0->y; y <= vec1->y; y++)
		{
			int32_t xStart = (uint16_t)vec0->x + (float)(y - vec0->y) * Xstep1;
			int32_t xEnd = (uint16_t)vec0->x + (float)(y - vec0->y) * Xstep2;

			float uStart = tex0->u + (float)(y - vec0->y) * Ustep1;
			float uEnd = tex0->u + (float)(y - vec0->y) * Ustep2;

			float vStart = tex0->v + (float)(y - vec0->y) * Vstep1;
			float vEnd = tex0->v + (float)(y - vec0->y) * Vstep2;

			float wStart = tex0->w + (float)(y - vec0->y) * Wstep1;
			float wEnd = tex0->w + (float)(y - vec0->y) * Wstep2;

			if (xStart > xEnd)
			{
				std::swap(xStart, xEnd);
				std::swap(uStart, uEnd);
				std::swap(vStart, vEnd);
				std::swap(wStart, wEnd);
			}

			TextureU = uStart;
			TextureV = vStart;
			TextureW = wStart;

			float tStep = 1.0f / ((float)(xEnd - xStart));
			float t = 0.0f;

			for (uint16_t x = xStart; x <= xEnd; x++)
			{
				TextureU = (1.0f - t) * uStart + t * uEnd;
				TextureV = (1.0f - t) * vStart + t * vEnd;
				TextureW = (1.0f - t) * wStart + t * wEnd;
				Color col = { 1.0f, 0.0f, 0.0f, };

				uint16_t temp1 = (uint16_t)((TextureU / TextureW) * (texture.MapResolution[0] - 1)) % texture.MapResolution[0];
				uint16_t temp2 = (uint16_t)((TextureV / TextureW) * (texture.MapResolution[1] - 1)) % texture.MapResolution[1];

				if ((TextureV < 2 && TextureV >= 0) && (TextureU < 2 && TextureU >= 0))
				{
					if (TextureW > AlphaDepthBuff.getDepth(x, y) || AlphaDepthBuff.getAlpha(x, y) != 1)
					{
						//imageBuff.PutPix(temp1, temp2, col);
						color = texture.Pixels[(uint16_t)((TextureV / TextureW) * (texture.MapResolution[1] - 1)) % texture.MapResolution[1]][(uint16_t)((TextureU / TextureW) * (texture.MapResolution[0] - 1)) % texture.MapResolution[0]];

						color.r = (color.r * color.a) + (imageBuff.GetPix(x, y).r * (1 - color.a));
						color.g = (color.g * color.a) + (imageBuff.GetPix(x, y).g * (1 - color.a));
						color.b = (color.b * color.a) + (imageBuff.GetPix(x, y).b * (1 - color.a));

						imageBuff.PutPix(x, y, color);
						AlphaDepthBuff.putDepth(x, y, TextureW);
						AlphaDepthBuff.putAlpha(x, y, color.a);
					}
				}
				t += tStep;
			}
		}
	}
	dy1 = (uint16_t)vec2->y - (uint16_t)vec1->y;
	dx1 = (uint16_t)vec2->x - (uint16_t)vec1->x;
	du1 = tex2->u - tex1->u;
	dv1 = tex2->v - tex1->v;
	dw1 = tex2->w - tex1->w;

	if (dy1)
	{
		Xstep1 = dx1 / (float)abs(dy1);
	}
	if (dy2)
	{
		Xstep2 = dx2 / (float)abs(dy2);
	}

	Ustep1 = 0.0f;
	Vstep1 = 0.0f;

	if (dy1)
	{
		Ustep1 = du1 / (float)abs(dy1);
	}
	if (dy1)
	{
		Vstep1 = dv1 / (float)abs(dy1);
	}
	if (dy1)
	{
		Wstep1 = dw1 / (float)abs(dy1);
	}
	if (dy1)
	{
		for (uint16_t y = vec1->y; y <= vec2->y; y++)
		{
			int32_t xStart = (uint16_t)vec1->x + (float)(y - (uint16_t)vec1->y) * Xstep1;
			int32_t xEnd = (uint16_t)vec0->x + (float)(y - (uint16_t)vec0->y) * Xstep2;

			float uStart = tex1->u + (float)(y - vec1->y) * Ustep1;
			float uEnd = tex0->u + (float)(y - vec0->y) * Ustep2;

			float vStart = tex1->v + (float)(y - vec1->y) * Vstep1;
			float vEnd = tex0->v + (float)(y - vec0->y) * Vstep2;

			float wStart = tex1->w + (float)(y - vec1->y) * Wstep1;
			float wEnd = tex0->w + (float)(y - vec0->y) * Wstep2;

			if (xStart > xEnd)
			{
				std::swap(xStart, xEnd);
				std::swap(uStart, uEnd);
				std::swap(vStart, vEnd);
				std::swap(wStart, wEnd);
			}

			TextureU = uStart;
			TextureV = vStart;
			TextureW = wStart;

			float tStep = 1.0f / ((float)(xEnd - xStart));
			float t = 0.0f;

			for (uint16_t x = xStart; x <= xEnd; x++)
			{
				TextureU = (1.0f - t) * uStart + t * uEnd;
				TextureV = (1.0f - t) * vStart + t * vEnd;
				TextureW = (1.0f - t) * wStart + t * wEnd;

				Color col = { 0.0f, 1.0f, 0.0f, };

				uint16_t temp1 = (uint16_t)((TextureU / TextureW) * (texture.MapResolution[0] - 1)) % texture.MapResolution[0];
				uint16_t temp2 = (uint16_t)((TextureV / TextureW) * (texture.MapResolution[1] - 1)) % texture.MapResolution[1];

				if ((TextureV < 2 && TextureV >= 0) && (TextureU < 2 && TextureU >= 0))
				{
					if (TextureW > AlphaDepthBuff.getDepth(x, y) || AlphaDepthBuff.getAlpha(x, y) != 1)
					{
						//imageBuff.PutPix(temp1, temp2, col);
						color = texture.Pixels[(uint16_t)((TextureV / TextureW) * (texture.MapResolution[1] - 1)) % texture.MapResolution[1]][(uint16_t)((TextureU / TextureW) * (texture.MapResolution[0] - 1)) % texture.MapResolution[0]];

						color.r = (color.r * color.a) + (imageBuff.GetPix(x, y).r * (1 - color.a));
						color.g = (color.g * color.a) + (imageBuff.GetPix(x, y).g * (1 - color.a));
						color.b = (color.b * color.a) + (imageBuff.GetPix(x, y).b * (1 - color.a));

						imageBuff.PutPix(x, y, color);
						AlphaDepthBuff.putDepth(x, y, TextureW);
						AlphaDepthBuff.putAlpha(x, y, color.a);
					}
				}
				t += tStep;
			}
		}
	}
};


void Graphics::DrawMesh(mesh mesh, Color color, ImageBuff& imageBuff, Alpha_DepthBuff& AlphaDepthBuff)
{
	triangle TransformedTri;
	triangle ViewedTri;
	triangle ProjectedTri;

	vec3D normal;
	vec3D line1;
	vec3D line2;
	
	matrix4x4 RotZMatrix = MakeZrotationMatrix(theta);

	matrix4x4 RotXMatrix = MakeXrotationMatrix(0.0f);

	matrix4x4 TransMatrix = MakeTranslationMatrix(mesh.WorldPos.x, mesh.WorldPos.z, mesh.WorldPos.y);

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

				ProjectedTri.vectors[0].x *= 0.5f * (float)Graphics::Resolution.width;
				ProjectedTri.vectors[0].y *= 0.5f * (float)Graphics::Resolution.height;

				ProjectedTri.vectors[1].x *= 0.5f * (float)Graphics::Resolution.width;
				ProjectedTri.vectors[1].y *= 0.5f * (float)Graphics::Resolution.height;

				ProjectedTri.vectors[2].x *= 0.5f * (float)Graphics::Resolution.width;
				ProjectedTri.vectors[2].y *= 0.5f * (float)Graphics::Resolution.height;
				//==========================================================================================================================

				TriangleToRasterVector.push_back(ProjectedTri);
				//DrawTriangle2filled(ProjectedTri, color, Buffer);
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
						TriAddCount = TrianglePlaneClip({ 0.0f, (float)Resolution.height - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, testTri, clippedTri[0], clippedTri[1]);
						break;
					}
					case 2:
					{
						TriAddCount = TrianglePlaneClip({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, testTri, clippedTri[0], clippedTri[1]);
						break;
					}
					case 3:
					{
						TriAddCount = TrianglePlaneClip({ (float)Resolution.width - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, testTri, clippedTri[0], clippedTri[1]);
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
			Color col = { 0,0,0,0 };
			//DrawTriangle2(Tri, col, imageBuff);
			DrawTriangle2filled(Tri, Tri.color, imageBuff, AlphaDepthBuff);
		}
	}
	//==========================================================================================================================
};

void Graphics::DrawMeshTextured(mesh mesh, BitMap& texture, ImageBuff& imageBuff, Alpha_DepthBuff& AlphaDepthBuff)
{
	triangle TransformedTri;
	triangle ViewedTri;
	triangle ProjectedTri;

	vec3D normal;
	vec3D line1;
	vec3D line2;

	matrix4x4 RotZMatrix = MakeZrotationMatrix(theta);

	matrix4x4 RotXMatrix = MakeXrotationMatrix(0.0f);

	matrix4x4 TransMatrix = MakeTranslationMatrix(mesh.WorldPos.x, mesh.WorldPos.z, mesh.WorldPos.y);

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
	Graphics::camera.GlobalPos.y += Graphics::camera.LocalPosDelta.y;

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

				ProjectedTri.color = { ClippedTri[n].color.r * DotProduct, ClippedTri[n].color.g * DotProduct, ClippedTri[n].color.b * DotProduct, ClippedTri[n].color.a };
				//==========================================================================================================================

				//Projection Matrix Multiplication
				//==========================================================================================================================
				ProjectedTri.vectors[0] = MatrixVectorMultiplication(ClippedTri[n].vectors[0], Graphics::ProjMatrix);
				ProjectedTri.vectors[1] = MatrixVectorMultiplication(ClippedTri[n].vectors[1], Graphics::ProjMatrix);
				ProjectedTri.vectors[2] = MatrixVectorMultiplication(ClippedTri[n].vectors[2], Graphics::ProjMatrix);
				//==========================================================================================================================

				ProjectedTri.texCoord[0] = ClippedTri[n].texCoord[0];
				ProjectedTri.texCoord[1] = ClippedTri[n].texCoord[1];
				ProjectedTri.texCoord[2] = ClippedTri[n].texCoord[2];

				ProjectedTri.texCoord[0].u = ProjectedTri.texCoord[0].u / ProjectedTri.vectors[0].w;
				ProjectedTri.texCoord[1].u = ProjectedTri.texCoord[1].u / ProjectedTri.vectors[1].w;
				ProjectedTri.texCoord[2].u = ProjectedTri.texCoord[2].u / ProjectedTri.vectors[2].w;

				ProjectedTri.texCoord[0].v = ProjectedTri.texCoord[0].v / ProjectedTri.vectors[0].w;
				ProjectedTri.texCoord[1].v = ProjectedTri.texCoord[1].v / ProjectedTri.vectors[1].w;
				ProjectedTri.texCoord[2].v = ProjectedTri.texCoord[2].v / ProjectedTri.vectors[2].w;

				ProjectedTri.texCoord[0].w = 1.0f / ProjectedTri.vectors[0].w;
				ProjectedTri.texCoord[1].w = 1.0f / ProjectedTri.vectors[1].w;
				ProjectedTri.texCoord[2].w = 1.0f / ProjectedTri.vectors[2].w;

				ProjectedTri.vectors[0] = DivVector(ProjectedTri.vectors[0], ProjectedTri.vectors[0].w);
				ProjectedTri.vectors[1] = DivVector(ProjectedTri.vectors[1], ProjectedTri.vectors[1].w);
				ProjectedTri.vectors[2] = DivVector(ProjectedTri.vectors[2], ProjectedTri.vectors[2].w);

				//Scaling
				//==========================================================================================================================
				vec3D ViewOffset = { 1.0f, 1.0f, 0.0f };

				ProjectedTri.vectors[0] = AddVectors(ProjectedTri.vectors[0], ViewOffset);
				ProjectedTri.vectors[1] = AddVectors(ProjectedTri.vectors[1], ViewOffset);
				ProjectedTri.vectors[2] = AddVectors(ProjectedTri.vectors[2], ViewOffset);

				ProjectedTri.vectors[0].x *= 0.5f * (float)Graphics::Resolution.width;
				ProjectedTri.vectors[0].y *= 0.5f * (float)Graphics::Resolution.height;

				ProjectedTri.vectors[1].x *= 0.5f * (float)Graphics::Resolution.width;
				ProjectedTri.vectors[1].y *= 0.5f * (float)Graphics::Resolution.height;

				ProjectedTri.vectors[2].x *= 0.5f * (float)Graphics::Resolution.width;
				ProjectedTri.vectors[2].y *= 0.5f * (float)Graphics::Resolution.height;
				//==========================================================================================================================

				TriangleToRasterVector.push_back(ProjectedTri);
				//DrawTriangle2filled(ProjectedTri, color, Buffer);
			}
		}
	}

	/*sort(TriangleToRasterVector.begin(), TriangleToRasterVector.end(), [](triangle& tri1, triangle& tri2)
		{
			float z1 = (tri1.vectors[0].z, tri1.vectors[1].z, tri1.vectors[2].z) / 3.0f;
			float z2 = (tri2.vectors[0].z, tri2.vectors[1].z, tri2.vectors[2].z) / 3.0f;
			return z1 > z2;
		});*/

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
					TriAddCount = TrianglePlaneClip({ 0.0f, (float)Resolution.height - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, testTri, clippedTri[0], clippedTri[1]);
					break;
				}
				case 2:
				{
					TriAddCount = TrianglePlaneClip({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, testTri, clippedTri[0], clippedTri[1]);
					break;
				}
				case 3:
				{
					TriAddCount = TrianglePlaneClip({ (float)Resolution.width - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, testTri, clippedTri[0], clippedTri[1]);
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
			Color col = { 0,0,0,0 };
			//DrawTriangle2filled(Tri, Tri.color, imageBuff, AlphaDepthBuff);
			DrawTriangle2textured(Tri, texture, imageBuff, AlphaDepthBuff);
			
		}
	}
	//==========================================================================================================================
};

void Graphics::refresh(ImageBuff& imageBuff)
{
	BeginDraw();

	//Create Bitmap
	//==========================================================================================================================
	D2D1_PIXEL_FORMAT PixForm = D2D1::PixelFormat();
	PixForm.format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	PixForm.alphaMode = D2D1_ALPHA_MODE_IGNORE;

	D2D1_BITMAP_PROPERTIES Properties = D2D1::BitmapProperties();
	rendertarget->GetDpi(&Properties.dpiX, &Properties.dpiY);
	Properties.pixelFormat = PixForm;

	D2D1_RECT_F rect = D2D1::RectF(0.0f, 0.0f, imageBuff.width, imageBuff.height);
	D2D1_SIZE_U size = D2D1::SizeU(imageBuff.width, imageBuff.height);
	HRESULT hr = rendertarget->CreateBitmap(size, imageBuff.PixelsPtr, imageBuff.width * 4, Properties, &BufferBmp);
	//==========================================================================================================================

	//Copy Buffer to Bitmap
	//==========================================================================================================================
	D2D1_RECT_U CopyRect = D2D1::RectU(0, 0, imageBuff.width, imageBuff.height);

	BufferBmp->CopyFromMemory(&CopyRect, imageBuff.PixelsPtr, imageBuff.width * 4);
	//==========================================================================================================================

	//Draw Bitmap
	//==========================================================================================================================
	rendertarget->DrawBitmap(BufferBmp, rect);
	//==========================================================================================================================
	BufferBmp->Release();

	EndDraw();
}

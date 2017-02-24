#ifndef OBJLOADER_H
#define OBJLOADER_H
#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>

struct Vertex
{
	float x, y, z;
	float norX, norY, norZ;
	float u, v;
};

struct Material {
	float Kdr, Kdg, Kdb;
	float Ksr, Ksg, Ksb;
	float Kar, Kag, Kab;
	float Ns;
	char textureFilePath[128];
};

enum TEX_COORD_TYPE {
	DIRECTX = 0,
	OPENGL = 1
};

using namespace DirectX;


Material* loadMTL(const char* filePath);

bool loadOBJ(const char* filePath, std::vector <Vertex> &vertices, std::vector <unsigned int> &indices, Material* &objectMaterial, TEX_COORD_TYPE texType = DIRECTX);

#endif
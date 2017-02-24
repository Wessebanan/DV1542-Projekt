#include <windows.h>    
#include <windowsx.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "InputHandler.h"
#include "NoiseGenerator.h"
#include "Timer.h"
#include "OBJLoader.h"
#include "Deferred.h"
#include <string>
#include <fstream>
#include <crtdbg.h>
#include <DDSTextureLoader.h>


#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
using namespace DirectX;

struct Object
{
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	unsigned int numIndices;
	OBJECT_TYPE objectType;
	XMMATRIX world;
};
Object Terrain;
Object Cube;
Object Bear;
Object Sphere;

XMMATRIX SphereWorldMatrices[10];

bool shadowsMapped = false;

void RenderDeferred(Deferred* def) 
{
	//-------Shadow map--------
	def->GetShadowmap()->BindShadowPass();
	def->DrawShadow(Terrain.vertexBuffer, Terrain.indexBuffer, Terrain.numIndices, Terrain.world);
	def->DrawShadow(Cube.vertexBuffer, Cube.indexBuffer, Cube.numIndices, Cube.world);
	def->DrawShadow(Bear.vertexBuffer, Bear.indexBuffer, Bear.numIndices, Bear.world);
	for (int i = 0; i < 10; i++)
	{
		def->DrawShadow(Sphere.vertexBuffer, Sphere.indexBuffer, Sphere.numIndices, SphereWorldMatrices[i]);
	}	
	//-------------------------

	def->InitialGeometryBinds();
	def->BindTerrain();
	def->Draw(Terrain.vertexBuffer, Terrain.indexBuffer, Terrain.numIndices, Terrain.world, TERRAIN);
	def->BindGenericObject();
	def->Draw(Cube.vertexBuffer, Cube.indexBuffer, Cube.numIndices, Cube.world, CUBE);
	def->Draw(Bear.vertexBuffer, Bear.indexBuffer, Bear.numIndices, Bear.world, BEAR);
	for (int i = 0; i < 10; i++)
	{
		def->Draw(Sphere.vertexBuffer, Sphere.indexBuffer, Sphere.numIndices, SphereWorldMatrices[i], SPHERE);
	}
	def->LightPass();
}

void CreateObjectBuffers(Deferred* def, Object* object, const char* filePath, OBJECT_TYPE objectType, TEX_COORD_TYPE texType = DIRECTX)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Material* objectMaterial = nullptr;

	bool result = loadOBJ(filePath, vertices, indices, objectMaterial, texType);

	def->CreateObjectTexture(objectMaterial, objectType);


	

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA objectData = {};
	objectData.pSysMem = vertices.data();

	if (FAILED(def->CreateBuffer(&vertexBufferDesc, &objectData, &object->vertexBuffer))) {
		MessageBoxA(NULL, "Error creating vertex buffer for object.", NULL, MB_OK);
		exit(-1);
	}

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * indices.size();
	unsigned int test1 = indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices.data();

	if (FAILED(def->CreateBuffer(&indexBufferDesc, &indexData, &object->indexBuffer)))
	{
		MessageBoxA(NULL, "Error creating index buffer for object.", NULL, MB_OK);
		exit(-1);
	}
}

void CreateTerrainBuffers(Deferred* def, ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer)
{
	int rows = 1024;
	int columns = 1024;

	//Texcoords:
	float u = 0.0f;
	float v = (float)columns / 10;

	NoiseGenerator noise1(def->GetDevicePointer(), 1024, 1024);
	float* heightmapData;
	heightmapData = noise1.loadHeightmap(L"TestMap9.RAW", 1024, 1024);
	float test1 = heightmapData[0];
	float test2 = heightmapData[1000];
	float test3 = heightmapData[1025];

	Vertex* vertices = new Vertex[rows*columns];
	unsigned long vertexIncrementer = 0;
	for (int i = 0; i < rows; i++) 
	{
		for (int j = 0; j < columns; j++)
		{
			vertices[vertexIncrementer] = 
			{ 
				(float)j,-20.0f + 100.0f * heightmapData[i*rows + j], (float)i, 
				0.0f, 0.0f, 0.0f, 
				u, v 
			};
			vertexIncrementer++;
			u += 0.1f;
		}
		u = 0;
		v -= 0.1f;
	}

	delete[] heightmapData;

	DWORD* indices = new DWORD[6 * (rows-1) * (columns-1)]; // 6 per quad
	unsigned long indexIncrementer = 0;
	for (int i = 0; i < rows - 1; i++) {
		for (int j = 0; j < columns - 1; j++) {
			unsigned long curr = i * rows + j;
			indices[indexIncrementer] = { curr };
			indices[indexIncrementer + 1] = { curr + columns };
			indices[indexIncrementer + 2] = { curr + 1 };
			indices[indexIncrementer + 3] = { curr + 1 };
			indices[indexIncrementer + 4] = { curr + columns };
			indices[indexIncrementer + 5] = { curr + columns + 1 };
			indexIncrementer = indexIncrementer + 6;

		}
	}

	for (int i = 0; i < 6 * 999 * 999; i += 3)
	{
		XMVECTOR edge1 = XMVectorSet(vertices[indices[i + 1]].x - vertices[indices[i]].x, vertices[indices[i + 1]].y - vertices[indices[i]].y, vertices[indices[i + 1]].z - vertices[indices[i]].z, 0.0f);
		XMVECTOR edge2 = XMVectorSet(vertices[indices[i + 2]].x - vertices[indices[i]].x, vertices[indices[i + 2]].y - vertices[indices[i]].y, vertices[indices[i + 2]].z - vertices[indices[i]].z, 0.0f);
		XMVECTOR faceNormal = XMVector3Cross(edge1, edge2);
		XMVector3Normalize(faceNormal);
		for (int j = 0; j < 3; j++)
		{
			vertices[indices[i + j]].norX = XMVectorGetX(faceNormal);
			vertices[indices[i + j]].norY = XMVectorGetY(faceNormal);
			vertices[indices[i + j]].norZ = XMVectorGetZ(faceNormal);
		}
	}

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * (6 * (rows - 1) * (columns - 1));
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	
	if (FAILED(def->CreateBuffer(&indexBufferDesc, &indexData, &Terrain.indexBuffer)))
	{
		MessageBoxA(NULL, "Error creating index buffer for terrain.", NULL, MB_OK);
		exit(-1);
	}

	D3D11_BUFFER_DESC terrainBufferDesc = {};
	terrainBufferDesc.ByteWidth = sizeof(Vertex) * rows * columns;
	terrainBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	terrainBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA terrainData = {};
	terrainData.pSysMem = vertices;

	if (FAILED(def->CreateBuffer(&terrainBufferDesc, &terrainData, &Terrain.vertexBuffer)))
	{
		MessageBoxA(NULL, "Error creating terrain buffer.", NULL, MB_OK);
		exit(-1);
	}
	delete[] vertices;
	delete[] indices;
}

void CreateObjects(Deferred* def)
{
	//Create terrain object
	CreateTerrainBuffers(def, Terrain.vertexBuffer, Terrain.indexBuffer);
	Terrain.numIndices = 6 * 999 * 999;
	Terrain.world = XMMatrixIdentity();

	//Create cube object
	CreateObjectBuffers(def, &Cube, "cube_green_phong_12_tris_TRIANGULATED.obj", CUBE, OPENGL);
	Cube.numIndices = 36; 
	Cube.world = XMMatrixRotationRollPitchYaw(0.34f, 1.47f, 2.01f) * XMMatrixScaling(50.0f, 50.0f, 50.0f) * XMMatrixTranslation(500, 30, 500);

	//Create bear object
	CreateObjectBuffers(def, &Bear, "bear.obj", BEAR, OPENGL);
	Bear.numIndices = 3912;
	srand(GetFrameTime());
	Bear.world = XMMatrixTranslation(160, -20, 180);

	//Create spheres
	float translationX = 100;
	float translationZ = 50;
	CreateObjectBuffers(def, &Sphere, "sphere.obj", SPHERE);
	Sphere.numIndices = 2280;
	for (int i = 0; i < 10; i++)
	{
		SphereWorldMatrices[i] = XMMatrixScaling(60, 60, 60) * XMMatrixTranslation(translationX, 120, 500 + translationZ * (i+1));
		translationZ *= -1;
		translationX += 100;
	}
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Deferred def(hInstance);
	MSG msg;

	if (!InitDirectInput(hInstance, def.GetWindowHandle())) {
		MessageBox(0, L"Failed to initialize Direct Input.", L"Error", MB_OK);
	}


	if (def.GetWindowHandle()) {

		def.SetHeightMapTexture(L"TestMap9.RAW", 1024, 1024);

		StartTimer(); // Starts global timer

		ShowWindow(def.GetWindowHandle(), nCmdShow);

		CreateObjects(&def);

		while (TRUE)
		{
			//Check to see if any messages are waiting in the queue
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				// translate keystroke messages into the right format
				TranslateMessage(&msg);

				// send the message to the WindowProc function
				DispatchMessage(&msg);

				// check to see if it's time to quit
				if (msg.message == WM_QUIT||destroyWindow==true)
					break;
			}
			else {
				DetectInput((float)GetFrameTime(), &def);
				
				RenderDeferred(&def); //<-- Funkar!


				// WEEEEW GAME CODE HERE LET'S GO
			}
		}
		DIKeyboard->Unacquire();
		DIMouse->Unacquire();
		DirectInput->Release();
		DestroyWindow(def.GetWindowHandle());

	}
	// return this part of the WM_QUIT message to Windows
	return (int)msg.wParam;
}
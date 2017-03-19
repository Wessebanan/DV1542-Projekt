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
#include "QuadTree.h"

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

const int numSpheres = 11;

MeshObject* MTerrain = nullptr;
MeshObject* MCube = nullptr;
MeshObject* MBear = nullptr;
MeshObject* MSpheres[numSpheres];



XMMATRIX SphereWorldMatrices[numSpheres];

float rotationAngle = 0.0f;

void RenderDeferred(Deferred* def) 
{
	//SphereWorldMatrices[10] = XMMatrixScaling(30, 30, 30) * XMMatrixRotationY(-rotationAngle * 10) * XMMatrixTranslation(100, 0, 0) * XMMatrixRotationY(rotationAngle) * XMMatrixTranslation(500, 100, 500);
	MSpheres[10]->RotateObject(0, -rotationAngle * 10, 0);
	MCube->RotateObject(0, -rotationAngle * 10, 0);
	MCube->MoveObjectToPosition(XMFLOAT3(512, 0, 512));
	XMFLOAT3 toMove = { 400.0f, 200.0f, 300.0f };
	MSpheres[10]->MoveObjectToPosition(toMove);
	MSpheres[9]->MoveObjectToPosition(XMFLOAT3(512, 50, 512));
	rotationAngle += 0.0000001f;

	if (rotationAngle > 2 * XM_PI)
	{
		rotationAngle = 0.0f;
	}

	//-------Shadow map drawing--------
	def->GetShadowmap()->BindShadowPass();
	def->DrawShadow(MTerrain->getVertexBuffer(), MTerrain->getIndexBuffer(), MTerrain->getNumIndices(), MTerrain->getWorldMatrix());
	def->DrawShadow(MCube->getVertexBuffer(), MCube->getIndexBuffer(), MCube->getNumIndices(), MCube->getWorldMatrix());
	def->DrawShadow(MBear->getVertexBuffer(), MBear->getIndexBuffer(), MBear->getNumIndices(), MBear->getWorldMatrix());
	for (int i = 0; i < numSpheres; i++)
	{
		def->DrawShadow(MSpheres[i]->getVertexBuffer(), MSpheres[i]->getIndexBuffer(), MSpheres[i]->getNumIndices(), MSpheres[i]->getWorldMatrix());
	}
	//-------------------------

	def->GetShadowmap()->UnbindDSV();
	//Blurs the shadow map using a gaussian filter in a compute shader.
	def->BlurShadowMap();

	//Binds shared by terrain and generic object.
	def->InitialGeometryBinds();
	def->BindTerrain();
	def->Draw(MTerrain->getVertexBuffer(), MTerrain->getIndexBuffer(), MTerrain->getNumIndices(), MTerrain->getWorldMatrix(), TERRAIN);
	def->BindGenericObject();
	//def->Draw(MCube->getVertexBuffer(), MCube->getIndexBuffer(), MCube->getNumIndices(), MCube->getWorldMatrix(), CUBE);
	//def->Draw(MBear->getVertexBuffer(), MBear->getIndexBuffer(), MBear->getNumIndices(), MBear->getWorldMatrix(), BEAR);
	//for (int i = 0; i < numSpheres; i++)
	//{
	//	def->Draw(MSpheres[i]->getVertexBuffer(), MSpheres[i]->getIndexBuffer(), MSpheres[i]->getNumIndices(), MSpheres[i]->getWorldMatrix(), SPHERE);
	//}
	//Lightpass contains the final draw call.


	std::vector<MeshObject*> allObjects;

	allObjects.push_back(MBear);
	allObjects.push_back(MCube);
	for (int i = 0; i < numSpheres; i++) {
		allObjects.push_back(MSpheres[i]);
	}

	QuadTree ObjectsTree(&allObjects, 512.0f, 512.0f, 512.0f);
	std::vector<MeshObject*> toRender = ObjectsTree.getVisibleObjects(def->getFrustumPointer());
	while(toRender.size() > 0) {
		MeshObject* currentRender = toRender.back();
		XMFLOAT4 boundingValues = currentRender->getBoundingValues();
		//if (def->getFrustumPointer()->CheckAABA(boundingValues.x, boundingValues.y, boundingValues.z, boundingValues.w)) {
			def->Draw(currentRender->getVertexBuffer(), currentRender->getIndexBuffer(), currentRender->getNumIndices(), currentRender->getWorldMatrix(), currentRender->getObjectType());
		//}
		toRender.pop_back();
	}
	def->LightPass();

}

void CreateObjectBuffers(Deferred* def, MeshObject* &object, const char* filePath, unsigned int numIndices, OBJECT_TYPE objectType, TEX_COORD_TYPE texType = DIRECTX)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	ID3D11Buffer* tempVertBuffer;
	ID3D11Buffer* tempIndexBuffer;
	XMFLOAT3 boundingValues = { -1, -1, -1 }; // xy: the X and Z offset of the bounding area from the origin in the mesh
	XMFLOAT3* bvPointer = &boundingValues;    // z:  the half-width of the AABA
	

	Material* objectMaterial = nullptr;

	bool result = loadOBJ(filePath, vertices, indices, objectMaterial, bvPointer, texType);

	def->CreateObjectTexture(objectMaterial, objectType);	

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth	= (UINT)(sizeof(Vertex) * vertices.size());
	vertexBufferDesc.BindFlags	= D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage		= D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA objectData = {};
	objectData.pSysMem = vertices.data();

	if (FAILED(def->GetDevicePointer()->CreateBuffer(&vertexBufferDesc, &objectData, &tempVertBuffer))) {
		MessageBoxA(NULL, "Error creating vertex buffer for object.", NULL, MB_OK);
		exit(-1);
	}

	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.Usage			= D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth		= (UINT)(sizeof(unsigned int) * indices.size());
	indexBufferDesc.BindFlags		= D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags	= 0;
	indexBufferDesc.MiscFlags		= 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices.data();

	if (FAILED(def->GetDevicePointer()->CreateBuffer(&indexBufferDesc, &indexData, &tempIndexBuffer)))
	{
		MessageBoxA(NULL, "Error creating index buffer for object.", NULL, MB_OK);
		exit(-1);
	}

	object = new MeshObject(tempVertBuffer, tempIndexBuffer, numIndices, objectType, boundingValues);
}

void CreateTerrainBuffers(Deferred* def, ID3D11Buffer* &vertexBuffer, ID3D11Buffer* &indexBuffer)
{
	int rows = 1024;
	int columns = 1024;

	//Texcoords:
	float u = 0.0f;
	float v = (float)columns / 10;

	NoiseGenerator noise1(def->GetDevicePointer(), 1024, 1024);
	float* heightmapData;
	heightmapData = noise1.loadHeightmap(L"TestMap9.RAW", 1024, 1024);

	Vertex* vertices = new Vertex[rows*columns];
	unsigned long vertexIncrementer = 0;
	for (int i = 0; i < rows; i++) 
	{
		for (int j = 0; j < columns; j++)
		{
			vertices[vertexIncrementer] = 
			{ 
				(float)j, -20.0f + 100.0f * heightmapData[i*rows + j], (float)i, 
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

	for (int i = 0; i < (6 * (rows - 1) * (columns - 1)); i += 3)
	{
		XMVECTOR p0 = XMVectorSet(vertices[indices[i]].x, vertices[indices[i]].y, vertices[indices[i]].z, 0.0f);
		XMVECTOR p1 = XMVectorSet(vertices[indices[i+1]].x, vertices[indices[i+1]].y, vertices[indices[i+1]].z, 0.0f);
		XMVECTOR p2 = XMVectorSet(vertices[indices[i+2]].x, vertices[indices[i+2]].y, vertices[indices[i+2]].z, 0.0f);

		XMVECTOR edge1 = p1 - p0;
		XMVECTOR edge2 = p2 - p0;
		XMVECTOR faceNormal = XMVector3Normalize(XMVector3Cross(edge1, edge2));		

		for (int j = 0; j < 3; j++)
		{
			vertices[indices[i + j]].norX = XMVectorGetX(faceNormal);
			vertices[indices[i + j]].norY = XMVectorGetY(faceNormal);
			vertices[indices[i + j]].norZ = XMVectorGetZ(faceNormal);
		}
	}

	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.Usage			= D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth		= sizeof(DWORD) * (6 * (rows - 1) * (columns - 1));
	indexBufferDesc.BindFlags		= D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags	= 0;
	indexBufferDesc.MiscFlags		= 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	
	if (FAILED(def->GetDevicePointer()->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer)))
	{
		MessageBoxA(NULL, "Error creating index buffer for terrain.", NULL, MB_OK);
		exit(-1);
	}

	D3D11_BUFFER_DESC terrainBufferDesc{};
	terrainBufferDesc.ByteWidth	= sizeof(Vertex) * rows * columns;
	terrainBufferDesc.BindFlags	= D3D11_BIND_VERTEX_BUFFER;
	terrainBufferDesc.Usage		= D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA terrainData = {};
	terrainData.pSysMem = vertices;

	if (FAILED(def->GetDevicePointer()->CreateBuffer(&terrainBufferDesc, &terrainData, &vertexBuffer)))
	{
		MessageBoxA(NULL, "Error creating terrain vertex buffer.", NULL, MB_OK);
		exit(-1);
	}
	delete[] vertices;
	delete[] indices;
}

void CreateObjects(Deferred* def)
{
	//Create terrain object
	ID3D11Buffer* tempVertBuffer = nullptr;
	ID3D11Buffer* tempIndexBuffer = nullptr;
	CreateTerrainBuffers(def, tempVertBuffer, tempIndexBuffer);

	MTerrain = new MeshObject(tempVertBuffer, tempIndexBuffer, 6 * 1023 * 1023, TERRAIN, XMFLOAT3(512, 512, 512));

	//Terrain.numIndices = 6 * 1023 * 1023;
	//Terrain.world = XMMatrixIdentity();

	//Create cube object
	CreateObjectBuffers(def, MCube, "cube_green_phong_12_tris_TRIANGULATED.obj", 36, CUBE, OPENGL);
	Cube.numIndices = 36; 
	MCube->RotateObject(0.34f, 1.47f, 2.01f);
	MCube->ScaleObject(50.0f, 50.0f, 50.0f);
	MCube->TranslateObject(500, 30, 500);
	// Cube.world = XMMatrixRotationRollPitchYaw(0.34f, 1.47f, 2.01f) * XMMatrixScaling(50.0f, 50.0f, 50.0f) * XMMatrixTranslation(500, 30, 500);

	//Create bear object
	CreateObjectBuffers(def, MBear, "bear.obj", 3912, BEAR, OPENGL);
	Bear.numIndices = 3912;
	MBear->TranslateObject(160, -20, 180);
	// Bear.world = XMMatrixTranslation(160, -20, 180);

	//Create spheres
	float translationX = 100;
	float translationZ = 50;
	CreateObjectBuffers(def, MSpheres[0], "sphere.obj", 2280, SPHERE);
	for (int i = 1; i < numSpheres; i++) {
		MSpheres[i] = new MeshObject(*MSpheres[0]);
	}
	Sphere.numIndices = 2280;
	for (int i = 0; i < numSpheres; i++)
	{
		MSpheres[i]->ScaleObject(30, 30, 30);
		MSpheres[i]->TranslateObject(translationX, 120, 500 + translationZ * (i + 1));
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
	//Inside Deferred and its members is
	//where the magic happens.
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
				RenderDeferred(&def);
			}
		}
		DIKeyboard->Unacquire();
		DIMouse->Unacquire();
		DirectInput->Release();
		DestroyWindow(def.GetWindowHandle());

		//Bear.indexBuffer->Release();
		//Sphere.indexBuffer->Release();
		//Cube.indexBuffer->Release();
		//Terrain.indexBuffer->Release();

		//Bear.vertexBuffer->Release();
		//Cube.vertexBuffer->Release();
		//Sphere.vertexBuffer->Release();
		//Terrain.vertexBuffer->Release();

		if (MTerrain != nullptr) {
			MTerrain->getIndexBuffer()->Release();
			MTerrain->getVertexBuffer()->Release();
			delete MTerrain;
		}

		MSpheres[0]->getIndexBuffer()->Release();
		MSpheres[0]->getVertexBuffer()->Release();
		for (int i = 0; i < numSpheres; i++) {
			if (MSpheres[i] != nullptr) {
				delete MSpheres[i];
			}
		}

		if (MBear != nullptr) {
			MBear->getIndexBuffer()->Release();
			MBear->getVertexBuffer()->Release();
			delete MBear;
		}
		if (MCube != nullptr) {
			MCube->getIndexBuffer()->Release();
			MCube->getVertexBuffer()->Release();
			delete MCube;
		}
	}
	// return this part of the WM_QUIT message to Windows
	return (int)msg.wParam;
}
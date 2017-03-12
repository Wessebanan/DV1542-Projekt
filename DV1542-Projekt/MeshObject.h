#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

enum OBJECT_TYPE
{
	TERRAIN = 0,
	CUBE = 1,
	BEAR = 2,
	SPHERE = 3
};


class MeshObject {
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	unsigned int numIndices;
	OBJECT_TYPE objectType;
	XMFLOAT3 worldPosition;
	XMMATRIX world;
	float boundingLowX, boundingHighX, boundingLowZ, boundingHighZ;





public:
	MeshObject(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, unsigned int numIndices, OBJECT_TYPE objectType, XMFLOAT4 boundingValues);
	~MeshObject();
	void Release();

	XMMATRIX getWorldMatrix();

	ID3D11Buffer* getVertexBuffer();
	ID3D11Buffer* getIndexBuffer();
	unsigned int getNumIndices();

	void RotateObject(float roll, float pitch, float yaw);
	void ScaleObject(float scaleFactorX, float scaleFactorY, float scaleFactorZ);
	void TranslateObject(float offsetX, float offsetY, float offsetZ);
	void MoveObjectToPosition(XMFLOAT3 worldPosition);
	void operator=(const MeshObject &other);

};
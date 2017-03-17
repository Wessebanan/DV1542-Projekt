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

	XMFLOAT2 boundingCenter;
	float boundingLowX, boundingHighX, boundingLowZ, boundingHighZ;





public:
	MeshObject(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, unsigned int numIndices, OBJECT_TYPE objectType, XMFLOAT3 boundingValues);
	MeshObject(const MeshObject &other);
	~MeshObject();
	void Release();

	XMMATRIX getWorldMatrix();

	ID3D11Buffer* getVertexBuffer();
	ID3D11Buffer* getIndexBuffer();
	unsigned int getNumIndices();
	XMFLOAT4 getBoundingValues();
	OBJECT_TYPE getObjectType();

	void RotateObject(float roll, float pitch, float yaw);
	void ScaleObject(float scaleFactorX, float scaleFactorY, float scaleFactorZ);
	void TranslateObject(float offsetX, float offsetY, float offsetZ);
	void MoveObjectToPosition(XMFLOAT3 worldPosition);
	void operator=(const MeshObject &other);

};
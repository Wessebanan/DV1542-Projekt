#include "MeshObject.h"

MeshObject::MeshObject(ID3D11Buffer * vertexBuffer, ID3D11Buffer * indexBuffer, unsigned int numIndices, OBJECT_TYPE objectType, XMFLOAT3 boundingValues) {
	this->vertexBuffer = vertexBuffer;
	this->indexBuffer = indexBuffer;
	this->numIndices = numIndices;
	this->objectType = objectType;
	this->world = XMMatrixIdentity();
	this->worldPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	// BoundingValues
	// x: the offset in x from mesh origin to bounding center
	// y: the offset in z from mesh origin to bounding center
	// z: the half-width of the bounding area
	this->boundingLowX = boundingValues.x - boundingValues.z;
	this->boundingHighX = boundingValues.x + boundingValues.z;
	this->boundingLowZ = boundingValues.y - boundingValues.z;
	this->boundingHighZ = boundingValues.y + boundingValues.z;
	this->boundingCenter = XMFLOAT2((boundingLowX + boundingHighX) / 2.0f, (boundingLowZ + boundingHighZ) / 2.0f);
}

MeshObject::MeshObject(const MeshObject & other) {
	this->vertexBuffer = other.vertexBuffer;
	this->indexBuffer = other.indexBuffer;
	this->numIndices = other.numIndices;
	this->objectType = other.objectType;
	this->world = other.world;
	this->worldPosition = other.worldPosition;
	this->boundingLowX = other.boundingLowX;
	this->boundingHighX = other.boundingHighX;
	this->boundingLowZ = other.boundingLowZ;
	this->boundingHighZ = other.boundingHighZ;
	this->boundingCenter = other.boundingCenter;
}

MeshObject::~MeshObject(){
	
}

void MeshObject::Release() {
	//if (this->vertexBuffer != nullptr) {
	//	this->vertexBuffer->Release();
	//}
	//if (this->indexBuffer != nullptr) {
	//	this->indexBuffer->Release();
	//}
}



XMMATRIX MeshObject::getWorldMatrix()
{
	return this->world;
}

ID3D11Buffer * MeshObject::getVertexBuffer()
{
	return this->vertexBuffer;
}

ID3D11Buffer * MeshObject::getIndexBuffer()
{
	return this->indexBuffer;
}

unsigned int MeshObject::getNumIndices()
{
	return this->numIndices;
}

XMFLOAT4 MeshObject::getBoundingValues()
{
	return XMFLOAT4(this->boundingLowX, this->boundingHighX, this->boundingLowZ, this->boundingHighZ);
}

OBJECT_TYPE MeshObject::getObjectType()
{
	return this->objectType;
}

void MeshObject::RotateObject(float roll, float pitch, float yaw) {
	XMMATRIX originTranslation = XMMatrixTranslation(-this->worldPosition.x, -this->worldPosition.y, -this->worldPosition.z);
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(roll, pitch, yaw);
	XMMATRIX completeRotation = originTranslation * rotation * XMMatrixTranslation(this->worldPosition.x, this->worldPosition.y, this->worldPosition.z);
	this->world = this->world * completeRotation;

}

void MeshObject::ScaleObject(float scaleFactorX, float scaleFactorY, float scaleFactorZ) {
	XMMATRIX originTranslation = XMMatrixTranslation(-this->worldPosition.x, -this->worldPosition.y, -this->worldPosition.z);
	XMMATRIX scaling = XMMatrixScaling(scaleFactorX, scaleFactorY, scaleFactorZ);
	XMMATRIX completeScaling = originTranslation * scaling * XMMatrixTranslation(this->worldPosition.x, this->worldPosition.y, this->worldPosition.z);
	this->world = this->world * completeScaling;

	XMMATRIX boundingTranslation = XMMatrixTranslation(-this->boundingCenter.x, 0.0f, -this->boundingCenter.y);
	XMMATRIX boundingScaling = boundingTranslation * scaling * XMMatrixTranslation(this->boundingCenter.x, 0.0f, this->boundingCenter.y);

	XMVECTOR boundingLow =  { this->boundingLowX, 0, this->boundingLowZ, 1   };
	XMVECTOR boundingHigh = { this->boundingHighX, 0, this->boundingHighZ, 1 };
	XMVECTOR scaledLow =  XMVector4Transform(boundingLow, boundingScaling);
	XMVECTOR scaledHigh = XMVector4Transform(boundingHigh, boundingScaling);
	this->boundingLowX =  XMVectorGetX(scaledLow);
	this->boundingHighX = XMVectorGetX(scaledHigh);
	this->boundingLowZ =  XMVectorGetZ(scaledLow);
	this->boundingHighZ = XMVectorGetZ(scaledHigh);

}

void MeshObject::TranslateObject(float offsetX, float offsetY, float offsetZ) {
	this->world = this->world * XMMatrixTranslation(offsetX, offsetY, offsetZ);
	this->worldPosition.x = this->worldPosition.x + offsetX;
	this->worldPosition.y = this->worldPosition.y + offsetY;
	this->worldPosition.z = this->worldPosition.z = offsetZ;
	this->boundingHighX += offsetX;
	this->boundingLowX += offsetX;
	this->boundingHighZ += offsetZ;
	this->boundingLowZ += offsetZ;
	this->boundingCenter.x += offsetX;
	this->boundingCenter.y += offsetZ;
}

void MeshObject::MoveObjectToPosition(XMFLOAT3 worldPosition) {
	XMVECTOR translation = { worldPosition.x - this->worldPosition.x, worldPosition.y - this->worldPosition.y, worldPosition.z - this->worldPosition.z };
	this->world = this->world * XMMatrixTranslationFromVector(translation);
	this->worldPosition = worldPosition;
	this->boundingLowX += XMVectorGetX(translation);
	this->boundingHighX += XMVectorGetX(translation);
	this->boundingLowZ += XMVectorGetZ(translation);
	this->boundingHighZ += XMVectorGetZ(translation);
	this->boundingCenter.x += XMVectorGetX(translation);
	this->boundingCenter.y = XMVectorGetZ(translation);

}

void MeshObject::operator=(const MeshObject & other) {
	this->vertexBuffer = other.vertexBuffer;
	this->indexBuffer = other.indexBuffer;
	this->numIndices = other.numIndices;
	this->objectType = other.objectType;
	this->world = other.world;
	this->worldPosition = other.worldPosition;
	this->boundingLowX = other.boundingLowX;
	this->boundingHighX = other.boundingHighX;
	this->boundingLowZ = other.boundingLowZ;
	this->boundingHighZ = other.boundingHighZ;
	this->boundingCenter = other.boundingCenter;
	
}

#include "MeshObject.h"

MeshObject::MeshObject(ID3D11Buffer * vertexBuffer, ID3D11Buffer * indexBuffer, unsigned int numIndices, OBJECT_TYPE objectType) {
	this->vertexBuffer = vertexBuffer;
	this->indexBuffer = indexBuffer;
	this->numIndices = numIndices;
	this->objectType = objectType;
	this->world = XMMatrixIdentity();
	this->worldPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

MeshObject::~MeshObject(){

}

void MeshObject::setWorldMatrix(XMMATRIX newWorld)
{
	this->world = newWorld;
}

XMMATRIX MeshObject::getWorldMatrix()
{
	return this->world;
}

void MeshObject::RotateObject(float roll, float pitch, float yaw) {
	XMMATRIX originTranslation = XMMatrixTranslation(-this->worldPosition.x, -this->worldPosition.y, -this->worldPosition.z);
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(roll, pitch, yaw);
	XMMATRIX completeRotation = originTranslation * rotation * XMMatrixTranspose(originTranslation);
	this->world = this->world * completeRotation;
}

void MeshObject::ScaleObject(float scaleFactorX, float scaleFactorY, float scaleFactorZ) {
	XMMATRIX originTranslation = XMMatrixTranslation(-this->worldPosition.x, -this->worldPosition.y, -this->worldPosition.z);
	XMMATRIX scaling = XMMatrixScaling(scaleFactorX, scaleFactorY, scaleFactorZ);
	XMMATRIX completeScaling = originTranslation * scaling * XMMatrixTranspose(originTranslation);
	this->world = this->world * completeScaling;
}

void MeshObject::TranslateObject(float offsetX, float offsetY, float offsetZ) {
	this->world = this->world * XMMatrixTranslation(offsetX, offsetY, offsetZ);
}

void MeshObject::MoveObjectToPosition(XMFLOAT3 worldPosition) {
	XMVECTOR newPosition = { worldPosition.x - this->worldPosition.x, worldPosition.y - this->worldPosition.y, worldPosition.z - this->worldPosition.z };
	this->world = this->world * XMMatrixTranslationFromVector(newPosition);

}

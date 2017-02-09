#include "Camera.h"

Camera::Camera() {
	this->defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	this->defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	this->camPosition = XMVectorSet(0.0f, 100.0f, 0.0f, 0.0f);
	this->camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	this->camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	this->camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	this->terrainData = nullptr;
	this->terrainWidth = 0;
	this->terrainHeight = 0;

	this->viewMatrix = XMMatrixLookAtLH(this->camPosition, this->defaultForward, this->camUp);
}

Camera::~Camera() {

}

XMMATRIX Camera::UpdateCamera(float leftRight, float backForward, float upDown, float pitch, float yaw) {
		XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, 0);
		XMVECTOR camLookAt = XMVector3TransformCoord(this->defaultForward, camRotationMatrix);
		camLookAt = XMVector3Normalize(camLookAt);
	
		XMMATRIX rotateYTempMatrix = XMMatrixRotationY(yaw);
	
		camRight = XMVector3TransformCoord(this->defaultRight, rotateYTempMatrix);
		camUp = XMVector3TransformCoord(camUp, rotateYTempMatrix);
		camForward = XMVector3TransformCoord(this->defaultForward, rotateYTempMatrix);
	
	
		this->camPosition += leftRight * camRight;
		this->camPosition += backForward * camForward;

		// Out of bounds prevention

		if (XMVectorGetX(camPosition) > this->terrainWidth - 30) {
			this->camPosition = XMVectorSetX(this->camPosition, this->terrainWidth - 30);
		}
		else if (XMVectorGetX(camPosition) < 30.0f) {
			this->camPosition = XMVectorSetX(this->camPosition, 30.0f);
		}
		if (XMVectorGetZ(camPosition) > this->terrainHeight - 30) {
			this->camPosition = XMVectorSetZ(this->camPosition, this->terrainHeight - 30);
		}
		else if (XMVectorGetZ(camPosition) < 30.0f) {
			this->camPosition = XMVectorSetZ(this->camPosition, 30.0f);
		}
		this->camPosition = XMVectorSetY(this->camPosition,this->CalculateHeight(XMVectorGetX(camPosition), XMVectorGetZ(camPosition)));
		XMVECTOR test = camPosition;
		camLookAt = camPosition + camLookAt;
		
		this->viewMatrix = XMMatrixLookAtLH(camPosition, camLookAt, camUp);
		return this->viewMatrix;
}

void Camera::SetTerrainData(float* dataArray, unsigned int width, unsigned int height) {
	if (this->terrainData != nullptr) {
		delete[] this->terrainData;
	}
	this->terrainData = dataArray;
	this->terrainWidth = width;
	this->terrainHeight = height;
}

float Camera::CalculateHeight(float newXPos, float newZPos) {
	// This function uses the X and Z coordinate of the camera
	// and then lerps between two points in the heightmap to position the camera along the terrain
	double lowerXPos = 0;
	float lerpValueX = modf(newXPos, &lowerXPos);
	if (lowerXPos < 0) {
		lowerXPos = -lowerXPos;
	}
	double upperXPos = lowerXPos + 1;

	double lowerZPos = 0;
	float lerpValueZ = modf(newZPos, &lowerZPos);
	if (lowerZPos < 0) {
		lowerZPos = -lowerZPos;
	}
	double upperZPos = lowerZPos + 1;

	double bottomLeftValue = this->terrainData[(int)lowerXPos + (int)lowerZPos * this->terrainWidth];
	double topLeftValue = this->terrainData[(int)upperXPos + (int)lowerZPos * this->terrainWidth];
	double bottomRightValue = this->terrainData[(int)lowerXPos + (int)upperZPos * this->terrainWidth];

	double lerpX = 100.0f * bottomLeftValue + lerpValueX * (topLeftValue - bottomLeftValue);
	double lerpZ = 100.0f *  bottomLeftValue + lerpValueZ * (bottomRightValue - bottomLeftValue);
	float finalValue = (lerpX + lerpZ) / 2;
	return finalValue;
}

XMMATRIX Camera::GetViewMatrix()
{
	return this->viewMatrix;
}

XMVECTOR Camera::GetCamPosition()
{
	return this->camPosition;
}


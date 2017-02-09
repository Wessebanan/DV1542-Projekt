#include "Camera.h"

Camera::Camera() {
	this->defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	this->defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	this->camPosition = XMVectorSet(0.0f, 0.0f, -2.0f, 0.0f);
	this->camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	this->camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	this->camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

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
	
	
		camPosition += leftRight * camRight;
		camPosition += backForward * camForward;
		camPosition += upDown * camUp;
		camLookAt = camPosition + camLookAt;
		
		this->viewMatrix = XMMatrixLookAtLH(camPosition, camLookAt, camUp);
		return this->viewMatrix;
}

XMMATRIX Camera::GetViewMatrix()
{
	return this->viewMatrix;
}

XMVECTOR Camera::GetCamPosition()
{
	return this->camPosition;
}


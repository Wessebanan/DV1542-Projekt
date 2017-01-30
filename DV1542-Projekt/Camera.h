#ifndef CAMERA_H
#define CAMERA_H
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;



XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR camPosition = XMVectorSet(0.0f, 0.0f, -2.0f, 0.0f);
XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

XMMATRIX viewMatrix = XMMatrixIdentity();

void updateCamera(float leftRight, float backForward, float pitch, float yaw) {
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, 0);
	XMVECTOR camLookAt = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camLookAt = XMVector3Normalize(camLookAt);

	XMMATRIX rotateYTempMatrix = XMMatrixRotationY(yaw);

	camRight = XMVector3TransformCoord(DefaultRight, rotateYTempMatrix);
	camUp = XMVector3TransformCoord(camUp, rotateYTempMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, rotateYTempMatrix);

	camPosition += leftRight * camRight;
	camPosition += backForward * camForward;

	camLookAt = camPosition + camLookAt;
	
	viewMatrix = XMMatrixLookAtLH(camPosition, camLookAt, camUp);
}
#endif
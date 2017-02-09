#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;

class Camera {
private:
	XMVECTOR defaultForward;
	XMVECTOR defaultRight;
	XMVECTOR camPosition;
	XMVECTOR camUp;
	XMVECTOR camForward;
	XMVECTOR camRight;
	XMMATRIX viewMatrix;


public:
	Camera();
	~Camera();
	XMMATRIX UpdateCamera(float leftRight, float backForward, float upDown, float pitch, float yaw);

	XMMATRIX GetViewMatrix();
	XMVECTOR GetCamPosition();

};

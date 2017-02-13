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

	float* terrainData;
	unsigned int terrainWidth;
	unsigned int terrainHeight;
	
	//-----JUMP STUFF-----
	bool startOfJumpYChecked;
	float startOfJumpY;
	//--------------------
public:
	Camera();
	~Camera();
	XMMATRIX UpdateCamera(float leftRight, float backForward, float upDown, float pitch, float yaw, bool* isJumping, float* totalHeightOfJump);

	void SetTerrainData(float* dataArray, unsigned int width, unsigned int height);
	float CalculateHeight(float newXPos, float newZPos);

	XMMATRIX GetViewMatrix();
	XMVECTOR GetCamPosition();

};

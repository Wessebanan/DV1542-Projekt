#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H
#include <windows.h>    
#include <windowsx.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "Deferred.h"
#include <DirectXMath.h>
#include "Camera.h"
#include "Timer.h"
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")


#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#include <dinput.h>

using namespace DirectX;

IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;

DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;

float rotx = 0;
float rotz = 0;
float scaleX = 1.0f;
float scaleY = 1.0f;
bool destroyWindow = false;
float moveLeftRight = 0.0f;
float moveBackForward = 0.0f;
float moveUpDown = 0.0f;

float camYaw = 0.0f;
float camPitch = 0.0f;
XMMATRIX Rotationx;
XMMATRIX Rotationz;

//---------JUMP STUFF-------
float maxDownSpeed = -200.0f;
float jumpSpeed = 0.0f;
float gravitation = -200.0f;
bool isJumping = false;
float totalHeightOfJump = 0.0f;
//--------------------------

bool InitDirectInput(HINSTANCE hInstance, HWND hwnd) {
	HRESULT hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL);
	hr = DirectInput->CreateDevice(GUID_SysKeyboard, &DIKeyboard, NULL);
	hr = DirectInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);

	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard); // Standard keyboard layout
	hr = DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NOWINKEY);

	hr = DIMouse->SetDataFormat(&c_dfDIMouse); // Standard 3 axes (x, y, scrollwheel) / 4 button mouse
	hr = DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);



	return SUCCEEDED(hr);
}
void DetectInput(float time, Deferred* deferred) {
	DIMOUSESTATE mouseCurrState;
	BYTE keyboardState[256];

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState); // Check whether mouse has moved since last check
	DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState); // Same as above, but keyboard

	float speed = 100.0f * time;

	if (keyboardState[DIK_ESCAPE] & 0x80)
		destroyWindow = true;
	if (keyboardState[DIK_LEFT] & 0x80 || keyboardState[DIK_A] & 0x80)
	{
		//rotz -= 1.0f * time;
		moveLeftRight -= speed;
	}
	if (keyboardState[DIK_RIGHT] & 0x80 || keyboardState[DIK_D] & 0x80)
	{
		//rotz += 1.0f * time;
		moveLeftRight += speed;
	}
	if (keyboardState[DIK_UP] & 0x80 || keyboardState[DIK_W] & 0x80)
	{
		//rotx += 1.0f * time;
		moveBackForward += speed;
	}
	if (keyboardState[DIK_DOWN] & 0x80 || keyboardState[DIK_S] & 0x80)
	{
		//rotx -= 1.0f * time;
		moveBackForward -= speed;
	}
	//----------JUMP STUFF-------------------
	if (keyboardState[DIK_SPACE] & 0x80) {
		if (!isJumping)
		{
			isJumping = true;
			jumpSpeed = 100.0f;
		}
		moveUpDown += speed;
	}
	if (isJumping)
	{
		totalHeightOfJump += jumpSpeed * time;
		if (jumpSpeed > maxDownSpeed)
		{
			jumpSpeed += gravitation * time;
		}
	}	
	//--------------------------------------
	if (keyboardState[DIK_X] & 0x80) {
		moveUpDown -= speed;
	}
	if (mouseCurrState.lX != mouseLastState.lX)
	{
		camYaw += mouseCurrState.lX * 0.001f;
	}
	if (mouseCurrState.lY != mouseLastState.lY)
	{
		camPitch += mouseCurrState.lY * 0.001f;
	}

	if (camPitch > 1.57f) {
		camPitch = 1.57f;
	}
	else if (camPitch < -1.57f) {
		camPitch = -1.57f;
	}
	deferred->GetCameraPointer()->UpdateCamera(moveLeftRight, moveBackForward, moveUpDown, camPitch, camYaw, &isJumping, &totalHeightOfJump);
	deferred->UpdateFrustum();
	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;
	moveUpDown = 0.0f;


	if (rotx > 6.28f)
		rotx -= 6.28f;
	else if (rotx < 0.0f)
		rotx = 6.28f + rotx;

	if (rotz > 6.28f)
		rotz -= 6.28f;
	else if (rotz < 0.0f)
		rotz = 6.28f + rotz;

	mouseLastState = mouseCurrState;

	return;

}
#endif
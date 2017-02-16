#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Camera.h"
#include "D3D.h"

class Shadowmap
{
private:
	D3D direct3D;
	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11Texture2D* depthStencilBuffer = nullptr;

	ID3D11VertexShader* VS_Shadow = nullptr;
	ID3D11PixelShader* PS_Shadow = nullptr;

	ID3D11Buffer* transformBuffer = nullptr;



public:
	Shadowmap(D3D direct3D);
	~Shadowmap();

	void CreateShaders();
	void Shadowpass(XMVECTOR lightDir);
};

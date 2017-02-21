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
	ID3D11ShaderResourceView* depthMapSRV = nullptr;
	ID3D11Texture2D* depthStencilBuffer = nullptr;

	ID3D11VertexShader* VS_Shadow = nullptr;
	ID3D11PixelShader* PS_Shadow = nullptr;

	ID3D11Buffer* transformBuffer = nullptr;

	D3D11_VIEWPORT viewPort;

	struct matrixData
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
	};
	matrixData WVP;
	XMVECTOR lightDir;
public:
	Shadowmap();
	Shadowmap(D3D direct3D, D3D11_VIEWPORT vp, int height, int width);
	~Shadowmap();

	ID3D11ShaderResourceView* GetSRV();

	void CreateTransformationMatrices();
	void CreateShaders();
	void BindShadowPass();
};

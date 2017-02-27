#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Camera.h"
#include "D3D.h"

class Shadowmap
{
private:
	ID3D11Device* device		= nullptr;
	ID3D11DeviceContext* devCon = nullptr;

	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11ShaderResourceView* depthMapSRV	 = nullptr;
	ID3D11Texture2D* depthStencilBuffer		 = nullptr;

	ID3D11VertexShader* VS_Shadow = nullptr;

	ID3D11InputLayout* inputLayout = nullptr;

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
	~Shadowmap();

	ID3D11ShaderResourceView** GetSRV();
	ID3D11Texture2D* getDepthBuffer();
	XMMATRIX getLightView();
	XMMATRIX getLightProj();

	void Initialize(D3D* direct3D, int height, int width, XMVECTOR lightDir, ID3D11InputLayout* inputLayout);
	void SwitchWorldMatrix(XMMATRIX world);
	void CreateTransformationMatrices();
	void CreateShader();
	void BindShadowPass();
};

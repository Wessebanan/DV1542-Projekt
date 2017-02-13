#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DDSTextureLoader.h>
#include <DirectXMath.h>
#include "Camera.h"

using namespace DirectX;
class SkyBox
{
private:
	ID3D11VertexShader* VS_SkyBox = nullptr;
	ID3D11PixelShader* PS_SkyBox = nullptr;

	D3D11_RASTERIZER_DESC rasDesc;
	D3D11_DEPTH_STENCIL_DESC DSdesc;

	XMMATRIX world;

public:
	SkyBox(ID3D11Device* dev, Camera playerCam);
	~SkyBox();

	XMMATRIX* getWorldMatrix();
	ID3D11VertexShader* getVS();
	ID3D11PixelShader* getPS();
	D3D11_RASTERIZER_DESC getRasDesc();
	D3D11_DEPTH_STENCIL_DESC getDepthDesc();
};

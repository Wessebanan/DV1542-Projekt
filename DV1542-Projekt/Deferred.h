#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Window.h"
#include "D3D.h"
using namespace DirectX;
const int BUFFER_COUNT = 4;

class Deferred 
{
private:
	Window window;
	D3D direct3D;

	// 0: normals, 1: diffuse, 2: specular, 3: position
	ID3D11Texture2D* textures[BUFFER_COUNT];
	ID3D11RenderTargetView* renderTargetViews[BUFFER_COUNT];
	ID3D11ShaderResourceView* shaderResourceViews[BUFFER_COUNT];
	
	D3D11_VIEWPORT viewPort;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;

	ID3D11InputLayout* vertexLayout;
	ID3D11VertexShader* vertexShader;
	ID3D11VertexShader* vertexShaderLight;
	ID3D11GeometryShader* geometryShader;
	ID3D11PixelShader* pixelShaderG;
	ID3D11PixelShader* pixelShaderL;

	ID3D11SamplerState* samplerState;
	ID3D11Buffer* transformBuffer;
	struct matrixData
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
	};
	matrixData WVP;

	ID3D11Buffer* fullscreenQuadBuffer;

public:
	Deferred(HINSTANCE hInstance);
	~Deferred();

	void CreateShaders();
	ID3D11Texture2D* GetTexture(int index);
	ID3D11RenderTargetView* GetRenderTargetView(int index);
	ID3D11ShaderResourceView* GetShaderResourceView(int index);
	bool Initialize();
	void GeometryPass(XMMATRIX viewMatrix);
	void LightPass();

	void Draw(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, int numIndices);

	void CreateTransformBuffer();

	HRESULT CreateBuffer(D3D11_BUFFER_DESC* bufferDesc, D3D11_SUBRESOURCE_DATA* subResData, ID3D11Buffer** buffer);

	HWND GetWindowHandle();
};

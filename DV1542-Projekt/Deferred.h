#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Window.h"
#include "Camera.h"
#include "D3D.h"
#include <DDSTextureLoader.h>
using namespace DirectX;
const int BUFFER_COUNT = 4;

const int TEXTURE_COUNT = 3;

class Deferred 
{
private:
	Window window;
	D3D direct3D;

	// 0: normals, 1: diffuse, 2: specular, 3: position
	ID3D11Texture2D* textures[BUFFER_COUNT];
	ID3D11RenderTargetView* renderTargetViews[BUFFER_COUNT];
	ID3D11ShaderResourceView* shaderResourceViews[BUFFER_COUNT];

	ID3D11ShaderResourceView* unbindingSRVs[BUFFER_COUNT] = { NULL };

	D3D11_VIEWPORT viewPort;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;

	ID3D11InputLayout* vertexLayout;
	ID3D11VertexShader* vertexShader;
	ID3D11VertexShader* vertexShaderLight;
	ID3D11GeometryShader* geometryShader;
	ID3D11PixelShader* pixelShaderG;
	ID3D11PixelShader* pixelShaderL;

	ID3D11VertexShader* vertexShaderGenericObject;
	ID3D11PixelShader* pixelShaderGenericObject;

	ID3D11SamplerState* samplerState;
	ID3D11Buffer* transformBuffer;

	ID3D11Texture2D* grassTexture = nullptr;
	ID3D11Texture2D* dirtTexture = nullptr;
	ID3D11Texture2D* rockTexture = nullptr;

	//grass: 1, dirt: 2, dirt: 3
	ID3D11ShaderResourceView* textureSRVs[TEXTURE_COUNT];

	ID3D11ShaderResourceView* grassSRV = nullptr;
	ID3D11ShaderResourceView* dirtSRV = nullptr;
	ID3D11ShaderResourceView* rockSRV = nullptr;

	ID3D11Texture2D* normalMap = nullptr;
	ID3D11ShaderResourceView* normalSRV = nullptr;

	struct matrixData
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
	};
	matrixData WVP;

	Camera playerCamera;

	ID3D11Buffer* camPosBuffer;

	XMVECTOR camPos;

public:
	Deferred(HINSTANCE hInstance);
	~Deferred();

	void CreateShaders();

	bool Initialize();

	void GeometryPass();

	void LightPass();

	void SetHeightMapTexture(std::wstring filepath, unsigned int width, unsigned int height);

	void Draw(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, int numIndices, unsigned long long pVertexSize, DXGI_FORMAT format);

	void CreateTransformBuffer();

	Camera* GetCameraPointer();

	HRESULT CreateBuffer(D3D11_BUFFER_DESC* bufferDesc, D3D11_SUBRESOURCE_DATA* subResData, ID3D11Buffer** buffer);

	HWND GetWindowHandle();

	void CreateTextures();

	void CreateCamPosBuffer();

	IDXGISwapChain* GetSwapChain();
};

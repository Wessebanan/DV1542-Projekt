#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Window.h"
#include "Camera.h"
#include "D3D.h"
#include <DDSTextureLoader.h>
#include "Shadowmap.h"
#include "OBJLoader.h"

using namespace DirectX;
const int BUFFER_COUNT = 5;

const int TEXTURE_COUNT = 3;

enum OBJECT_TYPE
{
	TERRAIN = 0,
	CUBE = 1,
	BEAR = 2,
	SPHERE = 3
};


class Deferred 
{
private:
	Window window;
	D3D direct3D;
	Shadowmap shadowmap;

	// 0: normals, 1: diffuses, 2: speculars, 3: positions, 4: light positions
	ID3D11Texture2D* textures[BUFFER_COUNT];
	ID3D11RenderTargetView* renderTargetViews[BUFFER_COUNT];
	ID3D11ShaderResourceView* shaderResourceViews[BUFFER_COUNT];

	ID3D11ShaderResourceView* unbindingSRVs[BUFFER_COUNT] = { NULL };
	ID3D11RenderTargetView* unbindingRTVs[BUFFER_COUNT] = { NULL };

	D3D11_VIEWPORT viewPort;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;

	ID3D11InputLayout* vertexLayout;

	ID3D11VertexShader* vertexShaderTerrain;
	ID3D11VertexShader* vertexShaderLight;
	ID3D11GeometryShader* geometryShaderTerrain;
	ID3D11PixelShader* pixelShaderTerrain;
	ID3D11PixelShader* pixelShaderLight;

	ID3D11VertexShader* vertexShaderGenericObject;
	ID3D11PixelShader* pixelShaderGenericObject;

	ID3D11SamplerState* samplerState;
	ID3D11Buffer* transformBuffer;

	ID3D11Texture2D* grassTexture = nullptr;
	ID3D11Texture2D* dirtTexture = nullptr;
	ID3D11Texture2D* rockTexture = nullptr;
	ID3D11Texture2D* brickTexture = nullptr;
	ID3D11Texture2D* bearTexture = nullptr;
	ID3D11Texture2D* sphereTexture = nullptr;

	//grass: 0, dirt: 1, rock: 2 (for terrain).
	ID3D11ShaderResourceView* textureSRVs[TEXTURE_COUNT];

	ID3D11ShaderResourceView* grassSRV = nullptr;
	ID3D11ShaderResourceView* dirtSRV = nullptr;
	ID3D11ShaderResourceView* rockSRV = nullptr;
	ID3D11ShaderResourceView* brickSRV = nullptr;
	ID3D11ShaderResourceView* bearSRV = nullptr;
	ID3D11ShaderResourceView* sphereSRV = nullptr;

	ID3D11Texture2D* TerrainNormalMap = nullptr;
	ID3D11ShaderResourceView* TerrainNormalSRV = nullptr;

	struct matrixData
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
		XMMATRIX lightView; //For shadow
		XMMATRIX lightProj; //mapping.
	};
	matrixData WVP;

	Camera playerCamera;

	ID3D11Buffer* camPosBuffer;
	ID3D11Buffer* lightDirBuffer;

	XMVECTOR camPos;
	XMVECTOR lightDir; //Istället för att hårdkoda på flera ställen.

public:
	Deferred(HINSTANCE hInstance);
	~Deferred();

	void CreateShaders();

	bool Initialize();

	void InitialGeometryBinds();
	void BindTerrain();
	void BindGenericObject();

	void LightPass();

	void SetHeightMapTexture(std::wstring filepath, unsigned int width, unsigned int height);

	void Draw(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, int numIndices, XMMATRIX world, OBJECT_TYPE type);
	void DrawShadow(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, int numIndices, XMMATRIX world);

	void CreateTransformBuffer();

	Camera* GetCameraPointer();

	HRESULT CreateBuffer(D3D11_BUFFER_DESC* bufferDesc, D3D11_SUBRESOURCE_DATA* subResData, ID3D11Buffer** buffer);

	HWND GetWindowHandle();

	void CreateTerrainTextures();
	void CreateObjectTexture(Material mat, OBJECT_TYPE type);

	void CreateCamPosBuffer();
	void CreateLightDirBuffer();

	IDXGISwapChain* GetSwapChain();
	ID3D11Device* GetDevicePointer();

	Shadowmap* GetShadowmap();
};

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
#include "Blurrer.h"
#include "MeshObject.h"
#include "Frustum.h"

using namespace DirectX;
const int BUFFER_COUNT = 5;

const int TEXTURE_COUNT = 3;




class Deferred 
{
private:
	Window window;
	D3D direct3D;

	Shadowmap shadowmap;
	int shadowMapWidth;
	int shadowMapHeight;

	Blurrer blurrer;

	// 0: normals, 1: diffuses, 2: speculars, 3: positions, 4: light positions
	ID3D11Texture2D* textures[BUFFER_COUNT]						= { nullptr };
	ID3D11RenderTargetView* renderTargetViews[BUFFER_COUNT]		= { nullptr };
	ID3D11ShaderResourceView* shaderResourceViews[BUFFER_COUNT] = { nullptr };

	ID3D11ShaderResourceView* unbindingSRVs[BUFFER_COUNT] = { NULL };
	ID3D11RenderTargetView* unbindingRTVs[BUFFER_COUNT]	  = { NULL };

	D3D11_VIEWPORT viewPort;
	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11Texture2D* depthStencilBuffer		 = nullptr;

	ID3D11InputLayout* vertexLayout = nullptr;

	ID3D11RasterizerState* noCull = nullptr;
	ID3D11RasterizerState* bfCull = nullptr;

	ID3D11VertexShader* vertexShaderTerrain		= nullptr;
	ID3D11VertexShader* vertexShaderLight		= nullptr;
	ID3D11GeometryShader* geometryShaderTerrain = nullptr;
	ID3D11PixelShader* pixelShaderTerrain		= nullptr;
	ID3D11PixelShader* pixelShaderLight			= nullptr;

	ID3D11VertexShader* vertexShaderGenericObject = nullptr;
	ID3D11PixelShader* pixelShaderGenericObject	  = nullptr;

	ID3D11SamplerState* samplerState = nullptr;
	ID3D11Buffer* transformBuffer	 = nullptr;
	ID3D11Buffer* materialBuffer	 = nullptr;

	Material* cubeMaterial	 = nullptr;
	Material* bearMaterial	 = nullptr;
	Material* sphereMaterial = nullptr;

	//grass: 0, dirt: 1, rock: 2 (for terrain).
	ID3D11ShaderResourceView* textureSRVs[TEXTURE_COUNT] = { nullptr };

	ID3D11ShaderResourceView* grassSRV			= nullptr;
	ID3D11ShaderResourceView* dirtSRV			= nullptr;
	ID3D11ShaderResourceView* rockSRV			= nullptr;
	ID3D11ShaderResourceView* brickSRV			= nullptr;
	ID3D11ShaderResourceView* bearSRV			= nullptr;
	ID3D11ShaderResourceView* sphereSRV			= nullptr;
	ID3D11ShaderResourceView* TerrainNormalSRV  = nullptr;

	struct matrixData
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
		XMMATRIX lightView; //For shadow
		XMMATRIX lightProj; //mapping.
		XMVECTOR viewDir; //For GS bf culling.
	};
	matrixData WVP;

	Camera playerCamera;
	Frustum playerFrustum;

	ID3D11Buffer* camPosBuffer	 = nullptr;
	ID3D11Buffer* lightDirBuffer = nullptr;

	XMVECTOR camPos;
	XMVECTOR lightDir;
	XMVECTOR lightPos;
public:
	Deferred(HINSTANCE hInstance);
	~Deferred();

	void CreateShaders();

	bool Initialize();

	//Does the binds which are shared between terrain and generic objects.
	void InitialGeometryBinds();
	//Specific binds for terrain.
	void BindTerrain();
	//Specific binds for generic obejects.
	void BindGenericObject();

	//Does needed binds for light pass, includes the final draw call.
	void LightPass();

	void SetHeightMapTexture(std::wstring filepath, unsigned int width, unsigned int height);

	//Draws one object, intended for the geometry pass.
	void Draw(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, int numIndices, XMMATRIX world, OBJECT_TYPE type);
	//Draws the depth of one object from the perspective of the directional light.
	void DrawShadow(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, int numIndices, XMMATRIX world);
	//Blurs the shadow map using a compute shader.
	void BlurShadowMap();

	void CreateTransformBuffer();
	void CreateMaterialBuffer();

	Camera* GetCameraPointer();

	HRESULT CreateBuffer(D3D11_BUFFER_DESC* bufferDesc, D3D11_SUBRESOURCE_DATA* subResData, ID3D11Buffer** buffer);

	HWND GetWindowHandle();

	void CreateTerrainTextures();
	void CreateObjectTexture(Material* mat, OBJECT_TYPE type);

	//camPosBuffer and lightDirBuffer are separate 
	//because camPos changes while the contents of lightDirBuffer do not.
	void CreateCamPosBuffer();
	void CreateLightDirBuffer();

	IDXGISwapChain* GetSwapChain();
	ID3D11Device* GetDevicePointer();

	Shadowmap* GetShadowmap();

	void UpdateFrustum();
};

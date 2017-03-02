#include "Deferred.h"
#include "NoiseGenerator.h"

Deferred::Deferred(HINSTANCE hInstance) :
	window(hInstance)
{	
	this->window.Initialize();
	this->direct3D.Initialize(this->window.GetWindow());

	this->lightDir = XMVectorSet(1.0f, -1.0f, 0.0f, 0.0f);
	this->lightPos = XMVectorSet(500.0f, 200.0f, 500.0f, 0.0f);
	this->shadowMapHeight = 2048;
	this->shadowMapWidth  = 2048;

	if (!this->Initialize())
	{
		MessageBoxA(NULL, "Error initializing deferred", NULL, MB_OK);
	}
	this->shadowmap.Initialize(this->direct3D.getDevice(), this->direct3D.getDevCon(), this->shadowMapHeight, this->shadowMapWidth, this->lightDir, this->vertexLayout);
	this->blurrer.Initialize(this->direct3D.getDevice(), this->direct3D.getDevCon(), *this->shadowmap.GetSRV(), this->shadowMapHeight, this->shadowMapWidth);

	this->WVP.world = XMMatrixIdentity();
	this->WVP.view	= XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, -2.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	this->WVP.proj	= XMMatrixPerspectiveFovLH(XM_PI*0.45f, 4.0f / 3.0f, 0.1f, 2000.0f);
	this->WVP.lightView = this->shadowmap.getLightView();
	this->WVP.lightProj = this->shadowmap.getLightProj();

	this->direct3D.getDevCon()->IASetInputLayout(this->vertexLayout);
	this->direct3D.getDevCon()->RSSetViewports(1, &this->viewPort);
}

Deferred::~Deferred()
{
	for (int i = 0; i < BUFFER_COUNT; i++) 
	{
		this->textures[i]->Release();
		this->renderTargetViews[i]->Release();
		this->shaderResourceViews[i]->Release();
	}
	this->depthStencilBuffer->Release();
	this->depthStencilView->Release();

	this->vertexLayout->Release();

	this->vertexShaderTerrain->Release();
	this->vertexShaderLight->Release();
	this->geometryShaderTerrain->Release();
	this->pixelShaderTerrain->Release();
	this->pixelShaderLight->Release();
	this->vertexShaderGenericObject->Release();
	this->pixelShaderGenericObject->Release();

	this->samplerState->Release();
	
	if (this->transformBuffer != nullptr)
	{
		this->transformBuffer->Release();
	}

	if (this->materialBuffer != nullptr)
	{
		this->materialBuffer->Release();
	}

	this->camPosBuffer->Release();
	this->lightDirBuffer->Release();

	if (this->cubeMaterial != nullptr) {
		delete this->cubeMaterial;
	}
	if (this->bearMaterial != nullptr) {
		delete this->bearMaterial;
	}
	if (this->sphereMaterial != nullptr) {
		delete this->sphereMaterial;
	}

	this->grassSRV->Release();
	this->dirtSRV->Release();
	this->rockSRV->Release();
	this->brickSRV->Release();
	this->bearSRV->Release();
	this->sphereSRV->Release();
	this->TerrainNormalSRV->Release();
	this->noCull->Release();
}

void Deferred::CreateShaders()
{
	//create vertex shader
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(
		L"VS_Terrain.hlsl",
		nullptr,				
		nullptr,				
		"main",					
		"vs_5_0",				
		0,						
		0,						
		&pVS,					
		nullptr										
	);

	this->direct3D.getDevice()->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &this->vertexShaderTerrain);

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	this->direct3D.getDevice()->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &this->vertexLayout);	
	pVS->Release();

	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"PS_Terrain.hlsl",
		nullptr,	
		nullptr,	
		"main",		
		"ps_5_0",	
		0,			
		0,			
		&pPS,		
		nullptr			
	);

	this->direct3D.getDevice()->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &this->pixelShaderTerrain);	
	pPS->Release();

	ID3DBlob* pGS = nullptr;
	D3DCompileFromFile(
		L"GS_Terrain.hlsl",
		nullptr,
		nullptr,
		"main",
		"gs_5_0",
		0,
		0,
		&pGS,
		nullptr
	);

	this->direct3D.getDevice()->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &this->geometryShaderTerrain);
	pGS->Release();

	ID3DBlob* pPSL = nullptr;

	D3DCompileFromFile(
		L"PS_Light.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_5_0",
		0,
		0,
		&pPSL,
		nullptr
	);

	this->direct3D.getDevice()->CreatePixelShader(pPSL->GetBufferPointer(), pPSL->GetBufferSize(), nullptr, &this->pixelShaderLight);
	pPSL->Release();

	ID3DBlob* pVSL = nullptr;
	
	D3DCompileFromFile(
		L"VS_Light.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		0,
		0,
		&pVSL,
		nullptr
	);	

	this->direct3D.getDevice()->CreateVertexShader(pVSL->GetBufferPointer(), pVSL->GetBufferSize(), nullptr, &this->vertexShaderLight);
	pVSL->Release();

	ID3DBlob* pVSGO = nullptr;

	D3DCompileFromFile(
		L"VS_GenericObject.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		0,
		0,
		&pVSGO,
		nullptr
	);
	this->direct3D.getDevice()->CreateVertexShader(pVSGO->GetBufferPointer(), pVSGO->GetBufferSize(), nullptr, &this->vertexShaderGenericObject);
	pVSGO->Release();

	ID3DBlob* pPSGO = nullptr;

	D3DCompileFromFile(
		L"PS_GenericObject.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_5_0",
		0,
		0,
		&pPSGO,
		nullptr
	);
	this->direct3D.getDevice()->CreatePixelShader(pPSGO->GetBufferPointer(), pPSGO->GetBufferSize(), nullptr, &this->pixelShaderGenericObject);
	pPSGO->Release();
}

bool Deferred::Initialize()
{
	bool result = true;

	D3D11_SAMPLER_DESC sDesc{};
	sDesc.Filter		 = D3D11_FILTER_MIN_MAG_MIP_POINT; //Point sampling for min, mag and mip.
	sDesc.AddressU		 = D3D11_TEXTURE_ADDRESS_WRAP; //Wrap because we want to 
	sDesc.AddressV		 = D3D11_TEXTURE_ADDRESS_WRAP; //repeat our seamless
	sDesc.AddressW		 = D3D11_TEXTURE_ADDRESS_WRAP; //textuers.
	sDesc.MaxAnisotropy	 = 1;
	sDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sDesc.MaxLOD		 = D3D11_FLOAT32_MAX;

	if (FAILED(this->direct3D.getDevice()->CreateSamplerState(&sDesc, &this->samplerState)))
	{
		result = false;
	}

	// Creating the textures.
	D3D11_TEXTURE2D_DESC texDesc{};
	texDesc.Width			 = this->window.GetWidth();
	texDesc.Height			 = this->window.GetHeight();
	texDesc.MipLevels		 = 1;
	texDesc.ArraySize		 = 1;
	texDesc.Format			 = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage			 = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags		 = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags	 = 0;
	texDesc.MiscFlags		 = 0;

	for (int i = 0; i < BUFFER_COUNT; i++) 
	{
		if (FAILED(this->direct3D.getDevice()->CreateTexture2D(&texDesc, NULL, &this->textures[i]))) 
		{
			result = false;
		}
	}

	// Creating the render target views.
	D3D11_RENDER_TARGET_VIEW_DESC RTviewDesc{};
	RTviewDesc.Format			  = texDesc.Format;
	RTviewDesc.ViewDimension	  = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTviewDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i < BUFFER_COUNT; i++) 
	{
		if (FAILED(this->direct3D.getDevice()->CreateRenderTargetView(this->textures[i], &RTviewDesc, &this->renderTargetViews[i])))
		{
			result = false;
		}
	}

	// Creating the shader resource views.
	D3D11_SHADER_RESOURCE_VIEW_DESC SRviewDesc{};
	SRviewDesc.Format					 = texDesc.Format;
	SRviewDesc.ViewDimension			 = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRviewDesc.Texture2D.MostDetailedMip = 0;
	SRviewDesc.Texture2D.MipLevels		 = 1;

	for (int i = 0; i < BUFFER_COUNT; i++) 
	{
		if (FAILED(this->direct3D.getDevice()->CreateShaderResourceView(this->textures[i], &SRviewDesc, &this->shaderResourceViews[i])))
		{
			result = false;
		}
	}		
	
	// Creating the depth stencil view and the depth stencil buffer.	
	D3D11_TEXTURE2D_DESC depthBufferDesc{};
	depthBufferDesc.Width			 = this->window.GetWidth();
	depthBufferDesc.Height			 = this->window.GetHeight();
	depthBufferDesc.MipLevels		 = 1;
	depthBufferDesc.ArraySize		 = 1;
	depthBufferDesc.Format			 = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.Usage			 = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags		 = D3D11_BIND_DEPTH_STENCIL;

	if (FAILED(this->direct3D.getDevice()->CreateTexture2D(&depthBufferDesc, NULL, &this->depthStencilBuffer)))
	{
		result = false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(this->direct3D.getDevice()->CreateDepthStencilView(this->depthStencilBuffer, &depthStencilViewDesc, &this->depthStencilView)))
	{
		result = false;
	}	
	
	// Creating the viewport.
	this->viewPort.Width	= (float)this->window.GetWidth();
	this->viewPort.Height	= (float)this->window.GetHeight();
	this->viewPort.MinDepth = 0.0f;
	this->viewPort.MaxDepth = 1.0f;
	this->viewPort.TopLeftX = 0;
	this->viewPort.TopLeftY = 0;

	this->CreateShaders();

	this->CreateTransformBuffer();
	this->CreateMaterialBuffer();
	this->CreateCamPosBuffer();
	this->CreateLightDirBuffer();

	this->CreateTerrainTextures();

	this->textureSRVs[0] = this->grassSRV;
	this->textureSRVs[1] = this->dirtSRV;
	this->textureSRVs[2] = this->rockSRV;

	D3D11_RASTERIZER_DESC rDesc{};
	rDesc.CullMode = D3D11_CULL_NONE;
	rDesc.FillMode = D3D11_FILL_SOLID;
	this->direct3D.getDevice()->CreateRasterizerState(&rDesc, &this->noCull);

	return result;
}

void Deferred::InitialGeometryBinds()
{
	//Unbinding the textures from input to render to them again.
	this->direct3D.getDevCon()->PSSetShaderResources(0, BUFFER_COUNT, this->unbindingSRVs);

	//Setting the g-buffer textures as render targets.
	this->direct3D.getDevCon()->OMSetRenderTargets(BUFFER_COUNT, this->renderTargetViews, this->depthStencilView);

	XMVECTOR normalizedLightDir = this->lightDir;
	XMVector4Normalize(normalizedLightDir);

	float normalClearColor[] = { -XMVectorGetX(normalizedLightDir), -XMVectorGetY(normalizedLightDir), -XMVectorGetZ(normalizedLightDir) };
	float diffuseClearColor[] = { 135.0f / 255.0f,206.0f / 255.0f,250.0f / 255.0f,0 };
	float clearColor[] = { 0, 0, 0, 0 };

	//Clearing the normal g-buffer with a vector pointing towards the light direction to avoid shading of the background.
	this->direct3D.getDevCon()->ClearRenderTargetView(this->renderTargetViews[0], normalClearColor);
	//Clearing the diffuse color g-buffer with a sky blue color.
	this->direct3D.getDevCon()->ClearRenderTargetView(this->renderTargetViews[1], diffuseClearColor);
	//Clearing the rest of the g-buffers with a black color.
	for (int i = 2; i < BUFFER_COUNT; i++)
	{
		this->direct3D.getDevCon()->ClearRenderTargetView(this->renderTargetViews[i], clearColor);
	}

	this->direct3D.getDevCon()->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->direct3D.getDevCon()->IASetInputLayout(this->vertexLayout);
	this->direct3D.getDevCon()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	

	//Setting the matrices to the transformBuffer with the relevant changes.
	D3D11_MAPPED_SUBRESOURCE transformDataPtr;
	this->direct3D.getDevCon()->Map(this->transformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &transformDataPtr);
	this->WVP.view = this->playerCamera.GetViewMatrix();
	this->WVP.viewDir = this->playerCamera.GetCamPosition();
	memcpy(transformDataPtr.pData, &this->WVP, sizeof(matrixData));
	this->direct3D.getDevCon()->Unmap(this->transformBuffer, 0);

	//Same process as for transformBuffer but for the camera position.
	D3D11_MAPPED_SUBRESOURCE camPosDataPtr;
	this->direct3D.getDevCon()->Map(this->camPosBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &camPosDataPtr);
	this->camPos = this->playerCamera.GetCamPosition();
	memcpy(camPosDataPtr.pData, &this->camPos, sizeof(XMVECTOR));
	this->direct3D.getDevCon()->Unmap(this->camPosBuffer, 0);
	
	this->direct3D.getDevCon()->RSSetViewports(1, &this->viewPort);
}

void Deferred::BindTerrain()
{
	//Setting the correct shaders for the terrain geometry pass.
	this->direct3D.getDevCon()->VSSetShader(this->vertexShaderTerrain, nullptr, 0);
	this->direct3D.getDevCon()->PSSetShader(this->pixelShaderTerrain, nullptr, 0);
	this->direct3D.getDevCon()->GSSetShader(this->geometryShaderTerrain, nullptr, 0);

	//Setting a generic sampler for sampling whatever needs to be sampled.
	this->direct3D.getDevCon()->PSSetSamplers(0, 1, &this->samplerState);
	this->direct3D.getDevCon()->VSSetSamplers(0, 1, &this->samplerState);

	//Setting the normal map to the pixel shader.
	this->direct3D.getDevCon()->PSSetShaderResources(3, 1, &this->TerrainNormalSRV);

	//Disabling back face culling for terrain because it is done in GS.
	this->direct3D.getDevCon()->RSSetState(this->noCull);
}

void Deferred::BindGenericObject()
{
	//Setting the correct shaders for the geometry pass.
	this->direct3D.getDevCon()->VSSetShader(this->vertexShaderGenericObject, nullptr, 0);
	this->direct3D.getDevCon()->PSSetShader(this->pixelShaderGenericObject, nullptr, 0);
	this->direct3D.getDevCon()->GSSetShader(nullptr, nullptr, 0);

	//Enabling back face culling again because there is no GS.
	this->direct3D.getDevCon()->RSSetState(this->bfCull);
}

void Deferred::LightPass()
{
	float clearColor[] = { 0, 0, 0, 0 };

	//Unbinding the g-buffer textures from OM.
	this->direct3D.getDevCon()->OMSetRenderTargets(BUFFER_COUNT, this->unbindingRTVs, nullptr);

	//Setting the back buffer as the sole render target.
	this->direct3D.getDevCon()->OMSetRenderTargets(1, this->direct3D.getBackBufferRTV(), this->depthStencilView);
	this->direct3D.getDevCon()->ClearRenderTargetView(*this->direct3D.getBackBufferRTV(), clearColor);

	//Setting the shaders for the light pass, no GS necessary.
	this->direct3D.getDevCon()->VSSetShader(this->vertexShaderLight, nullptr, 0);
	this->direct3D.getDevCon()->PSSetShader(this->pixelShaderLight, nullptr, 0);
	this->direct3D.getDevCon()->GSSetShader(nullptr, nullptr, 0);

	//Setting the same sampler as the geometry pass, binding the g-buffer textures as shader resources. VS gets transformbuffer.	
	this->direct3D.getDevCon()->PSSetSamplers(0, 1, &this->samplerState);				
	this->direct3D.getDevCon()->PSSetShaderResources(0, BUFFER_COUNT, this->shaderResourceViews);

	//Setting the shadow map depth buffer as a shader resource to the pixel shader.
	this->direct3D.getDevCon()->PSSetShaderResources(BUFFER_COUNT, 1, this->blurrer.GetBlurredSM());

	this->direct3D.getDevCon()->VSSetConstantBuffers(0, 1, &this->transformBuffer);	
	
	//Setting the camPosBuffer and lightDirBuffer to the pixel shader.
	this->direct3D.getDevCon()->PSSetConstantBuffers(0, 1, &this->camPosBuffer);
	this->direct3D.getDevCon()->PSSetConstantBuffers(1, 1, &this->lightDirBuffer);

	//Drawing a full screen quad generated by the system as no vertex buffer is set.
	this->direct3D.getDevCon()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	this->direct3D.getDevCon()->Draw(4, 0);
	this->direct3D.getSwapChain()->Present(1, 0);

	//Unbinding the depthbuffer SRV from PS for future shadow mapping
	this->direct3D.getDevCon()->PSSetShaderResources(BUFFER_COUNT, 1, &this->unbindingSRVs[0]);
}

void Deferred::SetHeightMapTexture(std::wstring filepath, unsigned int width, unsigned int height) 
{
	NoiseGenerator noise1(this->direct3D.getDevice(), width, height);
	// Loads the heightmap into the noiseGenerator and saved the data for use in the Camera
	this->playerCamera.SetTerrainData(noise1.loadHeightmap(filepath, width, height), width, height);

	//ID3D11ShaderResourceView* gTextureView = nullptr;
	//ID3D11SamplerState* gSamplerState = nullptr;

	//D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
	//ZeroMemory(&resourceViewDesc, sizeof(resourceViewDesc));

	//resourceViewDesc.Format = noise1.getTextureDesc().Format;
	//resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//resourceViewDesc.Texture2D.MipLevels = noise1.getTextureDesc().MipLevels;
	//resourceViewDesc.Texture2D.MostDetailedMip = 0;
	//HRESULT hr = this->direct3D.getDevice()->CreateShaderResourceView(noise1.getTexture(), &resourceViewDesc, &gTextureView);
	//if (!SUCCEEDED(hr)) {
	//	MessageBox(NULL, L"Something went wrong trying to create the Shader Resource View.", NULL, MB_ICONEXCLAMATION);
	//}

	//D3D11_SAMPLER_DESC samplerDesc = {};
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.MaxAnisotropy = 1;
	//samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//hr = this->direct3D.getDevice()->CreateSamplerState(&samplerDesc, &gSamplerState);
	//if (!SUCCEEDED(hr)) {
	//	MessageBox(NULL, L"Something went wrong trying to create the Sampler State", NULL, MB_ICONEXCLAMATION);
	//}

	//this->direct3D.getDevCon()->VSSetShaderResources(0, 1, &gTextureView);
	//this->direct3D.getDevCon()->VSSetSamplers(0, 1, &gSamplerState);

	//gTextureView->Release();
	//gSamplerState->Release();
	
}

void Deferred::Draw(ID3D11Buffer * vertexBuffer, ID3D11Buffer * indexBuffer, int numIndices, XMMATRIX world, OBJECT_TYPE type)
{
	if (!(vertexBuffer == nullptr))
	{ 
		UINT32 vertexSize = sizeof(float) * 8;
		UINT32 offset = 0;

		D3D11_MAPPED_SUBRESOURCE transformDataPtr;
		this->direct3D.getDevCon()->Map(this->transformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &transformDataPtr);
		this->WVP.world = world;
		memcpy(transformDataPtr.pData, &this->WVP, sizeof(matrixData));
		this->direct3D.getDevCon()->Unmap(this->transformBuffer, 0);

		D3D11_MAPPED_SUBRESOURCE materialDataPtr;
		Material* currentMat = this->cubeMaterial; // TODO: Add default material to class, cubeMaterial used as placeholder
		switch (type) {
		case CUBE: {
			currentMat = this->cubeMaterial;
			break;
		}
		case BEAR: {
			currentMat = this->bearMaterial;
			break;
		}
		case SPHERE: {
			currentMat = this->sphereMaterial;
			break;
		}
		}
		this->direct3D.getDevCon()->Map(this->materialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &materialDataPtr);
		memcpy(materialDataPtr.pData, currentMat, sizeof(Material));
		this->direct3D.getDevCon()->Unmap(this->materialBuffer, 0);

		switch (type)
		{
		case TERRAIN:
		{
			this->direct3D.getDevCon()->PSSetShaderResources(0, 3, this->textureSRVs);
			this->direct3D.getDevCon()->GSSetConstantBuffers(0, 1, &this->transformBuffer);
			break;
		}
		case CUBE:
		{
			this->direct3D.getDevCon()->PSSetShaderResources(0, 1, &this->brickSRV);
			this->direct3D.getDevCon()->VSSetConstantBuffers(0, 1, &this->transformBuffer);
			this->direct3D.getDevCon()->PSSetConstantBuffers(0, 1, &this->materialBuffer);
			break;
		}
		case BEAR:
		{
			this->direct3D.getDevCon()->PSSetShaderResources(0, 1, &this->bearSRV);
			this->direct3D.getDevCon()->VSSetConstantBuffers(0, 1, &this->transformBuffer);
			this->direct3D.getDevCon()->PSSetConstantBuffers(0, 1, &this->materialBuffer);
			break;
		}
		case SPHERE:
		{
			this->direct3D.getDevCon()->PSSetShaderResources(0, 1, &this->sphereSRV);
			this->direct3D.getDevCon()->VSSetConstantBuffers(0, 1, &this->transformBuffer);
			this->direct3D.getDevCon()->PSSetConstantBuffers(0, 1, &this->materialBuffer);
			break;
		}
		}

		this->direct3D.getDevCon()->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
		this->direct3D.getDevCon()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		this->direct3D.getDevCon()->DrawIndexed(numIndices, 0, 0);		
	}
}

void Deferred::DrawShadow(ID3D11Buffer * vertexBuffer, ID3D11Buffer * indexBuffer, int numIndices, XMMATRIX world)
{
	if (vertexBuffer != nullptr)
	{
		UINT32 vertexSize = sizeof(float) * 8;
		UINT32 vertexOffset = 0;
		this->shadowmap.SwitchWorldMatrix(world);
		this->direct3D.getDevCon()->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &vertexOffset);
		this->direct3D.getDevCon()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		this->direct3D.getDevCon()->DrawIndexed(numIndices, 0, 0);
	}
}

void Deferred::BlurShadowMap()
{
	this->blurrer.Blur();
}

void Deferred::CreateTransformBuffer()
{
	D3D11_BUFFER_DESC WVPdesc = {};
	WVPdesc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	WVPdesc.ByteWidth		= sizeof(matrixData);
	WVPdesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
	WVPdesc.Usage			= D3D11_USAGE_DYNAMIC;

	if (FAILED(this->direct3D.getDevice()->CreateBuffer(&WVPdesc, nullptr, &this->transformBuffer)))
	{
		MessageBoxA(NULL, "Error creating transform buffer.", NULL, MB_OK);
		exit(-1);
	}
}

void Deferred::CreateMaterialBuffer() {

	D3D11_BUFFER_DESC materialDesc = {};
	materialDesc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	materialDesc.ByteWidth		= sizeof(Material);
	materialDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialDesc.Usage			= D3D11_USAGE_DYNAMIC;

	if (FAILED(this->direct3D.getDevice()->CreateBuffer(&materialDesc, nullptr, &this->materialBuffer))) {
		MessageBoxA(NULL, "Error creating material buffer.", NULL, MB_OK);
		exit(-1);
	}
}

Camera * Deferred::GetCameraPointer()
{
	return &this->playerCamera;
}

HRESULT Deferred::CreateBuffer(D3D11_BUFFER_DESC * bufferDesc, D3D11_SUBRESOURCE_DATA * subResData, ID3D11Buffer ** buffer)
{
	return SUCCEEDED(this->direct3D.getDevice()->CreateBuffer(bufferDesc, subResData, buffer));
}

HWND Deferred::GetWindowHandle()
{
	return this->window.GetWindow();
}

void Deferred::CreateTerrainTextures()
{

	HRESULT hr;
	hr = CreateDDSTextureFromFile(this->direct3D.getDevice(), L"grassTex.dds", NULL, &this->grassSRV);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating grass texture.", NULL, MB_OK);
	}
	hr = CreateDDSTextureFromFile(this->direct3D.getDevice(), L"rockTex.dds", NULL, &this->rockSRV);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating rock texture.", NULL, MB_OK);
	}
	hr = CreateDDSTextureFromFile(this->direct3D.getDevice(), L"dirtTex.dds", NULL, &this->dirtSRV);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating dirt texture.", NULL, MB_OK);
	}
	hr = CreateDDSTextureFromFile(this->direct3D.getDevice(), L"NormalMap9.dds", NULL, &this->TerrainNormalSRV);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating normal map.", NULL, MB_OK);
	}
}

void Deferred::CreateObjectTexture(Material* mat, OBJECT_TYPE type) {

	HRESULT hr;

	const size_t size = strlen(mat->textureFilePath) + 1;
	wchar_t* wcharFilePath = new wchar_t[size];
	mbstowcs(wcharFilePath, mat->textureFilePath, size);

	switch (type)
	{
	case CUBE: {
		hr = CreateDDSTextureFromFile(this->direct3D.getDevice(), wcharFilePath, NULL, &this->brickSRV);
		this->cubeMaterial = mat;
		if (FAILED(hr))
		{
			MessageBoxA(NULL, "Error creating cube texture", NULL, MB_OK);
		}
		break;
	}
	case BEAR: {
		hr = CreateDDSTextureFromFile(this->direct3D.getDevice(), wcharFilePath, NULL, &this->bearSRV);
		this->bearMaterial = mat;
		if (FAILED(hr))
		{
			MessageBoxA(NULL, "Error creating bear texture", NULL, MB_OK);
		}
		break;
	}
	case SPHERE: {
		hr = CreateDDSTextureFromFile(this->direct3D.getDevice(), wcharFilePath, NULL, &this->sphereSRV);
		this->sphereMaterial = mat;
		if (FAILED(hr))
		{
			MessageBoxA(NULL, "Error creating sphere texture", NULL, MB_OK);
		}
		break;
	}
	}
	delete[] wcharFilePath;
}

void Deferred::CreateCamPosBuffer()
{
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.ByteWidth		= sizeof(XMVECTOR);
	bufDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
	bufDesc.Usage			= D3D11_USAGE_DYNAMIC;

	if (FAILED(this->direct3D.getDevice()->CreateBuffer(&bufDesc, nullptr, &this->camPosBuffer)))
	{
		MessageBoxA(NULL, "Error creating camPosBuffer.", NULL, MB_OK);
		exit(-1);
	}
}

void Deferred::CreateLightDirBuffer()
{
	struct lightDirBufferStruct
	{
		XMVECTOR lightDir; //Directional light.
		XMVECTOR shadowDimensions; //Dimensions of the shadow map.
		XMVECTOR lightPos; //Point light.
	};
	lightDirBufferStruct LDBS;
	LDBS.lightDir = this->lightDir;
	LDBS.shadowDimensions = XMVectorSet((float)this->shadowMapWidth, (float)this->shadowMapHeight, 0.0f, 0.0f);
	LDBS.lightPos = this->lightPos;

	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.ByteWidth = sizeof(lightDirBufferStruct);
	bufDesc.Usage	  = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = &LDBS;

	HRESULT hr = this->direct3D.getDevice()->CreateBuffer(&bufDesc, &subData, &this->lightDirBuffer);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating lightDirBuffer.", NULL, MB_OK);
	}
}

IDXGISwapChain * Deferred::GetSwapChain()
{
	return this->direct3D.getSwapChain();
}

ID3D11Device * Deferred::GetDevicePointer()
{
	return this->direct3D.getDevice();
}

Shadowmap * Deferred::GetShadowmap()
{
	return &this->shadowmap;
}




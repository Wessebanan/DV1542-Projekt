#include "Deferred.h"

Deferred::Deferred(HINSTANCE hInstance) :
	window(hInstance)
{	
	for (int i = 0; i < BUFFER_COUNT; i++) 
	{
		this->textures[i] = nullptr;
		this->renderTargetViews[i] = nullptr;
		this->shaderResourceViews[i] = nullptr;
	}
}

Deferred::~Deferred()
{
	for (int i = 0; i < BUFFER_COUNT; i++) 
	{
		this->textures[i]->Release();
		this->renderTargetViews[i]->Release();
		this->shaderResourceViews[i]->Release();
	}
}

void Deferred::CreateShaders()
{
	//create vertex shader
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(
		L"VertexShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"main",		// entry point
		"vs_5_0",		// shader model (target)
		0,				// shader compile options			// here DEBUGGING OPTIONS
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);

	this->direct3D.getDevice()->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &this->vertexShader);

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	this->direct3D.getDevice()->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &this->vertexLayout);
	// we do not need anymore this COM object, so we release it.
	pVS->Release();

	//create pixel shader
	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"PixelShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"main",		// entry point
		"ps_5_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pPS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);

	this->direct3D.getDevice()->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &this->pixelShaderG);
	// we do not need anymore this COM object, so we release it.
	pPS->Release();

	//create geomety shader (same way as the other shaders).
	ID3DBlob* pGS = nullptr;
	D3DCompileFromFile(
		L"GeometryShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"gs_5_0",
		0,
		0,
		&pGS,
		nullptr
	);

	this->direct3D.getDevice()->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &this->geometryShader);
	pGS->Release();
}

ID3D11Texture2D * Deferred::GetTexture(int index)
{
	return this->textures[index];
}

ID3D11RenderTargetView * Deferred::GetRenderTargetView(int index)
{
	return this->renderTargetViews[index];
}

ID3D11ShaderResourceView * Deferred::GetShaderResourceView(int index)
{
	return this->shaderResourceViews[index];
}

bool Deferred::Initialize()
{
	bool result = true;
	D3D11_TEXTURE2D_DESC texDesc{};
	D3D11_RENDER_TARGET_VIEW_DESC RTviewDesc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC SRviewDesc{};

	// Creating the textures.
	texDesc.Width = this->window.GetWidth();
	texDesc.Height = this->window.GetHeight();
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	for (int i = 0; i < BUFFER_COUNT; i++) 
	{
		if (FAILED(this->direct3D.getDevice()->CreateTexture2D(&texDesc, NULL, &this->textures[i]))) 
		{
			result = false;
		}
	}

	// Creating the render target views.
	RTviewDesc.Format = texDesc.Format;
	RTviewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTviewDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i < BUFFER_COUNT; i++) 
	{
		if (FAILED(this->direct3D.getDevice()->CreateRenderTargetView(this->textures[i], &RTviewDesc, &this->renderTargetViews[i])))
		{
			result = false;
		}
	}

	// Creating the shader resource views.
	SRviewDesc.Format = texDesc.Format;
	SRviewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRviewDesc.Texture2D.MostDetailedMip = 0;
	SRviewDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < BUFFER_COUNT; i++) 
	{
		if (FAILED(this->direct3D.getDevice()->CreateShaderResourceView(this->textures[i], &SRviewDesc, &this->shaderResourceViews[i])))
		{
			result = false;
		}
	}		
	
	// Creating the depth stencil view and the depth stencil buffer.
	
	D3D11_TEXTURE2D_DESC depthBufferDesc{};
	depthBufferDesc.Width = this->window.GetWidth();
	depthBufferDesc.Height = this->window.GetHeight();
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	if (FAILED(this->direct3D.getDevice()->CreateTexture2D(&depthBufferDesc, NULL, &this->depthStencilBuffer)))
	{
		result = false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(this->direct3D.getDevice()->CreateDepthStencilView(this->depthStencilBuffer, &depthStencilViewDesc, &this->depthStencilView)))
	{
		result = false;
	}	
	
	// Creating the viewport.
	this->viewPort.Width = this->window.GetWidth();
	this->viewPort.Height = this->window.GetHeight();
	this->viewPort.MinDepth = 0.0f;
	this->viewPort.MaxDepth = 1.0f;
	this->viewPort.TopLeftX = 0;
	this->viewPort.TopLeftY = 0;

	this->CreateShaders();

	return result;
}

void Deferred::GeometryPass()
{
	this->direct3D.getDevCon()->RSSetViewports(1, &this->viewPort);
	this->direct3D.getDevCon()->OMSetRenderTargets(BUFFER_COUNT, this->renderTargetViews, this->depthStencilView);

	this->ClearRenderTargets();



}

void Deferred::SetRenderTargets()
{
	// Bind the geometry render target views and depth stencil buffer to the output render pipeline.
	this->direct3D.getDevCon()->OMSetRenderTargets(BUFFER_COUNT, this->renderTargetViews, this->depthStencilView);

	this->direct3D.getDevCon()->RSSetViewports(1, &this->viewPort);
}

void Deferred::ClearRenderTargets()
{
	float clearColor[] = { 0,0,0,0 }; //Clear color is black.

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		this->direct3D.getDevCon()->ClearRenderTargetView(this->renderTargetViews[i], clearColor);
	}

	this->direct3D.getDevCon()->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

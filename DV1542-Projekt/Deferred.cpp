#include "Deferred.h"

Deferred::Deferred()
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

bool Deferred::Initialize(ID3D11Device * device, ID3D11DeviceContext * devcon, int texWidth, int texHeight)
{
	bool result = true;
	D3D11_TEXTURE2D_DESC texDesc{};
	D3D11_RENDER_TARGET_VIEW_DESC RTviewDesc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC SRviewDesc{};

	this->texHeight = texHeight;
	this->texWidth = texWidth;

	// Creating the textures.
	texDesc.Width = this->texWidth;
	texDesc.Height = this->texHeight;
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
		if (FAILED(device->CreateTexture2D(&texDesc, NULL, &this->textures[i]))) 
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
		if (FAILED(device->CreateRenderTargetView(this->textures[i], &RTviewDesc, &this->renderTargetViews[i]))) 
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
		if (FAILED(device->CreateShaderResourceView(this->textures[i], &SRviewDesc, &this->shaderResourceViews[i]))) 
		{
			result = false;
		}
	}		
	
	// Creating the depth stencil view and the depth stencil buffer.
	
	D3D11_TEXTURE2D_DESC depthBufferDesc{};
	depthBufferDesc.Width = this->texWidth;
	depthBufferDesc.Height = this->texHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	if (FAILED(device->CreateTexture2D(&depthBufferDesc, NULL, &this->depthStencilBuffer))) 
	{
		result = false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(device->CreateDepthStencilView(this->depthStencilBuffer, &depthStencilViewDesc, &this->depthStencilView)))
	{
		result = false;
	}	
	
	// Creating the viewport.
	this->viewPort.Width = this->texWidth;
	this->viewPort.Height = this->texHeight;
	this->viewPort.MinDepth = 0.0f;
	this->viewPort.MaxDepth = 1.0f;
	this->viewPort.TopLeftX = 0;
	this->viewPort.TopLeftY = 0;		

	return result;
}

void Deferred::GeometryPass()
{

}

void Deferred::SetRenderTargets(ID3D11DeviceContext * devcon)
{
	// Bind the geometry render target views and depth stencil buffer to the output render pipeline.
	devcon->OMSetRenderTargets(BUFFER_COUNT, this->renderTargetViews, this->depthStencilView);

	devcon->RSSetViewports(1, &this->viewPort);
}

void Deferred::ClearRenderTargets(ID3D11DeviceContext * devcon)
{
	float clearColor[] = { 0,0,0,0 }; //Clear color is black.

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		devcon->ClearRenderTargetView(this->renderTargetViews[i], clearColor);
	}

	devcon->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

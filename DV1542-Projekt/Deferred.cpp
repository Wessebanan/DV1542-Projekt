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

bool Deferred::Initialize(ID3D11Device * device, ID3D11DeviceContext * devcon)
{
	bool result = true;
	D3D11_TEXTURE2D_DESC texDesc{};
	D3D11_RENDER_TARGET_VIEW_DESC RTviewDesc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC SRviewDesc{};


	// Creating the textures.
	texDesc.Width = 640;
	texDesc.Height = 480;
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
	
	ID3D11Texture2D* depthStencilTex = nullptr;
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc{};
	depthStencilBufferDesc.Width = 640;
	depthStencilBufferDesc.Height = 480;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	if (FAILED(device->CreateTexture2D(&depthStencilBufferDesc, NULL, &depthStencilTex))) 
	{
		result = false;
	}
	

	return result;
}

void Deferred::BindGeometryPass()
{

}

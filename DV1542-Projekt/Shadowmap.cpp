#include "Shadowmap.h"

Shadowmap::Shadowmap(D3D direct3D, D3D11_VIEWPORT vp, int height, int width)
{
	this->direct3D = direct3D;
	this->viewPort = vp;

	D3D11_TEXTURE2D_DESC dsDesc{};
	dsDesc.Height = height;
	dsDesc.Width = width;
	dsDesc.CPUAccessFlags = 0;
	dsDesc.ArraySize = 1;
	dsDesc.MipLevels = 1;
	dsDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;;
	dsDesc.MiscFlags = 0;
	dsDesc.SampleDesc.Count = 1;
	dsDesc.Usage = D3D11_USAGE_DEFAULT;

	this->direct3D.getDevice()->CreateTexture2D(&dsDesc, nullptr, &this->depthStencilBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	this->direct3D.getDevice()->CreateDepthStencilView(this->depthStencilBuffer, &dsvDesc, &this->depthStencilView);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	this->direct3D.getDevice()->CreateShaderResourceView(this->depthStencilBuffer, &srvDesc, &this->depthMapSRV);

	this->CreateTransformationMatrices();

	D3D11_BUFFER_DESC bufDesc{};
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.ByteWidth = sizeof(XMMATRIX) * 3;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.MiscFlags = 0;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA subResData{};
	subResData.pSysMem = &this->WVP;
	
	this->direct3D.getDevice()->CreateBuffer(&bufDesc, &subResData, &this->transformBuffer);

	this->CreateShaders();

	this->direct3D.getDevCon()->RSSetViewports(1, &this->viewPort);
}

Shadowmap::~Shadowmap()
{
	this->VS_Shadow->Release();
	this->PS_Shadow->Release();
	this->transformBuffer->Release();
	this->depthMapSRV->Release();
	this->depthMapSRV->Release();
	this->depthStencilView->Release();
}

void Shadowmap::CreateTransformationMatrices()
{
	this->WVP.world = XMMatrixIdentity();
	this->WVP.view = XMMatrixLookAtLH(-100.0f*this->lightDir + XMVectorSet(0.0f, 0.0f, 500.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 500.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	this->WVP.proj = XMMatrixOrthographicLH(1000.0f, 1000.0f, 1.0f, 2500.0f);
}

void Shadowmap::CreateShaders()
{
	HRESULT hr;
	ID3D10Blob* pVS = nullptr;
	hr = D3DCompileFromFile(
		L"VS_Shadow.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		0,
		0,
		&pVS,
		nullptr
		);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "ERROR COMPILING VS_SHADOW", NULL, MB_OK);
		exit(-1);
	}
	
	this->direct3D.getDevice()->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &this->VS_Shadow);
	pVS->Release();
	
	ID3D10Blob* pPS = nullptr;
	hr = D3DCompileFromFile(
		L"PS_Shadow.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_5_0",
		0,
		0,
		&pPS,
		nullptr
		);

	if (FAILED(hr))
	{
		MessageBoxA(NULL, "ERROR COMPILING VS_SHADOW", NULL, MB_OK);
		exit(-1);
	}
		
	this->direct3D.getDevice()->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &this->PS_Shadow);
	pPS->Release();
}

void Shadowmap::Shadowpass(XMVECTOR lightDir)
{
	this->direct3D.getDevCon()->VSSetShader(this->VS_Shadow, nullptr, 0);
	this->direct3D.getDevCon()->PSSetShader(this->PS_Shadow, nullptr, 0);

	ID3D11RenderTargetView* nullRTV = 0;
	this->direct3D.getDevCon()->OMSetRenderTargets(1, &nullRTV, this->depthStencilView);

	this->direct3D.getDevCon()->VSSetConstantBuffers(0, 1, &this->transformBuffer);

	this->direct3D.getDevCon()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	this->direct3D.getDevCon()->Draw(4, 0);	
}

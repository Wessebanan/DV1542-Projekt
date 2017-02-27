#include "Shadowmap.h"

Shadowmap::Shadowmap()
{
	
}

Shadowmap::~Shadowmap()
{
	if(this->VS_Shadow != nullptr)
		this->VS_Shadow->Release();
	if(this->transformBuffer != nullptr)
		this->transformBuffer->Release();
	if(this->depthMapSRV != nullptr)
		this->depthMapSRV->Release();
	if(this->depthStencilBuffer != nullptr)
		this->depthStencilBuffer->Release();
	if(this->depthStencilView != nullptr)
		this->depthStencilView->Release();
}

ID3D11ShaderResourceView ** Shadowmap::GetSRV()
{
	return &this->depthMapSRV;
}

XMMATRIX Shadowmap::getLightView()
{
	return this->WVP.view;
}

XMMATRIX Shadowmap::getLightProj()
{
	return this->WVP.proj;
}

void Shadowmap::Initialize(D3D* direct3D, int height, int width, XMVECTOR lightDir, ID3D11InputLayout* inputLayout)
{
	this->device		= direct3D->getDevice();
	this->devCon		= direct3D->getDevCon();
	this->lightDir		= lightDir;
	this->inputLayout	= inputLayout;

	HRESULT hr;

	this->viewPort.Height	= height;
	this->viewPort.Width	= width;
	this->viewPort.MinDepth = 0.0f;
	this->viewPort.MaxDepth = 1.0f;
	this->viewPort.TopLeftX = 0;
	this->viewPort.TopLeftY = 0; 

	D3D11_TEXTURE2D_DESC dsDesc{};
	dsDesc.Height			= height;
	dsDesc.Width			= width;
	dsDesc.CPUAccessFlags	= 0;
	dsDesc.ArraySize		= 1;
	dsDesc.MipLevels		= 1;
	dsDesc.Format			= DXGI_FORMAT_R24G8_TYPELESS;
	dsDesc.BindFlags		= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	dsDesc.MiscFlags		= 0;
	dsDesc.SampleDesc.Count = 1;
	dsDesc.Usage			= D3D11_USAGE_DEFAULT;

	hr = this->device->CreateTexture2D(&dsDesc, nullptr, &this->depthStencilBuffer);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating depthbuffer texture.", NULL, MB_OK);
	}
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice  = 0;

	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer, &dsvDesc, &this->depthStencilView);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating depthstencilview.", NULL, MB_OK);
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format						= DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels			= 1;
	srvDesc.Texture2D.MostDetailedMip	= 0;

	hr = this->device->CreateShaderResourceView(this->depthStencilBuffer, &srvDesc, &this->depthMapSRV);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating depthbuffer srv.", NULL, MB_OK);
	}
	this->CreateTransformationMatrices();

	D3D11_BUFFER_DESC bufDesc{};
	bufDesc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.ByteWidth		= sizeof(matrixData);
	bufDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
	bufDesc.MiscFlags		= 0;
	bufDesc.Usage			= D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA subResData{};
	subResData.pSysMem = &this->WVP;

	hr = this->device->CreateBuffer(&bufDesc, &subResData, &this->transformBuffer);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating transformbuffer.", NULL, MB_OK);
	}
	this->CreateShader();
}

void Shadowmap::SwitchWorldMatrix(XMMATRIX world)
{
	D3D11_MAPPED_SUBRESOURCE dataPtr;

	this->devCon->Map(this->transformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	this->WVP.world = world;
	memcpy(dataPtr.pData, &this->WVP, sizeof(matrixData));
	this->devCon->Unmap(this->transformBuffer, 0);
	this->devCon->VSSetConstantBuffers(0, 1, &this->transformBuffer);
}

void Shadowmap::CreateTransformationMatrices()
{
	XMVECTOR focusPos	= XMVectorSet(512.0f, 20.0f, 512.0f, 1.0f);
	this->WVP.world		= XMMatrixIdentity();
	this->WVP.view		= XMMatrixLookAtLH(-400.0f * this->lightDir + focusPos, focusPos, XMVectorSet(6.0f, 12.0f, 0.0f, 0.0f));
	this->WVP.proj		= XMMatrixOrthographicLH(1100.0f, 1100.0f, 1.0f, 1500.0f);
}

void Shadowmap::CreateShader()
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
	
	this->device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &this->VS_Shadow);
	pVS->Release();
}

void Shadowmap::BindShadowPass()
{	
	this->devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->devCon->IASetInputLayout(this->inputLayout);

	this->devCon->VSSetShader(this->VS_Shadow, nullptr, 0);
	this->devCon->GSSetShader(nullptr, nullptr, 0);
	this->devCon->PSSetShader(nullptr, nullptr, 0);

	ID3D11RenderTargetView* nullRTV = 0;
	this->devCon->OMSetRenderTargets(1, &nullRTV, this->depthStencilView);
	this->devCon->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->devCon->RSSetViewports(1, &this->viewPort);
}

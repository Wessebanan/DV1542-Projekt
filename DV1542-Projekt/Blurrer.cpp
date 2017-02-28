#include "Blurrer.h"

Blurrer::Blurrer()
{
}

Blurrer::~Blurrer()
{
	this->blurCS->Release();
	this->blurredSMTex->Release();
	this->clampSampler->Release();
	this->RWtexUAV->Release();
	this->blurredSMSRV->Release();
	this->dimBuffer->Release();
}

void Blurrer::Initialize(ID3D11Device * device, ID3D11DeviceContext * devCon, ID3D11ShaderResourceView* shadowMap, int height, int width)
{
	HRESULT hr;

	this->device		= device;
	this->devCon		= devCon;
	this->shadowMapSRV	= shadowMap;
	this->height		= 2048;
	this->width			= 2048;

	this->CreateShader();
	this->CreateDimBuffer();

	D3D11_TEXTURE2D_DESC TexDesc{};
	TexDesc.Height			 = this->height;
	TexDesc.Width			 = this->width;
	TexDesc.ArraySize		 = 1;
	TexDesc.MipLevels		 = 1;
	TexDesc.Format			 = DXGI_FORMAT_R32_FLOAT;
	TexDesc.BindFlags		 = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.Usage			 = D3D11_USAGE_DEFAULT;

	hr = this->device->CreateTexture2D(&TexDesc, nullptr, &this->blurredSMTex);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating blurredSMtex", NULL, MB_OK);
		exit(-1);
	}

	//Creating an unordered access view for use in the CS.
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdesc{};
	UAVdesc.ViewDimension		= D3D11_UAV_DIMENSION_TEXTURE2D;
	UAVdesc.Format				= TexDesc.Format;
	UAVdesc.Buffer.FirstElement = 0;
	UAVdesc.Buffer.NumElements	= 2048 * 2048;
	UAVdesc.Buffer.Flags		= D3D11_BUFFER_UAV_FLAG_RAW;

	hr = this->device->CreateUnorderedAccessView(this->blurredSMTex, &UAVdesc, &this->RWtexUAV);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating UAV", NULL, MB_OK);
		exit(-1);
	}

	D3D11_SAMPLER_DESC sDesc{};
	sDesc.AddressU		 = D3D11_TEXTURE_ADDRESS_CLAMP; //We want to clamp
	sDesc.AddressV		 = D3D11_TEXTURE_ADDRESS_CLAMP; //to avoid weird effects
	sDesc.AddressW		 = D3D11_TEXTURE_ADDRESS_CLAMP; //near the edges.
	sDesc.Filter		 = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sDesc.MaxAnisotropy	 = 1;
	sDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sDesc.MaxLOD		 = D3D11_FLOAT32_MAX;

	hr = this->device->CreateSamplerState(&sDesc, &this->clampSampler);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating clampsampler", NULL, MB_OK);
		exit(-1);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format					  = TexDesc.Format;
	srvDesc.ViewDimension			  = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels		  = 1;

	hr = this->device->CreateShaderResourceView(this->blurredSMTex, &srvDesc, &this->blurredSMSRV);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating blurred SM SRV", NULL, MB_OK);
		exit(-1);
	}
}
void Blurrer::CreateShader()
{
	HRESULT hr;

	ID3DBlob* pCS = nullptr;
	hr = D3DCompileFromFile(
		L"CS_Blur.hlsl",
		nullptr,
		nullptr,
		"main",
		"cs_5_0",
		0,
		0,
		&pCS,
		nullptr
		);
	
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error compiling CS.", NULL, MB_OK);
		exit(-1);
	}

	this->device->CreateComputeShader(pCS->GetBufferPointer(), pCS->GetBufferSize(), nullptr, &this->blurCS);
	pCS->Release();
}

void Blurrer::CreateDimBuffer()
{
	struct Dimensions
	{
		int height;
		int width;
		int padding, padding2;
	};
	Dimensions dims = { this->height, this->width };

	D3D11_BUFFER_DESC bufDesc{};
	bufDesc.Usage	  = D3D11_USAGE_IMMUTABLE;
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.ByteWidth = sizeof(Dimensions);

	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = &dims;

	HRESULT hr = this->device->CreateBuffer(&bufDesc, &data, &this->dimBuffer);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating dim buffer", NULL, MB_OK);
		exit(-1);
	}
}

void Blurrer::Blur()
{
	this->devCon->CSSetShader(this->blurCS, nullptr, 0);
	this->devCon->CSSetShaderResources(0, 1, &this->shadowMapSRV);
	this->devCon->CSSetUnorderedAccessViews(0, 1, &this->RWtexUAV, 0);
	this->devCon->CSSetConstantBuffers(0, 1, &this->dimBuffer);

	//TODO: något klyftigt med dimensions för dispatch.
	this->devCon->Dispatch(64, 64, 1);

	ID3D11ShaderResourceView* nullSRV	= nullptr;
	ID3D11UnorderedAccessView* nullUAV	= nullptr;
	this->devCon->CSSetShaderResources(0, 1, &nullSRV);
	this->devCon->CSSetUnorderedAccessViews(0, 1, &nullUAV, 0);
}

ID3D11ShaderResourceView ** Blurrer::GetBlurredSM()
{
	return &this->blurredSMSRV;
}

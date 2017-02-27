#include "Blurrer.h"

Blurrer::Blurrer()
{
}

Blurrer::~Blurrer()
{
	this->blurCS->Release();
}

bool Blurrer::Initialize(ID3D11Device * device, ID3D11DeviceContext * devCon, ID3D11ShaderResourceView** shadowMap, ID3D11Texture2D* depthBuffer)
{
	bool result		= false;
	this->device	= device;
	this->devCon	= devCon;
	this->shadowMapSRV = *shadowMap;
	this->shadowMapTex = depthBuffer;

	this->CreateShader();

	D3D11_TEXTURE2D_DESC texDesc{};
	texDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;

	return result;
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

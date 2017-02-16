#include "Shadowmap.h"

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

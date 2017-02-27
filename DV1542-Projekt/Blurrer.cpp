#include "Blurrer.h"

Blurrer::Blurrer()
{
}

Blurrer::~Blurrer()
{
	this->blurCS->Release();
}

void Blurrer::CreateShader()
{
	/*HRESULT hr;

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

	hr = this->device->CreateComputeShader(pCS->GetBufferPointer, pCS->GetBufferSize, nullptr, &this->blurCS);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating CS.", NULL, MB_OK);
	}
	pCS->Release();*/
}

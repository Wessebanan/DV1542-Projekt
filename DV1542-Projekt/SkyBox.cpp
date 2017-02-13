#include "SkyBox.h"

SkyBox::SkyBox(ID3D11Device* dev, Camera playerCam)
{
	this->rasDesc.CullMode = D3D11_CULL_NONE;
	this->DSdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	ID3D10Blob* pVS;

	HRESULT hr;
	/*hr = D3DCompileFromFile(
		L"VS_SkyBox.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		0,
		0,
		nullptr);
	hr = dev->CreateVertexShader(pVS->GetBufferPointer, pVS->GetBufferSize, nullptr, &this->VS_SkyBox);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating skybox vs.", NULL, MB_OK);
	}*/

	/*hr = D3DCompileFromFile(
		L"PS_SkyBox.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_5_0",
		0,
		0,
		nullptr);*/
}

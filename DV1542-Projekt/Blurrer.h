#pragma once
#include<d3d11.h>
#include<d3dcompiler.h>
class Blurrer
{
private:
	ID3D11ComputeShader* blurCS	  = nullptr;
	ID3D11Device* device		  = nullptr;
	ID3D11DeviceContext* devCon	  = nullptr;
	ID3D11Texture2D* blurredSMTex = nullptr;
	ID3D11ShaderResourceView* blurredSMSRV = nullptr;
	ID3D11ShaderResourceView* shadowMapSRV = nullptr;
	ID3D11SamplerState* clampSampler	= nullptr;
	ID3D11UnorderedAccessView* RWtexUAV = nullptr;
	ID3D11Buffer* dimBuffer = nullptr;

	int height;
	int width;

public:
	Blurrer();
	~Blurrer();

	void Initialize(ID3D11Device* device, 
					ID3D11DeviceContext* devCon, 
					ID3D11ShaderResourceView* shadowMap,					
					int height, int width);

	void CreateShader();
	void CreateDimBuffer();

	void Blur();

	ID3D11ShaderResourceView** GetBlurredSM();
};

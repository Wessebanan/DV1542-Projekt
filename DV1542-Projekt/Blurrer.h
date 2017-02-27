#pragma once
#include<d3d11.h>
#include<d3dcompiler.h>
class Blurrer
{
private:
	ID3D11ComputeShader* blurCS			= nullptr;
	ID3D11Device* device				= nullptr;
	ID3D11DeviceContext* devCon			= nullptr;
	ID3D11Texture2D* blurredSMTex		= nullptr;
	ID3D11Texture2D* shadowMapTex		= nullptr;
	ID3D11ShaderResourceView* blurredSMsrv = nullptr;
	ID3D11ShaderResourceView* shadowMapSRV = nullptr;
public:
	Blurrer();
	~Blurrer();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* devCon, ID3D11ShaderResourceView** shadowMap, ID3D11Texture2D* depthBuffer);
	void CreateShader();

	void Blur(ID3D11ShaderResourceView** srv);
};

#pragma once
#include<d3d11.h>
#include<d3dcompiler.h>
class Blurrer
{
private:
	ID3D11ComputeShader* blurCS = nullptr;
	ID3D11Device* device		= nullptr;
	ID3D11DeviceContext* devCon = nullptr;

public:
	Blurrer();
	~Blurrer();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* devCon);
	void CreateShader();

	void Blur(ID3D11ShaderResourceView** srv);
};

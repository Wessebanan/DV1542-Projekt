#ifndef NOISEGENERATOR_H
#define NOISEGENERATOR_H
#define DEFERRED_H
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <string>

using namespace DirectX;

class NoiseGenerator {
private:
	int width;
	int height;
	ID3D11Device* gDevice;
	D3D11_TEXTURE2D_DESC noiseTextureDesc;
	ID3D11Texture2D* noiseTexture;




public:
	NoiseGenerator(ID3D11Device* device, int width, int height);
	~NoiseGenerator();
	void generateNoise();
	float* loadHeightmap(std::wstring fileName, int heightmapWidth, int heightmapHeight);
	ID3D11Texture2D* getTexture();
	D3D11_TEXTURE2D_DESC getTextureDesc();



};




#endif
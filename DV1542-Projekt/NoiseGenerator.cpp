#include "NoiseGenerator.h"
#include <time.h> 
#include <vector>
#include <fstream>
NoiseGenerator::NoiseGenerator(ID3D11Device* device, int width, int height){
	this->width = width;
	this->height = height;
	this->noiseTextureDesc.Width = this->width;
	this->noiseTextureDesc.Height = this->height;
	this->noiseTextureDesc.MipLevels = 1;
	this->noiseTextureDesc.ArraySize = 1;
	this->noiseTextureDesc.Format = DXGI_FORMAT_R32_FLOAT;
	this->noiseTextureDesc.SampleDesc.Count = 1;
	this->noiseTextureDesc.SampleDesc.Quality = 0;
	this->noiseTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	this->noiseTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	this->noiseTextureDesc.CPUAccessFlags = 0;
	this->noiseTextureDesc.MiscFlags = 0;

	this->gDevice = device;

	this->generateNoise();
}

NoiseGenerator::~NoiseGenerator(){
	this->noiseTexture->Release();
}

void NoiseGenerator::generateNoise() {
	//random noise
	srand((unsigned int)time(NULL));
	float* noiseData = new float[this->width * this->height];
	for (int i = 0; i < this->width * this->height; i++) {
			noiseData[i] = ((rand() % 32768) / 32768.0f);
	}
	
	// Check if a texture already exists
	if (this->noiseTexture != nullptr) {
		this->noiseTexture->Release();
	}
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = noiseData;
	data.SysMemPitch = this->width * sizeof(float);
	data.SysMemSlicePitch = this->width * this->height * sizeof(float);
	HRESULT hr = this->gDevice->CreateTexture2D(&this->noiseTextureDesc, &data, &this->noiseTexture);
	if (FAILED(hr)) {
		MessageBox(NULL, L"Something went wrong creating the noise texture", NULL, MB_OK);
	}



	delete[] noiseData;
}

float* NoiseGenerator::loadHeightmap(std::wstring fileName, int heightmapWidth, int heightmapHeight) {
	this->height = heightmapHeight;
	this->width = heightmapWidth;
	this->noiseTextureDesc.Width = this->width;
	this->noiseTextureDesc.Height = this->height;
	std::vector<unsigned char> in(this->height * this->width);

	std::ifstream inFile;
	inFile.open(fileName, std::ios_base::binary);

	if (inFile) {
		inFile.read((char*)&in[0], (std::streamsize)in.size());
		inFile.close();
	}

	float* mapData = new float[this->height * this->width];
	for (int i = 0; i < this->height * this->width; i++) {
		mapData[i] = in[i] / 255.0f;
	}
	// Check if a texture already exists
	if (this->noiseTexture != nullptr) {
		this->noiseTexture->Release();
	}
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = mapData;
	data.SysMemPitch = this->width * sizeof(float);
	data.SysMemSlicePitch = this->width * this->height * sizeof(float);
	HRESULT hr = this->gDevice->CreateTexture2D(&this->noiseTextureDesc, &data, &this->noiseTexture);
	if (FAILED(hr)) {
		MessageBox(NULL, L"Something went wrong creating the noise texture", NULL, MB_OK);
	}
	return mapData;
}



ID3D11Texture2D * NoiseGenerator::getTexture()
{
	return this->noiseTexture;
}

D3D11_TEXTURE2D_DESC NoiseGenerator::getTextureDesc()
{
	return this->noiseTextureDesc;
}

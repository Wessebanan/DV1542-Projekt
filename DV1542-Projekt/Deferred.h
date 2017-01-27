#ifndef DEFERRED_H
#define DEFERRED_H
#include <d3d11.h>
#include <d3dcompiler.h>

const int BUFFER_COUNT = 4;

class Deferred {
private:
	// 0: normals, 1: diffuse, 2: specular, 3: position
	ID3D11Texture2D* textures[BUFFER_COUNT];
	ID3D11RenderTargetView* renderTargetViews[BUFFER_COUNT];
	ID3D11ShaderResourceView* shaderResourceViews[BUFFER_COUNT];
	int texWidth;
	int texHeight;
	D3D11_VIEWPORT viewPort;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;

public:
	Deferred();
	~Deferred();
	ID3D11Texture2D* GetTexture(int index);
	ID3D11RenderTargetView* GetRenderTargetView(int index);
	ID3D11ShaderResourceView* GetShaderResourceView(int index);
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* devcon, int texWidth, int texHeight);
	void BindGeometryPass();
	void SetRenderTargets(ID3D11DeviceContext* devcon);
	void ClearRenderTargets(ID3D11DeviceContext* devcon);

	
};
#endif
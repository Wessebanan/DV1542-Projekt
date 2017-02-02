#ifndef D3D_H
#define D3D_H
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

class D3D
{
private:
	ID3D11Device* device;
	ID3D11DeviceContext* devCon;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* backBufferRTV;
public:
	D3D();
	~D3D();

	ID3D11Device* getDevice()const;
	ID3D11DeviceContext* getDevCon()const;
	IDXGISwapChain* getSwapChain()const;
	ID3D11RenderTargetView** getBackBufferRTV();

	bool Initialize(int width, int height, HWND window);
};

#endif

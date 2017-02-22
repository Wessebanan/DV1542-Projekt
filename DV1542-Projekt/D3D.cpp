#include "D3D.h"

D3D::D3D()
{
	/*this->backBufferRTV = nullptr;
	this->devCon = nullptr;
	this->device = nullptr;
	this->swapChain = nullptr;*/
}

D3D::~D3D()
{
	if (this->swapChain != nullptr)
	{
		this->swapChain->Release();
		this->swapChain = nullptr;
	}
	if (this->backBufferRTV != nullptr)
	{
		this->backBufferRTV->Release();
		this->backBufferRTV = nullptr;
	}
	if (this->devCon)
	{
		this->devCon->ClearState();
	}
	if (this->device != nullptr)
	{
		this->device->Release();
		this->device = nullptr;
	}
	if (this->devCon != nullptr)
	{
		this->devCon->Release();
		this->devCon = nullptr;
	}
}

ID3D11Device * D3D::getDevice() const
{
	return this->device;
}

ID3D11DeviceContext * D3D::getDevCon()
{
	return this->devCon;
}

IDXGISwapChain * D3D::getSwapChain() const
{
	return this->swapChain;
}

ID3D11RenderTargetView ** D3D::getBackBufferRTV()
{
	return &this->backBufferRTV;
}

bool D3D::Initialize(HWND window)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = window;                           // the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.BufferDesc.RefreshRate.Denominator = 120;
	scd.BufferDesc.RefreshRate.Numerator = 1;
	// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&this->swapChain,
		&this->device,
		NULL,
		&this->devCon);

	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Error creating device.", nullptr, MB_OK);
		exit(-1);
	}

	if (SUCCEEDED(hr))
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		this->device->CreateRenderTargetView(pBackBuffer, NULL, &this->backBufferRTV);
		pBackBuffer->Release();
		
	}
	return SUCCEEDED(hr);
}


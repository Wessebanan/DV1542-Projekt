#include <windows.h>    
#include <windowsx.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "InputHandler.h"
#include "NoiseGenerator.h"
#include "Timer.h"
#include "Deferred.h"
#include<string>
#include<fstream>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
using namespace DirectX;


HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);
HRESULT CreateDirect3DContext(HWND wndHandle);

IDXGISwapChain* gSwapChain = nullptr;
ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gDeviceContext = nullptr;
ID3D11RenderTargetView* gBackbufferRTV = nullptr;

ID3D11InputLayout* gVertexLayout = nullptr;
ID3D11VertexShader* gVertexShader = nullptr;
ID3D11PixelShader* gPixelShader = nullptr;
ID3D11GeometryShader* gGeometryShader = nullptr;

ID3D11Buffer* gTriangleBuffer = nullptr;
ID3D11Buffer* gTransformBuffer = nullptr;

ID3D11Buffer* gIndexBuffer=nullptr;

ID3D11DepthStencilView* gDepthStencilView=nullptr;
ID3D11Texture2D* gDepthStencilBuffer=nullptr;


struct matrixData {
	XMMATRIX WorldMatrix;						// NEW
	XMMATRIX ViewMatrix;						// NEW
	XMMATRIX ProjMatrix;					// NEW
};

matrixData WVP;

float rotationAngle = 0.0f;

void ParseObj(std::string filePath)
{

}

void RenderDeferred(Deferred def) 
{
	def.GeometryPass(viewMatrix);
	def.Draw(gTriangleBuffer, gIndexBuffer, 58806);
	def.LightPass();
}

void CreateDepthBuffer()
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = (float)640;
	depthStencilDesc.Height = (float)480;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	if (FAILED(gDevice->CreateTexture2D(&depthStencilDesc, NULL, &gDepthStencilBuffer)))
	{
		MessageBoxA(NULL, "Error creating depth buffer.", NULL, MB_OK);
		exit(-1);
	}
	if (FAILED(gDevice->CreateDepthStencilView(gDepthStencilBuffer, NULL, &gDepthStencilView)))
	{
		MessageBoxA(NULL, "Error creating ds view.", NULL, MB_OK);
		exit(-1);
	}

}

void CreateTriangle()
{
	struct Vertex
	{
		float x, y, z;
		float r, g, b;
	};
	//Vertex vertices[] =
	//{


	//	// Kuuube ish
	//	-0.5f, -0.5f, 0.0f,
	//	1.0f, 0.0f, 0.0f,

	//	-0.5f, 0.5f, 0.0f,
	//	0.0f, 1.0f, 0.0f,

	//	0.5f, -0.5f, 0.0f,
	//	0.0f, 0.0f, 1.0f,

	//	0.5f, 0.5f, 0.0f,
	//	0.5f, 0.0f, 0.5f,

	//	-0.5f, -0.5f, 1.0f,
	//	1.0f, 0.0f, 0.0f,

	//	-0.5f, 0.5f, 1.0f,
	//	0.0f, 1.0f, 0.0f,

	//	0.5f, -0.5f, 1.0f,
	//	0.0f, 0.0f, 1.0f,

	//	0.5f, 0.5f, 1.0f,
	//	0.5f, 0.0f, 0.5f,

	//};
	int rows = 1000;
	int columns = 1000;
	Vertex* vertices = new Vertex[rows*columns];
	unsigned long vertexIncrementer = 0;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			vertices[vertexIncrementer] = { (float)j, -20.0f, (float)i, 0.0f, 1.0f, 0.0f };
			vertexIncrementer++;
		}
	}
	
	D3D11_BUFFER_DESC triangleBufferDesc = {};
	triangleBufferDesc.ByteWidth = sizeof(Vertex) * rows * columns;
	triangleBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	triangleBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA triangleData = {};
	triangleData.pSysMem = vertices;

	if (FAILED(gDevice->CreateBuffer(&triangleBufferDesc, &triangleData, &gTriangleBuffer)))
	{
		MessageBoxA(NULL, "Error creating triangle buffer.", NULL, MB_OK);
		exit(-1);
	}

	//DWORD indices[] = {
	//	0, 1, 2,
	//	2, 3, 1,
	//	2,3,6,
	//	6,7,3,
	//	6,7,4,
	//	4,5,7,
	//	4,5,0,
	//	0,1,5,
	//	1,5,3,
	//	3,7,5,
	//	0,2,4,
	//	4,6,2,

	//	
	//};

	DWORD* indices = new DWORD[6 * (rows-1) * (columns-1)]; // 6 per quad, 81 quads total
	unsigned long indexIncrementer = 0;
	for (int i = 0; i < rows - 1; i++) {
		for (int j = 0; j < columns - 1; j++) {
			unsigned long curr = i * rows + j;
			indices[indexIncrementer] = { curr };
			indices[indexIncrementer + 1] = { curr + columns };
			indices[indexIncrementer + 2] = { curr + 1 };
			indices[indexIncrementer + 3] = { curr + 1 };
			indices[indexIncrementer + 4] = { curr + columns };
			indices[indexIncrementer + 5] = { curr + columns + 1 };
			indexIncrementer = indexIncrementer + 6;

		}
	}
	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * (6 * (rows - 1) * (columns - 1));
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;

	if (FAILED(gDevice->CreateBuffer(&indexBufferDesc, &indexData, &gIndexBuffer)))
	{
		MessageBoxA(NULL, "Error creating index buffer.", NULL, MB_OK);
		exit(-1);
	}
}

void CreateWVP()
{
	WVP.WorldMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	WVP.ViewMatrix = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, -2.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	WVP.ProjMatrix = XMMatrixPerspectiveFovLH(XM_PI*0.45f, 4.0f / 3.0f, 0.1f, 2000.0f);

	D3D11_BUFFER_DESC WVPdesc = {};
	WVPdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	WVPdesc.ByteWidth = sizeof(matrixData);
	WVPdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	WVPdesc.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(gDevice->CreateBuffer(&WVPdesc, nullptr, &gTransformBuffer)))
	{
		MessageBoxA(NULL, "Error creating transform buffer.", NULL, MB_OK);
		exit(-1);
	}

}

void CreateShaders()
{
	//create vertex shader
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(
		L"VertexShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"main",		// entry point
		"vs_5_0",		// shader model (target)
		0,				// shader compile options			// here DEBUGGING OPTIONS
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShader);

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	}; 

	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayout);
	// we do not need anymore this COM object, so we release it.
	pVS->Release();

	//create pixel shader
	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"PixelShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"main",		// entry point
		"ps_5_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pPS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShader);
	// we do not need anymore this COM object, so we release it.
	pPS->Release();

	//create geomety shader (same way as the other shaders).
	ID3DBlob* pGS = nullptr;
	D3DCompileFromFile(
		L"GeometryShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"gs_5_0",
		0,
		0,
		&pGS,
		nullptr
		);

	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &gGeometryShader);
	pGS->Release();
}

void SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = (float)640;
	vp.Height = (float)480;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	gDeviceContext->RSSetViewports(1, &vp);
	
}

void setHeightMapTexture() {
	NoiseGenerator noise1(gDevice, 255, 255);
	noise1.loadHeightmap(L"TestMap2.RAW", 255, 255);
	
	
	ID3D11ShaderResourceView* gTextureView = nullptr;
	ID3D11SamplerState* gSamplerState = nullptr;

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
	ZeroMemory(&resourceViewDesc, sizeof(resourceViewDesc));

	resourceViewDesc.Format = noise1.getTextureDesc().Format;
	resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceViewDesc.Texture2D.MipLevels = noise1.getTextureDesc().MipLevels;
	resourceViewDesc.Texture2D.MostDetailedMip = 0;
	HRESULT hr = gDevice->CreateShaderResourceView(noise1.getTexture(), &resourceViewDesc, &gTextureView);
	if (!SUCCEEDED(hr)) {
		MessageBox(NULL, L"Something went wrong trying to create the Shader Resource View.", NULL, MB_ICONEXCLAMATION);
	}

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = gDevice->CreateSamplerState(&samplerDesc, &gSamplerState);
	if (!SUCCEEDED(hr)) {
		MessageBox(NULL, L"Something went wrong trying to create the Sampler State", NULL, MB_ICONEXCLAMATION);
	}

	gDeviceContext->VSSetShaderResources(0, 1, &gTextureView);
	gDeviceContext->VSSetSamplers(0, 1, &gSamplerState);
}

void Render()
{
	// clear the back buffer to a deep blue
	float clearColor[] = { 0.0f, 0.749f, 1.0f, 1 };
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);



	//Set the sampler state and shader resource view to the fragment shader.
	//gDeviceContext->PSSetSamplers(0, 1, &gSamplerState);
	//gDeviceContext->PSSetShaderResources(0, 1, &gShaderResourceView);

	UINT32 vertexSize = sizeof(float) * 6;
	UINT32 offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &gTriangleBuffer, &vertexSize, &offset);
	gDeviceContext->IASetIndexBuffer(gIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Map constant buffer so that we can write to it.
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	gDeviceContext->Map(gTransformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);

	// -------------------------------------------------------------------------------------
	// Testing input stuff here
	// -------------------------------------------------------------------------------------

	WVP.ViewMatrix = viewMatrix; // viewMatrix is created in UpdateCamera();, which is called in the inputHandler;

	// -------------------------------------------------------------------------------------
	// Testing input stuff here /end
	// -------------------------------------------------------------------------------------
	memcpy(dataPtr.pData, &WVP, sizeof(matrixData));

	// UnMap constant buffer so that we can use it again in the GPU
	gDeviceContext->Unmap(gTransformBuffer, 0);
	//set resource to Geometry Shader
	gDeviceContext->GSSetConstantBuffers(0, 1, &gTransformBuffer);

	//Set the light position buffer to the fragment shader.
	//gDeviceContext->PSSetConstantBuffers(1, 1, &gLightBuffer);

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(gVertexLayout);

	gDeviceContext->DrawIndexed(6 * 999 * 999, 0, 0);
}


int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	// the handle for the window, filled by a function
	HWND hWnd;
	// this struct holds information for the window class
	WNDCLASSEX wc;

	// clear out the window class for use
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"WindowClass1";

	// register the window class
	RegisterClassEx(&wc);

	// AdjustWindowRect uses the desired client size (RECT wr) to specify the required window size


	RECT wr = { 0, 0, 640, 480 };    // set the size, but not the position
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size


	// create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL,
		L"WindowClass1",    // name of the window class
		L"Window",   // title of the window
		WS_OVERLAPPEDWINDOW,    // window style
		300,    // x-position of the window
		300,    // y-position of the window
		wr.right - wr.left,    // width of the window
		wr.bottom - wr.top,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		hInstance,    // application handle
		NULL);    // used with multiple windows, NULL

				  // display the window on the screen
	MSG msg;

	if (!InitDirectInput(hInstance, hWnd)) {
		MessageBox(0, L"Failed to initialize Direct Input.", L"Error", MB_OK);
	}


	if (hWnd) {
		

		CreateDirect3DContext(hWnd);

		SetViewport();

		CreateWVP();

		setHeightMapTexture();

		CreateShaders();

		CreateTriangle();

		StartTimer(); // Starts global timer

		ShowWindow(hWnd, nCmdShow);

		Deferred def(hInstance);

		// enter the main loop:

		// this struct holds Windows event messages


		while (TRUE)
		{
			// Check to see if any messages are waiting in the queue
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				// translate keystroke messages into the right format
				TranslateMessage(&msg);

				// send the message to the WindowProc function
				DispatchMessage(&msg);

				// check to see if it's time to quit
				if (msg.message == WM_QUIT||destroyWindow==true)
					break;
			}
			else {
				DetectInput(GetFrameTime());
				
				RenderDeferred(def); //<-- Funkar inte pga problem med någonting i lightpass.

				/*Render();
				gSwapChain->Present(1, 0);*/
				// WEEEEW GAME CODE HERE LET'S GO
			}
		}




		gTransformBuffer->Release();
		gTriangleBuffer->Release();
		gBackbufferRTV->Release();
		gVertexLayout->Release();
		gVertexShader->Release();
		gPixelShader->Release();
		gGeometryShader->Release();
		gSwapChain->Release();
		gDevice->Release();
		gDeviceContext->Release();
		DIKeyboard->Unacquire();
		DIMouse->Unacquire();
		DirectInput->Release();
		DestroyWindow(hWnd);
		gIndexBuffer->Release();
		gDepthStencilBuffer->Release();
		gDepthStencilView->Release();
	}

	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW; //Redraws the entire window if size changes.
	wcex.lpfnWndProc = WindowProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"BTH_D3D_DEMO";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"BTH_D3D_DEMO",
		L"BTH Direct3D Demo",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	return handle;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
		// this message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely
		PostQuitMessage(0);
		return 0;
	} break;
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}

HRESULT CreateDirect3DContext(HWND wndHandle)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = wndHandle;                           // the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.BufferDesc.RefreshRate.Denominator = 120;
	scd.BufferDesc.RefreshRate.Numerator = 1;
	// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext);

	if (SUCCEEDED(hr))
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		CreateDepthBuffer();
		// use the back buffer address to create the render target
		gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackbufferRTV);
		pBackBuffer->Release();

		// set the render target as the back buffer
		gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, gDepthStencilView);
	}
	return hr;
}
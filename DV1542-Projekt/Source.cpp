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

ID3D11Buffer* gIndexBuffer = nullptr;

ID3D11DepthStencilView* gDepthStencilView = nullptr;
ID3D11Texture2D* gDepthStencilBuffer = nullptr;


ID3D11Texture2D* grassTexture = nullptr;
ID3D11Texture2D* waterTexture = nullptr;
ID3D11Texture2D* dirtTexture = nullptr;

struct matrixData {
	XMMATRIX WorldMatrix;					
	XMMATRIX ViewMatrix;					
	XMMATRIX ProjMatrix;					
};

matrixData WVP;

float rotationAngle = 0.0f;

void RenderDeferred(Deferred* def) 
{
	def->GeometryPass(viewMatrix);
	def->Draw(gTriangleBuffer, gIndexBuffer, 6 * 999 * 999);
	def->LightPass();
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

void CreateTriangle(Deferred* def)
{
	struct Vertex
	{
		float x, y, z;
		float r, g, b;
	};

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


	if (FAILED(def->CreateBuffer(&triangleBufferDesc, &triangleData, &gTriangleBuffer)))
	{
		MessageBoxA(NULL, "crapped the fuck out.", NULL, MB_OK);
		exit(-1);
	}


	DWORD* indices = new DWORD[6 * (rows-1) * (columns-1)]; // 6 per quad
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

	delete[] vertices;
	
	//NEDAN ÄNDRAT TILL DEF ISTÄLLET FÖR gDevice FÖR ATT TESTA.
	if (FAILED(def->CreateBuffer(&indexBufferDesc, &indexData, &gIndexBuffer)))
	{
		MessageBoxA(NULL, "Error creating index buffer.", NULL, MB_OK);
		exit(-1);
	}
	delete[] indices;
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	Deferred def(hInstance);
	MSG msg;

	if (!InitDirectInput(hInstance, def.GetWindowHandle())) {
		MessageBox(0, L"Failed to initialize Direct Input.", L"Error", MB_OK);
	}


	if (def.GetWindowHandle()) {

		def.setHeightMapTexture(L"TestMap1.RAW", 1024, 1024);

		StartTimer(); // Starts global timer

		ShowWindow(def.GetWindowHandle(), nCmdShow);

		CreateTriangle(&def);

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
				
				RenderDeferred(&def); //<-- Funkar!


				// WEEEEW GAME CODE HERE LET'S GO
			}
		}

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
		DestroyWindow(def.GetWindowHandle());
		gIndexBuffer->Release();
		gDepthStencilBuffer->Release();
		gDepthStencilView->Release();
	}

	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}
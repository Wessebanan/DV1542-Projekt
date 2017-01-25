#include <windows.h>    
#include <windowsx.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
using namespace DirectX;

class Deferred {
public:
	ID3D11Texture2D* RTnormal;
	ID3D11Texture2D* RTposition;
	ID3D11Texture2D* RTspecular;
	ID3D11Texture2D* RTdiffuse;

private:
	void initialize(ID3D11Device device, );

};
#include "Window.h"

Window::Window(HINSTANCE instance) :
	hInstance(instance)
{
	this->window = nullptr;
	this->hInstance = GetModuleHandle(0);
	this->title = L"Imagination.";
	this->width = 640;
	this->height = 480;
}

Window::~Window()
{

}

LRESULT	CALLBACK Window::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (msg)
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
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool Window::Initialize()
{
	bool result = true;
	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW; //Redraws the entire window if size changes.
	wcex.lpfnWndProc = WindowProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"BTH_D3D_DEMO";
	if (!RegisterClassEx(&wcex))
		result = false;

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

	return result;
}

HWND Window::GetWindow() const
{
	return this->window;
}

int Window::GetHeight() const
{
	return this->height;
}

int Window::GetWidth() const
{
	return this->width;
}

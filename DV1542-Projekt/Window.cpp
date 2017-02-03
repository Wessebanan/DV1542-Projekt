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
	//WNDCLASSEX wcex{};
	//wcex.cbSize = sizeof(WNDCLASSEX);
	//wcex.style = CS_HREDRAW | CS_VREDRAW; //Redraws the entire window if size changes.
	//wcex.lpfnWndProc = WindowProc;
	//wcex.hInstance = hInstance;
	//wcex.lpszClassName = L"BTH_D3D_DEMO";
	//if (!RegisterClassEx(&wcex))
	//	result = false;

	//RECT rc = { 0, 0, 640, 480 };
	//AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	//this->window = CreateWindow(
	//	L"BTH_D3D_DEMO",
	//	L"BTH Direct3D Demo",
	//	WS_OVERLAPPEDWINDOW,
	//	CW_USEDEFAULT,
	//	CW_USEDEFAULT,
	//	rc.right - rc.left,
	//	rc.bottom - rc.top,
	//	nullptr,
	//	nullptr,
	//	this->hInstance,
	//	nullptr);

	WNDCLASSEX wc;

	// clear out the window class for use
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = this->WindowProc;
	wc.hInstance = this->hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"WindowClass1";

	// register the window class
	RegisterClassEx(&wc);

	// AdjustWindowRect uses the desired client size (RECT wr) to specify the required window size


	RECT wr = { 0, 0, 640, 480 };    // set the size, but not the position
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size


														  // create the window and use the result as the handle
	this->window = CreateWindowEx(NULL,
		L"WindowClass1",    // name of the window class
		L"Window",   // title of the window
		WS_OVERLAPPEDWINDOW,    // window style
		300,    // x-position of the window
		300,    // y-position of the window
		wr.right - wr.left,    // width of the window
		wr.bottom - wr.top,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		this->hInstance,    // application handle
		NULL);    // used with multiple windows, NULL


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

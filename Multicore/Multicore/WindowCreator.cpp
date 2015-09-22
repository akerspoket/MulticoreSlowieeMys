#include "WindowCreator.h"

#include <iostream>

WindowCreator::WindowCreator()
{
}


WindowCreator::~WindowCreator()
{
}

LRESULT CALLBACK WindowCreator::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
		// this message is read when the window is closed
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				return 0;
			}
		}break;
		
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

int WINAPI WindowCreator::WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"Find the Brick";
	
	RegisterClassEx(&wc);

	RECT wr = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindowEx(NULL,
		L"Find the Brick",    // name of the window class
		L"Our First Windowed Program",   // title of the window
		WS_OVERLAPPEDWINDOW,    // window style
		300,    // x-position of the window
		300,    // y-position of the window
		wr.right - wr.left,    // width of the window
		wr.bottom - wr.top,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		hInstance,    // application handle
		NULL);    // used with multiple windows, NULL

	ShowWindow(hWnd, nCmdShow);
	//ShowWindow(GetConsoleWindow(), SW_HIDE);
	MSG msg = {0};
	mGraphicsEngine = new GraphicsEngine();
	mGraphicsEngine->InitD3D(hWnd);
	mGraphicsEngine->InitPipeline();
	mGraphicsEngine->InitGraphics();
	InitDirectInput(wc.hInstance);
	while (true)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				break;
			}
		}
		else
		{
			//GAME CODE
			UpdateUserCMD();
			mGraphicsEngine->Update(1.0f, mCurrentCmd);
			mGraphicsEngine->RenderFrame();
		}
	
	}
	ShowWindow(GetConsoleWindow(), SW_SHOW);
	mGraphicsEngine->CleanD3D();
	return msg.wParam;
}

void WindowCreator::UpdateUserCMD()
{
	DIMOUSESTATE mouseCurrState;
	mCurrentCmd.a = 0;
	mCurrentCmd.d = 0;
	mCurrentCmd.w = 0;
	mCurrentCmd.s = 0;
	mCurrentCmd.mouseX = 0;
	mCurrentCmd.mouseY = 0;
	BYTE keyboardState[256];

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if (keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(GetActiveWindow(), WM_DESTROY, 0, 0);


	if (keyboardState[DIK_A] & 0x80)
	{
		mCurrentCmd.a = true;
		cout << "a detected";
	}
	if (keyboardState[DIK_D] & 0x80)
	{
		mCurrentCmd.d = true;
		cout << "d detected";
	}
	if (keyboardState[DIK_W] & 0x80)
	{
		mCurrentCmd.w = true;
		cout << "w detected";
	}
	if (keyboardState[DIK_S] & 0x80)
	{
		mCurrentCmd.s = true;
		cout << "s detected";
	}
	if ((mouseCurrState.lX != MouseLastState.lX) || (mouseCurrState.lY != MouseLastState.lY))
	{
		mCurrentCmd.mouseX += mouseCurrState.lX;

		mCurrentCmd.mouseY += mouseCurrState.lY * 0.001f;

		MouseLastState = mouseCurrState;
		cout << mCurrentCmd.mouseX;
		cout << mCurrentCmd.mouseY;
	}

	return;
}

bool WindowCreator::InitDirectInput(HINSTANCE hInstance)
{
	HRESULT hr;
	hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&DirectInput,
		NULL);

	hr = DirectInput->CreateDevice(GUID_SysKeyboard,
		&DIKeyboard,
		NULL);

	hr = DirectInput->CreateDevice(GUID_SysMouse,
		&DIMouse,
		NULL);

	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = DIKeyboard->SetCooperativeLevel(GetActiveWindow(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	hr = DIMouse->SetCooperativeLevel(GetActiveWindow(), DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}
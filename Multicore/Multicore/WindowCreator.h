#pragma once
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>
#include <Windows.h>
#include <windowsx.h>
#include "GraphicsEngine.h"
#include "Globals.h"
class WindowCreator
{
public:
	WindowCreator();
	~WindowCreator();
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	GraphicsEngine *mGraphicsEngine;
private:
	UserCMD mCurrentCmd;
	void UpdateUserCMD();
	bool InitDirectInput(HINSTANCE hInstance);
	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE MouseLastState;
	LPDIRECTINPUT8 DirectInput;
};
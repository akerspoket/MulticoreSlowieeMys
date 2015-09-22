#include <iostream>
#include <SDL2\SDL.h>
#include <windows.h>
#include <windowsx.h>
#include "GraphicsEngine.h"
#include "WindowCreator.h"
#include "InputSystem.h"
#include "Globals.h"

int main(int argc, char* args[])
{
	std::cout << "Starting project - Multicore";
	//Create window pointer
	SDL_Window* window = NULL;
	WindowCreator* windowCreator = new WindowCreator();
	windowCreator->WinMain(0, 0, 0,1);
	//&Initialize SDL
	//if (SDL_Init(SDL_INIT_VIDEO) < 0)
	//{
	//	printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	//}
	//else
	//{
	//	GetActiveWindow();
	//	//Create window
	//	window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	//	if (window == NULL)
	//	{
	//		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
	//	}
	//	else
	//	{
	//		GraphicsEngine* mGraphicsEngine = new GraphicsEngine();
	//		mGraphicsEngine->InitD3D(GetActiveWindow());
	//		mGraphicsEngine->InitPipeline();
	//		mGraphicsEngine->InitGraphics();

	//		InputSystem* mInput = new InputSystem();
	//		SDL_Event tevent;
	//		while (true)
	//		{
	//			while (SDL_PollEvent(&tevent))
	//			{
	//				switch (tevent.type)
	//				{
	//					// do nothing... yet
	//				}
	//			}

	//			mInput->Update();
	//			mGraphicsEngine->RenderFrame();
	//		}

	//	}
	//}
	return 0;
}
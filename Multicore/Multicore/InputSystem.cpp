#include "InputSystem.h"


InputSystem::InputSystem()
{
}


InputSystem::~InputSystem()
{
}

void InputSystem::CheckKeyboard()
{
	mKeyState = SDL_GetKeyboardState(NULL);
	if (mKeyState[SDL_SCANCODE_LEFT])
	{
		mUserCmd.mLeftArrowPressed = true;
	}
	else
	{
		mUserCmd.mLeftArrowPressed = false;
	}
	if (mKeyState[SDL_SCANCODE_RIGHT])
	{
		mUserCmd.mRightArrowPressed = true;
	}
	else
	{
		mUserCmd.mRightArrowPressed = false;
	}
	if (mKeyState[SDL_SCANCODE_A])
	{
		mUserCmd.mKeysPressed.push_back('a');
	}
	if (mKeyState[SDL_SCANCODE_D])
	{
		mUserCmd.mKeysPressed.push_back('d');
	}
	if (mKeyState[SDL_SCANCODE_ESCAPE])
	{
		exit(0);
	}
}
void InputSystem::HandleInput()
{
}

void InputSystem::Update()
{

	CheckKeyboard();
	HandleInput();
}

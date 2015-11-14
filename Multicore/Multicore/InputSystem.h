#pragma once
#include <vector>

class InputSystem
{
public:
	InputSystem();
	~InputSystem();
	void Update();
	void CheckKeyboard();
	void HandleInput();

	static struct UserCmd
	{
		std::vector<char>mKeysPressed;
		float mXMove;//for the mouse
		float mYMove;//for the mouse
		bool mRightArrowPressed;
		bool mLeftArrowPressed;
		bool mUpArrowPressed;
		bool mDownArrowPressed;
	};

	UserCmd mUserCmd;
	
};


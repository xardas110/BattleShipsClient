#pragma once
class KeyEvent
{
public:
	enum class KeyState
	{
		Released, Pressed, Repeated
	};
	int key, scancode, mods;
	KeyState state;
	KeyEvent(int key, int scancode, KeyState state, int mods)
		:key(key), scancode(scancode), state(state), mods(mods)
	{};
};

class UpdateEvent
{
public: 
	UpdateEvent(double deltatime, double totaltime, uint64_t framenr)
	: deltaTime(deltatime), totalTime(totaltime), frameNr(framenr)
{}
	double deltaTime;
	double totalTime;
	uint64_t frameNr;
};

class MouseMoveEvent
{
public:
	MouseMoveEvent(double xpos, double ypos)
		:xPos(xpos), yPos(ypos)
	{}
	double xPos, yPos;
};

class MouseClickEvent
{
public:
	enum class ButtonState : int
	{
		Released,
		Pressed
	};
	enum class Button : int
	{
		Left,
		Right,
		Middle
	};
	MouseClickEvent(int button, int action, int mods)
		:button((Button)button), state((ButtonState)action), mods(mods)
	{}
	Button button;
	ButtonState state;	
	int mods;
};
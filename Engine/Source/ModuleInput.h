#pragma once
#include "Module.h"

#include "SDL_gamecontroller.h"
#include "SDL_joystick.h"
#include "SDL_scancode.h"
#include "SDL_mouse.h"

#include <vector>
#include <array>
#include <string>

#define MAX_MOUSE_BUTTONS 5
#define MAX_KEYS 300

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

//Adapted from SDL_GameControllerAxis
enum class JAxis
{
	LeftStickHorizontal,
	LeftStickVertical,
	RightStickHorizontal,
	RightStickVertical,
	LeftTrigger,
	RightTrigger,
	Count
};


//Adapted from SDL_GameControllerButton
enum class Button
{
	A,
	B,
	X,
	Y,
	Back,
	Guide,
	Start,
	LS,
	RS,
	LB,
	RB,
	DPAD_Up,
	DPAD_Down,
	DPAD_Left,
	DPAD_Right,
	Count
};

class ModuleInput : public Module
{
public:
	
	ModuleInput(bool startEnabled = true);
	~ModuleInput();

	bool Init(JsonParsing& node) override;
	bool PreUpdate(float dt) override;
	bool CleanUp();

	bool IsJoystickAvailable(int joystickId);
	bool ControllerUpdate();

	bool LoadConfig(JsonParsing& node) override;
	bool SaveConfig(JsonParsing& node) override;

	void AddController(int id);
	void RemoveController(int id);

	void FillScancodeNameList();

	inline KeyState GetKey(int id) const { return keyboard[id]; }
	inline KeyState GetMouseButton(int id) const { return mouseButtons[id]; }
	inline int GetMouseX() const { return mouseX; }
	inline int GetMouseY() const { return mouseY; }
	inline int GetMouseZ() const { return mouseZ; }
	inline int GetMouseXMotion() const { return mouseXMotion; }
	inline int GetMouseYMotion() const { return mouseYMotion; }

	bool GetButton(int joystickId, Button button);
	bool GetButtonDown(int joystickId, Button button);
	bool GetButtonUp(int joystickId, Button button);
	bool GetAxis(int joystickId, JAxis axis);

	inline std::vector<std::string> GetInputList() const { return strings; }

	const char* keyNameList[MAX_KEYS];

private:
	KeyState* keyboard;
	KeyState mouseButtons[MAX_MOUSE_BUTTONS];
	int mouseX;
	int mouseY;
	int mouseZ;
	float mouseXMotion;
	float mouseYMotion;
	int mouseZMotion;
	
	SDL_GameController* pad;
	SDL_Joystick* joy;
	int joyID;
	std::array<bool, (int)Button::Count> buttons;
	std::array<bool, (int)Button::Count> lastButtons;
	std::array<float, (int)JAxis::Count> axes;
	std::array<float, (int)JAxis::Count> lastAxis;
	float deadzone;

	std::vector<std::string> strings;

	bool repeated = true;
};
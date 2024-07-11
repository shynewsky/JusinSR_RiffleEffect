#pragma once

#include "Base.h"



BEGIN(Engine)

enum class KeyType
{
	LeftMouse = VK_LBUTTON,
	RightMouse = VK_RBUTTON,
	HMouse = VK_MBUTTON,

	Up = VK_UP,
	Down = VK_DOWN,
	Left = VK_LEFT,
	Right = VK_RIGHT,
	SpaceBar = VK_SPACE,

	Enter = VK_RETURN,
	F1 = VK_F1,
	F2 = VK_F2,
	F5 = VK_F5,
	F6 = VK_F6,

	LALT = VK_LMENU,

	KEY_1 = '1',
	KEY_2 = '2',
	KEY_3 = '3',

	W = 'W',
	A = 'A',
	S = 'S',
	D = 'D',

	T = 'T',
	F = 'F',

	Q = 'Q',
	E = 'E',

	Z = 'Z',
	X = 'X',
	C = 'C',
	V = 'V',
	B = 'B',
};

enum class KeyState
{
	None,
	Press,
	Down,
	Up,

	End
};

enum
{
	KEY_TYPE_COUNT = static_cast<int>(UINT8_MAX) + 1,
	KEY_STATECOUNT = static_cast<int>(KeyState::End)
};
class KeyMgr final : public CBase
{
private:
	KeyMgr();
	virtual ~KeyMgr() = default;

public:
	HRESULT Initailize();
	void Update();

	//누르고 있을 때
	bool GetButton(KeyType key) { return GetState(key) == KeyState::Press; }
	//맨 처음 눌렀을 때
	bool GetButtonDown(KeyType key) { return GetState(key) == KeyState::Down; }
	//맨 처음 눌렀다가 땔 때
	bool GetButtonUp(KeyType key) { return GetState(key) == KeyState::Up; }

	POINT GetMousePos() { return _mousePos; }
private:
	KeyState GetState(KeyType Key) { return _states[static_cast<unsigned int>(Key)]; }

private:
	HWND _hwnd = 0;
	vector<KeyState> _states;
	POINT _mousePos;

public:
	static KeyMgr* Create();
	virtual void Free() override;
};


END
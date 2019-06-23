#pragma once

class DummyClientFramework
{
public:
	DummyClientFramework();
	~DummyClientFramework();

	void Create(HWND hWnd);
	void Draw(HDC hdc);
	void Update();
	void InputKey(UINT iMessage, WPARAM wParam, LPARAM lParam);

private:
	HWND hWnd;
};

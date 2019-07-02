#pragma once

class NetworkManager;

class DummyClientFramework
{
public:
	DummyClientFramework(/*NetworkManager**/);
	~DummyClientFramework();

	void Create(HWND hWnd);
	void Draw(HDC hdc);
	void Update();
	void InputKey(UINT iMessage, WPARAM wParam, LPARAM lParam);

private:
	void TestIntegrity();

private:
	HWND hWnd;
	std::unique_ptr<NetworkManager> networkManager;
};

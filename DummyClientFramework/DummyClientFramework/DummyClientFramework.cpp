#include "stdafx.h"

#include "DummyClientFramework.h"

#include "Define.h"

DummyClientFramework::DummyClientFramework()
{

}

DummyClientFramework::~DummyClientFramework()
{

}

void DummyClientFramework::Create(HWND hWnd)
{
	this->hWnd = hWnd;
}

void DummyClientFramework::Draw(HDC hdc)
{

}

void DummyClientFramework::Update()
{

}

void DummyClientFramework::InputKey(UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	using namespace KEY;

	switch (iMessage)
	{
	case WM_KEYDOWN:
	{
		if (wParam == VK_Q)
		{
			SendMessage(hWnd, WM_DESTROY, 0, 0);
			return;
		}
	}
	break;

	case WM_KEYUP:
	{

	}
	break;
	}
}
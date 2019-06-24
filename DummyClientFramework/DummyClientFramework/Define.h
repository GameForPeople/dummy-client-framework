#pragma once

// FIXED
namespace KEY
{
	enum {
		VK_0 = 48,
		VK_1 = 49,
		VK_2,
		VK_3,
		VK_4,
		VK_5,
		VK_6,
		VK_7,
		VK_8,
		VK_9,
		VK_A = 0x41,
		VK_B,
		VK_C,
		VK_D,
		VK_E,
		VK_F,
		VK_G,
		VK_H,
		VK_I,
		VK_J,
		VK_K,
		VK_L,
		VK_M,
		VK_N,
		VK_O,
		VK_P,
		VK_Q,
		VK_R,
		VK_S,
		VK_T,
		VK_U,
		VK_V,
		VK_W,
		VK_X,
		VK_Y,
		VK_Z
	};
}

// FIXED
namespace ERROR_UTIL
{
	_NORETURN void ERROR_QUIT(const WCHAR *msg);
	void ERROR_DISPLAY(const WCHAR *msg);
	void HandleRecvError();
	void HandleSendError();
}
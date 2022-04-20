#pragma once
#include "framework.h"


class timeCounter
{
public:
	long milliseconds;
	HWND hWnd;

	void updateTime(long ms);
	
	void init();
};

extern timeCounter tc;
#include "CNeoHud.h"

int* CNeoHud::getInstance()
{
	return ((int* (__stdcall*)())0x55A280)();
}

int* CNeoHud::setBoxStatus(int** self, int a, int b)
{
	return ((int* (__thiscall*)(int**, int, int))0x55A230)(self, a, b);
}

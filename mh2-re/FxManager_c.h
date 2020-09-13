#pragma once

#include "Vector.h"
#include "Memory.h"
#include "FxSystem_c.h"

class FxManager_c
{
public:
	static FxSystem_c* CreateFxSystem(int* table, char* fxName, int mem, int unk, int unk2);
	static FxSystem_c* CreateFxSystem(int* table, int* bp, float* cMatrix, int unk, int unk2);
	static int* FindFxSystemBP(char * texture);
};


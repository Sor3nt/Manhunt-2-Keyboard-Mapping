#include "FxManager_c.h"

FxSystem_c* FxManager_c::CreateFxSystem(int* table, char* fxName, int mem, int unk, int unk2)
{

	return (*(FxSystem_c * (__thiscall*)(int*, char*, int, int, int))(0x57B360))(table, fxName, mem, unk, unk2);
}
/*
FxSystem_c* FxManager_c::CreateFxSystem(int* table, int* bp, float* cMatrix, int unk, int unk2)
{

	return (*(FxSystem_c * (__thiscall*)(int*, int*, float*, int, int))(0x57A980))(table, bp, cMatrix, unk, unk2);
}
*/

int* FxManager_c::FindFxSystemBP(char* texture)
{
	return (*(int * (__cdecl*)(char*))(0x57A3E0))(texture);

}

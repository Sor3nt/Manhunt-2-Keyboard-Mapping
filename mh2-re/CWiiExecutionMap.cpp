#include "CWiiExecutionMap.h"


int* CWiiExecutionMap::getSingleton()
{
	return *(int**)ms_Singleton;

}

char* CWiiExecutionMap::DisplayIcons(int self, int executionId, int executionLevel)
{
	return ((char* (__thiscall*)(int, int, int))0x5B5D60)(self, executionId, executionLevel);
}

int* CWiiExecutionMap::GetDecal(int self, int weaponId, int level, int number)
{
	return ((int* (__thiscall*)(int, int, int, int))0x5B5B80)(self, weaponId, level, number);
}

int* CWiiExecutionMap::GetAction(int self, int executionId, int executionLevel, int b)
{
	return ((int* (__thiscall*)(int, int, int, int))0x5B5B60)(self, executionId, executionLevel, b);
}

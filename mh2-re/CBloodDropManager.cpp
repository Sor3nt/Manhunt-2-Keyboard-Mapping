#include "CBloodDropManager.h"

int gBloodDropManager = 0x77D788;

int CBloodDropManager::BloodFire(Vector4* vec, float flo1, int int1)
{
	return (*(int(__thiscall*)(int, Vector4*, float, int))0x5B78B0)(gBloodDropManager, vec, flo1, int1);

}

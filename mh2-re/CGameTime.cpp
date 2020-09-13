#include "CGameTime.h"

unsigned int CGameTime::GetRealTimeInMillisecs()
{
	return ((unsigned int(__stdcall*)())0x4B1840)();
}

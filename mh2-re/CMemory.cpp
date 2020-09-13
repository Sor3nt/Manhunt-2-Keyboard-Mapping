#include "CMemory.h"


int CMemory::Allocate(int size)
{
	return (*(int(__cdecl*)(int))(0x580ED0))(size);

}

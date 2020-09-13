#include "CClumpDict.h"

int CClumpDict::Initialise(int* self, char* txd, char* mdl)
{
	return ((int (__thiscall*)(int*, char*, char*))0x5BC3D0)(self, txd, mdl);

}

int CClumpDict::CheckDict(int* self)
{
	return ((int(__thiscall*)(int*))0x5BC270)(self);
}

#include "CEntityManager.h"

int CEntityManager::LoadCharacterClumpDict(int index)
{
	return ((int(__cdecl*)(int))0x4E8D30)(index);

}

int CEntityManager::GetEntityTypeDataFromName(char* name)
{
//	CallAndReturn<0x53D8D0, int, const char*, Args...>(line, msg, args...);

	//return CTypeData
	return ((int(__cdecl*)(char*))0x4E9130)(name);
}

CEntity* CEntityManager::FindInstance(const char* name)
{
	return ((CEntity*(__cdecl*)(const char*))0x4EC530)(name);
	
}

int CEntityManager::MakeLocalClumpDictCurrent(char* name) {
	return ((int (__cdecl*)(char*))0x5CA6A0)(name);

}

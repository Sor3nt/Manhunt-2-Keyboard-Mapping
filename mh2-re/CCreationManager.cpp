#include "CCreationManager.h"

CEntity* CCreationManager::CreateEntity(int* cTypeData)
{
	return ((CEntity*(__cdecl*)(int*))0x4E12A0)(cTypeData);
}

CEntity* CCreationManager::SpawnNamedEntityFromArchetype(char const* newName, char const* entityName, Vector3 pos)
{
	//call to sub_4E35F0
	return ((CEntity * (__cdecl*)(char const*, char const*, Vector3*))0x4E1920)(newName, entityName, &pos);
}

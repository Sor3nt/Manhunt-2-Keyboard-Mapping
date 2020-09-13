#pragma once
#include "Vector.h"
#include "CEntity.h"

class CCreationManager
{
	public:
		static CEntity* CreateEntity(int* cTypeData);
		static CEntity* SpawnNamedEntityFromArchetype(char const* newName, char const* entityName, Vector3 pos);

};


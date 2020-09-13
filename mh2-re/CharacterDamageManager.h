#pragma once
#include "CEntity.h"

class CharacterDamageManager
{
public:
	static void getInstance();
	static void addDecal(CEntity* entity, float param2, float param3, char const* texture, unsigned long unk);

};


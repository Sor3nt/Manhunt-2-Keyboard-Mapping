#pragma once
#include "CEntity.h"

class CImpactDamageMap
{
public:
	static void AddExplodeHeadDamageDecals( CEntity* entity );
	static void AddGunDamageDecal(CEntity* entity, unsigned long var1, Vector3* vec1, Vector3* vec2);

};


#include "CImpactDamageMap.h"

void CImpactDamageMap::AddExplodeHeadDamageDecals(CEntity* entity)
{
	return (*(void(__stdcall*)(CEntity*))(0x595C30))(entity);

}

void CImpactDamageMap::AddGunDamageDecal(CEntity* entity, unsigned long var1, Vector3* vec1, Vector3* vec2)
{

	//untestet
//	return (*(void(__stdcall*)(CEntity*, unsigned long, Vector3*, Vector3*))(0x595E70))(entity, var1, vec1, vec2);

}

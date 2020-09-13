#include "CCharacter.h"

int CCharacter::GetUsedCollectable(CEntity* entity)
{
	return ((int (__thiscall*)(CEntity*))0x4F66C0)(entity);
}

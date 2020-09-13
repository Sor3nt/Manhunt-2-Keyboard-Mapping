#include "CharacterDamageManager.h"

void CharacterDamageManager::getInstance()
{
	((void(__stdcall*)())0x5B8D00)();
	//could not find a better solution...
	__asm mov ecx, eax
}

void CharacterDamageManager::addDecal(CEntity* entity, float param2, float param3, char const* texture, unsigned long unk)
{
	return ((void(__cdecl*)(CEntity*, float, float, char const*, unsigned long))0x5B9270)(entity, param2, param3, texture, unk);
}

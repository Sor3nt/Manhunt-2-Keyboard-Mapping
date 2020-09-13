#include "CCollectable.h"

void CCollectable::InitHooks()
{
	Memory::VP::InjectHook(0x55B906, CCollectable::Hook_GetCollectableType, PATCH_CALL);

}

int __fastcall CCollectable::Hook_GetCollectableType(int collectable)
{
	int result;
	if (collectable == 0) {

		CEntity* player = CEntityManager::FindInstance("player(player)");
		int collectable = CCharacter::GetUsedCollectable(player);
		result = CCollectable::GetCollectableType(collectable);
		printf("result2 is %i\n ", result);
	}
	else {
		result = CCollectable::GetCollectableType(*(int*)collectable);
		printf("result is %i\n ", result);

	}
	return result;

}

int CCollectable::GetCollectableType(int collectable)
{
	return ((int(__thiscall*)(int))0x526960)(collectable);

}

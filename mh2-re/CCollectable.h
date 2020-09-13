#pragma once
#include "CCharacter.h"
#include "MemoryMgr.h"
#include "CEntityManager.h"

class CCollectable
{
public:
	static void InitHooks();
	static int __fastcall Hook_GetCollectableType(int collectable);
	static int GetCollectableType(int collectable);
};


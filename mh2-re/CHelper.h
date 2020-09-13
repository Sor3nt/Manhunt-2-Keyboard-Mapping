#pragma once

#include "CEntity.h"
#include "modAI_Interface.h"
#include "CScriptVM.h"
#include "CCreationManager.h"
#include "CEntityManager.h"
#include "CPed.h"
#include "String"
#include "MemoryMgr.h"

struct CString {
	char* c_str;
	int length;
};

class CHelper
{
public:
	static CEntity* SpawnPlaceAndShowEntity(char* newName, char * archetype, Vector3 pos);
	static void ChangeHead(CEntity* entity, char* typeDataName);
	static void ExplodeHead(CEntity* hunter);
	static void PatchString(int address, const char* str);
	

	static CString GetCString(char* str);

};


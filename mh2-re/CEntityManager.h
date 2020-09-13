#pragma once

#include "CEntity.h"

class CEntityManager
{
	public:
		static int LoadCharacterClumpDict(int index);
		static int GetEntityTypeDataFromName(char* name);
		static CEntity* FindInstance( const char* name);
		static int MakeLocalClumpDictCurrent(char* name);
};


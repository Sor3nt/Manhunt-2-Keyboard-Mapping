#pragma once

#include "CEntity.h"
#include "Vector.h"

class CPed
{
public:
	static void CPed::ExplodeHead(CEntity* entity, Vector4 explodePosition, int padStateMasks, int unk2);
	static void CPed::ChangeHead(CEntity* entity, char* entityName, int unk);
};


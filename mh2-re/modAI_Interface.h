#pragma once
#include "CEntity.h"

class modAI_Interface
{
public:
	static int AISCRIPT_AddAIEntity(const char* name);
	static int AISCRIPT_AIEntityPlayLoopedAnim(char* entityName, char* animationName);
	static int AISCRIPT_AIEntityPlayAnim(char* entityName, char* animationName);
	static int ExecutionStarted(CEntity* entity, bool boo);
};


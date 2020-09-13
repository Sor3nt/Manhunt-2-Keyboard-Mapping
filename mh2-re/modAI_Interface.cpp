#include "modAI_Interface.h"

int modAi = 0x6E5F78;

int modAI_Interface::AISCRIPT_AddAIEntity(const char* name)
{
	return ((int(__thiscall*)(int, const char*))0x42E0C0)(modAi, name);
}

int modAI_Interface::AISCRIPT_AIEntityPlayAnim(char* entityName, char* animationName)
{
	return (*(int(__thiscall*)(int, char*, char*))0x42F2D0)(modAi, entityName, animationName);
}


int modAI_Interface::AISCRIPT_AIEntityPlayLoopedAnim(char* entityName, char* animationName)
{
	return (*(int(__thiscall*)(int, char*, char*))0x42F330)(modAi, entityName, animationName);
}

int modAI_Interface::ExecutionStarted(CEntity* entity, bool boo)
{
	return (*(int(__thiscall*)(int, CEntity*, bool))0x42F640)(modAi, entity, boo);

}

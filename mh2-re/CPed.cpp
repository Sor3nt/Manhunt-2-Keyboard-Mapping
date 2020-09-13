#include "CPed.h"

void CPed::ExplodeHead(CEntity* entity, Vector4 explodePosition, int padStateMasks, int unk2)
{
	(*(void(__thiscall *)(CEntity*, Vector4*, int, int))(0x503050))(entity, &explodePosition, padStateMasks, unk2);
}

void CPed::ChangeHead(CEntity* entity, char* entityName, int unk)
{
	//*(char*)(entity + 4109) = 0x01;
	(*(void(__thiscall*)(CEntity*, char*, int))(0x503050))(entity, entityName, unk);
//	*(char*)(entity + 4108) = 0x00;

}

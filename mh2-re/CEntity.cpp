#include "CEntity.h"

/*
	4			float		Position X
	8			float		Position Y
	12			float		Position Z


	43	ptr		CString		name
	88	ptr		function	????
	100	ptr		function	Entity hidden
	112 ptr		byte		Entity class
	116			integer		Invulnnerable
	124 ptr		float		health

	240			integer		LODLevel
*/

int CEntity::SetName( char* name)
{
	return ((int(__thiscall*)(CEntity*, char*))0x4E1D10)(this, name);
}

void CEntity::MoveTo(Vector3 vec)
{
	(*(void(__thiscall * *)(CEntity*, Vector3*, signed int))(*(int*)this + 60))(this, &vec, 1);

}

void CEntity::Show()
{
	(*(void(__thiscall * *)(CEntity*, bool))(*(int*)this + 100))(this, false);
}

void CEntity::Hide()
{
	(*(void(__thiscall * *)(CEntity*, bool))(*(int*)this + 100))(this, true);
}

int CEntity::GetTrackedFXSlot()
{
	return (*(int(__thiscall *)(int))(0x4E2150))(*(int*)(this + 692));
}

Vector4 CEntity::GetPosition()
{
	return *(Vector4*)(this + 4);
}

char* CEntity::GetName()
{
	_CString* cstr = (_CString*)(this + 172);
	return cstr->c_str;
}

void CEntity::SetBodyBurned()
{
	*(char*)(this + 4109) = 0x01;

}

void CEntity::ChangeBodyTexture(char* textureName)
{

	//load dic
	(*(void(__thiscall*)())(0x4E9080))();

	//int texture = CEntityManager::MakeLocalClumpDictCurrent(textureName);
	int texture = ((int(__cdecl*)(char*))0x5CA6A0)(textureName);

	if (texture) {

		//CClump::GetFirstAtomic
		int atomic = (*(int(__thiscall*)(int*))(0x5BBFE0))(*((int**)this + 46));
		//CGeometry::GetMaterial
		unsigned int material = (*(unsigned int(__thiscall*)(int*, int))(0x5C0C20))(*(int**)(atomic + 12), 0);

		*(int*)material = texture;
	}
}

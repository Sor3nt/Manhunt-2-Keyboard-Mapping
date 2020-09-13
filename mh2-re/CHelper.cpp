#include "CHelper.h"



CEntity* CHelper::SpawnPlaceAndShowEntity(char* newName, char* archetype, Vector3 pos)
{

	CEntity* entity = CCreationManager::SpawnNamedEntityFromArchetype(newName, archetype, pos);

	if (entity) {
		modAI_Interface::AISCRIPT_AddAIEntity(newName);
		entity->Show();
		return entity;
	}
	else {
		return 0;
	}
}


void CHelper::ChangeHead(CEntity* entity, char* typeDataName)
{

	int typeData = CEntityManager::GetEntityTypeDataFromName(typeDataName);
	CEntity* head = CCreationManager::CreateEntity((int*)typeData);
	*(int*)(entity + 692) = (int)head;

}

void CHelper::ExplodeHead(CEntity* hunter)
{
	//Issue: i have no idea which kind of vector he need, the explosion starts always at the ground.

	CPed::ExplodeHead(hunter, hunter->GetPosition(), -1, 2);

}

void CHelper::PatchString(int address, const char* string)
{
	int i = 0;
	for (; i < strlen(string); i++)
		Memory::VP::Patch<char>(0x1804A670 + i, string[i]);

	Memory::VP::Patch<char>(0x1804A670 + i, 0x00);

}

CString CHelper::GetCString(char* str)
{
	CString nameTest;
	nameTest.c_str = str;
	nameTest.length = strlen(str); 
	return nameTest;
}


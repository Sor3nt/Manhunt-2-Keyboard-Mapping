#include "CPedBodyAnimFSM.h"
#include "AnimationNames.h"
#include "string"
#include "MemoryMgr.h"

int animationIndex;
int animationLimit;

void CPedBodyAnimFSM::InitHooks()
{

	animationLimit = (sizeof AnimationNames) / 4;

	//Update the size limit (regular 2080)
	Memory::VP::Patch<int>(0x417A58 + 1, animationLimit);
	Memory::VP::Patch<int>(0x4337F3 + 1, animationLimit);
	Memory::VP::Patch<int>(0x43C581 + 1, animationLimit);
	Memory::VP::Patch<int>(0x43D561 + 1, animationLimit);
	Memory::VP::Patch<int>(0x4889EF + 1, animationLimit);
	Memory::VP::Patch<int>(0x48CCC0 + 1, animationLimit);
	Memory::VP::Patch<int>(0x48D134 + 1, animationLimit);
	Memory::VP::Patch<int>(0x48D470 + 1, animationLimit);
	Memory::VP::Patch<int>(0x48D480 + 1, animationLimit);
	Memory::VP::Patch<int>(0x48D490 + 1, animationLimit);
	Memory::VP::Patch<int>(0x48D501 + 1, animationLimit);
	Memory::VP::Patch<int>(0x48DA95 + 1, animationLimit);
	Memory::VP::Patch<int>(0x48DB6D + 1, animationLimit);
	Memory::VP::Patch<int>(0x48DBE0 + 1, animationLimit);
	Memory::VP::Patch<int>(0x48DBF0 + 1, animationLimit);
	Memory::VP::Patch<int>(0x48DC00 + 1, animationLimit);
	Memory::VP::Patch<int>(0x48E5F1 + 1, animationLimit);
	Memory::VP::Patch<int>(0x48EC43 + 1, animationLimit);
	Memory::VP::Patch<int>(0x4AE427 + 1, animationLimit);
	Memory::VP::Patch<int>(0x4FA364 + 1, animationLimit);
	Memory::VP::Patch<int>(0x4FA9D5 + 1, animationLimit);
	Memory::VP::Patch<int>(0x4FAAD4 + 1, animationLimit);
	Memory::VP::Patch<int>(0x4FA801 + 1, animationLimit);
	Memory::VP::Patch<int>(0x500CE8 + 1, animationLimit);
	Memory::VP::Patch<int>(0x500D10 + 1, animationLimit);
	Memory::VP::Patch<int>(0x500EEB + 1, animationLimit);
	Memory::VP::Patch<int>(0x50101B + 1, animationLimit);
	Memory::VP::Patch<int>(0x501A81 + 1, animationLimit);
	Memory::VP::Patch<int>(0x503645 + 1, animationLimit);
	Memory::VP::Patch<int>(0x50361A + 1, animationLimit);
	Memory::VP::Patch<int>(0x5037FE + 1, animationLimit);
	Memory::VP::Patch<int>(0x503D55 + 1, animationLimit);
	Memory::VP::Patch<int>(0x5040DE + 1, animationLimit);
	
	
	
	
	
	
	
	
	
	

	
	
	
	
	
	

	Memory::VP::Patch<int>(0x4A2E6D + 2, animationLimit);
	Memory::VP::Patch<int>(0x4AEA31 + 2, animationLimit);
	Memory::VP::Patch<int>(0x4AEAF4 + 2, animationLimit);
	Memory::VP::Patch<int>(0x48C98D + 2, animationLimit);
	Memory::VP::Patch<int>(0x48C013 + 2, animationLimit);
	
	Memory::VP::Patch<int>(0x7D23D6 + 2, animationLimit);

	
	
	Memory::VP::Patch<int>(0x4481E1 - 4, animationLimit);
	Memory::VP::Patch<int>(0x4F943E - 4, animationLimit);

	//not sure
	Memory::VP::Patch<int>(0x502A5E - 4, animationLimit);
	
	
	
	//Replace CPedBodyAnimFSM::GetAnimName at 0x489520
	Memory::VP::InjectHook(0x489520, &CPedBodyAnimFSM::Hook_GetAnimName, PATCH_JUMP);
}

const char* __cdecl CPedBodyAnimFSM::Hook_GetAnimName()
{
	__asm mov animationIndex, esp

	animationIndex = *(int*)(animationIndex + 4);

	return CPedBodyAnimFSM::GetAnimName(animationIndex);
}


const char* CPedBodyAnimFSM::GetAnimName(int index)
{
	if (index >= animationLimit) return 0;
	return AnimationNames[index];
}

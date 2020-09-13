#pragma once
class CPedBodyAnimFSM
{
public:
	static void InitHooks();
	const char* __cdecl Hook_GetAnimName();
	static const char* GetAnimName(int index);
};


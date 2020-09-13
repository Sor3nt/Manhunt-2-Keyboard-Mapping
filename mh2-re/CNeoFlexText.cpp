#include "CNeoFlexText.h"

CNeoItem* CNeoFlexText::create(char* command, float* pos, float* size, float* unk)
{

	CString txt = CHelper::GetCString(command);

	return ((CNeoItem * (__cdecl*)(CString*, float*, float*, float*))0x558710)(&txt, pos, size, unk);
}



CNeoItem* CNeoFlexText::create(char* command, float left, float top, float width, float unk1b, float unk2a, float unk2b)
{

	float pos[2];
	pos[0] = left;
	pos[1] = top;


	float unk1[2];
	unk1[0] = width;
	unk1[1] = unk1b;

	float unk2[2];
	unk2[0] = unk2a;
	unk2[1] = unk2b;

	CNeoItem* item = CNeoFlexText::create(command, pos, unk1, unk2);

	//int textDicPtr = (int)item + (6 * 4);

	return item;

}

#include "CNeoPage.h"

int CNeoPage::addItemToPage(CNeoItem* item)
{
	return ((int (__thiscall*)(int*, CNeoItem*))0x5646F0)(page, item);
}

CNeoItem* CNeoPage::getItem(char* name)
{
//	return ((int (__thiscall*)(int*))0x564040)((int)page->page);
	return ((CNeoItem*(__thiscall*)(int, char**))0x564040)((int)page, &name);
}

void CNeoPage::setName(char* name)
{
	//set page name
	int pageNameAddr = (int)page + 0x23c;
	*(CString*)pageNameAddr = CHelper::GetCString(name);

}


char* CNeoPage::getName()
{
	//set page name
	int pageNameAddr = (int)page + 0x23c;
	return (*(CString*)pageNameAddr).c_str;

}


void CNeoPage::setParent(char* name)
{
	//set page name
	int pageNameAddr = (int)page + 0x234;
	*(CString*)pageNameAddr = CHelper::GetCString(name);

}


void CNeoPage::setBackgroundTransition(char* mode)
{
	int value = 1;
	if (mode == "hold") value = 1;
	if (mode == "sweep") value = 0;

	int addr = (int)page + 0x15;
	*(int*)addr = value;

}

void CNeoPage::setCenterMode(char* mode, float unk)
{
	int value = 3;
	if (mode == "left") value = 0;
	if (mode == "middle") value = 1;
	if (mode == "right") value = 2;
	if (mode == "no") value = 3;

	int modePtr = (int)page + 0x274;
	*(int*)modePtr = value;

	int unkPtr = (int)page + 0x278;
	*(float*)unkPtr = unk;



}

CNeoItem* CNeoPage::getSelectedItem()
{

	unsigned int v2; // ebx
	int* v3; // ebp
	unsigned int v4; // esi
	int* v5; // edi

	int* self = (int*)page;


	v2 = 0;
	v3 = self + 6;
	while (2)
	{
		v4 = 0;
		v5 = v3;
		do
		{
			if (*v5 && ((CNeoItem*)self[4 * v2 + 6 + v2 + v4])->isSelected()) {
				return ((CNeoItem*)self[4 * v2 + 6 + v2 + v4]);
			}
				
			++v4;
			++v5;
		} while (v4 < 5);
		++v2;
		v3 += 5;
		if (v2 < 0x19)
			continue;
		break;
	}
	return 0;
}

void CNeoPage::setAlphaGhosting(bool status)
{
	int addr = (int)page + 0x2D9;
	*(char*)addr = status;

}

void CNeoPage::setBoldGhosting(bool status)
{
	int addr = (int)page + 0x2DA;
	*(char*)addr = status;

}

void CNeoPage::setWarping(bool status)
{
	int addr = (int)page + 0x2DB;
	*(char*)addr = status;

}

void CNeoPage::resetHighlighting()
{
	((void(__thiscall*)(int))0x563FD0)((int)page);

}

void CNeoPage::setBackground(char* name, float left, float top, float unk1, float unk2)
{
	int namePtr = (int)page + 0x218;
	*(CString*)namePtr = CHelper::GetCString(name);

	float* posPtr = (float*)((int)page + 0x224);
	posPtr[0] = left;
	posPtr[1] = top;

	float* kaPtr = (float*)((int)page + 0x22C);
	kaPtr[0] = unk1;
	kaPtr[1] = unk2;

}


//(void**)(a1 + 536), a1 + 548, a1 + 556


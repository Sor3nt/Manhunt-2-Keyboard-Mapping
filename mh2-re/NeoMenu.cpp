#include "NeoMenu.h"

int* NeoMenu::getInstance()
{
	return ((int*(__stdcall*)())0x562DF0)();
}

LayeredPage* NeoMenu::findPageByName(char *name)
{
	int instance = (int)NeoMenu::getInstance();
	int count = *(int*)(instance + (60 * 4));
	printf("pages: %i", count);
	for (int i = 0; i < count; i++) {
		LayeredPage* page = (LayeredPage*)(instance + ((i * 4))+ (10*4) );

		if (strcmp(page->getName(), name) == 0) return page;
		printf(" name: %s ", page->getName());


	}
//	LayeredPage* page = (LayeredPage*)(instance + (60 * 4));
	return nullptr;
}

void NeoMenu::setActivePage(char* name, char boo)
{
	CString txt;
	txt.c_str = name;
	txt.length = strlen(name);

	int* instance = NeoMenu::getInstance();
	((void (__thiscall*)(int*, CString*, char))0x562B00)(instance, &txt, boo);

}

int NeoMenu::insertPage(CNeoPage* page)
{
	page->resetHighlighting();

	int* instance = NeoMenu::getInstance();
	return ((int(__thiscall*)(int*, int))0x5626A0)(instance, (int)page->page);

}

bool NeoMenu::isCurrentPageSetTo(char* name)
{
	CString txt;
	txt.c_str = name;
	txt.length = strlen(name);

	int* instance = NeoMenu::getInstance();
	return ((bool(__thiscall*)(int*, CString*))0x5623F0)(instance, &txt);

}

void NeoMenu::freeze(bool status)
{
	if (status) {
		Memory::VP::Patch<char>(0x565E10, 0xc3);
	}
	else {
		Memory::VP::Patch<char>(0x565E10, 0x51);

	}
}

void NeoMenu::setDefaultColor(int r, int g, int b, int a)
{
	((int(__cdecl*)(int, int, int, int))0x561910)(r,g,b,a);
}

#pragma once
#include "CHelper.h"
#include "CNeoPage.h"
#include "LayeredPage.h"

class NeoMenu
{
public:
	static int* getInstance();
	static LayeredPage* findPageByName(char* name);
	static void setActivePage( char* name, char boo);
	static int insertPage(CNeoPage* page);
	static bool isCurrentPageSetTo(char* name);
	static void freeze(bool status);
	static void setDefaultColor(int r, int g, int b, int a);
};


#pragma once
#include "CMemory.h"
#include "CHelper.h"
#include "CNeoItem.h"

class CNeoPage
{
public:
	int* page;

	CNeoItem* getSelectedItem();

	void setAlphaGhosting(bool status);
	void setBoldGhosting(bool status);
	void setWarping(bool status);
	void resetHighlighting();
	void setBackground(char* name, float left, float top, float unk1, float unk2);
	void setBackgroundTransition(char* mode);
	void setCenterMode(char* mode, float unk);
	void setName(char* name);
	char* getName();
	void setParent(char* name);
	int addItemToPage(CNeoItem* item);
	CNeoItem* getItem(char* name);
};


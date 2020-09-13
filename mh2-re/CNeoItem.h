#pragma once
#include "CHelper.h"
#include "Keyboard.h"

class CNeoItem
{
public:

	bool isSwitch();
	bool isKeybinding();
	char isSelected();
	void setLeft(float left);
	void setTop(float top);
	void test();
	void setBackgroundTransparent(bool status);
	void setTextHeight(float height);
	void setText2(char* text);
	char* getText2();
	void setText1(char* text);
	char* getText1();

	void setSwitchText1(char* text);

	void setTextWidth(float width);
	void setLabel(char* text);
	void setSelectable(bool status);
	void select(bool status);
	void setKeyBindingLabel(int action);

};


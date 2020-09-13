#pragma once
#include "CNeoItem.h"
#include "CHelper.h"

class CNeoSwitch
{
public:
	CNeoItem* create(char* command, char* text, float left, float top, char* button1Text, float btn1Left, float btn1Top, char* button2Text, float btn2Left, float btn2Top);
	CNeoItem* create(char* command, char* text, float left, float top, char* button1Text, float btn1Left, float btn1Top);

};


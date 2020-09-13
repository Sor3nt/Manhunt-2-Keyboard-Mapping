#pragma once
#include "CNeoItem.h"
#include "CHelper.h"
#include "string"

class CNeoCommand
{
public:
	CNeoItem* create(char* command, float* pos, char* text, char* backPage);
	CNeoItem* create(char* command, char* text, float top, float left, char* backPage);

};


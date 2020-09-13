#pragma once

#include "CNeoItem.h"
#include "string"
#include "CHelper.h"

class CNeoFlexText
{
public:
	CNeoItem* create(char* command, float* pos, float* size, float* unk);
	CNeoItem* create(char* command, float left, float top, float unk1a, float unk1b, float unk2a, float unk2b);
};


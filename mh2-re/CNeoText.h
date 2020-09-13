#pragma once

#include "CNeoItem.h"
#include "string"
#include "CHelper.h"

class CNeoText
{
public:
	CNeoItem* create(char* command, float* pos, float* sizeAnd);
	CNeoItem* create(char* command, float left, float top, float size, float unk);
};


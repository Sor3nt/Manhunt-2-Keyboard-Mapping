#pragma once

#include "CNeoItem.h"
#include "string"
#include "CHelper.h"

class CNeoPicture
{
public:
	CNeoItem* create(char* image, float* pos, float* sizeAnd);
	CNeoItem* create(char* image, float left, float top, float size, float unk);
};


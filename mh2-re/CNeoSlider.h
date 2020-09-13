#pragma once
#include "CNeoItem.h"
#include "CHelper.h"
#include "string"

class CNeoSlider
{
public:
	CNeoItem* create(char* command, char* text, float* pos, float* sliderPos, float unk1, float unk2, float unk3);
	CNeoItem* create(char* command, char* text, float left, float top, float size, float unk);

};


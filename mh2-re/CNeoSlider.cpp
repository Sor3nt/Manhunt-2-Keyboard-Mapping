#include "CNeoSlider.h"

CNeoItem* CNeoSlider::create(char* command, char* text, float* pos, float* sliderPos, float unk1, float unk2, float unk3)
{

	CString cCommand = CHelper::GetCString(command);
	CString cText = CHelper::GetCString(text);

	return ((CNeoItem * (__cdecl*)(CString*, CString*, float*, float*, float, float, float))0x55DFF0)(&cCommand, &cText, pos, sliderPos, unk1, unk2, unk3);


}

CNeoItem* CNeoSlider::create(char* command, char* text, float left, float top, float sliderLeft, float sliderTop)
{

	float pos[2];
	pos[0] = left;
	pos[1] = top;

	float sliderPos[2];
	sliderPos[0] = sliderLeft;
	sliderPos[1] = sliderTop;

	return CNeoSlider::create(command, text, pos, sliderPos, 1.0, 0.1, 0.5);
}


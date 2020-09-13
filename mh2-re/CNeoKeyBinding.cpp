#include "CNeoKeyBinding.h"

CNeoItem* CNeoKeyBinding::create(char* command, float left, float top, char* boxText, float textWidth, float textHeight)
{
	CString cCommand = CHelper::GetCString(command);
	CString cBoxText = CHelper::GetCString(boxText);

	float labelPos[2];
	labelPos[0] = left;
	labelPos[1] = top;

	float textSize[2];
	textSize[0] = textWidth;
	textSize[1] = textHeight;

	return ((CNeoItem * (__cdecl*)(CString*, float*, CString*, float*))0x55D2D0)(&cCommand, labelPos, &cBoxText, textSize);

}

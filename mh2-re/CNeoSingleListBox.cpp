#include "CNeoSingleListBox.h"

CNeoItem* CNeoSingleListBox::create(char* command, float left, float top, char* boxText, float boxLeft, float boxTop)
{
	CString cCommand = CHelper::GetCString(command);
	CString cBoxText = CHelper::GetCString(boxText);

	float labelPos[2];
	labelPos[0] = left;
	labelPos[1] = top;

	float boxPos[2];
	boxPos[0] = boxLeft;
	boxPos[1] = boxTop;

	return ((CNeoItem * (__cdecl*)(CString*, float*, CString*, float*))0x560F20)(&cCommand, labelPos, &cBoxText, boxPos);

}

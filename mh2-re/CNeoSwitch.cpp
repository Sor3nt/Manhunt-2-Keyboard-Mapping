#include "CNeoSwitch.h"

CNeoItem* CNeoSwitch::create(char* command, char* text, float left, float top, char* button1Text, float btn1Left, float btn1Top, char* button2Text, float btn2Left, float btn2Top)
{

	CString cCommand = CHelper::GetCString(command);
	CString cText = CHelper::GetCString(text);

	float pos[2];
	pos[0] = left;
	pos[1] = top;

	float buttons[4];
	buttons[0] = btn1Left;
	buttons[1] = btn1Top;
	buttons[2] = btn2Left;
	buttons[3] = btn2Top;

	CString buttonText[2];
	buttonText[0] = CHelper::GetCString(button1Text);
	buttonText[1] = CHelper::GetCString(button2Text);

	return ((CNeoItem * (__cdecl*)(CString*, CString*, float*, int, float*, CString*, int))0x55FD80)(&cCommand, &cText, pos, 2, buttons, buttonText, 0);

}


CNeoItem* CNeoSwitch::create(char* command, char* text, float left, float top, char* button1Text, float btn1Left, float btn1Top)
{

	CString cCommand = CHelper::GetCString(command);
	CString cText = CHelper::GetCString(text);

	float pos[2];
	pos[0] = left;
	pos[1] = top;

	float buttons[2];
	buttons[0] = btn1Left;
	buttons[1] = btn1Top;

	CString buttonText[1];
	buttonText[0] = CHelper::GetCString(button1Text);

	return ((CNeoItem * (__cdecl*)(CString*, CString*, float*, int, float*, CString*, int))0x55FD80)(&cCommand, &cText, pos, 1, buttons, buttonText, 0);

}



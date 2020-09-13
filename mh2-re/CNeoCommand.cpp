#include "CNeoCommand.h"

CNeoItem* CNeoCommand::create(char* command, float* pos, char* text, char* backPage)
{
	CString cCommand = CHelper::GetCString(command);
	CString cText = CHelper::GetCString(text);
	CString cBackPage = CHelper::GetCString(backPage);


	return ((CNeoItem * (__cdecl*)(CString*,  float*, CString*, CString*))0x55F120)(&cCommand, pos, &cText, &cBackPage);
}

CNeoItem* CNeoCommand::create(char* command, char* text, float top, float left, char* backPage)
{
	float pos[2];
	pos[0] = left;
	pos[1] = top;

	return CNeoCommand::create(command, pos, text, backPage);
}

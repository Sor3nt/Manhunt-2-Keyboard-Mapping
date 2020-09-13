#include "CNeoLinkToPage.h"

CNeoItem* CNeoLinkToPage::create(char* command, char* actionName, float* pos, int keepOrSweep)
{

	CString cCommand;
	cCommand.c_str = command;
	cCommand.length = strlen(command);

	CString cActionName;
	cActionName.c_str = actionName;
	cActionName.length = strlen(actionName);
	
	return ((CNeoItem * (__cdecl*)(CString*, CString*, float*, int))0x55FB20)(&cCommand, &cActionName, pos, keepOrSweep);
}


CNeoItem* CNeoLinkToPage::create(char* command, char* actionName, float left, float top, int keepOrSweep)
{

	float pos[2];
	pos[0] = left;
	pos[1] = top;

	return CNeoLinkToPage::create(command, actionName, pos, keepOrSweep);
}

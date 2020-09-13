#pragma once
#include "CHelper.h"
#include "CNeoItem.h"

class CNeoLinkToPage
{
public:
	CNeoItem* create(char* command, char* actionName, float* pos, int keepOrSweep);
	CNeoItem* create(char* command, char* actionName, float left, float top, int keepOrSweep);
};


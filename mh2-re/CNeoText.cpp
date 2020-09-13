#include "CNeoText.h"

CNeoItem* CNeoText::create(char* command, float* pos, float* size)
{
	CString txt = CHelper::GetCString(command);

	return ((CNeoItem * (__cdecl*)(CString*, float*, float*))0x55EFF0)(&txt, pos, size);


}

CNeoItem* CNeoText::create(char* command, float left, float top, float size, float unk)
{

	float pos[2];
	pos[0] = left;
	pos[1] = top;

	float sizes[2];
	sizes[0] = size;
	sizes[1] = unk;

	return CNeoText::create(command, pos, sizes);
}

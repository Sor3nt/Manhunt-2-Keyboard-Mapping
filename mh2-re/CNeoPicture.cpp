#include "CNeoPicture.h"

CNeoItem* CNeoPicture::create(char* image, float* pos, float* size)
{
	CString txt = CHelper::GetCString(image);

	return ((CNeoItem * (__cdecl*)(CString*, float*, float*))0x55CA50)(&txt, pos, size);


}

CNeoItem* CNeoPicture::create(char* image, float left, float top, float width, float height)
{

	float pos[2];
	pos[0] = left;
	pos[1] = top;

	float sizes[2];
	sizes[0] = width;
	sizes[1] = height;

	return CNeoPicture::create(image, pos, sizes);
}

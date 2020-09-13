#include "CMatrix.h"

void CMatrix::SetIdentity(float* matrix)
{
	(*(int(__thiscall*)(float*))(0x57E390))(matrix);

}

void CMatrix::SetRotation(float* matrix, float* vec, float rotation)
{
	(*(int(__thiscall*)(float*, float*, float))(0x57E440))(matrix, vec, rotation);

	
}

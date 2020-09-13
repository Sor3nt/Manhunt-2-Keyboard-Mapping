#pragma once
class CMatrix
{
public:
	static void SetIdentity(float* matrix);
	static void SetRotation(float* matrix, float* vec, float rotation);
};
	

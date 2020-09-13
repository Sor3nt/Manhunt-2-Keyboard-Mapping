#pragma once

#include "Vector.h"
//#include "CEntityManager.h"

//hack because chelper includes alread centy -.-
struct _CString {
	char* c_str;
	int length;
};

class CEntity
{
	public:
		int SetName( char* name);
		void MoveTo(Vector3 vec);
		void Show();
		void Hide();
		int GetTrackedFXSlot();
		

		//Custom function, correct name not known ?!
		Vector4 GetPosition();
		char* GetName();
		void SetBodyBurned();
		void ChangeBodyTexture(char* textureName);

};


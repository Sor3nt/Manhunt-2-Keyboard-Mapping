#pragma once
#include "map"

class MyTrans
{
public:
	std::map<char*, char*> translation;

	void addTranslation(char* key, char* text);
	int get(char* key);


	static wchar_t* GetWC(char* c)
	{
		const size_t cSize = strlen(c) + 1;
		wchar_t* wc = new wchar_t[cSize];
		mbstowcs(wc, c, cSize);

		return wc;
	}

};


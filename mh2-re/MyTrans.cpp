#include "MyTrans.h"


void MyTrans::addTranslation(char* key, char* text)
{
	translation.insert(std::pair<char*, char*>(key, text));
}

int MyTrans::get(char* key)
{
	std::map<char*, char*>::iterator it = translation.begin();

	for (it = translation.begin(); it != translation.end(); ++it){
		if (strcmp(it->first, key) == 0){
			wchar_t* a = GetWC(it->second);
			return (int)a;
		}
	}

	wchar_t* notFound = L"Not found";
	return (int)notFound;

}
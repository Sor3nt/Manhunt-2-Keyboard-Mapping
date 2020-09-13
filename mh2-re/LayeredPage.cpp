#include "LayeredPage.h"

LayeredPage::LayeredPage(){
	int mem = CMemory::Allocate(736);

	//CNeoItem::create
	page = ((int* (__thiscall*)(int))0x553100)(mem);
	page = (int*)page;
}

#include "CNeoItem.h"



bool CNeoItem::isSwitch()
{
	return *(int*)(this + (29 * 4)) == 11;
}


bool CNeoItem::isKeybinding()
{
	return *(int*)(this + (29 * 4)) == 19;
}

char CNeoItem::isSelected()
{
	return *(char*)(this + 100);
}

void CNeoItem::setLeft(float left)
{
	*(float*)(this + (7 * 4)) = left;
}

void CNeoItem::setTop(float top)
{
	*(float*)(this + (8 * 4)) = top;
}

void CNeoItem::setLabel(char* text)
{
	CString cText = CHelper::GetCString(text);
	*(CString*)(this + (3 * 4)) = cText;
}

void CNeoItem::setSelectable(bool status)
{
	*(char*)(this + 112) = status;
}

void CNeoItem::select(bool status)
{
	*(char*)(this + 100) = status;

}

void CNeoItem::setBackgroundTransparent(bool status)
{
	*(char*)(this + 132) = status;
}


void CNeoItem::test()
{
	CString cText = CHelper::GetCString("hmm");
	printf("hier ptr %p ", this);
	int tmp = (int)this;
	tmp += 0x7C;

	tmp = *(int*)tmp;
	tmp += 0x0c;

	*(CString*)(tmp ) = cText;
}


void CNeoItem::setTextWidth(float width)
{
	*(float*)(this + (9 * 4)) = width;
}

void CNeoItem::setTextHeight(float height)
{
	*(float*)(this + (10 * 4)) = height;
}

void CNeoItem::setText2(char* text)
{
	CString cText = CHelper::GetCString(text);
	*(CString*)(this + (3 * 4)) = cText;
}

char* CNeoItem::getText2()
{
	return (*(CString*)(this + (3 * 4))).c_str;
}


void CNeoItem::setText1(char* text)
{
	CString cText = CHelper::GetCString(text);
	*(CString*)(this + (1 * 4)) = cText;
}

char* CNeoItem::getText1()
{
	return (*(CString*)(this + (1 * 4))).c_str;
}

void CNeoItem::setSwitchText1(char* text)
{
	CString cText = CHelper::GetCString(text);

	int tmp = (int)this;
	tmp += 0x7C;

	tmp = *(int*)tmp;
	tmp += 0x0c;

	*(CString*)(tmp) = cText;
}


void CNeoItem::setKeyBindingLabel(int action)
{

//	CString cText = CHelper::GetCString(text);
	*(int*)(this + (32 * 4)) = action;
}

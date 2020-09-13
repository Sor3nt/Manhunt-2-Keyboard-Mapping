#pragma once
class CClumpDict
{
public:
	static const int ms_globalClumpDict = 0x6ECE38;
	static int Initialise(int* self, char* txd, char* mdl);
	static int CheckDict(int* self);
};


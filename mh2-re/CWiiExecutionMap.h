#pragma once
class CWiiExecutionMap
{
public:

	static const int ms_Singleton = 0x77B4DC;
	static int* getSingleton();
	char* DisplayIcons(int self, int executionId, int executionLevel);
	static int* GetDecal(int self, int executionId, int a, int b);
	static int* GetAction(int self, int executionId, int executionLevel, int b);

};


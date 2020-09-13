#include "CMouseExecution.h"

void CMouseExecution::resetAction( int weapon)
{
	CMouseExecution::resetAction(CMouseExecution::STAGE::WHITE, weapon);
	CMouseExecution::resetAction(CMouseExecution::STAGE::YELLOW, weapon);
	CMouseExecution::resetAction(CMouseExecution::STAGE::RED, weapon);
}


void CMouseExecution::setFailstate(int state, int weapon, int stage)
{
	int* singleton = CWiiExecutionMap::getSingleton();

	//workaround...
	char* FAILSTATEString[] = {
			"NO_ANIM_DEAD",
			"NO_ANIM_RECOVER",
			"WOUNDED_TO_RECOVER",
			"ON_KNEES_TO_RECOVER",
			"ON_KNEES_TURNED_TO_RECOVER",
			"WOUNDED_TURNED_TO_RECOVER",
			"WOUNDED_TO_DEAD",
			"WOUNDED_TO_DEAD_TOWARDS_PLAYER",
			"ON_BACK_TO_RECOVER",
			"ON_BACK_TO_DIE",
			"FACING_TOWARDS_PLAYER_TO_DIE",
			"FACING_TOWARDS_PLAYER_TO_RECOVER",
			"PUKE_TO_RECOVER",
			"GET_UP_TO_RECOVER",
			"STANDING_TO_DIE_FACE_DW",
			"STANDING_TO_DIE_FACE_UP"
	};

	((int(__thiscall*)(int*, int, int, char*))0x5B5D20)(singleton, weapon, stage, FAILSTATEString[state]);

	

//	singleton[12 * (weapon + 4 * stage) + 176129 + singleton[4 * stage + 180257 + weapon]] = state;

}

void CMouseExecution::resetAction(int stage, int weapon)
{
	int* singleton = CWiiExecutionMap::getSingleton();
	int position = CMouseExecution::STAGE::WHITE + 4 * WEAPON::SHARD;

	//set entry count to 0
	singleton[180257 + position] = 0;
	//set active entry to 0
	singleton[16513 + position] = 0;

}


int CMouseExecution::addAction( int nameIndex, int levelIndex, int actionIndex, float numOfFrames, float secondsDistance, int unk3)
{
	int* singleton = CWiiExecutionMap::getSingleton();
	return ((int(__thiscall*)(int*, int, int, int, float, float, int))0x5B5860)(singleton, nameIndex, levelIndex, actionIndex, numOfFrames, secondsDistance, unk3);
}



int CMouseExecution::addAction(int nameIndex, int levelIndex, int actionIndex, float numOfFrames, float secondsDistance, int unk3, int failstate)
{
	
	int res = CMouseExecution::addAction(nameIndex, levelIndex, actionIndex, numOfFrames, secondsDistance, unk3);
	CMouseExecution::setFailstate(failstate, nameIndex, levelIndex);

	return res;
}
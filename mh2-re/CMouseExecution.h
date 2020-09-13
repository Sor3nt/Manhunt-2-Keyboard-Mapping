#pragma once
#include "CWeapon.h"
#include "CWiiExecutionMap.h"


class CMouseExecution
{
public:

	enum MOUSE_ACTION {
		MOUSE_NONE = 0,
		MOUSE_RIGHT = 1,
		MOUSE_LEFT = 2,
		MOUSE_UP = 3,
		MOUSE_DOWN = 4,
		MOUSE_SHAKE = 5,
		MOUSE_GRAB = 6,
		MOUSE_CIRCLE = 7,
		MOUSE_SAW = 8,
		MOUSE_BUTTON = 9
	};

	

	enum FAILSTATE {
		NO_ANIM_DEAD = 0,
		NO_ANIM_RECOVER = 1,
		WOUNDED_TO_RECOVER = 2,
		ON_KNEES_TO_RECOVER = 3,
		ON_KNEES_TURNED_TO_RECOVER = 4,
		WOUNDED_TURNED_TO_RECOVER = 5,
		WOUNDED_TO_DEAD = 6,
		WOUNDED_TO_DEAD_TOWARDS_PLAYER = 7,
		ON_BACK_TO_RECOVER = 8,
		ON_BACK_TO_DIE = 9,
		FACING_TOWARDS_PLAYER_TO_DIE = 10,
		FACING_TOWARDS_PLAYER_TO_RECOVER = 11,
		PUKE_TO_RECOVER = 12,
		GET_UP_TO_RECOVER = 13,
		STANDING_TO_DIE_FACE_DW = 14,
		STANDING_TO_DIE_FACE_UP = 15

	};

	enum STAGE {
		WHITE = 0,
		YELLOW = 1,
		RED = 2,
		JUMP = 3
	};

	static void setFailstate(int state, int weapon, int stage);
	static void resetAction(int stage, int weapon);
	static void resetAction(int weapon);
	static int addAction(int nameIndex, int levelIndex, int actionIndex, float numOfFrames, float secondsDistance, int unk3);
	static int addAction(int nameIndex, int levelIndex, int actionIndex, float numOfFrames, float secondsDistance, int unk3, int failstate);
};


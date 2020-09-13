#pragma once

struct mhGlobalData {

	//4 ptr (?)
	//8 ptr (?)
	//142 wMonth
	//146 wDay
	//140 Year
	//148 Hour
	//150 Minute
	//152 Second

	//156 NextLevel
	//160 CurrentLevel
	//164 ForceNextLevel

	char _pad[200];

	int Widescreen;			//200
	int ScreenH;			//204
	int ScreenV;			//208
	int ControllerSetup;	//212
	int InvertLook;			//216
	int AimSensitivity;		//220
	int Vibration;			//224
	int LookX;				//228
	int LookY;				//232
	int AdvancedGun;		//236
	int InteractiveMode;	//240
	int TargetX;			//244
	int TargetY;			//248

	//unused / unknown
	char _pad2[4];			//252

	int Difficulty;			//256
};
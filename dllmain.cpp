
#include "String";
#include "windows.h"
#include "SDKDDKVer.h"

#include "MemoryMgr.h"
#include "mh2-re/CMemory.h"

#include "mh2-re/CHelper.h"
#include "mh2-re/MyTrans.h"

#include "mh2-re/CInput.h"
#include "mh2-re/Keyboard.h"

#include "mh2-re/NeoMenu.h"
#include "mh2-re/LayeredPage.h"
#include "mh2-re/CNeoText.h"
#include "mh2-re/CNeoPage.h"
#include "mh2-re/CNeoLinkToPage.h"
#include "mh2-re/CNeoKeyBinding.h"
#include "mh2-re/CNeoPicture.h"


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:

bool KeyHit(unsigned int keyCode) { return (GetKeyState(keyCode) & 0x8000) != 0; }
bool ButtonHit(unsigned int keyCode) { return (GetKeyState(keyCode) & 0x100) != 0; }

// calling
template <unsigned int address, typename... Args>
static void Call(Args... args) {
	reinterpret_cast<void(__cdecl*)(Args...)>(address)(args...);
}

template <typename Ret, unsigned int address, typename... Args>
static Ret CallAndReturn(Args... args) {
	return reinterpret_cast<Ret(__cdecl*)(Args...)>(address)(args...);
}

template <unsigned int address, typename C, typename... Args>
static void CallMethod(C _this, Args... args) {
	reinterpret_cast<void(__thiscall*)(C, Args...)>(address)(_this, args...);
}

template <typename Ret, unsigned int address, typename C, typename... Args>
static Ret CallMethodAndReturn(C _this, Args... args) {
	return reinterpret_cast<Ret(__thiscall*)(C, Args...)>(address)(_this, args...);
}


template<typename ...Args>
static void WriteDebug(int line, const char* msg, Args ...args)
{
	Call<0x53D8D0, int, const char*, Args...>(line, msg, args...);
}



int getPressedKey() {
	int max = 254;
	do {
		if (KeyHit(max)) {
			return max;
		}
		if (ButtonHit(max)) {
			return max;
		}
	} while (max--);

	return -1;
}

char* converrtDirectInputCodeToName(int keycode) {
	switch (keycode) {
	case 0: return "unsued";
	case -1: return "PRESS A KEY";
	case -32: return "MOUSE RIGHT";
	case -34: return "MOUSE LEFT";
	case -28: return "MOUSE LEFT";
	case -29: return "MOUSE RIGHT";

	case 0xE0: return "MOUSE RIGHT";
	case 0xDE: return "MOUSE LEFT";
	case 0x01: return "ESCAPE";
	case 0x02: return "1";
	case 0x03: return "2";
	case 0x04: return "3";
	case 0x05: return "4";
	case 0x06: return "5";
	case 0x07: return "6";
	case 0x08: return "7";
	case 0x09: return "8";
	case 0x0A: return "9";
	case 0x0B: return "0";
	case 0x0C: return "MINUS";
	case 0x0D: return "EQUALS";
	case 0x0E: return "BACK";
	case 0x0F: return "TAB";
	case 0x10: return "Q";
	case 0x11: return "W";
	case 0x12: return "E";
	case 0x13: return "R";
	case 0x14: return "T";
	case 0x15: return "Y";
	case 0x16: return "U";
	case 0x17: return "I";
	case 0x18: return "O";
	case 0x19: return "P";
	case 0x1A: return "LBRACKET";
	case 0x1B: return "RBRACKET";
	case 0x1C: return "RETURN";
	case 0x1D: return "LContol";
	case 0x1E: return "A";
	case 0x1F: return "S";
	case 0x20: return "D";
	case 0x21: return "F";
	case 0x22: return "G";
	case 0x23: return "H";
	case 0x24: return "J";
	case 0x25: return "K";
	case 0x26: return "L";
	case 0x27: return "SEMICOLON";
	case 0x28: return "APOSTROPHE";
	case 0x29: return "GRAVE";
	case 0x2A: return "LSHIFT";
	case 0x2B: return "BACKSLASH";
	case 0x2C: return "Z";
	case 0x2D: return "X";
	case 0x2E: return "C";
	case 0x2F: return "V";
	case 0x30: return "B";
	case 0x31: return "N";
	case 0x32: return "M";
	case 0x33: return "COMMA";
	case 0x34: return "PERIOD";
	case 0x35: return "SLASH";
	case 0x36: return "RSHIFT";
	case 0x37: return "MULTIPLY";
	case 0x38: return "LMENU";
	case 0x39: return "SPACE";
	case 0x3A: return "CAPITAL";
	case 0x3B: return "F1";
	case 0x3C: return "F2";
	case 0x3D: return "F3";
	case 0x3E: return "F4";
	case 0x3F: return "F5";
	case 0x40: return "F6";
	case 0x41: return "F7";
	case 0x42: return "F8";
	case 0x43: return "F9";
	case 0x44: return "F10";
	case 0x45: return "NUMLOCK";
	case 0x46: return "SCROLL";
	case 0x47: return "NUMPAD7";
	case 0x48: return "NUMPAD8";
	case 0x49: return "NUMPAD9";
	case 0x4A: return "SUBTRACT";
	case 0x4B: return "NUMPAD4";
	case 0x4C: return "NUMPAD5";
	case 0x4D: return "NUMPAD6";
	case 0x4E: return "ADD";
	case 0x4F: return "NUMPAD1";
	case 0x50: return "NUMPAD2";
	case 0x51: return "NUMPAD3";
	case 0x52: return "NUMPAD0";
	case 0x53: return "DECIMAL";
	case 0x57: return "F11";
	case 0x58: return "F12";
	case 0x64: return "F13";
	case 0x65: return "F14";
	case 0x66: return "F15";
	case 0x70: return "KANA";
	case 0x79: return "CONVERT";
	case 0x7B: return "NOCONVERT";
	case 0x7D: return "YEN";
	case 0x8D: return "NUMPADEQUALS";
	case 0x90: return "CIRCUMFLEX";
	case 0x91: return "AT";
	case 0x92: return "COLON";
	case 0x93: return "UNDERLINE";
	case 0x94: return "KANJI";
	case 0x95: return "STOP";
	case 0x96: return "AX";
	case 0x97: return "UNLABELED";
	case 0x9C: return "NUMPADENTER";
	case 0x9D: return "RCONTROL";

	case 0xB3: return "NUMPADCOMMA";
	case 0xB5: return "DIVIDE";
	case 0xB7: return "SYSRQ";
	case 0xB8: return "RMENU";
	case 0xC5: return "PAUSE";
	case 0xC7: return "HOME";
	case 0xC8: return "UP";
	case 0xC9: return "PRIOR";
	case 0xCB: return "LEFT";
	case 0xCD: return "RIGHT";
	case 0xCF: return "END";
	case 0xD0: return "DOWN";
	case 0xD1: return "NEXT";
	case 0xD2: return "INSERT";
	case 0xD3: return "DELETE";
	case 0xDB: return "LWIN";
	case 0xDC: return "RWIN";
	case 0xDD: return "APPS";
	default:
		printf("Unable to find Name for DirectInput Code %i   \n", keycode);
		return "unsupported";
	}
}

int convertKeyCodeToDirectInputCode(int keycode) {

	switch (keycode) {
		//case 0: return 0x00;

	case 1: return Keyboard::KEY_MOUSE_LEFT;
	case 2: return Keyboard::KEY_MOUSE_RIGHT;
		//	case 2: return "mouse right";
	case 8: return Keyboard::KEY_BACK;
	case 9: return Keyboard::KEY_TAB;
	case 13: return Keyboard::KEY_RETURN;
	case 16: return Keyboard::KEY_LSHIFT;
	case 17: return Keyboard::KEY_LContol;
		//	case 18: return Keyboard::ALT;
	case 19: return Keyboard::KEY_PAUSE;
		//	case 20: return "caps lock";
	case 27: return Keyboard::KEY_ESCAPE;
	case 32: return Keyboard::KEY_SPACE;
		//	case 33: return "page up";
			//case 34: return "page down";
	case 35: return Keyboard::KEY_END;
	case 36: return Keyboard::KEY_HOME;
	case 37: return Keyboard::KEY_LEFT;
	case 38: return Keyboard::KEY_UP;
	case 39: return Keyboard::KEY_RIGHT;
	case 40: return Keyboard::KEY_DOWN;
	case 45: return Keyboard::KEY_INSERT;
	case 46: return Keyboard::KEY_DELETE;
	case 48: return Keyboard::KEY_0;
	case 49: return Keyboard::KEY_1;
	case 50: return Keyboard::KEY_2;
	case 51: return Keyboard::KEY_3;
	case 52: return Keyboard::KEY_4;
	case 53: return Keyboard::KEY_5;
	case 54: return Keyboard::KEY_6;
	case 55: return Keyboard::KEY_7;
	case 56: return Keyboard::KEY_8;
	case 57: return Keyboard::KEY_9;
	case 65: return Keyboard::KEY_A;
	case 66: return Keyboard::KEY_B;
	case 67: return Keyboard::KEY_C;
	case 68: return Keyboard::KEY_D;
	case 69: return Keyboard::KEY_E;
	case 70: return Keyboard::KEY_F;
	case 71: return Keyboard::KEY_G;
	case 72: return Keyboard::KEY_H;
	case 73: return Keyboard::KEY_I;
	case 74: return Keyboard::KEY_J;
	case 75: return Keyboard::KEY_K;
	case 76: return Keyboard::KEY_L;
	case 77: return Keyboard::KEY_M;
	case 78: return Keyboard::KEY_N;
	case 79: return Keyboard::KEY_O;
	case 80: return Keyboard::KEY_P;
	case 81: return Keyboard::KEY_Q;
	case 82: return Keyboard::KEY_R;
	case 83: return Keyboard::KEY_S;
	case 84: return Keyboard::KEY_T;
	case 85: return Keyboard::KEY_U;
	case 86: return Keyboard::KEY_V;
	case 87: return Keyboard::KEY_W;
	case 88: return Keyboard::KEY_X;
	case 89: return Keyboard::KEY_Y;
	case 90: return Keyboard::KEY_Z;
	case 91: return Keyboard::KEY_LWIN;
	case 92: return Keyboard::KEY_RWIN;
		//case 93: return "select key";
	case 96: return Keyboard::KEY_NUMPAD0;
	case 97: return Keyboard::KEY_NUMPAD1;
	case 98: return Keyboard::KEY_NUMPAD2;
	case 99: return Keyboard::KEY_NUMPAD3;
	case 100: return Keyboard::KEY_NUMPAD4;
	case 101: return Keyboard::KEY_NUMPAD5;
	case 102: return Keyboard::KEY_NUMPAD6;
	case 103: return Keyboard::KEY_NUMPAD7;
	case 104: return Keyboard::KEY_NUMPAD8;
	case 105: return Keyboard::KEY_NUMPAD9;
	case 106: return Keyboard::KEY_MULTIPLY;
	case 107: return Keyboard::KEY_ADD;
	case 109: return Keyboard::KEY_SUBTRACT;
	case 110: return Keyboard::KEY_DECIMAL;
	case 111: return Keyboard::KEY_DIVIDE;
	case 112: return Keyboard::KEY_F1;
	case 113: return Keyboard::KEY_F2;
	case 114: return Keyboard::KEY_F3;
	case 115: return Keyboard::KEY_F4;
	case 116: return Keyboard::KEY_F5;
	case 117: return Keyboard::KEY_F6;
	case 118: return Keyboard::KEY_F7;
	case 119: return Keyboard::KEY_F8;
	case 120: return Keyboard::KEY_F9;
	case 121: return Keyboard::KEY_F10;
	case 122: return Keyboard::KEY_F11;
	case 123: return Keyboard::KEY_F12;
	case 144: return Keyboard::KEY_NUMLOCK;

	case 162: return Keyboard::KEY_LContol;

	case 186: return Keyboard::KEY_SEMICOLON;
	case 187: return Keyboard::KEY_EQUALS;
	case 188: return Keyboard::KEY_COMMA;
		//case 189: return  "dash";
	case 190: return Keyboard::KEY_PERIOD;
	case 191: return Keyboard::KEY_SLASH;
	case 192: return Keyboard::KEY_GRAVE;
	case 219: return Keyboard::KEY_LBRACKET;
	case 220: return Keyboard::KEY_BACKSLASH;
	case 221: return Keyboard::KEY_RBRACKET;
		//case 222:  "single quote";

	default:
		//	printf("Unable to find Keybard::KEY for KeyCode %i   \n", keycode);
		return 0;
	}

}


LayeredPage* page1;
LayeredPage* page2;
CNeoItem* btn1;
bool waitToApply = false;


void addBindingEntry(LayeredPage* controllerPage, char* index, float left, float top, KeyboardAction action, char* actionStr) {
	CNeoKeyBinding* myBinding = new CNeoKeyBinding();

	//button down / button up is not supported, create a color mark
	if (CInput::activeMapping[action].Code < 0) {
		NeoMenu::setDefaultColor(108, 82, 75, 170);
	}

	CNeoItem* item = myBinding->create(index, left, top, index, 0.7, 0.7);
	item->setKeyBindingLabel(CInput::activeMapping[action].Code);
	item->setText1(actionStr);

	//button down / button up is not supported, disable selection
	if (CInput::activeMapping[action].Code >= 0) {
		item->setSelectable(true);
	}

	controllerPage->addItemToPage(item);

	if (CInput::activeMapping[action].Code < 0) {
		NeoMenu::setDefaultColor(200, 210, 175, 170);
	}

}

void CreateLayeredPage() {

	CNeoText* text = new CNeoText();
	CNeoLinkToPage* linkToPage = new CNeoLinkToPage();
	CNeoKeyBinding* myBinding = new CNeoKeyBinding();


	/*
		PAGE 1 -  Movements
	*/
	LayeredPage* controllerPage = new LayeredPage();
	page1 = controllerPage;
	controllerPage->setName("mymovementkeys");
	controllerPage->setParent("controls");

	controllerPage->setAlphaGhosting(false);
	controllerPage->setBoldGhosting(false);
	controllerPage->setWarping(false);

	NeoMenu::setDefaultColor(150, 30, 0, 170);
	controllerPage->addItemToPage(text->create("Coded by Sor3nt", 0.82, 0.02, 0.8, 0.8));

	controllerPage->addItemToPage(text->create("MOVMAP_T", 0.08, 0.124, 1.3, 1.4));
	NeoMenu::setDefaultColor(200, 210, 175, 170);

	//left
	addBindingEntry(controllerPage, "A_FWD_PC", 0.1, 0.25, KeyboardAction::ACTION_MOVE_FORWARD, "17");
	addBindingEntry(controllerPage, "A_BACK_PC", 0.1, 0.30, KeyboardAction::ACTION_MOVE_BACKWARDS, "18");
	addBindingEntry(controllerPage, "A_STRFL_PC", 0.1, 0.35, KeyboardAction::ACTION_MOVE_LEFT, "19");
	addBindingEntry(controllerPage, "A_STRFR_PC", 0.1, 0.40, KeyboardAction::ACTION_MOVE_RIGHT, "20");

	addBindingEntry(controllerPage, "A_PEEKL_PC", 0.1, 0.50, KeyboardAction::ACTION_PEEKL, "21");
	addBindingEntry(controllerPage, "A_PEEKR_PC", 0.1, 0.55, KeyboardAction::ACTION_PEEKR, "22");
	addBindingEntry(controllerPage, "A_LKBK_PC", 0.1, 0.60, KeyboardAction::ACTION_LOOKBACK, "2");
	addBindingEntry(controllerPage, "jump", 0.1, 0.65, KeyboardAction::ACTION_JUMP, "11");

	//right
	addBindingEntry(controllerPage, "SPRINT", 0.52, 0.25, KeyboardAction::ACTION_SPRINT, "27");
	addBindingEntry(controllerPage, "A_RUN_PC", 0.52, 0.30, KeyboardAction::ACTION_RUN, "29");
	addBindingEntry(controllerPage, "A_CRAWL_PC", 0.52, 0.35, KeyboardAction::ACTION_CRAWL, "45");
	addBindingEntry(controllerPage, "crouch", 0.52, 0.40, KeyboardAction::ACTION_CROUCH, "46");

	addBindingEntry(controllerPage, "A_SNEAK_PC", 0.52, 0.50, KeyboardAction::ACTION_SNEAK, "28");
	addBindingEntry(controllerPage, "A_WALLH_PC", 0.52, 0.55, KeyboardAction::ACTION_WALLSQUASH, "1");
	addBindingEntry(controllerPage, "A_CLIMB_PC", 0.52, 0.60, KeyboardAction::ACTION_CLIMB, "11");

	controllerPage->addItemToPage(linkToPage->create("controls", "Apply", 0.1, 0.90, 0));

	NeoMenu::insertPage(controllerPage);


	/*
		PAGE 2 - Actions
	*/
	controllerPage = new LayeredPage();
	page2 = controllerPage;
	controllerPage->setName("myactionkeys");
	controllerPage->setParent("controls");

	controllerPage->setAlphaGhosting(false);
	controllerPage->setBoldGhosting(false);
	controllerPage->setWarping(false);

	NeoMenu::setDefaultColor(150, 30, 0, 170);
	controllerPage->addItemToPage(text->create("Coded by Sor3nt", 0.82, 0.02, 0.8, 0.8));
	controllerPage->addItemToPage(text->create("ACTMAP_T", 0.08, 0.124, 1.3, 1.4));
	NeoMenu::setDefaultColor(200, 210, 175, 170);

	//left
	addBindingEntry(controllerPage, "Shoot/Hard hit", 0.1, 0.25, KeyboardAction::ACTION_FIRE1, "7");
	addBindingEntry(controllerPage, "A_RLOAD_PC", 0.1, 0.30, KeyboardAction::ACTION_RELOAD, "3");
	addBindingEntry(controllerPage, "A_FIREW_PC", 0.1, 0.35, KeyboardAction::ACTION_PUNCH, "9");
	addBindingEntry(controllerPage, "A_EXECU_PC", 0.1, 0.40, KeyboardAction::ACTION_EXECUTE, "53");

	addBindingEntry(controllerPage, "USE", 0.1, 0.50, KeyboardAction::ACTION_USE, "0");
	addBindingEntry(controllerPage, "A_INV1_PC", 0.1, 0.55, KeyboardAction::ACTION_INVENTORY_SLOT1, "39");
	addBindingEntry(controllerPage, "A_INV2_PC", 0.1, 0.60, KeyboardAction::ACTION_INVENTORY_SLOT2, "40");
	addBindingEntry(controllerPage, "A_INV3_PC", 0.1, 0.65, KeyboardAction::ACTION_INVENTORY_SLOT3, "41");
	addBindingEntry(controllerPage, "A_INV4_PC", 0.1, 0.70, KeyboardAction::ACTION_INVENTORY_SLOT4, "42");
	addBindingEntry(controllerPage, "A_MENU_PC", 0.1, 0.75, KeyboardAction::ACTION_MENU, "24");

	//right
	addBindingEntry(controllerPage, "A_LKBK_PC", 0.52, 0.25, KeyboardAction::ACTION_LOOKBACK, "2");
	addBindingEntry(controllerPage, "A_PICUP_PC", 0.52, 0.30, KeyboardAction::ACTION_PICKUP, "4");
	addBindingEntry(controllerPage, "A_DROP_PC", 0.52, 0.35, KeyboardAction::ACTION_DROP, "5");
	addBindingEntry(controllerPage, "A_BLOCK_PC", 0.52, 0.40, KeyboardAction::ACTION_BLOCK, "14");

	addBindingEntry(controllerPage, "A_ZMIN_PC", 0.52, 0.50, KeyboardAction::ACTION_ZOOMIN, "25");
	addBindingEntry(controllerPage, "A_ZMOUT_PC", 0.52, 0.55, KeyboardAction::ACTION_ZOOMOUT, "26");
	addBindingEntry(controllerPage, "A_GUNAM_PC", 0.52, 0.60, KeyboardAction::ACTION_ZOOMAIM, "31");
	addBindingEntry(controllerPage, "A_TARGT_PC", 0.52, 0.65, KeyboardAction::ACTION_LOCKON, "32");
	addBindingEntry(controllerPage, "A_FP_PC", 0.52, 0.70, KeyboardAction::ACTION_LOOKAROUND, "38");
	addBindingEntry(controllerPage, "monkey", 0.1, 0.75, KeyboardAction::ACTION_MONKEYAIM, "33");

	controllerPage->addItemToPage(linkToPage->create("controls", "Apply", 0.1, 0.90, 0));

	NeoMenu::insertPage(controllerPage);

}

CNeoItem* getSelectedItem() {

	if (NeoMenu::isCurrentPageSetTo("mymovementkeys")) {
		return page1->getSelectedItem();
	}

	if (NeoMenu::isCurrentPageSetTo("myactionkeys")) {
		return page2->getSelectedItem();
	}

	return 0;
}

int __fastcall CText__GetFromKey16(int* ptr, int keyCode, int key, int unk)
{

	char* name = converrtDirectInputCodeToName(keyCode);

	wchar_t* wName = MyTrans::GetWC(name);
	return (int)wName;

}


bool waitForInject = true;

void Init()
{

	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	//hook the KeyBinding Index translation so we can set our own text
	Memory::VP::InjectHook(0x5602B3, CText__GetFromKey16, PATCH_CALL);

	while (true)
	{

		/*
			Wait for main menu to appear
			Create the new NEO pages and update the current menu links
		*/
		if (waitForInject) {

			if (NeoMenu::isCurrentPageSetTo("start")) {

				CreateLayeredPage();

				//Update Links from original settings menu
				LayeredPage* controlsPage = NeoMenu::findPageByName("controls");
				controlsPage->getItem("movementkeys")->setText1("mymovementkeys");
				controlsPage->getItem("actionkeys")->setText1("myactionkeys");

				waitForInject = false;
			}

			Sleep(500);

			continue;
		}

		/*
			Wait for a key press
			Loads the selected element, update his Label and apply the keycode
		*/
		if (waitToApply) {

			int pressedKeyCode = getPressedKey();
			if (pressedKeyCode != -1) {

				//Update label text for new key
				CNeoItem* item = getSelectedItem();
				item->setKeyBindingLabel(convertKeyCodeToDirectInputCode(pressedKeyCode));

				//convert the char* to an integer (these is the action index)
				int keyIndex = atoi(item->getText1());

				//overwrite the current keycode with the new one
				CInput::activeMapping[keyIndex].Code = convertKeyCodeToDirectInputCode(pressedKeyCode);

				waitToApply = false;

				Sleep(500);
			}

			continue;
		}


		if (KeyHit(VK_RETURN)) {
			CNeoItem* item = getSelectedItem();

			if (item != 0 && item->isKeybinding()) {
				item->setKeyBindingLabel(-1);
				waitToApply = true;
			}

			Sleep(500);
			continue;
		}

		Sleep(200);

	}
}


extern "C"
{
	__declspec(dllexport) void InitializeASI()
	{
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Init), nullptr, 0, nullptr);

	}
}

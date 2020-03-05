
#include "MemoryMgr.h"
int hook_continue1 = 0x56F7FD;
int hook_continue2 = 0x56F84D;
int pressedKeyDown;
int pressedKeyUp;

int SwapKeyboardKey(int key) {


	if (key == 16) return 17; // q pressed, return w
	if (key == 15) return 16; // tab pressed, return q
	if (key == 45) return 31; // x pressed, return s


	return key;
}

void __declspec(naked) OnKeyDown() {

	_asm {
		//load the original vaule( esp+12 ) into eax 
		mov     eax, [esp + 12]

		//move eax to our local variable
		mov        pressedKeyDown, eax
	}

	pressedKeyDown = SwapKeyboardKey(*(int*)pressedKeyDown);

	_asm {
		//move the keycode (pressedKeyDown) to the original place back (esi)
		mov     esi, pressedKeyDown

		pushad
		popad

		// jump back 
		jmp hook_continue1
	}

}


void __declspec(naked) OnKeyUp() {

	_asm {
		//load the original vaule( esp+12 ) into eax 
		mov     eax, [esp + 12]

		//move eax to our local variable
		mov        pressedKeyUp, eax
	}

	pressedKeyUp = SwapKeyboardKey(*(int*)pressedKeyUp);

	_asm {
		//move the keycode (pressedKeyDown) to the original place back (edx)
		mov edx, pressedKeyUp

		pushad
		popad

		// jump back 
		jmp hook_continue2
	}
}
extern "C"
{
	__declspec(dllexport) void InitializeASI()
	{
		//inject into keyboard keyup and keydown to allow keyboard remapping
		Memory::VP::Patch<char>(0x56F7EE, 0xE9);
		Memory::VP::Patch<int>(0x56F7EE + 1, (int)OnKeyDown - ((int)0x56F7EE + 5));

		Memory::VP::Patch<char>(0x56F83E, 0xE9);
		Memory::VP::Patch<int>(0x56F83E + 1, (int)OnKeyUp - ((int)0x56F83E + 5));
	}
}



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

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <cstring>

#define WIN32_LEAN_AND_MEAN

// =========================
// Defaults / tracked indices
// =========================

// Map to store default keybindings
std::map<int, int> g_defaultBindings;

// Vectors to store pointers to the UI elements we create for resetting them
std::vector<CNeoItem*> g_movementBindingItems;
std::vector<CNeoItem*> g_actionBindingItems;

// Page sync flags
bool g_movementPageSynced = false;
bool g_actionPageSynced = false;

// Only persist the actions you expose in your custom pages.
// (These are the "extra keys" that the vanilla menu often doesn't save reliably.)
static const int kTrackedActions[] = {
    0, 1, 2, 3, 4, 5, 7, 9, 11, 12, 14,
    17, 18, 19, 20, 21, 22,
    24, 25, 26,
    28, 29,
    31, 32,
    38,
    39, 40, 41, 42,
    45, 46,
    48, // TOGGLE_TORCH (custom handled)
    53
};
static const int kTrackedActionsCount = (int)(sizeof(kTrackedActions) / sizeof(kTrackedActions[0]));

// Saved codes (ini) for tracked actions
static bool g_savedValid[60] = {};
static int  g_savedCode[60] = {};

// Ini path beside the ASI
static std::string g_cfgPath;
static HMODULE g_hThisModule = nullptr;

// =========================
// Input helpers
// =========================
static inline bool KeyHit(unsigned int keyCode) { return (GetAsyncKeyState(keyCode) & 0x8000) != 0; }
static inline bool ButtonHit(unsigned int keyCode) { return (GetAsyncKeyState(keyCode) & 0x8000) != 0; }

// calling
template <unsigned int address, typename... Args>
static void Call(Args... args) {
    reinterpret_cast<void(__cdecl*)(Args...)>(address)(args...);
}

template<typename ...Args>
static void WriteDebug(int line, const char* msg, Args ...args)
{
    Call<0x53D8D0, int, const char*, Args...>(line, msg, args...);
}

// =========================
// Ini helpers
// =========================
static inline std::string Trim(std::string s)
{
    auto notSpace = [](unsigned char c) { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    return s;
}

static std::string ToUpperASCII(std::string s)
{
    for (char& ch : s) ch = (char)std::toupper((unsigned char)ch);
    return s;
}

static std::string StripInlineComment(std::string s)
{
    size_t p = s.find_first_of(";#");
    if (p != std::string::npos) s = s.substr(0, p);
    return Trim(s);
}

static std::string NormalizeToken(std::string s)
{
    s = Trim(s);
    if (s.size() >= 2 && ((s.front() == '"' && s.back() == '"') || (s.front() == '\'' && s.back() == '\'')))
        s = s.substr(1, s.size() - 2);

    s = ToUpperASCII(s);

    std::string out;
    out.reserve(s.size());

    auto push_us = [&]() {
        if (!out.empty() && out.back() != '_') out.push_back('_');
        };

    for (unsigned char c : s)
    {
        if (std::isalnum(c)) out.push_back((char)c);
        else if (c == ' ' || c == '-' || c == '\t') push_us();
        else if (c == '_') push_us();
        // ignore other punctuation
    }

    while (!out.empty() && out.back() == '_') out.pop_back();
    return out;
}

static bool IsTrackedAction(int idx)
{
    for (int i = 0; i < kTrackedActionsCount; ++i)
        if (kTrackedActions[i] == idx) return true;
    return false;
}

static std::string BuildIniPathBesideAsi()
{
    char modPath[MAX_PATH]{};

    HMODULE hm = g_hThisModule;
    if (!hm)
        hm = GetModuleHandleA(nullptr);

    GetModuleFileNameA(hm, modPath, MAX_PATH);

    std::string p(modPath);

    // split dir + filename
    size_t slash = p.find_last_of("\\/");
    std::string dir = (slash != std::string::npos) ? p.substr(0, slash + 1) : std::string();
    std::string file = (slash != std::string::npos) ? p.substr(slash + 1) : p;

    // remove extension
    size_t dot = file.find_last_of('.');
    if (dot != std::string::npos)
        file = file.substr(0, dot);

    // <asi_base>.ini
    return dir + file + ".ini";
}

static bool FileExists(const std::string& path)
{
    DWORD attr = GetFileAttributesA(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES) && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

static void SeedSavedFromCurrentMappingIfMissing()
{
    for (int i = 0; i < kTrackedActionsCount; ++i)
    {
        int idx = kTrackedActions[i];
        if (idx < 0 || idx >= 60) continue;

        if (!g_savedValid[idx])
        {
            g_savedValid[idx] = true;
            g_savedCode[idx] = CInput::activeMapping[idx].Code;
        }
    }
}

// =========================
// Names for annotations
// =========================
static const char* ActionIndexToName(int idx)
{
    switch (idx)
    {
        // Movement page
    case 17: return "MOVE_FORWARD";
    case 18: return "MOVE_BACKWARDS";
    case 19: return "MOVE_LEFT_STRAFE";
    case 20: return "MOVE_RIGHT_STRAFE";
    case 21: return "PEEK_LEFT";
    case 22: return "PEEK_RIGHT";
    case 2:  return "LOOKBACK";
    case 12: return "GRAPPLE";
    case 29: return "RUN";
    case 45: return "CRAWL";
    case 46: return "CROUCH";
    case 28: return "SNEAK";
    case 1:  return "WALLSQUASH";
    case 11: return "CLIMB";

        // Action page
    case 7:  return "FIRE_HARD_PUNCH";
    case 3:  return "RELOAD";
    case 9:  return "PUNCH_ATTACK";
    case 53: return "EXECUTE";
    case 0:  return "USE";
    case 39: return "INVENTORY_SLOT_1";
    case 40: return "INVENTORY_SLOT_2";
    case 41: return "INVENTORY_SLOT_3";
    case 42: return "INVENTORY_SLOT_4";
    case 24: return "MENU";
    case 4:  return "PICKUP";
    case 5:  return "DROP";
    case 14: return "BLOCK";
    case 25: return "ZOOM_IN";
    case 26: return "ZOOM_OUT";
    case 31: return "ZOOM_AIM";
    case 32: return "LOCK_ON_TARGET";
    case 38: return "LOOKAROUND_FP";
    case 48: return "TOGGLE_TORCH";
    default: return "UNKNOWN_ACTION";
    }
}

// =========================
// Key name conversion (for UI + ini tokens)
// =========================
static const char* converrtDirectInputCodeToName(int keycode)
{
    switch (keycode) {
    case 0:   return "*No Keybind Set*";
    case -1:  return "PRESS A KEY";
    case -32: return "MOUSE RIGHT";
    case -34: return "MOUSE LEFT";
    case -28: return "MOUSE X1";
    case -29: return "MOUSE X2";

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
    case 0x1D: return "LCONTROL";
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
    case 0x9C: return "NUMPADENTER";
    case 0x9D: return "RCONTROL";
    default:
        return "unsupported";
    }
}

static std::string KeyCodeToIniToken(int code)
{
    if (code == 0)  return "NONE";
    if (code == -1) return "PRESS_A_KEY";

    // digits are ambiguous in text (your name function returns "1".."0"),
    // so write them as KEY_1..KEY_0.
    if (code >= 0x02 && code <= 0x0A) {
        int d = code - 0x01; // 0x02->1 ... 0x0A->9
        return std::string("KEY_") + char('0' + d);
    }
    if (code == 0x0B) return "KEY_0";

    const char* raw = converrtDirectInputCodeToName(code);
    if (!raw || std::strcmp(raw, "unsupported") == 0)
        return std::to_string(code);

    std::string tok = NormalizeToken(raw);

    if (tok == "NO_KEYBIND_SET") return "NONE";
    if (tok == "PRESS_A_KEY")    return "PRESS_A_KEY";
    if (tok == "MOUSE_LEFT")     return "MOUSE_LEFT";
    if (tok == "MOUSE_RIGHT")    return "MOUSE_RIGHT";
    if (tok == "MOUSE_X1")       return "MOUSE_X1";
    if (tok == "MOUSE_X2")       return "MOUSE_X2";

    // keep your scancode names as-is (LCONTROL, LMENU, etc.)
    return tok;
}

static bool IniTokenToKeyCode(const std::string& tokenIn, int& outCode)
{
    std::string t = StripInlineComment(tokenIn);
    if (t.empty()) return false;

    // numeric (decimal or hex like 0x38) -> accept as-is for backward compatibility
    try {
        size_t used = 0;
        int v = std::stoi(t, &used, 0);
        if (used == t.size()) {
            outCode = v;
            return true;
        }
    }
    catch (...) {
        // not numeric -> continue
    }

    std::string key = NormalizeToken(t);

    // aliases
    if (key == "NO_KEYBIND_SET") key = "NONE";
    if (key == "PRESSAKEY")      key = "PRESS_A_KEY";
    if (key == "LALT" || key == "LEFTALT") key = "LMENU";
    if (key == "CAPSLOCK")       key = "CAPITAL";
    if (key == "ENTER")          key = "RETURN";

    // specials
    if (key == "NONE") { outCode = 0; return true; }
    if (key == "PRESS_A_KEY") { outCode = -1; return true; }

    // mouse
    if (key == "MOUSE_LEFT") { outCode = -34; return true; }
    if (key == "MOUSE_RIGHT") { outCode = -32; return true; }
    if (key == "MOUSE_X1") { outCode = -28; return true; }
    if (key == "MOUSE_X2") { outCode = -29; return true; }

    // digit keys (top row)
    if (key == "KEY_1") { outCode = 0x02; return true; }
    if (key == "KEY_2") { outCode = 0x03; return true; }
    if (key == "KEY_3") { outCode = 0x04; return true; }
    if (key == "KEY_4") { outCode = 0x05; return true; }
    if (key == "KEY_5") { outCode = 0x06; return true; }
    if (key == "KEY_6") { outCode = 0x07; return true; }
    if (key == "KEY_7") { outCode = 0x08; return true; }
    if (key == "KEY_8") { outCode = 0x09; return true; }
    if (key == "KEY_9") { outCode = 0x0A; return true; }
    if (key == "KEY_0") { outCode = 0x0B; return true; }

    // direct mapping names -> scancodes (as per your table)
    static const std::map<std::string, int> k = {
        {"ESCAPE",0x01},
        {"MINUS",0x0C}, {"EQUALS",0x0D}, {"BACK",0x0E}, {"BACKSPACE",0x0E}, {"TAB",0x0F},
        {"Q",0x10},{"W",0x11},{"E",0x12},{"R",0x13},{"T",0x14},{"Y",0x15},{"U",0x16},{"I",0x17},{"O",0x18},{"P",0x19},
        {"LBRACKET",0x1A},{"RBRACKET",0x1B},{"RETURN",0x1C},{"LCONTROL",0x1D},
        {"A",0x1E},{"S",0x1F},{"D",0x20},{"F",0x21},{"G",0x22},{"H",0x23},{"J",0x24},{"K",0x25},{"L",0x26},
        {"SEMICOLON",0x27},{"APOSTROPHE",0x28},{"GRAVE",0x29},
        {"LSHIFT",0x2A},{"BACKSLASH",0x2B},
        {"Z",0x2C},{"X",0x2D},{"C",0x2E},{"V",0x2F},{"B",0x30},{"N",0x31},{"M",0x32},
        {"COMMA",0x33},{"PERIOD",0x34},{"SLASH",0x35},{"RSHIFT",0x36},
        {"MULTIPLY",0x37},{"LMENU",0x38},{"SPACE",0x39},{"CAPITAL",0x3A},
        {"F1",0x3B},{"F2",0x3C},{"F3",0x3D},{"F4",0x3E},{"F5",0x3F},{"F6",0x40},{"F7",0x41},{"F8",0x42},{"F9",0x43},{"F10",0x44},
        {"NUMLOCK",0x45},{"SCROLL",0x46},
        {"NUMPAD7",0x47},{"NUMPAD8",0x48},{"NUMPAD9",0x49},
        {"SUBTRACT",0x4A},{"NUMPAD4",0x4B},{"NUMPAD5",0x4C},{"NUMPAD6",0x4D},
        {"ADD",0x4E},{"NUMPAD1",0x4F},{"NUMPAD2",0x50},{"NUMPAD3",0x51},{"NUMPAD0",0x52},{"DECIMAL",0x53},
        {"F11",0x57},{"F12",0x58},
        {"F13",0x64},{"F14",0x65},{"F15",0x66},
        {"NUMPADENTER",0x9C},{"RCONTROL",0x9D},
    };

    auto it = k.find(key);
    if (it != k.end()) {
        outCode = it->second;
        return true;
    }

    return false;
}

static void WriteIniReferenceBlock(std::ofstream& out)
{
    out << "# ------------------------------\n";
    out << "# Action indices (INI left side)\n";
    out << "# ------------------------------\n";
    out << "# Movement page\n";
    out << "# 17 = MOVE FORWARD\n";
    out << "# 18 = MOVE BACKWARDS\n";
    out << "# 19 = MOVE LEFT (strafe)\n";
    out << "# 20 = MOVE RIGHT (strafe)\n";
    out << "# 21 = PEEK LEFT\n";
    out << "# 22 = PEEK RIGHT\n";
    out << "# 2  = LOOKBACK\n";
    out << "# 12 = GRAPPLE\n";
    out << "# 29 = RUN\n";
    out << "# 45 = CRAWL\n";
    out << "# 46 = CROUCH\n";
    out << "# 28 = SNEAK\n";
    out << "# 1  = WALLSQUASH\n";
    out << "# 11 = CLIMB\n";
    out << "#\n";
    out << "# Action page\n";
    out << "# 7  = FIRE / hard punch\n";
    out << "# 3  = RELOAD\n";
    out << "# 9  = PUNCH / attack\n";
    out << "# 53 = EXECUTE\n";
    out << "# 0  = USE\n";
    out << "# 39 = INVENTORY SLOT 1\n";
    out << "# 40 = INVENTORY SLOT 2\n";
    out << "# 41 = INVENTORY SLOT 3\n";
    out << "# 42 = INVENTORY SLOT 4\n";
    out << "# 24 = MENU\n";
    out << "# 4  = PICKUP\n";
    out << "# 5  = DROP\n";
    out << "# 14 = BLOCK\n";
    out << "# 25 = ZOOM IN\n";
    out << "# 26 = ZOOM OUT\n";
    out << "# 31 = ZOOM AIM\n";
    out << "# 32 = LOCK ON / TARGET\n";
    out << "# 38 = LOOKAROUND (FP)\n";
    out << "# 48 = TOGGLE TORCH (custom)\n";
    out << "#\n";

    out << "# -----------------------------------\n";
    out << "# Mappable key/button IDs (right side)\n";
    out << "# -----------------------------------\n";
    out << "# Special\n";
    out << "# NONE = 0\n";
    out << "# PRESS_A_KEY = -1\n";
    out << "#\n";
    out << "# Mouse (negative codes)\n";
    out << "# MOUSE_LEFT  = -34\n";
    out << "# MOUSE_RIGHT = -32\n";
    out << "# MOUSE_X1    = -28  (Mouse Button 4)\n";
    out << "# MOUSE_X2    = -29  (Mouse Button 5)\n";
    out << "#\n";
    out << "# Keyboard (DirectInput-style scan codes; shown as hex -> decimal)\n";
    out << "# 0x01->1   ESCAPE\n";
    out << "# 0x02->2   KEY_1\n";
    out << "# 0x03->3   KEY_2\n";
    out << "# 0x04->4   KEY_3\n";
    out << "# 0x05->5   KEY_4\n";
    out << "# 0x06->6   KEY_5\n";
    out << "# 0x07->7   KEY_6\n";
    out << "# 0x08->8   KEY_7\n";
    out << "# 0x09->9   KEY_8\n";
    out << "# 0x0A->10  KEY_9\n";
    out << "# 0x0B->11  KEY_0\n";
    out << "# 0x0C->12  MINUS\n";
    out << "# 0x0D->13  EQUALS\n";
    out << "# 0x0E->14  BACK (Backspace)\n";
    out << "# 0x0F->15  TAB\n";
    out << "# 0x10->16  Q\n";
    out << "# 0x11->17  W\n";
    out << "# 0x12->18  E\n";
    out << "# 0x13->19  R\n";
    out << "# 0x14->20  T\n";
    out << "# 0x15->21  Y\n";
    out << "# 0x16->22  U\n";
    out << "# 0x17->23  I\n";
    out << "# 0x18->24  O\n";
    out << "# 0x19->25  P\n";
    out << "# 0x1A->26  LBRACKET\n";
    out << "# 0x1B->27  RBRACKET\n";
    out << "# 0x1C->28  RETURN (Enter)\n";
    out << "# 0x1D->29  LCONTROL\n";
    out << "# 0x1E->30  A\n";
    out << "# 0x1F->31  S\n";
    out << "# 0x20->32  D\n";
    out << "# 0x21->33  F\n";
    out << "# 0x22->34  G\n";
    out << "# 0x23->35  H\n";
    out << "# 0x24->36  J\n";
    out << "# 0x25->37  K\n";
    out << "# 0x26->38  L\n";
    out << "# 0x27->39  SEMICOLON\n";
    out << "# 0x28->40  APOSTROPHE\n";
    out << "# 0x29->41  GRAVE\n";
    out << "# 0x2A->42  LSHIFT\n";
    out << "# 0x2B->43  BACKSLASH\n";
    out << "# 0x2C->44  Z\n";
    out << "# 0x2D->45  X\n";
    out << "# 0x2E->46  C\n";
    out << "# 0x2F->47  V\n";
    out << "# 0x30->48  B\n";
    out << "# 0x31->49  N\n";
    out << "# 0x32->50  M\n";
    out << "# 0x33->51  COMMA\n";
    out << "# 0x34->52  PERIOD\n";
    out << "# 0x35->53  SLASH\n";
    out << "# 0x36->54  RSHIFT\n";
    out << "# 0x37->55  MULTIPLY (numpad *)\n";
    out << "# 0x38->56  LMENU (Left Alt)\n";
    out << "# 0x39->57  SPACE\n";
    out << "# 0x3A->58  CAPITAL (Caps Lock)\n";
    out << "# 0x3B->59  F1\n";
    out << "# 0x3C->60  F2\n";
    out << "# 0x3D->61  F3\n";
    out << "# 0x3E->62  F4\n";
    out << "# 0x3F->63  F5\n";
    out << "# 0x40->64  F6\n";
    out << "# 0x41->65  F7\n";
    out << "# 0x42->66  F8\n";
    out << "# 0x43->67  F9\n";
    out << "# 0x44->68  F10\n";
    out << "# 0x45->69  NUMLOCK\n";
    out << "# 0x46->70  SCROLL\n";
    out << "# 0x47->71  NUMPAD7\n";
    out << "# 0x48->72  NUMPAD8\n";
    out << "# 0x49->73  NUMPAD9\n";
    out << "# 0x4A->74  SUBTRACT (numpad -)\n";
    out << "# 0x4B->75  NUMPAD4\n";
    out << "# 0x4C->76  NUMPAD5\n";
    out << "# 0x4D->77  NUMPAD6\n";
    out << "# 0x4E->78  ADD (numpad +)\n";
    out << "# 0x4F->79  NUMPAD1\n";
    out << "# 0x50->80  NUMPAD2\n";
    out << "# 0x51->81  NUMPAD3\n";
    out << "# 0x52->82  NUMPAD0\n";
    out << "# 0x53->83  DECIMAL (numpad .)\n";
    out << "# 0x57->87  F11\n";
    out << "# 0x58->88  F12\n";
    out << "# 0x64->100 F13\n";
    out << "# 0x65->101 F14\n";
    out << "# 0x66->102 F15\n";
    out << "# 0x9C->156 NUMPADENTER\n";
    out << "# 0x9D->157 RCONTROL\n";
    out << "#\n";
}

// =========================
// Save/Load ini
// =========================
static void SaveExtraBindsToFile()
{
    if (g_cfgPath.empty())
        g_cfgPath = BuildIniPathBesideAsi();

    std::ofstream out(g_cfgPath, std::ios::trunc);
    if (!out.is_open())
        return;

    out << "# MH2 extra keybinds (custom pages)\n";
    out << "# format: index=KEY_NAME   (numbers still accepted too)\n";
    out << "# example: 29=MOUSE_X1\n";
    out << "# example: 29=-28\n\n";

    WriteIniReferenceBlock(out);

    out << "# ------------------------------\n";
    out << "# Current saved bindings\n";
    out << "# ------------------------------\n";

    for (int i = 0; i < kTrackedActionsCount; ++i)
    {
        int idx = kTrackedActions[i];
        if (idx < 0 || idx >= 60) continue;

        int code = CInput::activeMapping[idx].Code;

        std::string token = KeyCodeToIniToken(code);
        out << idx << "=" << token << " ; " << ActionIndexToName(idx) << "\n";

        g_savedValid[idx] = true;
        g_savedCode[idx] = code;
    }
}

static void LoadExtraBindsFromFile()
{
    if (g_cfgPath.empty())
        g_cfgPath = BuildIniPathBesideAsi();

    std::ifstream in(g_cfgPath);
    if (!in.is_open())
        return;

    std::string line;
    while (std::getline(in, line))
    {
        line = Trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';')
            continue;

        size_t eq = line.find('=');
        if (eq == std::string::npos)
            continue;

        std::string a = Trim(line.substr(0, eq));
        std::string b = Trim(line.substr(eq + 1));
        b = StripInlineComment(b);

        try {
            int idx = std::stoi(a);
            if (idx < 0 || idx >= 60) continue;
            if (!IsTrackedAction(idx)) continue;

            int code = 0;
            if (!IniTokenToKeyCode(b, code))
                continue;

            g_savedValid[idx] = true;
            g_savedCode[idx] = code;
        }
        catch (...) {
            // ignore bad lines
        }
    }
}

static void ApplySavedExtraBinds()
{
    for (int i = 0; i < kTrackedActionsCount; ++i)
    {
        int idx = kTrackedActions[i];
        if (idx < 0 || idx >= 60) continue;

        if (g_savedValid[idx])
            CInput::activeMapping[idx].Code = g_savedCode[idx];
    }
}

// =========================
// Key capture
// =========================
static bool  g_ignoreReturnUntilUp = false;

static int getPressedKey()
{
    int max = 254;
    do {
        if (KeyHit(max)) {
            if (g_ignoreReturnUntilUp && max == VK_RETURN)
                continue;
            return max;
        }
        if (ButtonHit(max)) {
            if (g_ignoreReturnUntilUp && max == VK_RETURN)
                continue;
            return max;
        }
    } while (max--);

    return -1;
}

static int convertKeyCodeToDirectInputCode(int keycode)
{
    switch (keycode) {
    case VK_LBUTTON:   return Keyboard::KEY_MOUSE_LEFT;
    case VK_RBUTTON:   return Keyboard::KEY_MOUSE_RIGHT;
    case VK_XBUTTON1:  return -28; // extra mouse button 1
    case VK_XBUTTON2:  return -29; // extra mouse button 2

    case VK_BACK:      return Keyboard::KEY_BACK;
    case VK_TAB:       return Keyboard::KEY_TAB;
    case VK_RETURN:    return Keyboard::KEY_RETURN;
    case VK_SHIFT:     return Keyboard::KEY_LSHIFT;
    case VK_CONTROL:   return Keyboard::KEY_LContol;
    case VK_PAUSE:     return Keyboard::KEY_PAUSE;
    case VK_ESCAPE:    return Keyboard::KEY_ESCAPE;
    case VK_SPACE:     return Keyboard::KEY_SPACE;
    case VK_END:       return Keyboard::KEY_END;
    case VK_HOME:      return Keyboard::KEY_HOME;
    case VK_LEFT:      return Keyboard::KEY_LEFT;
    case VK_UP:        return Keyboard::KEY_UP;
    case VK_RIGHT:     return Keyboard::KEY_RIGHT;
    case VK_DOWN:      return Keyboard::KEY_DOWN;
    case VK_INSERT:    return Keyboard::KEY_INSERT;
    case VK_DELETE:    return Keyboard::KEY_DELETE;

        // 0-9
    case '0': return Keyboard::KEY_0;
    case '1': return Keyboard::KEY_1;
    case '2': return Keyboard::KEY_2;
    case '3': return Keyboard::KEY_3;
    case '4': return Keyboard::KEY_4;
    case '5': return Keyboard::KEY_5;
    case '6': return Keyboard::KEY_6;
    case '7': return Keyboard::KEY_7;
    case '8': return Keyboard::KEY_8;
    case '9': return Keyboard::KEY_9;

        // A-Z
    case 'A': return Keyboard::KEY_A;
    case 'B': return Keyboard::KEY_B;
    case 'C': return Keyboard::KEY_C;
    case 'D': return Keyboard::KEY_D;
    case 'E': return Keyboard::KEY_E;
    case 'F': return Keyboard::KEY_F;
    case 'G': return Keyboard::KEY_G;
    case 'H': return Keyboard::KEY_H;
    case 'I': return Keyboard::KEY_I;
    case 'J': return Keyboard::KEY_J;
    case 'K': return Keyboard::KEY_K;
    case 'L': return Keyboard::KEY_L;
    case 'M': return Keyboard::KEY_M;
    case 'N': return Keyboard::KEY_N;
    case 'O': return Keyboard::KEY_O;
    case 'P': return Keyboard::KEY_P;
    case 'Q': return Keyboard::KEY_Q;
    case 'R': return Keyboard::KEY_R;
    case 'S': return Keyboard::KEY_S;
    case 'T': return Keyboard::KEY_T;
    case 'U': return Keyboard::KEY_U;
    case 'V': return Keyboard::KEY_V;
    case 'W': return Keyboard::KEY_W;
    case 'X': return Keyboard::KEY_X;
    case 'Y': return Keyboard::KEY_Y;
    case 'Z': return Keyboard::KEY_Z;

        // Numpad
    case VK_NUMPAD0:   return Keyboard::KEY_NUMPAD0;
    case VK_NUMPAD1:   return Keyboard::KEY_NUMPAD1;
    case VK_NUMPAD2:   return Keyboard::KEY_NUMPAD2;
    case VK_NUMPAD3:   return Keyboard::KEY_NUMPAD3;
    case VK_NUMPAD4:   return Keyboard::KEY_NUMPAD4;
    case VK_NUMPAD5:   return Keyboard::KEY_NUMPAD5;
    case VK_NUMPAD6:   return Keyboard::KEY_NUMPAD6;
    case VK_NUMPAD7:   return Keyboard::KEY_NUMPAD7;
    case VK_NUMPAD8:   return Keyboard::KEY_NUMPAD8;
    case VK_NUMPAD9:   return Keyboard::KEY_NUMPAD9;
    case VK_MULTIPLY:  return Keyboard::KEY_MULTIPLY;
    case VK_ADD:       return Keyboard::KEY_ADD;
    case VK_SUBTRACT:  return Keyboard::KEY_SUBTRACT;
    case VK_DECIMAL:   return Keyboard::KEY_DECIMAL;
    case VK_DIVIDE:    return Keyboard::KEY_DIVIDE;

        // F1-F12
    case VK_F1:  return Keyboard::KEY_F1;
    case VK_F2:  return Keyboard::KEY_F2;
    case VK_F3:  return Keyboard::KEY_F3;
    case VK_F4:  return Keyboard::KEY_F4;
    case VK_F5:  return Keyboard::KEY_F5;
    case VK_F6:  return Keyboard::KEY_F6;
    case VK_F7:  return Keyboard::KEY_F7;
    case VK_F8:  return Keyboard::KEY_F8;
    case VK_F9:  return Keyboard::KEY_F9;
    case VK_F10: return Keyboard::KEY_F10;
    case VK_F11: return Keyboard::KEY_F11;
    case VK_F12: return Keyboard::KEY_F12;

        // F13-F15 (match your scancode labels table)
    case VK_F13: return 0x64;
    case VK_F14: return 0x65;
    case VK_F15: return 0x66;

        // OEM / punctuation
    case VK_OEM_1:      return Keyboard::KEY_SEMICOLON;
    case VK_OEM_PLUS:   return Keyboard::KEY_EQUALS;
    case VK_OEM_COMMA:  return Keyboard::KEY_COMMA;
    case VK_OEM_PERIOD: return Keyboard::KEY_PERIOD;
    case VK_OEM_2:      return Keyboard::KEY_SLASH;
    case VK_OEM_3:      return Keyboard::KEY_GRAVE;
    case VK_OEM_4:      return Keyboard::KEY_LBRACKET;
    case VK_OEM_5:      return Keyboard::KEY_BACKSLASH;
    case VK_OEM_6:      return Keyboard::KEY_RBRACKET;

    default:
        return 0;
    }
}

// =========================
// Torch toggle (custom action 48)
// =========================
static const int kAction_ToggleTorch = 48;

// Filled addresses (from your post)
static constexpr uintptr_t ADDR_ms_pPlayer = 0x00789490;          // *(CPed**)ms_pPlayer
static constexpr uintptr_t ADDR_GetUsedCollectable = 0x004F66C6;  // __fastcall, uses EAX as index
static constexpr uintptr_t ADDR_SwitchTorchOn = 0x005271F1;
static constexpr uintptr_t ADDR_SwitchTorchOff = 0x00527201;

static inline void* GetPlayerPed()
{
    return *(void**)ADDR_ms_pPlayer;
}

static inline bool SafeReadInt(uintptr_t addr, int& out)
{
#if defined(_M_IX86)
    __try {
        out = *(int*)addr;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        out = 0;
        return false;
    }
#else
    out = *(int*)addr;
    return true;
#endif
}

static inline bool IsTorchCapableCollectable(void* collectable)
{
    if (!collectable) return false;

#if defined(_M_IX86)
    __try {
        uintptr_t def = *(uintptr_t*)((uintptr_t)collectable + 0x6C);
        if (!def) return false;
        return (*(int*)(def + 0x2B0) != 0);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
#else
    uintptr_t def = *(uintptr_t*)((uintptr_t)collectable + 0x6C);
    if (!def) return false;
    return (*(int*)(def + 0x2B0) != 0);
#endif
}

#if defined(_M_IX86)
static __forceinline void* Call_GetUsedCollectable_WithIndex(void* ped, int idx)
{
    void* ret = nullptr;
    __asm
    {
        mov  ecx, ped
        mov  eax, idx
        call ADDR_GetUsedCollectable
        mov  ret, eax
    }
    return ret;
}
#else
static __forceinline void* Call_GetUsedCollectable_WithIndex(void* ped, int idx)
{
    (void)ped; (void)idx;
    return nullptr;
}
#endif

static void* FindPlayerTorchWeapon(void* playerPed)
{
    if (!playerPed) return nullptr;

    // inventory-ish pointer is at +0x1BC (seen in GetUsedCollectable)
    uintptr_t inv = 0;
#if defined(_M_IX86)
    __try {
        inv = *(uintptr_t*)((uintptr_t)playerPed + 0x1BC);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        inv = 0;
    }
#else
    inv = *(uintptr_t*)((uintptr_t)playerPed + 0x1BC);
#endif
    if (!inv) return nullptr;

    // Best-guess: "current used index" is usually near start of the inventory object.
    // Try a few common offsets and only accept a result that is torch-capable.
    static const int kIdxOffsets[] = { 0x04, 0x10, 0x14, 0x18, 0x1C };

    for (int off : kIdxOffsets)
    {
        int idx = -1;
        if (!SafeReadInt(inv + (uintptr_t)off, idx))
            continue;

        if (idx < -1 || idx > 64)
            continue;

        void* w = Call_GetUsedCollectable_WithIndex(playerPed, idx);
        if (IsTorchCapableCollectable(w))
            return w;
    }

    // Last resort: try a small range of indices (avoid scanning the whole array)
    for (int idx = 0; idx < 16; ++idx)
    {
        void* w = Call_GetUsedCollectable_WithIndex(playerPed, idx);
        if (IsTorchCapableCollectable(w))
            return w;
    }

    return nullptr;
}

static bool DirectInputCodeToVK(int diCode, UINT& outVK)
{
    if (diCode == -34) { outVK = VK_LBUTTON; return true; }
    if (diCode == -32) { outVK = VK_RBUTTON; return true; }
    if (diCode == -28) { outVK = VK_XBUTTON1; return true; }
    if (diCode == -29) { outVK = VK_XBUTTON2; return true; }

    if (diCode == 0 || diCode == -1) return false;

    switch (diCode)
    {
    case 0x01: outVK = VK_ESCAPE; return true;
    case 0x0E: outVK = VK_BACK; return true;
    case 0x0F: outVK = VK_TAB; return true;
    case 0x1C: outVK = VK_RETURN; return true;
    case 0x39: outVK = VK_SPACE; return true;

    case 0x02: outVK = '1'; return true;
    case 0x03: outVK = '2'; return true;
    case 0x04: outVK = '3'; return true;
    case 0x05: outVK = '4'; return true;
    case 0x06: outVK = '5'; return true;
    case 0x07: outVK = '6'; return true;
    case 0x08: outVK = '7'; return true;
    case 0x09: outVK = '8'; return true;
    case 0x0A: outVK = '9'; return true;
    case 0x0B: outVK = '0'; return true;

    case 0x10: outVK = 'Q'; return true;
    case 0x11: outVK = 'W'; return true;
    case 0x12: outVK = 'E'; return true;
    case 0x13: outVK = 'R'; return true;
    case 0x14: outVK = 'T'; return true;
    case 0x15: outVK = 'Y'; return true;
    case 0x16: outVK = 'U'; return true;
    case 0x17: outVK = 'I'; return true;
    case 0x18: outVK = 'O'; return true;
    case 0x19: outVK = 'P'; return true;

    case 0x1E: outVK = 'A'; return true;
    case 0x1F: outVK = 'S'; return true;
    case 0x20: outVK = 'D'; return true;
    case 0x21: outVK = 'F'; return true;
    case 0x22: outVK = 'G'; return true;
    case 0x23: outVK = 'H'; return true;
    case 0x24: outVK = 'J'; return true;
    case 0x25: outVK = 'K'; return true;
    case 0x26: outVK = 'L'; return true;

    case 0x2C: outVK = 'Z'; return true;
    case 0x2D: outVK = 'X'; return true;
    case 0x2E: outVK = 'C'; return true;
    case 0x2F: outVK = 'V'; return true;
    case 0x30: outVK = 'B'; return true;
    case 0x31: outVK = 'N'; return true;
    case 0x32: outVK = 'M'; return true;

    case 0x3B: outVK = VK_F1; return true;
    case 0x3C: outVK = VK_F2; return true;
    case 0x3D: outVK = VK_F3; return true;
    case 0x3E: outVK = VK_F4; return true;
    case 0x3F: outVK = VK_F5; return true;
    case 0x40: outVK = VK_F6; return true;
    case 0x41: outVK = VK_F7; return true;
    case 0x42: outVK = VK_F8; return true;
    case 0x43: outVK = VK_F9; return true;
    case 0x44: outVK = VK_F10; return true;
    case 0x57: outVK = VK_F11; return true;
    case 0x58: outVK = VK_F12; return true;
    case 0x64: outVK = VK_F13; return true;
    case 0x65: outVK = VK_F14; return true;
    case 0x66: outVK = VK_F15; return true;

    default:
        return false;
    }
}

static void SwitchTorchState(void* weapon, bool on)
{
    if (!weapon) return;

#if defined(_M_IX86)
    __try {
        if (on) {
            ((void(__thiscall*)(void*))ADDR_SwitchTorchOn)(weapon);
        }
        else {
            ((void(__thiscall*)(void*))ADDR_SwitchTorchOff)(weapon);
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        // ignore
    }
#else
    if (on) ((void(__thiscall*)(void*))ADDR_SwitchTorchOn)(weapon);
    else    ((void(__thiscall*)(void*))ADDR_SwitchTorchOff)(weapon);
#endif
}

static void HandleTorchToggleHotkey(bool allowRun, bool isCapturingKey)
{
    // run only in-game (not on our keybind pages), and never while capturing
    if (!allowRun || isCapturingKey)
        return;

    static bool  s_prevDown = false;
    static bool  s_torchOn = false;
    static void* s_lastWeapon = nullptr;
    static DWORD s_lastApply = 0;

    int diCode = CInput::activeMapping[kAction_ToggleTorch].Code;

    UINT vk = 0;
    if (!DirectInputCodeToVK(diCode, vk))
    {
        s_prevDown = false;
        return;
    }

    bool down = (GetAsyncKeyState((int)vk) & 0x8000) != 0;
    bool pressed = down && !s_prevDown;
    s_prevDown = down;

    void* player = GetPlayerPed();
    if (!player)
        return;

    void* weapon = FindPlayerTorchWeapon(player);
    if (weapon != s_lastWeapon)
    {
        s_lastWeapon = weapon;
        s_torchOn = false;
        s_lastApply = 0;
    }

    if (!weapon)
        return;

    // Toggle on press
    if (pressed)
    {
        s_torchOn = !s_torchOn;
        SwitchTorchState(weapon, s_torchOn);
        WriteDebug(__LINE__, "[KM] Torch toggle: %s (weapon=%p)", s_torchOn ? "ON" : "OFF", weapon);
        s_lastApply = GetTickCount();
    }

    // If enabled, periodically re-apply ON to fight the game forcing it off.
    if (s_torchOn)
    {
        DWORD now = GetTickCount();
        if (now - s_lastApply > 250)
        {
            SwitchTorchState(weapon, true);
            s_lastApply = now;
        }
    }
}

// =========================
// Pages / UI
// =========================
LayeredPage* page1;
LayeredPage* page2;

static void InitializeDefaultBindings()
{
    g_defaultBindings[17] = 0x11;
    g_defaultBindings[18] = 0x1F;
    g_defaultBindings[19] = 0x1E;
    g_defaultBindings[20] = 0x20;
    g_defaultBindings[21] = 0x10;
    g_defaultBindings[22] = 0x12;
    g_defaultBindings[2] = 0x2D;
    g_defaultBindings[12] = 0;
    g_defaultBindings[29] = 0x2A;
    g_defaultBindings[45] = 0x39;
    g_defaultBindings[46] = 0x2F;
    g_defaultBindings[28] = 0x2D;
    g_defaultBindings[1] = 0x2E;
    g_defaultBindings[11] = 0x39;
    g_defaultBindings[7] = 0x30;
    g_defaultBindings[3] = 0x13;
    g_defaultBindings[9] = -34;
    g_defaultBindings[53] = -34;
    g_defaultBindings[0] = 0x39;
    g_defaultBindings[39] = 0x02;
    g_defaultBindings[40] = 0x03;
    g_defaultBindings[41] = 0x04;
    g_defaultBindings[42] = 0x05;
    g_defaultBindings[24] = 0x01;
    g_defaultBindings[4] = 0x21;
    g_defaultBindings[5] = 0x21;
    g_defaultBindings[14] = 0x0F;
    g_defaultBindings[25] = -32;
    g_defaultBindings[26] = -34;
    g_defaultBindings[31] = -32;
    g_defaultBindings[32] = -32;
    g_defaultBindings[38] = 0x14;

    g_defaultBindings[48] = 0; // TOGGLE_TORCH default: NONE
}

static void ResetBindsToDefault(LayeredPage* page)
{
    if (!page) return;

    std::vector<CNeoItem*>* itemsToUpdate = nullptr;
    if (page == page1)      itemsToUpdate = &g_movementBindingItems;
    else if (page == page2) itemsToUpdate = &g_actionBindingItems;
    else return;

    for (auto const& kv : g_defaultBindings) {
        int actionIndex = kv.first;
        int defaultKey = kv.second;
        if (actionIndex >= 0 && actionIndex < 60) {
            CInput::activeMapping[actionIndex].Code = defaultKey;
            if (IsTrackedAction(actionIndex)) {
                g_savedValid[actionIndex] = true;
                g_savedCode[actionIndex] = defaultKey;
            }
        }
    }

    for (CNeoItem* item : *itemsToUpdate) {
        if (!item) continue;
        int actionIndex = atoi(item->getText1());
        auto it = g_defaultBindings.find(actionIndex);
        if (it != g_defaultBindings.end())
            item->setKeyBindingLabel(it->second);
    }

    SaveExtraBindsToFile();
}

static void addBindingEntry(LayeredPage* controllerPage, const char* index, float left, float top, KeyboardAction action, const char* actionStr)
{
    CNeoKeyBinding* myBinding = new CNeoKeyBinding();

    if (CInput::activeMapping[action].Code < 0)
        NeoMenu::setDefaultColor(108, 82, 75, 170);

    CNeoItem* item = myBinding->create(const_cast<char*>(index), left, top, const_cast<char*>(index), 0.7, 0.7);
    item->setKeyBindingLabel(CInput::activeMapping[action].Code);
    item->setText1(const_cast<char*>(actionStr));

    if (CInput::activeMapping[action].Code >= 0)
        item->setSelectable(true);

    controllerPage->addItemToPage(item);

    if (controllerPage == page1)      g_movementBindingItems.push_back(item);
    else if (controllerPage == page2) g_actionBindingItems.push_back(item);

    if (CInput::activeMapping[action].Code < 0)
        NeoMenu::setDefaultColor(200, 210, 175, 170);
}

static void SyncUiFromMapping(std::vector<CNeoItem*>& items)
{
    for (CNeoItem* item : items)
    {
        if (!item) continue;
        const char* actionText = item->getText1();
        if (!actionText) continue;

        int idx = atoi(actionText);
        if (idx < 0 || idx >= 60) continue;

        item->setKeyBindingLabel(CInput::activeMapping[idx].Code);
    }
}

static void CreateLayeredPage()
{
    CNeoText* text = new CNeoText();
    CNeoLinkToPage* linkToPage = new CNeoLinkToPage();

    // --- PAGE 1 - Movements ---
    page1 = new LayeredPage();
    page1->setName(const_cast<char*>("mymovementkeys"));
    page1->setParent(const_cast<char*>("controls"));
    g_movementBindingItems.clear();
    page1->setAlphaGhosting(false);
    page1->setBoldGhosting(false);
    page1->setWarping(false);

    NeoMenu::setDefaultColor(150, 30, 0, 170);
    page1->addItemToPage(text->create(const_cast<char*>("Coded by Sor3nt & Hellwire"), 0.1, 0.02, 0.6, 0.6));
    page1->addItemToPage(text->create(const_cast<char*>("Notice: Use arrow and enter keys to set new keybinds"), 0.55, 0.89, 0.7, 0.7));
    page1->addItemToPage(text->create(const_cast<char*>("MOVMAP_T"), 0.08, 0.124, 1.3, 1.4));
    NeoMenu::setDefaultColor(200, 210, 175, 170);

    addBindingEntry(page1, "A_FWD_PC", 0.1, 0.25, KeyboardAction::ACTION_MOVE_FORWARD, "17");
    addBindingEntry(page1, "A_BACK_PC", 0.1, 0.30, KeyboardAction::ACTION_MOVE_BACKWARDS, "18");
    addBindingEntry(page1, "A_STRFL_PC", 0.1, 0.35, KeyboardAction::ACTION_MOVE_LEFT, "19");
    addBindingEntry(page1, "A_STRFR_PC", 0.1, 0.40, KeyboardAction::ACTION_MOVE_RIGHT, "20");
    addBindingEntry(page1, "A_PEEKL_PC", 0.1, 0.50, KeyboardAction::ACTION_PEEKL, "21");
    addBindingEntry(page1, "A_PEEKR_PC", 0.1, 0.55, KeyboardAction::ACTION_PEEKR, "22");
    addBindingEntry(page1, "A_LKBK_PC", 0.1, 0.60, KeyboardAction::ACTION_LOOKBACK, "2");
    addBindingEntry(page1, "grapple", 0.52, 0.25, KeyboardAction::ACTION_GRAPPLE, "12");
    addBindingEntry(page1, "A_RUN_PC", 0.52, 0.30, KeyboardAction::ACTION_RUN, "29");
    addBindingEntry(page1, "A_CRAWL_PC", 0.52, 0.35, KeyboardAction::ACTION_CRAWL, "45");
    addBindingEntry(page1, "crouch", 0.52, 0.40, KeyboardAction::ACTION_CROUCH, "46");
    addBindingEntry(page1, "A_SNEAK_PC", 0.52, 0.50, KeyboardAction::ACTION_SNEAK, "28");
    addBindingEntry(page1, "A_WALLH_PC", 0.52, 0.55, KeyboardAction::ACTION_WALLSQUASH, "1");
    addBindingEntry(page1, "A_CLIMB_PC", 0.52, 0.60, KeyboardAction::ACTION_CLIMB, "11");

    page1->addItemToPage(linkToPage->create(const_cast<char*>("controls"), const_cast<char*>("apply"), 0.1, 0.90, 0));
    CNeoItem* resetItem1 = text->create(const_cast<char*>("reset"), 0.25, 0.90, 0, 0);
    resetItem1->setSelectable(true);
    resetItem1->setText1(const_cast<char*>("reset_button"));
    page1->addItemToPage(resetItem1);
    NeoMenu::insertPage(page1);

    // --- PAGE 2 - Actions ---
    page2 = new LayeredPage();
    page2->setName(const_cast<char*>("myactionkeys"));
    page2->setParent(const_cast<char*>("controls"));
    g_actionBindingItems.clear();
    page2->setAlphaGhosting(false);
    page2->setBoldGhosting(false);
    page2->setWarping(false);

    NeoMenu::setDefaultColor(150, 30, 0, 170);
    page2->addItemToPage(text->create(const_cast<char*>("Coded by Sor3nt & Hellwire"), 0.1, 0.02, 0.6, 0.6));
    page2->addItemToPage(text->create(const_cast<char*>("Notice: Use arrow and enter keys to set new keybinds"), 0.55, 0.89, 0.7, 0.7));
    page2->addItemToPage(text->create(const_cast<char*>("ACTMAP_T"), 0.08, 0.124, 1.3, 1.4));
    NeoMenu::setDefaultColor(200, 210, 175, 170);

    addBindingEntry(page2, "fire/hard punch", 0.1, 0.25, KeyboardAction::ACTION_FIRE1, "7");
    addBindingEntry(page2, "A_RLOAD_PC", 0.1, 0.30, KeyboardAction::ACTION_RELOAD, "3");
    addBindingEntry(page2, "A_FIREW_PC", 0.1, 0.35, KeyboardAction::ACTION_PUNCH, "9");
    addBindingEntry(page2, "A_EXECU_PC", 0.1, 0.40, KeyboardAction::ACTION_EXECUTE, "53");
    addBindingEntry(page2, "USE", 0.1, 0.50, KeyboardAction::ACTION_USE, "0");
    addBindingEntry(page2, "A_INV1_PC", 0.1, 0.55, KeyboardAction::ACTION_INVENTORY_SLOT1, "39");
    addBindingEntry(page2, "A_INV2_PC", 0.1, 0.60, KeyboardAction::ACTION_INVENTORY_SLOT2, "40");
    addBindingEntry(page2, "A_INV3_PC", 0.1, 0.65, KeyboardAction::ACTION_INVENTORY_SLOT3, "41");
    addBindingEntry(page2, "A_INV4_PC", 0.1, 0.70, KeyboardAction::ACTION_INVENTORY_SLOT4, "42");
    addBindingEntry(page2, "A_MENU_PC", 0.52, 0.25, KeyboardAction::ACTION_MENU, "24");
    addBindingEntry(page2, "A_PICUP_PC", 0.52, 0.30, KeyboardAction::ACTION_PICKUP, "4");
    addBindingEntry(page2, "A_DROP_PC", 0.52, 0.35, KeyboardAction::ACTION_DROP, "5");
    addBindingEntry(page2, "A_BLOCK_PC", 0.52, 0.40, KeyboardAction::ACTION_BLOCK, "14");
    addBindingEntry(page2, "A_ZMIN_PC", 0.52, 0.50, KeyboardAction::ACTION_ZOOMIN, "25");
    addBindingEntry(page2, "A_ZMOUT_PC", 0.52, 0.55, KeyboardAction::ACTION_ZOOMOUT, "26");
    addBindingEntry(page2, "A_GUNAM_PC", 0.52, 0.60, KeyboardAction::ACTION_ZOOMAIM, "31");
    addBindingEntry(page2, "A_TARGT_PC", 0.52, 0.65, KeyboardAction::ACTION_LOCKON, "32");
    addBindingEntry(page2, "A_FP_PC", 0.52, 0.70, KeyboardAction::ACTION_LOOKAROUND, "38");
    addBindingEntry(page2, "toggle torch", 0.52, 0.75, (KeyboardAction)48, "48");

    page2->addItemToPage(linkToPage->create(const_cast<char*>("controls"), const_cast<char*>("apply"), 0.1, 0.90, 0));
    CNeoItem* resetItem2 = text->create(const_cast<char*>("reset"), 0.25, 0.90, 0, 0);
    resetItem2->setSelectable(true);
    resetItem2->setText1(const_cast<char*>("reset_button"));
    page2->addItemToPage(resetItem2);
    NeoMenu::insertPage(page2);

    delete text;
    delete linkToPage;
}

static CNeoItem* getSelectedItem()
{
    if (page1 && NeoMenu::isCurrentPageSetTo(const_cast<char*>("mymovementkeys")))
        return page1->getSelectedItem();
    if (page2 && NeoMenu::isCurrentPageSetTo(const_cast<char*>("myactionkeys")))
        return page2->getSelectedItem();
    return nullptr;
}

// FIXED: no leak, static buffer
static int __fastcall CText__GetFromKey16(int* ptr, int keyCode, int key, int unk)
{
    static wchar_t wNameBuffer[64];
    const char* name = converrtDirectInputCodeToName(keyCode);
    mbstowcs_s(nullptr, wNameBuffer, 64, name, _TRUNCATE);
    return (int)wNameBuffer;
}

// =========================
// Main loop state
// =========================
CRITICAL_SECTION g_cs;
bool g_pagesInjected = false;
bool g_waitToApply = false;
CNeoItem* g_currentBindingItem = nullptr;
DWORD g_lastKeyTime = 0;

// periodic re-apply (survive game overwriting mapping)
static DWORD g_lastReapplyTime = 0;

// ENTER swallow on page entry + edge trigger
static DWORD g_pageEnterGraceUntil = 0;
static int   g_lastPageKind = 0;     // 0 none, 1 movement, 2 action
static bool  g_prevReturnDown = false;

// =========================
// Main loop
// =========================
static void Init()
{
    InitializeCriticalSection(&g_cs);
    InitializeDefaultBindings();
    Memory::VP::InjectHook(0x5602B3, CText__GetFromKey16, PATCH_CALL);

    // ini beside ASI
    if (g_cfgPath.empty())
        g_cfgPath = BuildIniPathBesideAsi();

    LoadExtraBindsFromFile();

    while (true)
    {
        // --- Part 1: Page Injection ---
        if (!g_pagesInjected)
        {
            if (NeoMenu::isCurrentPageSetTo(const_cast<char*>("start")))
            {
                // First safe-ish time to ensure the ini exists and our saved set is sensible
                if (!FileExists(g_cfgPath))
                {
                    SeedSavedFromCurrentMappingIfMissing();
                    SaveExtraBindsToFile(); // generates ini beside ASI
                }

                ApplySavedExtraBinds();

                CreateLayeredPage();

                EnterCriticalSection(&g_cs);
                LayeredPage* controlsPage = NeoMenu::findPageByName(const_cast<char*>("controls"));
                if (controlsPage) {
                    CNeoItem* movementItem = controlsPage->getItem("movementkeys");
                    CNeoItem* actionItem = controlsPage->getItem("actionkeys");

                    if (movementItem) movementItem->setText1(const_cast<char*>("mymovementkeys"));
                    if (actionItem)   actionItem->setText1(const_cast<char*>("myactionkeys"));
                }
                g_pagesInjected = true;
                LeaveCriticalSection(&g_cs);
            }

            Sleep(200);
            continue;
        }

        DWORD now = GetTickCount();

        bool inMovementPage = NeoMenu::isCurrentPageSetTo(const_cast<char*>("mymovementkeys"));
        bool inActionPage = NeoMenu::isCurrentPageSetTo(const_cast<char*>("myactionkeys"));
        int pageKind = inMovementPage ? 1 : (inActionPage ? 2 : 0);

        // capture flag snapshot for torch handler
        bool localWaitToApply = false;
        EnterCriticalSection(&g_cs);
        localWaitToApply = g_waitToApply;
        LeaveCriticalSection(&g_cs);

        // Run torch toggle only when NOT in our pages (i.e. in-game), and not while capturing
        HandleTorchToggleHotkey(pageKind == 0, localWaitToApply);

        // ENTER edge trigger + swallow on page entry
        bool returnDown = KeyHit(VK_RETURN);

        if (g_ignoreReturnUntilUp && !returnDown)
            g_ignoreReturnUntilUp = false;

        bool returnPressed = (returnDown && !g_prevReturnDown);
        g_prevReturnDown = returnDown;

        if (pageKind != g_lastPageKind)
        {
            g_lastPageKind = pageKind;

            if (pageKind != 0)
            {
                // swallow the ENTER that opened the page
                g_ignoreReturnUntilUp = true;
                g_pageEnterGraceUntil = now + 250;
                g_lastKeyTime = now;

                // avoid generating an "edge" on the first tick inside the page
                g_prevReturnDown = true;
            }
        }

        // Periodic re-apply to survive the game reloading defaults / not saving these indices.
        // Skip while user is actively capturing a key.
        if (!localWaitToApply)
        {
            if (now - g_lastReapplyTime > 1000) {
                g_lastReapplyTime = now;
                ApplySavedExtraBinds();
            }
        }

        // Sync UI when entering pages
        if (inMovementPage)
        {
            if (!g_movementPageSynced)
            {
                ApplySavedExtraBinds();
                SyncUiFromMapping(g_movementBindingItems);
                g_movementPageSynced = true;
            }
        }
        else g_movementPageSynced = false;

        if (inActionPage)
        {
            if (!g_actionPageSynced)
            {
                ApplySavedExtraBinds();
                SyncUiFromMapping(g_actionBindingItems);
                g_actionPageSynced = true;
            }
        }
        else g_actionPageSynced = false;

        // Only handle input on our pages
        if (!(inMovementPage || inActionPage))
        {
            Sleep(50);
            continue;
        }

        // Capture/apply flow
        if (localWaitToApply)
        {
            // wait for RETURN release so we don't bind RETURN immediately
            if (g_ignoreReturnUntilUp) {
                Sleep(30);
                continue;
            }

            if (now - g_lastKeyTime > 200)
            {
                int pressedKeyCode = getPressedKey();
                if (pressedKeyCode != -1)
                {
                    g_lastKeyTime = now;

                    EnterCriticalSection(&g_cs);
                    g_waitToApply = false;
                    CNeoItem* bindingItem = g_currentBindingItem;
                    g_currentBindingItem = nullptr;
                    LeaveCriticalSection(&g_cs);

                    if (bindingItem)
                    {
                        int directInputCode = convertKeyCodeToDirectInputCode(pressedKeyCode);
                        bindingItem->setKeyBindingLabel(directInputCode);

                        const char* actionText = bindingItem->getText1();
                        if (actionText)
                        {
                            int keyIndex = atoi(actionText);
                            if (keyIndex >= 0 && keyIndex < 60)
                            {
                                CInput::activeMapping[keyIndex].Code = directInputCode;

                                if (IsTrackedAction(keyIndex))
                                {
                                    g_savedValid[keyIndex] = true;
                                    g_savedCode[keyIndex] = directInputCode;
                                    SaveExtraBindsToFile();
                                }
                            }
                        }
                    }

                    Sleep(150);
                }
            }
        }
        else if (!g_ignoreReturnUntilUp && returnPressed && now >= g_pageEnterGraceUntil)
        {
            if (now - g_lastKeyTime > 300)
            {
                g_lastKeyTime = now;

                CNeoItem* selectedItem = getSelectedItem();
                if (selectedItem)
                {
                    if (selectedItem->isKeybinding())
                    {
                        selectedItem->setKeyBindingLabel(-1);

                        EnterCriticalSection(&g_cs);
                        g_currentBindingItem = selectedItem;
                        g_waitToApply = true;
                        LeaveCriticalSection(&g_cs);

                        // prevent RETURN being captured immediately
                        g_ignoreReturnUntilUp = true;
                    }
                    else
                    {
                        const char* tag = selectedItem->getText1();
                        if (tag && strcmp(tag, "reset_button") == 0)
                        {
                            LayeredPage* currentPage = inMovementPage ? page1 : page2;
                            if (currentPage)
                                ResetBindsToDefault(currentPage);
                        }
                    }
                }

                Sleep(150);
            }
        }

        Sleep(50);
    }

    DeleteCriticalSection(&g_cs);
}

// =========================
// Entry
// =========================
extern "C" __declspec(dllexport) void InitializeASI()
{
    // Determine the module handle of THIS ASI/DLL so the ini is placed next to it
    GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)&InitializeASI,
        &g_hThisModule
    );

    CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Init), nullptr, 0, nullptr);
}

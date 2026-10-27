// ArchiTools Add-On for Archicad
// Adds the "ArchiTools" menu. Every command is passed to the ArchiTools window
// (ArchiTools.exe); if the window is not open, the add-on starts it.
// Keyboard shortcuts: Options > Work Environment > Keyboard Shortcuts.
// The source is kept pure ASCII on purpose: Russian texts are UTF-8 byte escapes.

#include "APIEnvir.h"
#include "ACAPinc.h"

#include "ResourceIds.hpp"
#include "RS.hpp"
#include "UniString.hpp"

#include <stdio.h>
#include <string.h>
#include <wchar.h>

namespace {

struct Command {
	Int32		index;		// menu item index in the 'STR#' ID_ADDON_MENU resource
	const char*	tool;		// command name understood by ArchiTools.exe
	const char*	titleUtf8;	// Russian menu text (UTF-8)
};

const Command kCommands[] = {
	{ 1, "number_zones",
	  "\xD0\x9D\xD1\x83\xD0\xBC\xD0\xB5\xD1\x80\xD0\xB0\xD1\x86\xD0\xB8\xD1\x8F\x20\xD0\xB2\xD1\x8B\xD0\xB4\xD0\xB5\xD0\xBB\xD0\xB5\xD0\xBD\xD0\xBD\xD1\x8B\xD1\x85\x20\xD0\xB7\xD0\xBE\xD0\xBD" },   // Number Selected Zones
	{ 2, "click_toggle",
	  "\xD0\x9D\xD1\x83\xD0\xBC\xD0\xB5\xD1\x80\xD0\xB0\xD1\x86\xD0\xB8\xD1\x8F\x20\xD0\xB7\xD0\xBE\xD0\xBD\x20\xD0\xBF\xD0\xBE\x20\xD0\xBA\xD0\xBB\xD0\xB8\xD0\xBA\xD0\xB0\xD0\xBC\x20\x28\xD0\xB2\xD0\xBA\xD0\xBB\x2F\xD0\xB2\xD1\x8B\xD0\xBA\xD0\xBB\x29" },   // Number Zones by Clicks (on-off)
	{ 3, "check_alignment",
	  "\xD0\x9A\xD1\x80\xD0\xB0\xD1\x8F\x20\xD0\xB7\xD0\xBE\xD0\xBD\x20\xD0\xBD\xD0\xB5\x20\xD0\xBF\xD0\xBE\x20\xD0\xBB\xD0\xB8\xD0\xBD\xD0\xB8\xD0\xB8\x20\xD1\x81\xD1\x82\xD0\xB5\xD0\xBD\x20\xD0\xB8\x20\xD0\xBA\xD0\xBE\xD0\xBB\xD0\xBE\xD0\xBD\xD0\xBD" },   // Zone Edges Not On Wall Lines
	{ 4, "watch_toggle",
	  "\xD0\xA1\xD0\xBB\xD0\xB5\xD0\xB6\xD0\xB5\xD0\xBD\xD0\xB8\xD0\xB5\x20\xD0\xB7\xD0\xB0\x20\xD0\xB8\xD1\x81\xD0\xBF\xD1\x80\xD0\xB0\xD0\xB2\xD0\xBB\xD0\xB5\xD0\xBD\xD0\xB8\xD1\x8F\xD0\xBC\xD0\xB8\x20\x28\xD0\xB2\xD0\xBA\xD0\xBB\x2F\xD0\xB2\xD1\x8B\xD0\xBA\xD0\xBB\x29" },   // Watch Corrections (on-off)
	{ 5, "clear",
	  "\xD0\xA1\xD0\xBD\xD1\x8F\xD1\x82\xD1\x8C\x20\xD0\xBF\xD0\xBE\xD0\xB4\xD1\x81\xD0\xB2\xD0\xB5\xD1\x82\xD0\xBA\xD1\x83\x20\xD0\xB8\x20\xD0\xBE\xD1\x82\xD0\xBC\xD0\xB5\xD1\x82\xD0\xBA\xD0\xB8" },   // Clear Highlight and Marks
	{ 6, "number_openings",
	  "\xD0\x94\xD0\xB2\xD0\xB5\xD1\x80\xD0\xB8\x20\xD0\xB8\x20\xD0\xBE\xD0\xBA\xD0\xBD\xD0\xB0\x20\xD0\xBF\xD0\xBE\x20\xD0\xBF\xD0\xBE\xD0\xBC\xD0\xB5\xD1\x89\xD0\xB5\xD0\xBD\xD0\xB8\xD1\x8F\xD0\xBC" },   // Number Doors and Windows
	{ 7, "check_geometry",
	  "\xD0\x9F\xD1\x80\xD0\xBE\xD0\xB2\xD0\xB5\xD1\x80\xD0\xBA\xD0\xB0\x20\xD0\xB3\xD0\xB5\xD0\xBE\xD0\xBC\xD0\xB5\xD1\x82\xD1\x80\xD0\xB8\xD0\xB8\x20\xD0\xB7\xD0\xBE\xD0\xBD" },   // Check Zone Geometry
	{ 8, "check_zones",
	  "\xD0\x9F\xD1\x80\xD0\xBE\xD0\xB2\xD0\xB5\xD1\x80\xD0\xBA\xD0\xB0\x20\xD0\xBD\xD0\xBE\xD0\xBC\xD0\xB5\xD1\x80\xD0\xBE\xD0\xB2\x20\xD0\xB8\x20\xD0\xBD\xD0\xB0\xD0\xB7\xD0\xB2\xD0\xB0\xD0\xBD\xD0\xB8\xD0\xB9\x20\xD0\xB7\xD0\xBE\xD0\xBD" },   // Check Zone Numbers and Names
	{ 9, "tep",
	  "\xD0\xA2\xD0\xAD\xD0\x9F\x20\xD0\xB8\x20\xD1\x8D\xD0\xBA\xD1\x81\xD0\xBF\xD0\xBB\xD0\xB8\xD0\xBA\xD0\xB0\xD1\x86\xD0\xB8\xD1\x8F\x20\xD0\xB2\x20\x45\x78\x63\x65\x6C" },   // Area Report to Excel
	{ 10, "show",
	  "\xD0\x9E\xD1\x82\xD0\xBA\xD1\x80\xD1\x8B\xD1\x82\xD1\x8C\x20\xD0\xBE\xD0\xBA\xD0\xBD\xD0\xBE\x20\x41\x72\x63\x68\x69\x54\x6F\x6F\x6C\x73" },   // Open ArchiTools Window
};

const Int32 kCommandCount = static_cast<Int32> (sizeof (kCommands) / sizeof (kCommands[0]));

const char* kNoWindowMessage = "\x41\x72\x63\x68\x69\x54\x6F\x6F\x6C\x73\x3A\x20\xD0\xBE\xD0\xBA\xD0\xBD\xD0\xBE\x20\xD0\xBF\xD1\x80\xD0\xBE\xD0\xB3\xD1\x80\xD0\xB0\xD0\xBC\xD0\xBC\xD1\x8B\x20\xD0\xBD\xD0\xB5\x20\xD0\xBD\xD0\xB0\xD0\xB9\xD0\xB4\xD0\xB5\xD0\xBD\xD0\xBE\x2E\x20\xD0\x9E\xD0\xB4\xD0\xB8\xD0\xBD\x20\xD1\x80\xD0\xB0\xD0\xB7\x20\xD0\xB7\xD0\xB0\xD0\xBF\xD1\x83\xD1\x81\xD1\x82\xD0\xB8\xD1\x82\xD0\xB5\x20\x41\x72\x63\x68\x69\x54\x6F\x6F\x6C\x73\x2E\x65\x78\x65\x20\xD0\xB2\xD1\x80\xD1\x83\xD1\x87\xD0\xBD\xD1\x83\xD1\x8E\x20\x2D\x20\xD0\xBF\xD0\xBE\xD1\x81\xD0\xBB\xD0\xB5\x20\xD1\x8D\xD1\x82\xD0\xBE\xD0\xB3\xD0\xBE\x20\xD0\xBA\xD0\xBE\xD0\xBC\xD0\xB0\xD0\xBD\xD0\xB4\xD1\x8B\x20\xD0\xBC\xD0\xB5\xD0\xBD\xD1\x8E\x20\x41\x72\x63\x68\x69\x54\x6F\x6F\x6C\x73\x20\xD0\xB1\xD1\x83\xD0\xB4\xD1\x83\xD1\x82\x20\xD0\xBE\xD1\x82\xD0\xBA\xD1\x80\xD1\x8B\xD0\xB2\xD0\xB0\xD1\x82\xD1\x8C\x20\xD0\xB5\xD0\xB3\xD0\xBE\x20\xD1\x81\xD0\xB0\xD0\xBC\xD0\xB8\x2E";

const size_t kPathSize = 1024;


bool MakeDataPath (WCHAR* out, size_t outSize, const WCHAR* fileName)
{
	const DWORD length = GetEnvironmentVariableW (L"APPDATA", out, static_cast<DWORD> (outSize));
	if (length == 0 || length >= outSize) {
		return false;
	}
	if (wcscat_s (out, outSize, L"\\ArchiTools\\") != 0) {
		return false;
	}
	return wcscat_s (out, outSize, fileName) == 0;
}


// ArchiTools.exe touches alive.txt every second while its window is open.
bool WindowIsAlive ()
{
	WCHAR path[kPathSize] = {};
	if (!MakeDataPath (path, kPathSize, L"alive.txt")) {
		return false;
	}
	WIN32_FILE_ATTRIBUTE_DATA data = {};
	if (!GetFileAttributesExW (path, GetFileExInfoStandard, &data)) {
		return false;
	}
	FILETIME nowTime = {};
	GetSystemTimeAsFileTime (&nowTime);
	ULARGE_INTEGER now = {};
	ULARGE_INTEGER written = {};
	now.LowPart = nowTime.dwLowDateTime;
	now.HighPart = nowTime.dwHighDateTime;
	written.LowPart = data.ftLastWriteTime.dwLowDateTime;
	written.HighPart = data.ftLastWriteTime.dwHighDateTime;
	if (now.QuadPart <= written.QuadPart) {
		return true;
	}
	return (now.QuadPart - written.QuadPart) < 40000000ULL;		// 4 seconds in 100 ns units
}


// The open window picks up command.txt within a fraction of a second.
bool WriteCommand (const char* tool)
{
	WCHAR tempPath[kPathSize] = {};
	WCHAR commandPath[kPathSize] = {};
	if (!MakeDataPath (tempPath, kPathSize, L"command.tmp") || !MakeDataPath (commandPath, kPathSize, L"command.txt")) {
		return false;
	}
	FILE* file = nullptr;
	if (_wfopen_s (&file, tempPath, L"wb") != 0 || file == nullptr) {
		return false;
	}
	const size_t length = strlen (tool);
	const size_t written = fwrite (tool, 1, length, file);
	fclose (file);
	if (written != length) {
		return false;
	}
	return MoveFileExW (tempPath, commandPath, MOVEFILE_REPLACE_EXISTING) != FALSE;
}


// ArchiTools.exe writes its own full path to exe_path.txt (UTF-8).
bool ReadExePath (WCHAR* out, int outSize)
{
	WCHAR path[kPathSize] = {};
	if (!MakeDataPath (path, kPathSize, L"exe_path.txt")) {
		return false;
	}
	FILE* file = nullptr;
	if (_wfopen_s (&file, path, L"rb") != 0 || file == nullptr) {
		return false;
	}
	char buffer[4096] = {};
	const size_t readCount = fread (buffer, 1, sizeof (buffer) - 1, file);
	fclose (file);

	const char* start = buffer;
	size_t length = readCount;
	if (length >= 3 && static_cast<unsigned char> (buffer[0]) == 0xEF &&
		static_cast<unsigned char> (buffer[1]) == 0xBB && static_cast<unsigned char> (buffer[2]) == 0xBF) {
		start += 3;
		length -= 3;
	}
	while (length > 0 && (start[length - 1] == '\r' || start[length - 1] == '\n' || start[length - 1] == ' ')) {
		--length;
	}
	if (length == 0) {
		return false;
	}
	const int converted = MultiByteToWideChar (CP_UTF8, 0, start, static_cast<int> (length), out, outSize - 1);
	if (converted <= 0) {
		return false;
	}
	out[converted] = 0;
	return GetFileAttributesW (out) != INVALID_FILE_ATTRIBUTES;
}


bool LaunchWindow (const char* tool)
{
	WCHAR exePath[kPathSize] = {};
	if (!ReadExePath (exePath, static_cast<int> (kPathSize))) {
		return false;
	}
	WCHAR toolName[64] = {};
	if (MultiByteToWideChar (CP_UTF8, 0, tool, -1, toolName, 64) <= 0) {
		return false;
	}
	WCHAR commandLine[kPathSize + 128] = {};
	const size_t commandSize = kPathSize + 128;
	if (wcscpy_s (commandLine, commandSize, L"\"") != 0 ||
		wcscat_s (commandLine, commandSize, exePath) != 0 ||
		wcscat_s (commandLine, commandSize, L"\" --run ") != 0 ||
		wcscat_s (commandLine, commandSize, toolName) != 0) {
		return false;
	}
	WCHAR workFolder[kPathSize] = {};
	if (wcscpy_s (workFolder, kPathSize, exePath) != 0) {
		return false;
	}
	WCHAR* lastSlash = wcsrchr (workFolder, L'\\');
	if (lastSlash != nullptr) {
		*lastSlash = 0;
	}

	STARTUPINFOW startup = {};
	startup.cb = static_cast<DWORD> (sizeof (startup));
	PROCESS_INFORMATION process = {};
	if (!CreateProcessW (exePath, commandLine, nullptr, nullptr, FALSE, 0, nullptr,
						 lastSlash != nullptr ? workFolder : nullptr, &startup, &process)) {
		return false;
	}
	CloseHandle (process.hThread);
	CloseHandle (process.hProcess);
	return true;
}


void RunTool (const char* tool)
{
	if (WindowIsAlive () && WriteCommand (tool)) {
		return;
	}
	if (LaunchWindow (tool)) {
		return;
	}
	GS::UniString message (kNoWindowMessage, CC_UTF8);
	ACAPI_WriteReport (message, true);
}


GSErrCode MenuCommandHandler (const API_MenuParams* menuParams)
{
	if (menuParams->menuItemRef.menuResID != ID_ADDON_MENU) {
		return NoError;
	}
	for (Int32 i = 0; i < kCommandCount; ++i) {
		if (kCommands[i].index == menuParams->menuItemRef.itemIndex) {
			RunTool (kCommands[i].tool);
			break;
		}
	}
	return NoError;
}


void ApplyRussianMenuTexts ()
{
	for (Int32 i = 0; i < kCommandCount; ++i) {
		API_MenuItemRef itemRef = {};
		itemRef.menuResID = ID_ADDON_MENU;
		itemRef.itemIndex = kCommands[i].index;
		GS::UniString text (kCommands[i].titleUtf8, CC_UTF8);
		ACAPI_MenuItem_SetMenuItemText (&itemRef, nullptr, &text);
	}
}

}	// namespace


API_AddonType CheckEnvironment (API_EnvirParams* envir)
{
	RSGetIndString (&envir->addOnInfo.name, ID_ADDON_INFO, 1, ACAPI_GetOwnResModule ());
	RSGetIndString (&envir->addOnInfo.description, ID_ADDON_INFO, 2, ACAPI_GetOwnResModule ());
	return APIAddon_Preload;
}


GSErrCode RegisterInterface (void)
{
	return ACAPI_MenuItem_RegisterMenu (ID_ADDON_MENU, 0, MenuCode_UserDef, MenuFlag_Default);
}


GSErrCode Initialize (void)
{
	const GSErrCode err = ACAPI_MenuItem_InstallMenuHandler (ID_ADDON_MENU, MenuCommandHandler);
	ApplyRussianMenuTexts ();
	return err;
}


GSErrCode FreeData (void)
{
	return NoError;
}

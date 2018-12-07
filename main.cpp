#include "nvse/nvse/PluginAPI.h"
#include "nvse/nvse/nvse_version.h"
#include "nvse/nvse/SafeWrite.h"

void patchScreenshotCheck();
bool versionCheck(const NVSEInterface* nvse);
void handleIniOptions();


HMODULE noScreenshotHandle;
int g_bPrintToConsole = 1;

extern "C" {

	BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved) {
		if (dwReason == DLL_PROCESS_ATTACH)
			noScreenshotHandle = (HMODULE)hDllHandle;
		return TRUE;
	}

	bool NVSEPlugin_Query(const NVSEInterface *nvse, PluginInfo *info) {
		/* fill out the info structure */
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "No Screenshot Popup";
		info->version = 1;

		handleIniOptions();

		return versionCheck(nvse);
	}

	bool NVSEPlugin_Load(const NVSEInterface *nvse) {
		patchScreenshotCheck();
		return true;
	}

};

void handleIniOptions() {
	char filename[MAX_PATH];
	GetModuleFileNameA(noScreenshotHandle, filename, MAX_PATH);
	strcpy((char *)(strrchr(filename, '\\') + 1), "no_screenshot_popup.ini");
	g_bPrintToConsole = GetPrivateProfileIntA("Main", "bPrintToConsole", 1, filename);
}

void patchScreenshotCheck() {
	UInt32 messagePopupAddress = 0x878E5C;
	if (g_bPrintToConsole) {
		WriteRelCall(messagePopupAddress, (UInt32)Console_Print);
	}
	else {
		SafeWriteBuf(messagePopupAddress, "\x90\x90\x90\x90\x90", 5);
	}
}



bool versionCheck(const NVSEInterface* nvse) {
	if (nvse->isEditor) return false;
	if (nvse->nvseVersion < NVSE_VERSION_INTEGER) {
		_ERROR("NVSE version too old (got %08X expected at least %08X)", nvse->nvseVersion, NVSE_VERSION_INTEGER);
		return false;
	}
	return true;
}

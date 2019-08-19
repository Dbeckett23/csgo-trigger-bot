#include "MemoryManager.h"
#include <iostream>

using std::cout;                    using std::endl;
using std::string;


MemoryManager::MemoryManager()
{
	this->hProcess = NULL;
	this->dwPID = NULL;
	string process = "csgo.exe";
	char clientDllName[] = "client_panorama.dll";
	char engineDll[] = "engine.dll";
	try
	{
		if (!AttachProcess(process)) throw 1;
		this->ClientDLL = GetModule(clientDllName);
		this->EngineDLL = GetModule(engineDll);
		this->ClientDLL_Base = (DWORD)this->ClientDLL.modBaseAddr;
		this->EngineDLL_Base = (DWORD)this->EngineDLL.modBaseAddr;
		if (this->ClientDLL_Base == 0x0) throw 2;
		if (this->EngineDLL_Base == 0x0) throw 3;
		this->ClientDLL_Size = this->ClientDLL.dwSize;
		this->EngineDLL_Size = this->EngineDLL.dwSize;
		cout << "TriggerBot is running..." << endl;
	}
	catch (int iEx)
	{
		switch (iEx)
		{
		case 1: MessageBoxA(NULL, "CS:GO must be running", "ERROR", MB_ICONSTOP | MB_OK); exit(0); break;
		case 2: MessageBoxA(NULL, "Couldn't find client_panorama.dll", "ERROR", MB_ICONSTOP | MB_OK); exit(0); break;
		case 3: MessageBoxA(NULL, "Couldn't find Engine.dll", "ERROR", MB_ICONSTOP | MB_OK); exit(0); break;
		default: throw;
		}
	}
}

MemoryManager::~MemoryManager()
{
	CloseHandle(this->hProcess);
}

bool MemoryManager::AttachProcess(string &processName)
{
	PROCESSENTRY32 procEntry32;
	// take snap shot of all processes.
	HANDLE procSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	// check if snap was successful.
	if (procSnap == INVALID_HANDLE_VALUE) {
		cout << "Failed to take a snap shot" << endl;
		return false;
	}

	// set the size of the structure before using it... always have to do this.
	procEntry32.dwSize = sizeof(PROCESSENTRY32);

	// check information of the first process, exit if unsuccessful.
	if (!Process32First(procSnap, &procEntry32))
	{
		cout << "Error at first entry." << endl;
		return false;
	}

	do {
		//cout << "Process Name: " << procEntry32.szExeFile << endl; // debugger
		string exeName = procEntry32.szExeFile;
		// breaks loop once the processName is found, in this case "csgo.exe"
		if (exeName == processName) {
			//cout << "CSGO EXE FOUND BREAKING LOOP" << endl;
			this->dwPID = procEntry32.th32ProcessID;
			CloseHandle(procSnap);
			this->hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->dwPID);
			//cout <<"CSGO Process ID: " << this->dwPID << endl; // debugger
			return true;
		}

	} while (Process32Next(procSnap, &procEntry32));

	return false;
}

MODULEENTRY32 MemoryManager::GetModule(char * moduleName)
{
	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	MODULEENTRY32 mEntry;
	mEntry.dwSize = sizeof(mEntry);

	const WCHAR *modNameChar;
	int nChars = MultiByteToWideChar(CP_ACP, 0, moduleName, -1, NULL, 0);
	modNameChar = new WCHAR[nChars];
	MultiByteToWideChar(CP_ACP, 0, moduleName, -1, (LPWSTR)modNameChar, nChars);

	do {
		//cout << mEntry.szModule << endl;
		string mName = mEntry.szModule;
		if (mName == moduleName)
		{
			CloseHandle(hModule);
			//cout << moduleName << " FOUND BREAKING LOOP" << endl; //debugger
			return mEntry;
		}

	} while (Module32Next(hModule, &mEntry));

	CloseHandle(hModule);
	mEntry.modBaseAddr = 0x0;
	return mEntry;
}



DWORD MemoryManager::GetProcID()
{
	return this->dwPID;
}

HANDLE MemoryManager::GetProcHandle()
{
	return this->hProcess;
}

//void printError(TCHAR* msg)
//{
//	DWORD eNum;
//	TCHAR sysMsg[256];
//	TCHAR* p;
//
//	eNum = GetLastError();
//	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//		NULL, eNum,
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
//		sysMsg, 256, NULL);
//
//	// Trim the end of the line and terminate it with a null
//	p = sysMsg;
//	while ((*p > 31) || (*p == 9))
//		++p;
//	do { *p-- = 0; } while ((p >= sysMsg) &&
//		((*p == '.') || (*p < 33)));
//
//	// Display the message
//	std::cout << "\n  WARNING: failed with error " << msg << eNum << sysMsg << std::endl;
//}
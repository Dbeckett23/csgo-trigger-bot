#pragma once
#ifndef GAURD_MEMORYMANAGER_H
#define GAURD_MEMORYMANAGER_H

#include <Windows.h>
#include <TlHelp32.h>
#include <string>
class MemoryManager
{
public:
	MemoryManager();
	~MemoryManager();

	MODULEENTRY32 ClientDLL, EngineDLL;
	DWORD ClientDLL_Base, ClientDLL_Size, EngineDLL_Base, EngineDLL_Size;

	bool AttachProcess(std::string & processName);
	MODULEENTRY32 GetModule(char * DWORD);

	DWORD GetProcID();
	HANDLE GetProcHandle();

	template<class c> c Read(DWORD dwAdress);
	template<class c> BOOL Write(DWORD dwAddress, c ValueToWrite);

private:
	DWORD dwPID;
	HANDLE hProcess;
};

#endif

template<class c>
inline c MemoryManager::Read(DWORD dwAddress)
{
	c val;
	ReadProcessMemory(hProcess, (LPVOID)dwAddress, &val, sizeof(c), NULL);
	return val;
}

template<class c>
inline BOOL MemoryManager::Write(DWORD dwAddress, c ValueToWrite)
{
	return WriteProcessMemory(hProcess, (LPVOID)dwAddress, &ValueToWrite, sizeof(c), NULL);
}

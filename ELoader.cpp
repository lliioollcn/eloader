// ELoader.cpp: 定义应用程序的入口点。
//

#include "ELoader.h"
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <atlbase.h>
#include <conio.h>
#include <stdlib.h>
#include <tchar.h>
#include <malloc.h>
#include <TlHelp32.h>

HANDLE h_token;
HANDLE h_remote_process;
TCHAR msg[MAX_PATH];

BOOL inject_dll(LPCSTR dll_path, const DWORD remote_pro_id) {
	DWORD path_len = strlen(dll_path) + 1;
	SIZE_T write_len;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &h_token))
	{
		TOKEN_PRIVILEGES tkp;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid); tkp.PrivilegeCount = 1; tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(h_token, FALSE, &tkp, sizeof(tkp), NULL, NULL);
		std::cout << "OpenSuccess" << std::endl;
	}
	if ((h_remote_process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, remote_pro_id)) == NULL)
	{
		std::cout << "OpenError" << std::endl;
		return FALSE;
	}
	char* lib_func_buf;
	lib_func_buf = (char*)VirtualAllocEx(h_remote_process, NULL, path_len, MEM_COMMIT, PAGE_READWRITE);
	if (lib_func_buf == NULL) {
		std::cout << "BufReadError" << std::endl;
		return FALSE;
	}

	if (WriteProcessMemory(h_remote_process, lib_func_buf, (void*)dll_path, path_len, &write_len) == 0) {
		std::cout << "WriteError!" << std::endl;
		return FALSE;
	}

	if (path_len != write_len)
	{
		std::cout << "WriteNotFull!" << std::endl;
		return FALSE;
	}

	PTHREAD_START_ROUTINE load_start_addr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryA");
	if (load_start_addr == NULL)
	{
		std::cout << "LoadError" << std::endl;
		return FALSE;
	}
	HANDLE h_remote_thread;
	if ((h_remote_thread = CreateRemoteThread(h_remote_process, NULL, 0, load_start_addr, lib_func_buf, 0, NULL)) == NULL) {
		std::cout << "InjectError" << std::endl;
		return FALSE;
	}
	return TRUE;
}

DWORD RejectAllProcess(LPCSTR ProcessName, LPCSTR path)
{
	DWORD ProcessId = 0;
	HANDLE process = NULL;
	process = CreateToolhelp32Snapshot(TH32CS_SNAPALL, ::GetCurrentProcessId());
	PROCESSENTRY32 pe = { sizeof(pe) };
	while (::Process32Next(process, &pe))
	{
		if (::lstrcmpi(pe.szExeFile, ProcessName) == 0)
		{
			ProcessId = pe.th32ProcessID;
			std::cout << "SuccessFind: " << ProcessId << std::endl;
			std::cout << "Rejectin: " << ProcessId << std::endl;
			if (inject_dll(path, ProcessId)) {
				std::cout << "Rejectin: " << ProcessId << " success" << std::endl;
			}
			else {
				std::cout << "Rejectin: " << ProcessId << " failed" << std::endl;
			}
			//MessageBox(NULL,pe.szExeFile,"hello",MB_OK);
		}
		//MessageBox(NULL,pe.szExeFile,"hello",MB_OK);
	}

	return ProcessId;
}

std::string GetDllDir()
{
	char exeFullPath[MAX_PATH];
	std::string strPath = "";
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	strPath = (std::string)exeFullPath;
	int pos = strPath.find_last_of("\\", strPath.length());
	std::string final = strPath.substr(0, pos) + "\\eloader_dll.dll";
	return final;
}

int main(int argc, char** argv) {
	USES_CONVERSION;

	PCSTR processname = "java.exe";
	std::string ws = GetDllDir();
	LPCSTR path = ws.c_str();
	RejectAllProcess(processname,path);
	system("pause");
	return 0;
}
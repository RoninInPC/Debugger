#pragma once
#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_
#include <windows.h>
#include<list>
#include<map>
#include<string>
#include<tlhelp32.h>

#include"disas.h"
#include"breakpoint.h"
#include"library.h"
#include"thread.h"
#include"moduledumplibraryfunction.h"
#include"modulefindinstruction.h"
#include"moduledumplistfunction.h"
#include"moduletracefunction.h"
#include"LoggerFactory.h" 
class ConsoleDebugger {
	friend class BuilderConsoleDebugger;
	friend class ModuleDumpListFunction;
	friend class BreakPoint;
	friend class Library;
	friend class Thread;

private:

	enum class DEBUGGER_STATE {
		NOT_TRACE = 0,
		BREAK_SOFT = 1,
		BREAK_HARD = 2,
		TRACE = 3
	} state;

	bool isTrace;
	bool isRun;

	bool isActive = false;

	HANDLE debugProcess;
	std::string nameProcess; 

	std::list<BreakPoint> breakPointList; //лист breakpoint-ов
	std::list<Library> libraryList; //лист библиотек
	std::list<Thread> threadList; //лист потоков

	Logger* logger = LoggerFactory::MakeLogger(); //консольный логгер

	//работа модулей
	bool isWorkingOne = true;

	bool isWorkingTwo = true;

	bool isWorkingThree = true;

	bool isWorkingFour = true;
	//модули
	ModuleFindInstruction moduleOne;

	ModuleDumpListFunction moduleTwo;

	ModuleTraceFunction moduleThree;
	
	ModuleDumpLibraryFunction moduleFour;

public:
	bool CreateDebugProces(const std::string& procName);

private:
	bool SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);

	DWORD64 FindProcessId(const std::string& processName);

public:

	bool DebugActiveProces(int procID);

private:

	bool SetBreakpoint(DWORD64 tid, void* addr, BreakPoint::TYPE type);

	bool DeleteBreakpoint(DWORD64 tid, void* addr);

private:
	void EventCreateProcess(DWORD64 pid, DWORD64 tid, LPCREATE_PROCESS_DEBUG_INFO procDebugInfo);

	void EventExitProcess(DWORD64 pid, DWORD64 tid, LPEXIT_PROCESS_DEBUG_INFO procDebugInfo);

	void EventCreateThread(DWORD64 pid, DWORD64 tid, LPCREATE_THREAD_DEBUG_INFO threadDebugInfo);

	void EventExitThread(DWORD64 pid, DWORD64 tid, LPEXIT_THREAD_DEBUG_INFO threadDebugInfo);

	void EventLoadDll(DWORD64 pid, DWORD64 tid, LPLOAD_DLL_DEBUG_INFO dllDebugInfo);

	void EventUnloadDll(DWORD64 pid, DWORD64 tid, LPUNLOAD_DLL_DEBUG_INFO dllDebugInfo);

	void EventOutputDebugString(DWORD64 pid, DWORD64 tid, LPOUTPUT_DEBUG_STRING_INFO debugStringInfo);

private:

	void RequestAction(void);

	DWORD64 BreakpointEvent(DWORD64 tid, ULONG_PTR exceptionAddr);

	DWORD64 EventException(DWORD64 pid, DWORD64 tid, LPEXCEPTION_DEBUG_INFO exceptionDebugInfo);

public:

	void DebugRun(void);
};


class BuilderConsoleDebugger {
private:
	ConsoleDebugger ans;
public:
	BuilderConsoleDebugger workingModuleOne(bool isWorking) {
		ans.isWorkingOne = isWorking;
		return *this;
	}
	BuilderConsoleDebugger workingModuleTwo(bool isWorking) {
		ans.isWorkingTwo = isWorking;
		return *this;
	}
	BuilderConsoleDebugger workingModuleThree(bool isWorking) {
		ans.isWorkingThree = isWorking;
		return *this;
	}
	BuilderConsoleDebugger workingModuleFour(bool isWorking) {
		ans.isWorkingFour = isWorking;
		return *this;
	}
	ConsoleDebugger build() {
		return ans;
	}

};


#endif
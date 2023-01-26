#pragma once
#ifndef _MODULEDUMPLIBRARYFUNCTION_H_
#define _MODULEDUMPLIBRARYFUNCTION_H_
#include<map>
#include<string>
#include<windows.h>
#include<stack>

#include"breakpoint.h"
#include"context.h"
#include"LoggerFactory.h"
class ModuleDumpLibraryFunction {
	friend class BuilderModuleDumpLibraryFunction;
private:
	std::map<void*, std::string> addresInfo;

    std::stack<void*> lastBreakpointAddress;
    int stacktraceDeep = 20;

    Logger* outLogger = LoggerFactory::MakeLogger();

    bool isFirstTimeSet = true;
    bool isWorking = true;

private:
    std::string countSpace(int count) {
        std::string ans1;
        for (int i = 0; i < count; i++)
            ans1 += " ";
        return ans1;
    };
    #ifndef _AMD64_
    std::pair<std::string,int> stackTraceOut32(HANDLE debugProcess, DWORD64 tid) {
        std::string ans = "STACKTRACE INFO:\n";
        int countArg = 0;

        auto context = ContextHelper::GetContext(tid);
        std::stack<DWORD> stackOfMainFrame;
        stackOfMainFrame.push(context.Ebp);
        int counting = 0;
        while (stackOfMainFrame.size() > 0 && counting <stacktraceDeep) {
            auto back = stackOfMainFrame.top();
            stackOfMainFrame.pop();
            ans += countSpace(0) + MakerString::MakeHexNum(back) + "\n";
            bool isNotZero = true;
            for (int i = 0; i < stacktraceDeep; i++) {
                void* newEpb;
                back += i * 4;
                ReadProcessMemory(debugProcess, (void*)back, &newEpb, 4, NULL);
                ans += countSpace(1) + MakerString::MakeHexNum((DWORD)newEpb);
                if (i == 0) {
                    ans += " BEFORE STACKFRAME";
                    if(newEpb!=0)
                        stackOfMainFrame.push((DWORD)newEpb);
                }
                if (i == 1) {
                    ans += " BACK ADDRESS";
                }
                if (i > 1 && isNotZero) {
                    if ((DWORD)newEpb == 0) {
                        isNotZero = false;
                    }
                    else
                        countArg++;                 
                }
                ans += "\n";
            }
            counting++;
        }

        return { ans + "\n",countArg };
    }
    #else   
    std::pair<std::string, int> stackTraceOut64(HANDLE debugProcess, DWORD64 tid) {
        std::string ans = "STACKTRACE INFO:\n";
        int countArg = 0;

        auto context = ContextHelper::GetContext(tid);
        std::stack<DWORD64> stackOfMainFrame;
        stackOfMainFrame.push(context.Rbp);
        int counting = 0;
        while (stackOfMainFrame.size() > 0 && counting < stacktraceDeep) {
            auto back = stackOfMainFrame.top();
            stackOfMainFrame.pop();
            ans += countSpace(0) + MakerString::MakeHexNum(back) + "\n";
            bool isNotZero = true;
            for (int i = 0; i < stacktraceDeep; i++) {
                void* newEpb;
                back += i * 8;
                ReadProcessMemory(debugProcess, (void*)back, &newEpb, 8, NULL);
                ans += countSpace(1) + MakerString::MakeHexNum((DWORD64)newEpb);
                if (i == 0) {
                    ans += " BEFORE STACKFRAME";
                    if (newEpb != 0)
                        stackOfMainFrame.push((DWORD64)newEpb);
                }
                if (i == 1) {
                    ans += " BACK ADDRESS";
                }
                if (i > 1 && isNotZero) {
                    if ((DWORD64)newEpb == 0) {
                        isNotZero = false;
                    }
                    else
                        countArg++;
                }
                ans += "\n";
}
            counting++;
        }

        return { ans + "\n",countArg };
        
    }
    #endif;

    std::pair<std::string, int> GetStackTrace(HANDLE debugProcess,DWORD64 tid) {
        #ifndef _AMD64_
        return stackTraceOut32(debugProcess, tid);
        #else
        return stackTraceOut64(debugProcess, tid);
        #endif;
    }

public:

    bool findAddr(void* addr) {
        return addresInfo.find(addr) != addresInfo.end();
    }

	void addLibraryFunction(HANDLE debugProcess, void* lpBaseOfDll, const std::string& LibraryName) {
        if (!isWorking)
            return;
        //читаем из процесса таблицу экспорта по адресу lpBaseOfDll
        auto size = sizeof(IMAGE_DOS_HEADER);
        void* lib = malloc(size);
        ReadProcessMemory(debugProcess, lpBaseOfDll, lib, size, NULL);

        auto dheader = (PIMAGE_DOS_HEADER)lib;
        auto addr = (void*)((int)lpBaseOfDll + dheader->e_lfanew);
        size = sizeof(IMAGE_NT_HEADERS);
        void* header = malloc(size);
        ReadProcessMemory(debugProcess, addr, header, size, NULL);

        PIMAGE_NT_HEADERS pheader = (PIMAGE_NT_HEADERS)header;
        addr = (BYTE*)lpBaseOfDll + pheader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
        size = sizeof(IMAGE_EXPORT_DIRECTORY);
        void* exports = malloc(size);
        ReadProcessMemory(debugProcess, addr, exports, size, NULL);

        PIMAGE_EXPORT_DIRECTORY pexports = (PIMAGE_EXPORT_DIRECTORY)exports;
        void* addr1 = (void*)((int)lpBaseOfDll + pexports->AddressOfNames);
        void* addr2 = (void*)((int)lpBaseOfDll + pexports->AddressOfFunctions);
        auto sizeNames = pexports->NumberOfNames * pexports->NumberOfNames * sizeof(BYTE) * sizeof(BYTE);
        auto sizeFunctions = pexports->NumberOfFunctions * pexports->NumberOfFunctions * sizeof(BYTE) * sizeof(BYTE);
        void* names = malloc(sizeNames);
        void* functions = malloc(sizeFunctions);
        ReadProcessMemory(debugProcess, addr1, names, sizeNames, NULL);
        ReadProcessMemory(debugProcess, addr2, functions, sizeFunctions, NULL);
        BYTE** bnames = (BYTE**)(names);
        BYTE** bfunctions = (BYTE**)(functions);
        //std::ofstream out("out.txt", std::ios::app);
        //заносим функции из таблицы экспорта в addressInfo
        for (int i = 0; i < pexports->NumberOfNames; i++) {

            addr1 = (void*)((BYTE*)lpBaseOfDll + (int)bnames[i]);
            addr2 = (void*)((BYTE*)lpBaseOfDll + (int)bfunctions[i]);
            void* name = malloc(100 * 4);
            void* addressFun = malloc(100 * 4);
            ReadProcessMemory(debugProcess, addr1, name, 100 * 4, NULL);
            ReadProcessMemory(debugProcess, addr2, addressFun, 100 * 4, NULL);

            std::string info = "LIBRARY: " + LibraryName + " FUNCTION NAME: " + (char*)name;
            int absoluteAddr = (int)lpBaseOfDll + (int)addressFun;
            std::wstringstream ss;
            ss << LibraryName.c_str();
            auto wstr = ss.str();
            auto v = GetProcAddress(GetModuleHandle(wstr.c_str()), (char*)name);
            addresInfo.insert({ (void*)v,info });
        }
        //out.close();
	}

    void setLibraryBreakpoints(HANDLE debugProcess, std::list<BreakPoint>& points, DWORD tid) {
        if (!isWorking) {
            return;
        }
        if (isFirstTimeSet) {
            for (auto i = addresInfo.begin(); i != addresInfo.end(); i++) {
                BreakPoint::SetBreakpoint(debugProcess, points, (*i).first, BreakPoint::TYPE::SOFT, tid);
            }
            isFirstTimeSet = false;
        }
    }

    void saveDeleteBreakpointFunction(HANDLE debugProcess, DWORD64 tid, void* addr) {
        if (findAddr(addr) && isWorking) {
            //сохраняем адрес последней удалённой точки останова функции
            lastBreakpointAddress.push(addr);
                        
            //получаем контекст и ставим флаг трассировки в него 
            ContextHelper::SetFlagTraceInContext(tid);
            //вывод о функции
            outInfoAboutFunction(debugProcess, tid, addr);
        }
    }

    void resetDeleteBreakpoint(HANDLE debugProcess, std::list<BreakPoint>& list, DWORD64 tid) {
        if (lastBreakpointAddress.size() > 0 && isWorking) {
            //востанавливаем точку останова функции 
            BreakPoint::SetBreakpoint(debugProcess, list, lastBreakpointAddress.top(), BreakPoint::TYPE::SOFT, tid);
            lastBreakpointAddress.pop();
        }
    }

    void outInfoAboutFunction(HANDLE debugProcess, DWORD64 tid, void* addr) {
        if (!isWorking)
            return;
        outLogger->write("ADDR: " + std::to_string((DWORD64)addr) + " " + addresInfo[addr] + "\n");
        auto traceAndArgs = GetStackTrace(debugProcess, tid);
        outLogger->write("COUNT ARGS (MAYBY?): " + std::to_string(traceAndArgs.second) + "\n");
        if (traceAndArgs.second > 0) {
            outLogger->write("MAYBY stdecl OR cdecl NOTATION. SEE A INSTRUCTION TYPE ret.\n");
        }
        else {
        #ifndef _AMD64_
            std::string notation = " m_fastcall ";
        #else
            std::string notation = " fastcall ";
        #endif;
            outLogger->write("MAYBY" + notation + "OR PROCEDURE\n");
        }
        outLogger->write(traceAndArgs.first);
    }
};


class BuilderModuleDumpLibraryFunction {
private:
	ModuleDumpLibraryFunction ans;
public:
    BuilderModuleDumpLibraryFunction outLogger(Logger* logger) {
        ans.outLogger = logger;
        return *this;
    }
    BuilderModuleDumpLibraryFunction stacktraceDeep(int deep) {
        ans.stacktraceDeep = deep;
        return *this;
    }
    BuilderModuleDumpLibraryFunction isWorking(bool isWorking) {
        ans.isWorking = isWorking;
        return *this;
    }
	ModuleDumpLibraryFunction build() {
		return ans;
	}
};


#endif;
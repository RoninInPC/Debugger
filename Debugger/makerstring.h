#pragma once
#ifndef _MAKERSTRING_H_
#define _MAKERSTRING_H_
#include<string>
#include<windows.h>
#include<vector>
#include<sstream>
#include<chrono>
#include<ctime>
#include<format>


#include"disas.h"
//класс создающий строки из разных данных
class MakerString {
public:
    static std::string OutInfo(const std::string& nameFun, const std::string& nameNum, void* num) {
        std::string ans = nameFun + ":\n";
        ans += nameNum + "\n";
        ans += MakeHexNum((DWORD64)num) + "\n";
        return ans;
    }

    static std::string MakeNameOfFile(const std::string& nameProcess, const std::string& nameModule,const std::string& type) {

        time_t rawtime;
        struct tm timeinfo;
        char buffer[80];                                // строка, в которой будет храниться текущее время

        time(&rawtime);                               // текущая дата в секундах
        localtime_s(&timeinfo,&rawtime);               // текущее локальное время, представленное в структуре

        strftime(buffer, 80, "%I_%M%p", &timeinfo);

        std::string str(buffer);

        int num = 0;
        for (int i = 1; i < nameProcess.size(); i++) {
            if (nameProcess[i] == '\\') {
                num = i;
            }
        }

        if (nameProcess[num] == '\\')
            num++;
        std::string ans = nameProcess.substr(num,nameProcess.size())+ "_" + nameModule +"_" + str + "." + type;
        return ans;
    }

    static std::pair<int, std::vector<std::string>> DisasDebugProcess(HANDLE debugProcess, void* addr, DWORD64 instCount) {
        DWORD64 size = 15 * instCount;
        PBYTE buf = (PBYTE)malloc(size);
        unsigned int i;
        unsigned int offset = 0;

        if (!buf) {
            return { 0,{} };
        }

        ReadProcessMemory(debugProcess, addr, buf, size, NULL);
        std::vector<std::string> ans;

        for (i = 0; i < instCount; ++i) {
            char asmBuf[128];
            char hexBuf[128];
            unsigned int len = DisasInstruction(&buf[offset], size - offset, (UINT_PTR)addr + offset, asmBuf, hexBuf);
            if (!len) {
                break;
            }
            if(instCount==1)
                //printf("%p: %-16s %s\n", (UINT_PTR)addr + offset, hexBuf, asmBuf);
            ans.push_back("ADDR: " + MakeHexNum((UINT_PTR)addr + offset) + " CODE: " + hexBuf + " " + asmBuf);
            offset += len;
        }

        return { offset,ans };
    }

    static std::string GetStringFromHandle(HANDLE handle) {
        WCHAR name[200];
        GetFinalPathNameByHandle(handle, name, 200, FILE_NAME_NORMALIZED);
        std::wstring copy(name);
        std::string ans(copy.begin(), copy.end());

        int num = 0;
        for (int i = 0; i < ans.size(); i++) {
            if (isalpha(ans[i])) {
                num = i;
                break;
            }
        }

        return ans.substr(num, ans.size());


    }

    static std::string MakeHexNum(DWORD64 dword) { //перевод 10-го числа в 16-ое

        std::stringstream ss;
        ss << std::hex << dword;
        std::string str(ss.str());

        for (int i = 0; i < str.size(); i++) {
            str[i] = std::toupper(str[i]);
        }
        return str;
    }
public:

    static std::string MakeRegisterContext(CONTEXT& context) {
        //сборка строки из значения всех регистров для 32-х и 64-х файлов
#ifndef _AMD64_
        std::vector<std::string> baseRegisters{ "Eax", "Ebx", "Ecx", "Edx","Edi", "Esi", "Ebp", "Esp","Eip" };
#else
        std::vector<std::string> baseRegisters{ "Rax", "Rbx", "Rcx", "Rdx","Rdi", "Rsi", "Rbp", "Rsp","Rip" };
#endif

        std::string ans;
        ans += baseRegisters[0] + "=";
#ifndef _AMD64_
        ans += MakeHexNum(context.Eax);
#else       
        ans += MakeHexNum(context.Rax);
#endif
        ans += " " + baseRegisters[1] + "=";
#ifndef _AMD64_
        ans += MakeHexNum(context.Ebx);
#else       
        ans += MakeHexNum(context.Rbx);
#endif
        ans += " " + baseRegisters[2] + "=";
#ifndef _AMD64_
        ans += MakeHexNum(context.Ecx);
#else       
        ans += MakeHexNum(context.Rcx);
#endif
        ans += " " + baseRegisters[3] + "=";
#ifndef _AMD64_
        ans += MakeHexNum(context.Edx);
#else       
        ans += MakeHexNum(context.Rdx);
#endif
        ans += " " + baseRegisters[4] + "=";
#ifndef _AMD64_
        ans += MakeHexNum(context.Edi);
#else       
        ans += MakeHexNum(context.Rdi);
#endif
        ans += " " + baseRegisters[5] + "=";
#ifndef _AMD64_
        ans += MakeHexNum(context.Esi);
#else       
        ans += MakeHexNum(context.Rsi);
#endif
        ans += " " + baseRegisters[6] + "=";
#ifndef _AMD64_
        ans += MakeHexNum(context.Ebp);
#else       
        ans += MakeHexNum(context.Rbp);
#endif
        ans += " " + baseRegisters[7] + "=";
#ifndef _AMD64_
        ans += MakeHexNum(context.Esp);
#else       
        ans += MakeHexNum(context.Rsp);
#endif
        ans += " " + baseRegisters[8] + "=";
#ifndef _AMD64_
        ans += MakeHexNum(context.Eip);
#else       
        ans += MakeHexNum(context.Rip);
#endif
        return ans;
    }

};


#endif
#include "debugger.h"

bool ConsoleDebugger::CreateDebugProces(const std::string& procName){
    STARTUPINFO startupInfo;
    PROCESS_INFORMATION procInfo;
    bool ret;


    RtlZeroMemory(&startupInfo, sizeof(startupInfo));
    RtlZeroMemory(&procInfo, sizeof(procInfo));
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_SHOWNORMAL;

    std::wstring name(procName.begin(), procName.end());

    ret = CreateProcess(name.c_str(),
        NULL,
        NULL,
        NULL,
        TRUE,
        DEBUG_ONLY_THIS_PROCESS,
        NULL,
        NULL,
        &startupInfo,
        &procInfo);

    this->debugProcess = procInfo.hProcess;
    this->nameProcess = procName;

    //инициализация модулей
    this->moduleOne = BuilderFindModuleInstraction().outLogger(LoggerFactory::MakeLogger(
        MakerString::MakeNameOfFile(nameProcess, "moduleOne", "txt")))
        .isWorking(isWorkingOne).build();

    this->moduleTwo = BuilderModuleDumpListFunction().outLogger(LoggerFactory::MakeLogger(
        MakerString::MakeNameOfFile(nameProcess, "moduleTwo", "txt")))
        .parsing(2000, 20, 20)
        .isWorking(isWorkingTwo).build();

    this->moduleThree = BuilderModuleTraceFunction().outLogger(LoggerFactory::MakeLogger(
        MakerString::MakeNameOfFile(nameProcess, "moduleThree", "txt")))
        .isWorking(isWorkingThree).build();

    this->moduleFour = BuilderModuleDumpLibraryFunction().outLogger(LoggerFactory::MakeLogger(
        MakerString::MakeNameOfFile(nameProcess, "moduleFour", "txt")))
        .isWorking(isWorkingFour).build();

    CloseHandle(procInfo.hThread);

    return ret;
}

bool ConsoleDebugger::SetBreakpoint(DWORD64 tid, void* addr, BreakPoint::TYPE type)
{
    return BreakPoint::SetBreakpoint(this->debugProcess, this->breakPointList, addr, type, tid);
}

bool ConsoleDebugger::DeleteBreakpoint(DWORD64 tid, void* addr)
{
    return BreakPoint::DeleteBreakpoint(this->debugProcess, this->breakPointList, addr, tid);
}

void ConsoleDebugger::EventCreateProcess(DWORD64 pid, DWORD64 tid, LPCREATE_PROCESS_DEBUG_INFO procDebugInfo){
    MakerString::DisasDebugProcess(this->debugProcess,procDebugInfo->lpStartAddress, 5);

    SetBreakpoint(tid, procDebugInfo->lpStartAddress, BreakPoint::TYPE::SOFT);
    char buf[200],buf2[200];
    snprintf(buf, sizeof(buf), "CREATE PROCESS %d(%d) %p\n", pid, tid, procDebugInfo->lpImageName);
    std::string str(buf);
    logger->write(str);
    snprintf(buf2, sizeof(buf2), " BASE: %p  START: %p\n", procDebugInfo->lpBaseOfImage, procDebugInfo->lpStartAddress);
    std::string str2(buf2);
    logger->write(str2);
}

void ConsoleDebugger::EventExitProcess(DWORD64 pid, DWORD64 tid, LPEXIT_PROCESS_DEBUG_INFO procDebugInfo){
    char buf[200];
    snprintf(buf, sizeof(buf), "EXIT PROCESS %d(%d) CODE: %d\n", pid, tid, procDebugInfo->dwExitCode);
    std::string str(buf);
    logger->write(str);
}

void ConsoleDebugger::EventCreateThread(DWORD64 pid, DWORD64 tid, LPCREATE_THREAD_DEBUG_INFO threadDebugInfo){
    auto thread = BuilderThread().pid(pid).tid(tid).addr(threadDebugInfo->lpStartAddress).build();
    this->threadList.push_back(thread);
    logger->write("THREAD LOAD " + thread.getInfo());
}

bool ConsoleDebugger::SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if (!LookupPrivilegeValue(
        NULL,            // lookup privilege on local system
        lpszPrivilege,   // privilege to lookup 
        &luid))        // receives LUID of privilege
    {
        return false;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    // Enable the privilege or disable all privileges.

    if (!AdjustTokenPrivileges(
        hToken,
        false,
        &tp,
        sizeof(TOKEN_PRIVILEGES),
        (PTOKEN_PRIVILEGES)NULL,
        (PDWORD)NULL))
    {
       
        return false;
    }

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
    {
        return false;
    }

    return true;
}

void ConsoleDebugger::EventExitThread(DWORD64 pid, DWORD64 tid, LPEXIT_THREAD_DEBUG_INFO threadDebugInfo){
    auto thread = Thread::findInListByPidAndTid(this->threadList, pid, tid);
    bool ans = Thread::deleteInListByPidAndTid(this->threadList, pid, tid);
    if (ans)
        logger->write("THREAD UNLOAD " + thread.getInfo());
}

DWORD64 ConsoleDebugger::FindProcessId(const std::string& processName){
    uintptr_t processID = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pe32{};
        pe32.dwSize = sizeof(pe32);

        if (Process32First(hSnap, &pe32))
        {
            do
            {
                if (!_strcmpi(processName.c_str(), (const char*)pe32.szExeFile))
                {
                    processID = pe32.th32ProcessID;
                    break;
                }

            } while (Process32Next(hSnap, &pe32));
        }
    }

    if (hSnap)
        CloseHandle(hSnap);

    return processID;
}

bool ConsoleDebugger::DebugActiveProces(int procID){
    SetPrivilege(GetCurrentProcess(), SE_DEBUG_NAME, true);

    this->nameProcess = std::to_string(procID);
    this->debugProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
    this->isActive = true;

    //инициализация модулей
    this->moduleOne = BuilderFindModuleInstraction().outLogger(LoggerFactory::MakeLogger(
        MakerString::MakeNameOfFile(nameProcess, "moduleOne", "txt")))
        .isWorking(isWorkingOne).build();

    this->moduleTwo = BuilderModuleDumpListFunction().outLogger(LoggerFactory::MakeLogger(
        MakerString::MakeNameOfFile(nameProcess, "moduleTwo", "txt")))
        .parsing(2000, 20, 20)
        .isWorking(isWorkingTwo).build();
    
    this->moduleThree = BuilderModuleTraceFunction().outLogger(LoggerFactory::MakeLogger(
        MakerString::MakeNameOfFile(nameProcess, "moduleThree", "txt")))
        .isWorking(isWorkingThree).build();

    this->moduleFour = BuilderModuleDumpLibraryFunction().outLogger(LoggerFactory::MakeLogger(
        MakerString::MakeNameOfFile(nameProcess, "moduleFour", "txt")))
        .isWorking(isWorkingFour).build();

    return DebugActiveProcess(procID);
}

void ConsoleDebugger::EventLoadDll(DWORD64 pid, DWORD64 tid, LPLOAD_DLL_DEBUG_INFO dllDebugInfo){
    auto libraryName = MakerString::GetStringFromHandle(dllDebugInfo->hFile);
    
    auto library = BuilderLibrary().pid(pid).tid(tid).
        addr(dllDebugInfo->lpBaseOfDll).name(libraryName).build();
    this->libraryList.push_back(library);

    moduleFour.addLibraryFunction(debugProcess, dllDebugInfo->lpBaseOfDll, libraryName);
     
    logger->write("LIBRARY LOAD " + library.getInfo());
}

void ConsoleDebugger::EventUnloadDll(DWORD64 pid, DWORD64 tid, LPUNLOAD_DLL_DEBUG_INFO dllDebugInfo){
    auto library = Library::findInListByAddr(this->libraryList, dllDebugInfo->lpBaseOfDll);
    bool ans = Library::deleteInListByAddr(this->libraryList, dllDebugInfo->lpBaseOfDll);
    if (ans)
        logger->write("LIBRARY UNLOAD " + library.getInfo());
}

void ConsoleDebugger::EventOutputDebugString(DWORD64 pid, DWORD64 tid, LPOUTPUT_DEBUG_STRING_INFO debugStringInfo){
    char buf[200];
    snprintf(buf, sizeof(buf), "Debug string %d(%d) %p\n", pid, tid, debugStringInfo->lpDebugStringData);
    std::string str(buf);
    logger->write(str);
}

void ConsoleDebugger::RequestAction(void){
    std::string ans;
    BOOL isRet = FALSE;

    // если до этого выбрали одно из перманентных действий
    if (this->isRun) {
        return;
    }

    while (!isRet) {
        std::cin >> ans;
        switch (ans[0]) {
        case 'B':
            this->isRun = TRUE;
        case 'b':
            this->state = DEBUGGER_STATE::BREAK_SOFT;
            isRet = TRUE;
            break;

        /*case 'H':
            this->isRun = TRUE;
        case 'h':
            this->state = DEBUGGER_STATE::BREAK_HARD;
            isRet = TRUE;
            break;*/
        case 'O':
            switch (ans[1]) {
            case 'L':

                logger->write("LIBRARIES:\n");
                for (auto a = this->libraryList.begin(); a != this->libraryList.end(); a++) {
                    logger->write(" " + (*a).getInfo());
                }
                logger->write("\n");

                break;
            case 'T':

                logger->write("THREADS:\n");
                for (auto a = this->threadList.begin(); a != this->threadList.end(); a++) {
                    logger->write(" " + (*a).getInfo());
                }
                logger->write("\n");

                break;
            default:
                break;
            }

            break;

        case 'T':
            this->isRun = TRUE;
        case 't':
            this->state = DEBUGGER_STATE::TRACE;
            this->isTrace = TRUE;
            isRet = TRUE;
            break;

        case 'r':
            this->isRun = TRUE;
            this->state = DEBUGGER_STATE::TRACE;
            this->isTrace = 1;
            isRet = TRUE;
            break;

        case '?':
            puts("B - permanent soft breakpoint\n"
                "b - once soft breakpoint\n"
                //"H - permanent hard breakpoint\n"
                //"h - once soft breakpoint\n"
                "T - permanent trap flag\n"
                "t - once trap flag\n"
                "r - run\n");
            break;

        default:
            break;

        }
    }

    return;

}

DWORD64 ConsoleDebugger::BreakpointEvent(DWORD64 tid, ULONG_PTR exceptionAddr) {
   
    DWORD64 continueFlag = DBG_EXCEPTION_NOT_HANDLED;
    HANDLE thread;
    CONTEXT context;

    auto breakPoint = BreakPoint::findInListByAddr(this->breakPointList, (void*)exceptionAddr);

    // если событие вызвано не нашими действиями,
    // то не обрабатываем его
    if (!this->isTrace && breakPoint.addr==0 && breakPoint.saveByte==0) {
        return DBG_EXCEPTION_NOT_HANDLED;
    }

    // если по этому адресу установлена точка останова, удаляем её
    if (breakPoint.addr != 0 && breakPoint.saveByte != 0) {
        DeleteBreakpoint(tid, (void*)exceptionAddr);
    }

    //модифицированная функция, возращающая строку(ки) инструкции и instSize
    auto instSizeAndString = MakerString::DisasDebugProcess(this->debugProcess, (PVOID)exceptionAddr, 1);
    size_t instSize = instSizeAndString.first;
    std::string instruction = instSizeAndString.second[0];


    thread = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, FALSE, tid);
    if (thread != INVALID_HANDLE_VALUE) {
        context.ContextFlags = CONTEXT_ALL;
        GetThreadContext(thread, &context);
        if (breakPoint.addr != 0 && breakPoint.saveByte != 0) {
   #ifdef _AMD64_
            context.Rip = exceptionAddr;
#else   // _AMD64_
            context.Eip = exceptionAddr;
#endif  // _AMD64_
            if (breakPoint.type == BreakPoint::TYPE::HARD) {
                context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
                context.Dr6 = 0;
                SetThreadContext(thread, &context);
            }
        }
    }

    //использование модуля, в котором есть база искомых инструкций + пишущий с помощью логгера в файл
    moduleOne.outputAboutInstruction(instruction, context);

    //использование модуля 2, в котором на данный момент ищется окончание функции (чтобы вывести возвращаемое значение)
    moduleTwo.seeCountCallAndRetAndOut(instruction, context);

    //использование модуля 3, в котором при помощи просматривания инструкций ищет функции
    moduleThree.workingModule(this->debugProcess, instruction, context);

    // запрашиваем дальнейшее действие
    RequestAction();

    switch (this->state) {
    case DEBUGGER_STATE::BREAK_HARD:
        SetBreakpoint(tid, (void*)(exceptionAddr + instSize), BreakPoint::TYPE::HARD);
        break;
    case DEBUGGER_STATE::BREAK_SOFT:
        SetBreakpoint(tid, (void*)(exceptionAddr + instSize), BreakPoint::TYPE::SOFT);
        break;
    case DEBUGGER_STATE::TRACE:

        context.EFlags |= 0x100;
        break;
    }

    if (thread != INVALID_HANDLE_VALUE) {
        context.ContextFlags = CONTEXT_CONTROL;
        SetThreadContext(thread, &context);
        CloseHandle(thread);
    }

    return DBG_CONTINUE;
}

DWORD64 ConsoleDebugger::EventException(DWORD64 pid, DWORD64 tid, LPEXCEPTION_DEBUG_INFO exceptionDebugInfo){
    DWORD64 continueFlag = (DWORD64)DBG_EXCEPTION_NOT_HANDLED;

    switch (exceptionDebugInfo->ExceptionRecord.ExceptionCode) {

        // программная точка останова
    case EXCEPTION_BREAKPOINT:

        continueFlag = BreakpointEvent(tid, (ULONG_PTR)exceptionDebugInfo->ExceptionRecord.ExceptionAddress);
        

        //выставление точек останова в библиотечных функциях (модуль два)
        moduleTwo.setLibraryBreakpoints(this->debugProcess, this->breakPointList, 0);
        //выставление точек останова в библиотечных функциях (модуль четыре)
        moduleFour.setLibraryBreakpoints(this->debugProcess, this->breakPointList, 0);

        //сохранение удалённой точки останова из библиотеки, 
        //выставление флага трассировки,вывод разобранных аргументов и адреса возврата (модуль два)
        moduleTwo.saveDeleteBreakpointFunction(this->debugProcess, tid, (void*)exceptionDebugInfo->ExceptionRecord.ExceptionAddress);
        
        //сохранение удалённой точки останова из библиотеки, 
        //выставление флага трассировки,вывод разобранных аргументов и адреса возврата (модуль четыре)
        moduleFour.saveDeleteBreakpointFunction(this->debugProcess, tid, (void*)exceptionDebugInfo->ExceptionRecord.ExceptionAddress);
        
        break;

        // флаг трассировки или аппаратная точка останова
    case EXCEPTION_SINGLE_STEP:
        continueFlag = BreakpointEvent(tid, (ULONG_PTR)exceptionDebugInfo->ExceptionRecord.ExceptionAddress);

        //восстановление точки останова на библиотечной функции
        moduleTwo.resetDeleteBreakpoint(this->debugProcess, this->breakPointList, tid);

        //восстановление точки останова на библиотечной функции
        moduleFour.resetDeleteBreakpoint(this->debugProcess, this->breakPointList, tid);
        break;

    case EXCEPTION_PRIV_INSTRUCTION:
        
        break;

    }
    auto exc = exceptionDebugInfo->ExceptionRecord.ExceptionCode;

    if(exc != 0x80000004 && exc != 0x80000003) //если это не код трассировки или точки останова, выводим исключение
        printf("  Exception %d(%d) %p: %x\n", pid, tid, exceptionDebugInfo->ExceptionRecord.ExceptionAddress, exc);

    return continueFlag;
}

void ConsoleDebugger::DebugRun(void){
    bool completed = false;
    bool attached = false;

    while (!completed) {
        DEBUG_EVENT debugEvent;
        DWORD64 continueFlag = DBG_CONTINUE;

        // ожидаем поступления отладочного сообщения
        if (!WaitForDebugEvent(&debugEvent, INFINITE)) {
            break;
        }

        switch (debugEvent.dwDebugEventCode) {
        case CREATE_PROCESS_DEBUG_EVENT:
            EventCreateProcess(debugEvent.dwProcessId, debugEvent.dwThreadId, &debugEvent.u.CreateProcessInfo);
            break;

        case EXIT_PROCESS_DEBUG_EVENT:
            EventExitProcess(debugEvent.dwProcessId, debugEvent.dwThreadId, &debugEvent.u.ExitProcess);
            completed = TRUE;
            break;

        case CREATE_THREAD_DEBUG_EVENT:
            EventCreateThread(debugEvent.dwProcessId, debugEvent.dwThreadId, &debugEvent.u.CreateThread);
            break;

        case EXIT_THREAD_DEBUG_EVENT:
            EventExitThread(debugEvent.dwProcessId, debugEvent.dwThreadId, &debugEvent.u.ExitThread);
            break;

        case LOAD_DLL_DEBUG_EVENT:
            EventLoadDll(debugEvent.dwProcessId, debugEvent.dwThreadId, &debugEvent.u.LoadDll);
            break;

        case UNLOAD_DLL_DEBUG_EVENT:
            EventUnloadDll(debugEvent.dwProcessId, debugEvent.dwThreadId, &debugEvent.u.UnloadDll);
            break;

        case OUTPUT_DEBUG_STRING_EVENT:
            EventOutputDebugString(debugEvent.dwProcessId, debugEvent.dwThreadId, &debugEvent.u.DebugString);
            break;

        case EXCEPTION_DEBUG_EVENT:
            if (!attached) {
                // Первое исключение для начала отладки
                attached = TRUE;
            }
            else {
                continueFlag = EventException(debugEvent.dwProcessId, debugEvent.dwThreadId, &debugEvent.u.Exception);
            }
            break;

        default:
            logger->write("Unexpected debug event: " + std::to_string(debugEvent.dwDebugEventCode) + "\n");
        }

        if (!ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, continueFlag)) {
            logger->write("Error continuing debug event\n");
        }
    }
    if (this->isActive) {//если активный, то останавливаем отладку
        DebugActiveProcessStop(std::stoi((nameProcess)));
        return;
    }

    CloseHandle(this->debugProcess);

    return;

}

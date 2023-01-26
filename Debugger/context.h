#pragma once
#ifndef _CONTEXT_H_
#define _CONTEXT_H_
#include<Windows.h>

class ContextHelper {

public:
    static CONTEXT GetContext(DWORD64 tid)
    {
        CONTEXT context;
        auto thread = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, FALSE, tid);
        if (thread != INVALID_HANDLE_VALUE) {
            context.ContextFlags = CONTEXT_ALL;
            GetThreadContext(thread, &context);
            context.ContextFlags = CONTEXT_CONTROL;
            SetThreadContext(thread, &context);
            CloseHandle(thread);
        }
        return context;
    }
    
    static CONTEXT SetFlagTraceInContext(DWORD64 tid)
    {
        CONTEXT context;
        auto thread = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, FALSE, tid);
        if (thread != INVALID_HANDLE_VALUE) {
            context.ContextFlags = CONTEXT_ALL;
            GetThreadContext(thread, &context);
            context.EFlags |= 0x100;
            context.ContextFlags = CONTEXT_CONTROL;
            SetThreadContext(thread, &context);
            CloseHandle(thread);
        }
        return context;
    }

};


#endif;

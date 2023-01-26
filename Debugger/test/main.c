/*
    ������ ������������� SEH ��� ��������������� ����.

    ������ ���� �������������       24.09.2014
*/


#include <windows.h>
#include <stdio.h>


#define CODE_SIZE   100
BYTE code[CODE_SIZE];


//
// �������� ����������� SEH.
//
EXCEPTION_DISPOSITION
__cdecl
_except_handler(
 struct _EXCEPTION_RECORD *ExceptionRecord,
 void * EstablisherFrame,
 struct _CONTEXT *ContextRecord,
 void * DispatcherContext ) {
    return ExceptionContinueSearch;
}


//
// ������� ������������� ������ ���������
// ����������� SEH �������.
//
void SomeFun(void) {

DWORD handler = (DWORD)_except_handler;

    __asm {                // ������� ��������� EXCEPTION_REGISTRATION:
        push    offset _except_handler    // ����� ������� ������ ����������� ����������.
        push    FS:[0]     // ����� ����������� EXECEPTION_REGISTRATION.
        mov     FS:[0],ESP // ��������� � ��������� ������ EXECEPTION_REGISTRATION.
        }

    // ���������� ���
    // ...

    __asm {                  // ������� EXECEPTION_REGISTRATION �� ���������� ������.
        mov     eax,[ESP]    // �������� ��������� �� ����������
                             // EXECEPTION_REGISTRATION.
        mov     FS:[0], EAX  // ������������� � ������ ������ ����������
                             // EXECEPTION_REGISTRATION.
        add     esp, 8       // ������� �� ����� ��������� EXECEPTION_REGISTRATION.
        }
}


// 
// ����������, ���������� ����������
// ��� ������� ���������� ���� �� ������.
//
int _except_handler_main (EXCEPTION_POINTERS *er) {

    printf ("Exception by address %x\n", er->ContextRecord->Eip);

    //*
    // �������� ������� ���������� � cli �� nop
    *((BYTE*)er->ContextRecord->Eip) = 0x90;
    // ���������� ���������� �������� ������� � nop �� cli
    *((BYTE*)er->ContextRecord->Eip-1) = 0xfa;
    //*/

    // ������ �� ��� ��� ���������� ��������� ��������� ���������� ����������.
    return EXCEPTION_CONTINUE_EXECUTION;

    // �������� ���������� � ���� __except
    //return EXCEPTION_EXECUTE_HANDLER;
}



int main() {

DWORD i;
DWORD oldProtect;

    // �������� ����� ������������ cli (����� 0xFA)
    for (i = 0; i < CODE_SIZE; ++i) {
        code[i] = 0xFA;
        }
    // ������������� ����� �� ����������
    VirtualProtect (code, 10, PAGE_EXECUTE_READWRITE, &oldProtect);
    printf ("******************************************%08X\n", _except_handler_main);

    // � ����� _try ����������� ���������� ���
    __try {
        // ������� ���������� ���� � ������,
        // ��� ������� ���������� ������ ���������� �� ������ (cli)
        // ��������� ����������
        ((void(*)(void)) code)();
        // � ���� ���������������� ���� ��������� ����� ��������
        // �� ������ code (������ ���������� cli), ������������ �� ���,
        // � ������ ������������ �� ����� (������ �������������� nop)
        }

    // ���������� ��������� � ���������� ���� �������������� � ����� __except
    __except (_except_handler_main (GetExceptionInformation())) {
        // ���� ���� ������� ����������, ���� ������� ����� EXCEPTION_EXECUTE_HANDLER
        printf ("�������������� ����������\n");
        }

    return 0;
}

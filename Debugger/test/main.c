/*
    Пример использования SEH для самотрассировки кода.

    Маткин Илья Александрович       24.09.2014
*/


#include <windows.h>
#include <stdio.h>


#define CODE_SIZE   100
BYTE code[CODE_SIZE];


//
// Прототип обработчика SEH.
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
// Функция демонстрирует пример установки
// обработчика SEH вручную.
//
void SomeFun(void) {

DWORD handler = (DWORD)_except_handler;

    __asm {                // Создаем структуру EXCEPTION_REGISTRATION:
        push    offset _except_handler    // Адрес функции нашего обработчика исключений.
        push    FS:[0]     // Адрес предыдущего EXECEPTION_REGISTRATION.
        mov     FS:[0],ESP // Добавляем в связанный список EXECEPTION_REGISTRATION.
        }

    // защищаемый код
    // ...

    __asm {                  // Удаляем EXECEPTION_REGISTRATION из связанного списка.
        mov     eax,[ESP]    // Получаем указатель на предыдущий
                             // EXECEPTION_REGISTRATION.
        mov     FS:[0], EAX  // Устанавливаем в начале списка предыдущий
                             // EXECEPTION_REGISTRATION.
        add     esp, 8       // Удаляем из стека структуру EXECEPTION_REGISTRATION.
        }
}


// 
// Обработчик, получающий управление
// при попытке исполнения кода из буфера.
//
int _except_handler_main (EXCEPTION_POINTERS *er) {

    printf ("Exception by address %x\n", er->ContextRecord->Eip);

    //*
    // изменяем текущую инструкцию с cli на nop
    *((BYTE*)er->ContextRecord->Eip) = 0x90;
    // предыдущую инструкцию изменяем обратно с nop на cli
    *((BYTE*)er->ContextRecord->Eip-1) = 0xfa;
    //*/

    // Просим ОС еще раз попытаться выполнить вызвавшую исключение инструкцию.
    return EXCEPTION_CONTINUE_EXECUTION;

    // Передаем управление в блок __except
    //return EXCEPTION_EXECUTE_HANDLER;
}



int main() {

DWORD i;
DWORD oldProtect;

    // забиваем буфер инструкциями cli (опкод 0xFA)
    for (i = 0; i < CODE_SIZE; ++i) {
        code[i] = 0xFA;
        }
    // устанавливаем права на исполнение
    VirtualProtect (code, 10, PAGE_EXECUTE_READWRITE, &oldProtect);
    printf ("******************************************%08X\n", _except_handler_main);

    // в блоке _try выполняется защищаемый код
    __try {
        // передаём управление коду в буфере,
        // при попытке исполнения первой инструкции из буфера (cli)
        // произойдёт исключение
        ((void(*)(void)) code)();
        // в окне дизассемблерного кода поставить точку останова
        // на начало code (первую инструкцию cli), остановиться на ней,
        // а дальше трассировать по шагам (увидим перемещающийся nop)
        }

    // исключение возникшие в защищаемом коде обрабатываются в блоке __except
    __except (_except_handler_main (GetExceptionInformation())) {
        // этот блок получит управление, если функция вернёт EXCEPTION_EXECUTE_HANDLER
        printf ("Необработанное исключение\n");
        }

    return 0;
}

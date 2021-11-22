#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <istream>
#include <sstream>
#include <ctime>
#include <ratio>
#include <chrono>

#include "detours.h"
#include "Retcheck.h"
#include "Addresses.h"
#include "Convert.h"

std::chrono::high_resolution_clock::time_point t1;

uintptr_t RS = 0;
lua_State* LS;

uintptr_t Des = ASLR(0x1747B50);

std::string ToHex(unsigned int a) {
    std::stringstream ss;
    ss << std::hex << a;
    std::string r = "";

    if (ss.str().length() == 1) {
        r = "0" + ss.str();
    }
    else {
        r = ss.str();
    }

    return r;
}

int Execute(std::string Bytecode) {
    for (int i = 0; i < Bytecode.size(); ++i) {
        std::cout << ToHex(static_cast<int>(Bytecode.c_str()[i])) << " ";
    }
    std::cout << "\n";

    Retcheck::EnableBypass();
    Deserializer(RS, "CHUNK", Bytecode.c_str(), Bytecode.size(), 0);
    Spawn(RS);
    Retcheck::DisableBypass();
  
    return 1;
}

/*DWORD WINAPI input(PVOID lvpParameter)
{
    std::string WholeScript = "";
    HANDLE hPipe;
    char buffer[999999];
    DWORD dwRead;
    hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\Phase"),
        PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
        PIPE_WAIT,
        1,
        999999,
        999999,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL);
    while (hPipe != INVALID_HANDLE_VALUE)
    {
        if (ConnectNamedPipe(hPipe, NULL) != FALSE)
        {
            while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
            {
                buffer[dwRead] = '\0';
                try {
                    try {
                        WholeScript = WholeScript + buffer;
                    }
                    catch (...) {
                    }
                }
                catch (std::exception e) {

                }
                catch (...) {

                }
            }
            if (luaL_loadstring(LS, WholeScript.c_str())) {

                std::cout << "Error!" << "\n";
            }
            else {

                std::string Bytecode = "";

                LClosure LC = (LS->top - 1)->value.gc->cl.l;
                Proto* P = LC.p;

                PushByte(1, Bytecode);

                std::vector<std::string> ktable = {};

                ConvertConstants(P, ktable);
                for (int i = 0; i < P->sizep; ++i) {
                    ConvertConstants(P->p[i], ktable);
                }

                PushByte(ktable.size(), Bytecode);

                for (int i = 0; i < ktable.size(); ++i) {
                    PushByte(ktable[i].size(), Bytecode);
                    Bytecode += ktable[i];
                }

                std::string bruhh = "";

                ConvertProto(P, bruhh, true, ktable);
                for (int i = 0; i < P->sizep; ++i)
                    ConvertProto(P->p[i], bruhh, false, ktable);

                PushByte(P->is_vararg - 1, Bytecode);
                PushByte(P->maxstacksize, Bytecode);
                PushByte(P->nups, Bytecode);
                PushByte(P->numparams, Bytecode);
                PushByte(P->sizep + 1, Bytecode);

                Bytecode += bruhh;




                for (int i = 0; i < 6; ++i) {
                    PushByte(0, Bytecode);
                }

                Execute(Bytecode);
            }

            WholeScript = "";
        }
        DisconnectNamedPipe(hPipe);
    }
}*/

void SetIdentity(DWORD RS, int Identity) {
    *reinterpret_cast<uint8_t*>(*reinterpret_cast<uintptr_t*>(RS + 0x6C) + 0x18) = Identity;
}

void Init() {
    if (RS == 0) {
        std::cout << "Error: You have an extremely shit computer!" << "\n";
    }
    else {

        Retcheck::Init();
        std::cout << "Lua State: " << RS << "\n";
        LS = luaL_newstate();
        //SetIdentity(RS, 6);
        std::string Script = R"(
local o = "hello" print(o)
        )";
        if (luaL_loadstring(LS, Script.c_str())) {

            std::cout << "Error!" << "\n";
        }
        else {
            t1 = std::chrono::high_resolution_clock::now();
            std::string Bytecode = "";

            LClosure LC = (LS->top - 1)->value.gc->cl.l;
            Proto* P = LC.p;

            PushByte(1, Bytecode);

            std::vector<std::string> ktable = {};

            ConvertConstants(P, ktable);
            for (int i = 0; i < P->sizep; ++i) {
                ConvertConstants(P->p[i], ktable);
            }

            PushByte(ktable.size(), Bytecode);

            for (int i = 0; i < ktable.size(); ++i) {
                PushByte(ktable[i].size(), Bytecode);
                Bytecode += ktable[i];
            }

            std::string bruhh = "";

            ConvertProto(P, bruhh, true, ktable);
            for (int i = 0; i < P->sizep; ++i)
                ConvertProto(P->p[i], bruhh, false, ktable);

            PushByte(P->is_vararg - 1, Bytecode);
            PushByte(P->maxstacksize, Bytecode);
            PushByte(P->nups, Bytecode);
            PushByte(P->numparams, Bytecode);
            PushByte(P->sizep + 1, Bytecode);

            Bytecode += bruhh;




            for (int i = 0; i < 6; ++i) {
                PushByte(0, Bytecode);
            }
             std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

  std::cout << "It took me " << time_span.count() << " seconds.";
  std::cout << std::endl;
            Execute(Bytecode);
        }
        
    }
}

char* __fastcall Idx2AdrHook(int a1, int a2) {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&reinterpret_cast<void*&>(Index2Addr), &Idx2AdrHook);
    DetourTransactionCommit();
    RS = a1;

    Init();

    return index2adr_(a1, a2);
}

void __fastcall DesHook(int a1, const char* a2, const char* a3, int a4, int a5) {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&reinterpret_cast<void*&>(Des), &DesHook);
    DetourTransactionCommit();

    for (int i = 0; i < a4; ++i) {
        std::cout << ToHex(static_cast<unsigned int>(a3[i])) << " ";
    }
    std::cout << "\n";

}


int main() {
  
    DWORD nOldProtect;
    if (!VirtualProtect(FreeConsole, 1, PAGE_EXECUTE_READWRITE, &nOldProtect))
        return FALSE;
    *(BYTE*)(FreeConsole) = 0xC3;
    if (!VirtualProtect(FreeConsole, 1, nOldProtect, &nOldProtect))
        return FALSE;
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "r", stdin);
    HWND ConsoleHandle = GetConsoleWindow();
    SetWindowPos(ConsoleHandle, HWND_TOPMOST, 50, 20, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);


    bool DebugDes = false;

    if (DebugDes) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&reinterpret_cast<void*&>(Des), &DesHook);
        DetourTransactionCommit();
    }
    else {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&reinterpret_cast<void*&>(Index2Addr), &Idx2AdrHook);
        DetourTransactionCommit();
    }

}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)main, 0, 0, 0);
        break;
    default:
        break;
    }
    return TRUE;
}


#include <Windows.h>
#include <iostream>
//56 8B F1 80 7E 07 00 75 LuaD_call

uint64_t ASLR(uint64_t address);

typedef int(__fastcall* tdes)(int v1, const char* v2, const char* v3, int v4, int v5);
typedef int(__cdecl* tspawn)(int v1);
typedef char* (__fastcall* r_index2adr_typedef)(int v1, int v2);

extern tdes Deserializer;
extern r_index2adr_typedef index2adr_;
extern tspawn Spawn;
extern uintptr_t Index2Addr;
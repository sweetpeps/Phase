#include "Retcheck.h"
#include "Addresses.h"
#include <Windows.h>
#include <iostream>

namespace Retcheck {
    uint64_t Address = ASLR(0x636700);
    byte oldbyte;
    DWORD old;

    void Init() {
        oldbyte = *reinterpret_cast<byte*>(Address);
    }

    void EnableBypass() {
        VirtualProtect(reinterpret_cast<void*>(Address), 1, PAGE_EXECUTE_READWRITE, &old);
        *reinterpret_cast<byte*>(Address) = 0xC3;
        VirtualProtect(reinterpret_cast<void*>(Address), 1, old, &old);
    }

    void DisableBypass() {
        VirtualProtect(reinterpret_cast<void*>(Address), 1, PAGE_EXECUTE_READWRITE, &old);
        *reinterpret_cast<byte*>(Address) = oldbyte;
        VirtualProtect(reinterpret_cast<void*>(Address), 1, old, &old);
    }
}
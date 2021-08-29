#include "Addresses.h"

uint64_t ASLR(uint64_t address) {
	return address - 0x400000 + (uint64_t)GetModuleHandleA(0);
}

tdes Deserializer = reinterpret_cast<tdes>((ASLR(0x1747B50)));
tspawn Spawn = reinterpret_cast<tspawn>((ASLR(0x7D4F90)));
r_index2adr_typedef index2adr_ = reinterpret_cast<r_index2adr_typedef>((ASLR(0x1744DD0)));
uintptr_t Index2Addr = ASLR(0x1744DD0);


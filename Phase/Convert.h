#include <iostream>
#include <vector>

extern "C" {
#include "Lua\lua.h"
#include "Lua\lua.hpp"
#include "Lua\lualib.h"
#include "Lua\lauxlib.h"
#include "Lua\luaconf.h"
#include "Lua\llimits.h"
#include "Lua\lobject.h"
#include "Lua\lopcodes.h"
#include "Lua\ldo.h"
#include "Lua\lstate.h"
}

struct RBXInstruction {
    int64_t Opcode;
    int64_t A;
    int64_t B;
    int64_t C;

    RBXInstruction* Sub;
};

enum RBXOpcode : int64_t
{
    NOOP = 0x00,
    MARKUPVAL = 0x12,
    INITVA = 0xA3,
    INIT = 0xC0,
    MOVE = 0x52,
    LOADK = 0x6F,
    LOADNIL = 0xC6,
    LOADBOOL = 169,
    LOADTABLE = 0xE2,
    LOADINT = 0x8C,
    NEWTABLE = 0xFF,
    GETUPVAL = 0xFB,
    GETGLOBAL = 0x35,
    GETGLOBALOPT = 0xA4,
    GETTABLE = 0x87,
    GETTABLEK = 0x4D,
    GETTABLEN = 0x13,
    SETUPVAL = 0xDE,
    SETTABLE = 0x6A,
    SETGLOBAL = 0x18,
    SETTABLEK = 0x30,
    SETLIST = 0xC5,
    UNM = 0x39,
    NOT = 0x56,
    LEN = 0x1C,
    CONCAT = 0x73,
    TFORLOOP = 0x6E,
    FORPREP = 0xA8,
    FORLOOP = 0x8B,
    PAIRSPREP = 0x17,
    PAIRSLOOP = 0xFA,
    SELF = 0xBC,
    ADD = 0x95,
    SUB = 0x78,
    MUL = 0x5B,
    DIV = 0x3E,
    POW = 0x04,
    MOD = 0x21,
    ADDK = 0x43,
    SUBK = 0x26,
    MULK = 0x09,
    DIVK = 0xEC,
    POWK = 0xB2,
    MODK = 0xCF,
    JMP = 0x65,
    MEDIUMJMP = 0x43,
    LONGJMP = 0x69,
    EQ = 0xF1,
    NEQ = 0x9A,
    LT = 0xB7,
    GT = 0x60,
    LE = 0xD4,
    GE = 0x7D,
    AND = 0xAD,
    TESTJMP = 0x2B,
    TEST = 0x0E,
    CALL = 0x9F,
    VARARG = 0xDD,
    CLOSURE = 0xD9,
    CLOSE = 0xC1,
    RETURN = 0x82,
};

RBXInstruction* CreateInstruction(int64_t Opcode, int64_t A, int64_t B, int64_t C);
void PushInstruction(RBXInstruction* instr, std::string& bytecode);
void PushByte(int64_t byte, std::string& bytecode);
void ConvertProto(Proto* P, std::string &Bytecode, bool first, std::vector<std::string> ktable);
void ConvertConstants(Proto* P, std::vector<std::string>& ktable);
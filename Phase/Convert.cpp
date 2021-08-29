#include "Convert.h"
#include <Windows.h>
#include <iostream>
#include <sstream>

RBXInstruction* CreateInstruction(int64_t Opcode, int64_t A, int64_t B, int64_t C) {
    RBXInstruction* Instr = new RBXInstruction();

    Instr->Opcode = Opcode;
    Instr->A = A;
    Instr->B = B;
    Instr->C = C;

    return Instr;
}

void PushInstruction(RBXInstruction* instr, std::string& bytecode) {
    PushByte(instr->Opcode, bytecode);
    PushByte(instr->A, bytecode);
    PushByte(instr->B, bytecode);
    PushByte(instr->C, bytecode);

   if (instr->Sub != NULL)
        PushInstruction(instr->Sub, bytecode);
}

void PushByte(int64_t byte, std::string &bytecode) {
   bytecode += static_cast<char>(byte);
}

int FindKLocation(std::vector<std::string> ktable, Proto* P, int index) {
    std::string stratidx = reinterpret_cast<const char*>(reinterpret_cast<TString*>(P->k[index].value.gc) + 1);
    return std::distance(ktable.begin(), std::find(ktable.begin(), ktable.end(), stratidx));
}

void ConvertProto(Proto* P, std::string& Bytecode, bool first, std::vector<std::string> ktable) {
    std::vector<RBXInstruction*> RBXInstructions = {};
  
    int sizecode = 0;
    
    if (first) {
        RBXInstruction* RBXInstr = CreateInstruction(0xA3, 0, 0, 0);
        RBXInstructions.push_back(RBXInstr);
        sizecode += 1;
    }

    for (int i = 0; i < P->sizecode; ++i) {
        Instruction instr = P->code[i];

        int A = GETARG_A(instr);
        int B = GETARG_B(instr);
        int C = GETARG_C(instr);
        int Bx = GETARG_Bx(instr);
        int sBx = GETARG_sBx(instr);

        switch (GET_OPCODE(instr)) {
        case OP_GETGLOBAL: {
            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::GETGLOBAL, A, 0, 0);
            RBXInstr->Sub = CreateInstruction(FindKLocation(ktable, P, Bx), 0, 0, 0);
            RBXInstructions.push_back(RBXInstr);
            sizecode += 2;

          

            continue;
        }
        case OP_SELF: {
            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::SELF, A, B, 0xA1);
            RBXInstr->Sub = CreateInstruction(FindKLocation(ktable, P, INDEXK(C)), 0, 0, 0);
            RBXInstructions.push_back(RBXInstr);
            sizecode += 2;

            

            continue;
        }
        case OP_FORPREP: {
            std::cout << A << "\n";
            std::cout << B << "\n";
            std::cout << C << "\n";
            std::cout << Bx << "\n";
            std::cout << sBx << "\n";

            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::FORPREP, A, sBx, 0);
            RBXInstructions.push_back(RBXInstr);
            sizecode += 1;

            

            continue;
        }
        case OP_FORLOOP: {
            std::cout << A << "\n";
            std::cout << B << "\n";
            std::cout << C << "\n";
            std::cout << Bx << "\n";
            std::cout << sBx << "\n";

            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::FORLOOP, A, sBx, 0);
            RBXInstructions.push_back(RBXInstr);
            sizecode += 1;

            

            continue;
        }
        case OP_SETGLOBAL: {
            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::SETGLOBAL, A, 0, 0);
            RBXInstr->Sub = CreateInstruction(FindKLocation(ktable, P, Bx), 0, 0, 0);
            RBXInstructions.push_back(RBXInstr);
            sizecode += 2;

            

            continue;
        }
        case OP_LOADK: {
            std::cout << A << "\n";
            std::cout << B << "\n";
            std::cout << C << "\n";
            std::cout << Bx << "\n";
            std::cout << sBx << "\n";
            if (P->k[Bx].tt == LUA_TSTRING) {
                RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::LOADK, A, FindKLocation(ktable, P, Bx), 0);
                RBXInstructions.push_back(RBXInstr);

              
                
            }
            else {
                int num = P->k[Bx].value.n;
                int CNum = 0;

                while (num >= 256) {
                    num -= 256;
                    CNum += 1;
                }

                RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::LOADINT, A, num, CNum);
                RBXInstructions.push_back(RBXInstr);

                
            }



            sizecode += 1;
            continue;
        }
        case OP_CALL: {
            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::CALL, A, B, C);
            RBXInstructions.push_back(RBXInstr);

            sizecode += 1;


            continue;
        }
        case OP_RETURN: {
            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::RETURN, A, B, C);
            RBXInstructions.push_back(RBXInstr);

            sizecode += 1;

           

            continue;
        }
        case OP_GETTABLE: {
            int freereg = ++P->maxstacksize - 1;

            RBXInstruction* RBXInstrLoadK = CreateInstruction(RBXOpcode::LOADK, freereg, FindKLocation(ktable, P, INDEXK(C)), 0);
            RBXInstructions.push_back(RBXInstrLoadK);

         

            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::GETTABLE, A, B, freereg);
            RBXInstructions.push_back(RBXInstr);
            
            

            sizecode += 2;
            continue;
        }
        case OP_SETTABLE: {
            std::cout << A << "\n";
            std::cout << B << "\n";
            std::cout << C << "\n";
            std::cout << Bx << "\n";
            std::cout << sBx << "\n";
            std::cout << INDEXK(C) << "\n";
            int freereg1 = ++P->maxstacksize - 1;
         

            

            int freereg2 = ++P->maxstacksize - 1;
            
            switch (P->k[INDEXK(C)].tt) {
            case LUA_TSTRING: {
               
                 RBXInstruction* RBXInstrSetk = CreateInstruction(RBXOpcode::LOADK, freereg2, FindKLocation(ktable, P, INDEXK(C)), 0);
                 RBXInstructions.push_back(RBXInstrSetk);

                 //RBXInstruction* RBXInstrSetk = CreateInstruction(RBXOpcode::MOVE, freereg2, freereg1 - 1, 0);
                 //RBXInstructions.push_back(RBXInstrSetk);

                break;
            }
            case LUA_TNUMBER: {
              int num = P->k[INDEXK(C)].value.n;
                std::cout << num << "\n";
                int CNum = 0;

                while (num >= 256) {
                    num -= 256;
                    CNum += 1;
                }

                RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::LOADINT, freereg2, num, CNum);
                RBXInstructions.push_back(RBXInstr);
                
                

                break;
            }
            }
            RBXInstruction* RBXInstrLoadK = CreateInstruction(RBXOpcode::LOADK, freereg1, FindKLocation(ktable, P, INDEXK(B)), 0);
            RBXInstructions.push_back(RBXInstrLoadK);
            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::SETTABLE, freereg2, A, freereg1);
            RBXInstructions.push_back(RBXInstr);

            

            sizecode += 3;

            continue;
        }
        case OP_LOADBOOL: {
            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::LOADBOOL, A, B, C);
            RBXInstructions.push_back(RBXInstr);

            

            sizecode += 1;

            continue;
        }
        case OP_LOADNIL: {
            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::LOADNIL, A, B, C);
            RBXInstructions.push_back(RBXInstr);

            

            sizecode += 1;

            continue;
        }
        case OP_MOVE: {
            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::MOVE, A, B, 0);
            RBXInstructions.push_back(RBXInstr);
            sizecode += 1;

            

            continue;
        }
        case OP_CLOSURE: {
            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::CLOSURE, A, Bx, 0);
            RBXInstructions.push_back(RBXInstr);
            sizecode += 1;

            

            continue;
        }
        case OP_CLOSE: {
            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::CLOSE, A, 0, 0);
            RBXInstructions.push_back(RBXInstr);
            sizecode += 1;

            

            continue;
        }
        case OP_JMP: {
            std::cout << A << "\n";
            std::cout << B << "\n";
            std::cout << C << "\n";
            std::cout << Bx << "\n";
            std::cout << sBx << "\n";
            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::JMP, 0, 0xf4, 0xff);
            RBXInstructions.push_back(RBXInstr);
            sizecode += 1;

            

            continue;
        }
        case OP_EQ: {
            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::EQ, A, B, C);
            RBXInstructions.push_back(RBXInstr);
            sizecode += 1;



            continue;
        }
        case OP_NEWTABLE: {
            RBXInstruction* RBXInstr = CreateInstruction(RBXOpcode::NEWTABLE, A, B, C);
            RBXInstructions.push_back(RBXInstr);
            sizecode += 1;
            continue;
  
        }
        }
    }

    PushByte(sizecode, Bytecode);

    for (int i = 0; i < RBXInstructions.size(); ++i) {
        switch (RBXInstructions[i]->Opcode)
        {
        case RBXOpcode::SELF: {
            int atcall = i;

            while (RBXInstructions[atcall]->Opcode != RBXOpcode::CALL) {
                ++atcall;
            }
            RBXInstructions.insert(RBXInstructions.begin() + atcall, RBXInstructions[i]);
            RBXInstructions.erase(RBXInstructions.begin() + i);

            break;
        }
        case RBXOpcode::SETTABLE: {        
            if (RBXInstructions[i - 2]->Opcode != RBXOpcode::LOADK && RBXInstructions[i - 2]->Opcode != RBXOpcode::LOADINT)
            RBXInstructions[i - 2]->A = RBXInstructions[i]->A;

            break;
        }
        default:
            break;
        }
    }

    for (int i = 0; i < RBXInstructions.size(); ++i) {
        PushInstruction(RBXInstructions[i], Bytecode);
    }
  
    std::vector<std::string> BytecodeConstants = {};
    int ConstantsSize = 0;

    for (int o = 0; o < P->sizek; ++o) {
        std::string thestr = "";
        switch (P->k[o].tt) {
        case LUA_TSTRING: {
            PushByte(3, thestr);
            PushByte(++ConstantsSize, thestr);

                BytecodeConstants.push_back(thestr);
                break;
        }
        case LUA_TNUMBER: {
            PushByte(2, thestr);
            PushByte(++ConstantsSize, thestr);

    
                BytecodeConstants.push_back(thestr);
           
                break;
        }
        case LUA_TBOOLEAN: {
            // PushByte(1, thestr);
            break;
        }
        }


    }

    PushByte(ConstantsSize, Bytecode);

    for (int i = 0; i < BytecodeConstants.size(); ++i) {
        Bytecode.append(BytecodeConstants[i]);
    }
   
}

void ConvertConstants(Proto* P, std::vector<std::string> &ktable) {
    for (int i = 0; i < P->sizek; ++i) {
        switch (P->k[i].tt) {
        case LUA_TSTRING: {
            const char* str = reinterpret_cast<const char*>(reinterpret_cast<TString*>(P->k[i].value.gc) + 1);

            if (std::find(ktable.begin(), ktable.end(), str) == ktable.end()) {
                ktable.push_back(str);
            }

            continue;
        }
        }
    }
}
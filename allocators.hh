#ifndef __ALLOCATORS_HH__
#define __ALLOCATORS_HH__

#include <bits/stdc++.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "TypeNode.hh"
#include "lowfat-ptr-info.hh"
#include "pin.H"

using std::stringstream;

typedef std::map<uint64_t, std::pair<const char *, uint64_t> > TypesCount;
typedef TypesCount TypesCount;

extern ofstream *out;
extern TypesCount TC;
extern std::map<int, std::vector<int> > TypeTreeTID;
extern std::map<uint64_t, int> HashMapTID;
extern UINT64 NumOfCalls;

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
extern UINT64 icount;

uint64_t Val2Str2(const PIN_REGISTER &value, const UINT size) {
    switch (size) {
        case 1:
            // *out<< "PINT_REGISTER val = " << hex
            //           << (uint64_t)(*(value.byte)) << '\n';
            return (uint64_t)(*(value.byte));
            break;
        case 2:
            // *out<< "PINT_REGISTER val = " << hex
            //           << (uint64_t)(*(value.word)) << '\n';
            return (uint64_t)(*(value.word));
            break;
        case 4:
            // *out<< "PINT_REGISTER val = " << hex
            //           << (uint64_t)(*(value.dword)) << '\n';
            return (uint64_t)(*(value.dword));
            break;
        case 8:
            // *out<< "PINT_REGISTER val = " << hex << (*(value.qword))
            //           << '\n';
            return (*(value.qword));
            break;
        default:
            return 0;
    }
}

string Val2Str(const void *value, UINT size) {
    unsigned int size_tmp = size;
    stringstream sstr;
    sstr << hex;
    const unsigned char *cval = (const unsigned char *)value;
    // Traverse cval from end to beginning since the MSB is in the last block of
    // cval.
    while (size) {
        --size;
        sstr << (unsigned int)cval[size];
    }

    switch (size_tmp) {
        case 1:
            *out << "(uint8_t) Val2Str is loaded with value = "
                 << *((uint8_t *)value) << ", size= " << size_tmp
                 << " and return value=" << string("0x") + sstr.str() << '\n';
            break;
        case 2:
            *out << "(uint16_t) Val2Str is loaded with value = "
                 << *((uint16_t *)value) << ", size= " << size_tmp
                 << " and return value=" << string("0x") + sstr.str() << '\n';
            break;
        case 4:
            *out << "(uint32_t) Val2Str is loaded with value = "
                 << *((uint32_t *)value) << ", size= " << size_tmp
                 << " and return value=" << string("0x") + sstr.str() << '\n';
            break;
        case 8:
            *out << "(uint64_t) Val2Str is loaded with value = "
                 << *((uint64_t *)value) << ", size= " << size_tmp
                 << " and return value=" << string("0x") + sstr.str() << '\n';
            break;
    }
    return string("0x") + sstr.str();
}

uint64_t Str2Val(std::string hexStr) {
    uint64_t value;
    std::stringstream iss;
    iss << std::hex << hexStr;
    iss >> value;
    return value;
}

// uint64_t PinReg2Val (void* value, unsigned int size){

//     unsigned char* raw = ( unsigned char*)value;

//     switch (size)
//     {
//         case 1:
//         {
//             uint64_t result = (raw[0]);
//             return result;
//         }
//         case 2:
//         {
//             uint64_t result = (raw[0] << 8 | raw[1]);
//             return result;
//         }
//         case 4:
//         {
//             uint64_t result = (raw[0] << 24 | raw[1] << 16 | raw[2] << 8 |
//             raw[3]); return result;
//         }
//         case 8:
//         {
//             uint64_t result = (raw[0] << 56 | raw[1] << 48 | raw[2] << 40 |
//             raw[3] << 32 | raw[4] << 24 | raw[5] << 16 | raw[6] << 8 |
//             raw[7]); return result;
//         }
//         default:
//         {
//             assert(0);
//             break;
//         }
//     }

//     return 0;

// }

// This function is called before every instruction is executed
VOID docount() {
    icount++;
    uint64_t nonzero_tid = 0;
    if (icount % 100000000 == 0) {
        //*out << std::dec << icount/100000000 << " " << NumOfCalls << endl;

        for (TypesCount::iterator it = TC.begin(); it != TC.end(); it++) {
            if (it->second.second != 0) {
                *out << std::dec << it->second.second << " ";
                it->second.second = 0;
                nonzero_tid++;
            }
        }
        *out << nonzero_tid << std::endl;
        NumOfCalls = 0;
    }
}

static void PrintRegistersVectorized(CHAR *where, string *disass, ADDRINT pc,
                                     const CONTEXT *ctx,
                                     std::vector<UINT32> *RRegs,
                                     std::vector<UINT32> *WRegs) {
    *out << "PrintRegistersVectorized: " << where << ": " << std::hex << pc
         << ": " << *disass << std::endl;

    *out << "Reads: " << std::endl;
    for (uint64_t i = 0; i < RRegs->size(); i++) {
        const UINT grRegSize = REG_Size((REG)(*RRegs)[i]);
        PIN_REGISTER val;
        PIN_GetContextRegval(ctx, (REG)(*RRegs)[i],
                             reinterpret_cast<UINT8 *>(&val));

        // switch (grRegSize) {
        //     case 1:
        //         *out<< "PINT_REGISTER val = " << hex << (uint64_t)
        //         (*(val.byte)) << '\n'; break;
        //     case 2:
        //         *out<< "PINT_REGISTER val = " << hex << (uint64_t)
        //         (*(val.word)) << '\n'; break;
        //     case 4:
        //         *out<< "PINT_REGISTER val = " << hex << (uint64_t)
        //         (*(val.dword)) << '\n'; break;
        //     case 8:
        //         *out<< "PINT_REGISTER val = " << hex << (*(val.qword))
        //         << '\n'; break;
        // }

        // uint64_t addr = Str2Val(Val2Str(&val, grRegSize));
        uint64_t addr2 = Val2Str2(val, grRegSize);
        *out << "PINT_REGISTER val = " << hex << addr2 << '\n';
        // uint64_t addr = Val2Str2(val, grRegSize);
        // *out << "//Ahmad: " <<

        void *ptr = (void *)addr2;

        std::string ptr_type = "";
        ptr_type += "(HEAP)";

        if (lowfat_is_heap_ptr(ptr)) {
            *out << "11111111111111111111\n";
            size_t idx = lowfat_index(ptr);
            *out << "222222222222222222222\n";
            if (idx > EFFECTIVE_LOWFAT_NUM_REGIONS_LIMIT ||
                _LOWFAT_MAGICS[idx] == 0) {
                ;
            } else {
                void *base = lowfat_base(ptr);
                // Get the object meta-data and calculate the allocation bounds.
                EFFECTIVE_META *meta = (EFFECTIVE_META *)base;
                *out << "ADDR: " << addr2 << " " << ptr << " " << base << " "
                     << meta->size << std::endl;
                base = (void *)(meta + 1);
                const EFFECTIVE_TYPE *t = meta->type;
                if (t == NULL) {
                    *out << "HERE ";
                    ;
                } else {
                    // std::string name(t->info->name); ptr_type += name;

                    if (EFFECTIVE_UNLIKELY(t->info == NULL)) {
                        assert(0);
                    }
                    if (EFFECTIVE_UNLIKELY(t->info == NULL)) {
                        assert(0);
                    }
                    *out << "HERE2 " << std::dec << t->info->name << "("
                         << t->info->tid_info->tid << ")";
                }
            }
        } else if (lowfat_is_global_ptr(ptr)) {
            *out << "222222222222222222222\n";
            size_t idx = lowfat_index(ptr);
            if (idx > EFFECTIVE_LOWFAT_NUM_REGIONS_LIMIT ||
                _LOWFAT_MAGICS[idx] == 0) {
                ;
            } else {
                void *base = lowfat_base(ptr);
                // Get the object meta-data and calculate the allocation bounds.
                EFFECTIVE_META *meta = (EFFECTIVE_META *)base;
                *out << "ADDR: " << addr2 << " " << ptr << " " << base << " "
                     << meta->size << std::endl;
                base = (void *)(meta + 1);
                
                assert(meta != NULL);
                const EFFECTIVE_TYPE *t = meta->type;
                *out << "123123123123\n";
                if (t == NULL) {
                    *out << "HERE ";
                    ;
                } else {
                    // std::string name(t->info->name); ptr_type += name;

                    if (EFFECTIVE_UNLIKELY(t->info == NULL)) {
                        assert(0);
                    }
                    if (EFFECTIVE_UNLIKELY(t->info == NULL)) {
                        assert(0);
                    }
                    *out << "HERE2 " << std::dec << t->info->name << "("
                         << t->info->tid_info->tid << ")";
                }
            }
            // } else if (lowfat_is_stack_ptr(ptr)) {
            //     *out<< "3333333333333333333333333333\n";
            //     size_t idx = lowfat_index(ptr);
            //     if (idx > EFFECTIVE_LOWFAT_NUM_REGIONS_LIMIT ||
            //         _LOWFAT_MAGICS[idx] == 0) {
            //         ;
            //     } else {
            //         void *base = lowfat_base(ptr);
            //         // Get the object meta-data and calculate the allocation
            //         bounds. EFFECTIVE_META *meta = (EFFECTIVE_META *)base;
            //         *out << "ADDR: " << addr2 << " " << ptr << " " << base <<
            //         " "
            //              << meta->size << std::endl;
            //         base = (void *)(meta + 1);
            //         const EFFECTIVE_TYPE *t = meta->type;
            //         if (t == NULL) {
            //             *out << "HERE ";
            //             ;
            //         } else {
            //             // std::string name(t->info->name); ptr_type += name;

            //             if (EFFECTIVE_UNLIKELY(t->info == NULL)) {
            //                 assert(0);
            //             }
            //             if (EFFECTIVE_UNLIKELY(t->info == NULL)) {
            //                 assert(0);
            //             }
            //             *out << "HERE2 " << std::dec << t->info->name << "("
            //                  << t->info->tid_info->tid << ")";
            //         }
            //     }
        }

        *out << REG_StringShort((REG)(*RRegs)[i]) << "(" << grRegSize
             << ")"
             //  << "(" << Val2Str(&val, grRegSize)
             //  << ") "
             << "(" << hex << addr2 << ") "
             << " ";
    }

    *out << std::endl << "Writes: " << std::endl;
    for (uint64_t i = 0; i < WRegs->size(); i++) {
        const UINT grRegSize = REG_Size((REG)(*WRegs)[i]);
        PIN_REGISTER val;
        PIN_GetContextRegval(ctx, (REG)(*WRegs)[i],
                             reinterpret_cast<UINT8 *>(&val));
        uint64_t addr2 = Val2Str2(val, grRegSize);
        *out << "PIN_REGISTER val = " << hex << addr2 << '\n';
        *out << REG_StringShort((REG)(*WRegs)[i]) << "(" << grRegSize
             << ")"
             //  << "(" << Val2Str(&val, grRegSize) << ") ";
             << "(" << hex << addr2 << ") ";
    }

    *out << std::endl;
}

// VOID FreeArgsBefore(CHAR * name, ADDRINT ptr)
// {

// }

// VOID MallocArgsBefore(CHAR * name, ADDRINT size, ADDRINT type)
// {

// }

// VOID MallocRetAfter(CHAR * name, ADDRINT ptr)
// {

// }

static void PrintRegisters(ADDRINT pc, const CONTEXT *ctxt) {
    *out << "-------------------- " << std::hex << pc << std::dec
         << " ------------------" << std::endl;
    for (int reg = (int)REG_GR_BASE; reg <= (int)REG_GR_LAST; ++reg) {
        // For the integer registers, it is safe to use ADDRINT. But make sure
        // to pass a pointer to it.
        ADDRINT val;
        PIN_GetContextRegval(ctxt, (REG)reg, reinterpret_cast<UINT8 *>(&val));

        void *ptr = (void *)val;
        std::string ptr_type = "";

        if (lowfat_is_heap_ptr(ptr)) {
            ptr_type += "(HEAP)";
        } else if (lowfat_is_global_ptr(ptr)) {
            ptr_type += "(GLOBAL)";
        } else if (lowfat_is_stack_ptr(ptr)) {
            size_t idx = lowfat_index(ptr);
            if (idx > EFFECTIVE_LOWFAT_NUM_REGIONS_LIMIT ||
                _LOWFAT_MAGICS[idx] == 0) {
                ;
            } else {
                ptr_type += "(STACK)";
            }
        }

        *out << REG_StringShort((REG)reg) << ": " << std::hex << val << ptr_type
             << endl;
    }
}

// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v) {
    // Insert a call to docount before every instruction, no arguments are
    // passed
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_END);

    if (!RTN_Valid(INS_Rtn(ins))) return;

    // sanetize instructions from shared libraries
    if ((!IMG_Valid(SEC_Img(RTN_Sec(INS_Rtn(ins)))) ||
         !IMG_IsMainExecutable(SEC_Img(RTN_Sec(INS_Rtn(ins))))))
        return;

    // SANITIZING SOME OF THE EFFECTIVE AND LOWFAT FUNCTIONS
    std::string rtn_name = RTN_Name(INS_Rtn(ins));
    if ((rtn_name.find("lowfat_") != std::string::npos) ||
        (rtn_name.find("effective_") != std::string::npos) ||
        (rtn_name.find("EFFECTIVE_") != std::string::npos) ||
        (rtn_name.find("LOWFAT_") != std::string::npos)) {
        //*out << "Function Name: " << rtn_name << std::endl;
        return;
    } else {
        //*out << "Function Name: " << rtn_name << std::endl;
    }

    // IN CASE WE NEED THE WHOLE CONTEXT
    // INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)PrintRegisters, IARG_INST_PTR
    // , IARG_CONST_CONTEXT, IARG_END);

    std::string *dis = new std::string(INS_Disassemble(ins));
    std::vector<UINT32> *maxNumRRegs = new std::vector<UINT32>();
    std::vector<UINT32> *maxNumWRegs = new std::vector<UINT32>();

    for (UINT32 i = 0; i < INS_MaxNumRRegs(ins); i++) {
        if (REG_is_gr64(INS_RegR(ins, i)))
            maxNumRRegs->push_back(INS_RegR(ins, i));
        else if (REG_is_gr32(INS_RegR(ins, i)))
            maxNumRRegs->push_back(INS_RegR(ins, i));
        else if (REG_is_gr16(INS_RegR(ins, i)))
            maxNumRRegs->push_back(INS_RegR(ins, i));
        else if (REG_is_gr8(INS_RegR(ins, i)))
            maxNumRRegs->push_back(INS_RegR(ins, i));
    }
    for (UINT32 i = 0; i < INS_MaxNumWRegs(ins); i++) {
        if (REG_is_gr64(INS_RegW(ins, i)))
            maxNumWRegs->push_back(INS_RegW(ins, i));
        else if (REG_is_gr32(INS_RegW(ins, i)))
            maxNumWRegs->push_back(INS_RegW(ins, i));
        else if (REG_is_gr16(INS_RegW(ins, i)))
            maxNumWRegs->push_back(INS_RegW(ins, i));
        else if (REG_is_gr8(INS_RegW(ins, i)))
            maxNumWRegs->push_back(INS_RegW(ins, i));
    }

    if (maxNumRRegs->size() != 0 || maxNumWRegs->size() != 0) {
        if (INS_IsValidForIpointAfter(ins)) {
            INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)PrintRegistersVectorized,
                           IARG_ADDRINT, "IPOINT_AFTER", IARG_PTR, dis,
                           IARG_INST_PTR, IARG_CONST_CONTEXT, IARG_PTR,
                           maxNumRRegs, IARG_PTR, maxNumWRegs, IARG_END);
        } else {
            INS_InsertCall(ins, IPOINT_BEFORE,
                           (AFUNPTR)PrintRegistersVectorized, IARG_ADDRINT,
                           "IPOINT_BEFORE", IARG_PTR, dis, IARG_INST_PTR,
                           IARG_CONST_CONTEXT, IARG_PTR, maxNumRRegs, IARG_PTR,
                           maxNumWRegs, IARG_END);
        }
    }
}

#endif
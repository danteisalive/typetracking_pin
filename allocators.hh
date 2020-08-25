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
#include "predictor.hh"

using std::stringstream;

typedef std::map<uint64_t, std::pair<const char *, uint64_t> > TypesCount;
typedef TypesCount TypesCount;

typedef std::map<std::string, uint64_t> InsTypeCount;
typedef InsTypeCount InsTypeCount;

extern ofstream *out;
extern TypesCount TC;
extern InsTypeCount ITC;

// extern std::map<int, std::vector<int> > TypeTreeTID;
// extern std::map<uint64_t, int> HashMapTID;

extern UINT64 NumOfCalls;

extern std::map<int, std::map<int, std::set<std::pair<int, int> > > > typeTree;

extern DefaultLVPT *lvpt;

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
extern UINT64 icount;

uint64_t Val2Str(const PIN_REGISTER &value, const UINT size) {
    //*out << "Called Val2Str2 with size=" << size << "\n" << std::flush;
    switch (size) {
        case 1:
            // *out<< "PINT_REGISTER val = " << hex
            //           << (uint64_t)(*(value.byte)) << '\n' << std::flush;
            return (uint64_t)(*(value.byte));
            break;
        case 2:
            // *out<< "PINT_REGISTER val = " << hex
            //           << (uint64_t)(*(value.word)) << '\n' << std::flush;
            return (uint64_t)(*(value.word));
            break;
        case 4:
            // *out<< "PINT_REGISTER val = " << hex
            //           << (uint64_t)(*(value.dword)) << '\n' << std::flush;
            return (uint64_t)(*(value.dword));
            break;
        case 8:
            // *out<< "PINT_REGISTER val = " << hex << (*(value.qword))
            //           << '\n' << std::flush;
            return (*(value.qword));
            break;
        default:
            return 0;
    }
}

// This function is called before every instruction is executed
VOID docount() {
    icount++;
    uint64_t nonzero_tid = 0;
    nonzero_tid = nonzero_tid;
    if (icount % 100000000 == 0) {
        //*out << std::dec << icount/100000000 << " " << NumOfCalls << endl;

        // for (TypesCount::iterator it = TC.begin(); it != TC.end(); it++) {
        //     if (it->second.second != 0) {
        //         *out << std::dec << it->second.second << " " << std::flush;
        //         it->second.second = 0;
        //         nonzero_tid++;
        //     }
        // }
        // *out << nonzero_tid << '\n' << std::flush;
        NumOfCalls = 0;

        int numOfRulesUsedDuringThisPeriod = 0;
        for (InsTypeCount::iterator it = ITC.begin(); it != ITC.end(); it++) {
            if (it->second != 0) {
                //*out << std::dec << it->first << "(" << it->second <<  ")\n";
                it->second = 0;
                numOfRulesUsedDuringThisPeriod++;
            }
        }
        *out << std::dec << numOfRulesUsedDuringThisPeriod << " "
             << (double)lvpt->LVPTMissprediction / lvpt->LVPTNumOfAccesses
             << '\n'
             << std::flush;
        //*out <<
        //"------------------------------------------------------------------------\n"
        //<< std::flush;
    }
}

bool findPtrType(const EFFECTIVE_INFO *info) {
    assert(info->num_entries >= 0);

    if (info->num_entries == 0) {
        *out << std::dec << info->name << "\n" << std::flush;
        ;
        //*out << std::dec << info->hash << "\n" << std::flush;;
        // *out << std::hex << info->entries[0].lb << "\n" << std::flush;;
        // *out << std::hex << info->entries[0].ub << "\n" << std::flush;;
    } else {
        for (uint32_t i = 0; i < info->num_entries; i++) {
            findPtrType(info->entries[i].type);
        }
    }

    return false;
}

static void PrintRegistersVectorized(CHAR *where, string *disass, ADDRINT pc,
                                     const CONTEXT *ctx, ADDRINT opcode,
                                     std::vector<UINT32> *RRegs,
                                     std::vector<UINT32> *WRegs) {
    bool isSrcPointer = false;
    isSrcPointer = isSrcPointer;
    bool isDestPointer = false;
    isDestPointer = isDestPointer;
    std::string srcTypeIds = "";
    std::string destTypeIds = "";
    // *out << "PrintRegistersVectorized: " << where << ": (" << std::hex << pc
    // <<
    //     std::dec << ") Opcode: (" << (UINT32)opcode << ") (" << std::hex <<
    //     OPCODE_StringShort (opcode) << ") (" << *disass << ")\n" <<
    //     std::flush;

    // *out << "Reads: " << '\n' << std::flush;
    for (uint64_t i = 0; i < RRegs->size(); i++) {
        const UINT grRegSize = REG_Size((REG)(*RRegs)[i]);
        PIN_REGISTER val;
        PIN_GetContextRegval(ctx, (REG)(*RRegs)[i],
                             reinterpret_cast<UINT8 *>(&val));

        // uint64_t addr = Str2Val(Val2Str(&val, grRegSize));
        uint64_t addr2 = Val2Str(val, grRegSize);
        addr2 = addr2;

        //*out << "PINT_REGISTER val = " << hex << addr2 << '\n' << std::flush;

        void *ptr = (void *)addr2;

        if (lowfat_is_ptr(ptr)) {
            if (lowfat_is_heap_ptr(ptr)) {
                //*out << "11111111111111111111\n" << std::flush;
                size_t idx = lowfat_index(ptr);
                //*out << "222222222222222222222\n" << std::flush;
                if (idx > EFFECTIVE_LOWFAT_NUM_REGIONS_LIMIT ||
                    _LOWFAT_MAGICS[idx] == 0) {
                    //*out << "JJJJJJJJJJJJJJJJJJJJJ\n" << std::flush;
                    ;
                } else {
                    void *base = lowfat_base(ptr);
                    // Get the object meta-data and calculate the allocation
                    // bounds.
                    EFFECTIVE_META *meta = (EFFECTIVE_META *)base;
                    // *out << "ADDR: " << addr2 << " " << ptr << " " << base
                    //      << " " << meta->size << '\n'
                    //      << std::flush;
                    base = (void *)(meta + 1);

                    // *out << "AAAAAAAAAAAAAAAAAAAAAAAAaa\n" << std::flush;
                    const EFFECTIVE_TYPE *t = meta->type;

                    // if (EFFECTIVE_UNLIKELY(t->size == 0)) return;

                    EFFECTIVE_BOUNDS bases = {(intptr_t)base, (intptr_t)base};
                    EFFECTIVE_BOUNDS sizes = {0, (intptr_t)meta->size};
                    EFFECTIVE_BOUNDS bounds = bases + sizes;
                    bounds = bounds;
                    // *out << "PPPPPPPPPPPPPPPPPPPPPPPPPPP\n" << std::flush;

                    if (EFFECTIVE_UNLIKELY(t == NULL)) {
                        // *out << "HERE \n" << std::flush;
                        ;
                    } else {
                        // std::string name(t->info->name); ptr_type += name;

                        if (EFFECTIVE_UNLIKELY(t->info == NULL)) {
                            assert(0);
                        }
                        *out << "Effective info type id = "
                             << t->info->tid_info->tid << "\n";

// std::map<int, std::map<int, std::set<std::pair<int, int> > > > typeTree;

                        size_t offset = (uint8_t *)ptr - (uint8_t *)base;
                        if (typeTree.find(t->info->tid_info->tid) == typeTree.end()) {
                            *out << "Could not find effective_info\n";
                        } else {
                            if (typeTree[t->info->tid_info->tid].find(offset) == typeTree[t->info->tid_info->tid].end()) {
                                *out << "Could not find offset: " << offset << "\n";
                            } else {
                                std::set<std::pair<int, int> > pairs = typeTree[t->info->tid_info->tid][offset];
                                std::set<std::pair<int, int> >::iterator it;
                                for (it = pairs.begin(); it != pairs.end(); it++) {
                                    *out << "offset typeID = " << it->first << '\n';
                                }
                            }
                        }
                        *out << "---------------------\n";


                        if (offset >= t->size) {
                            // The `offset' is >= sizeof(T).  Thus `ptr' may be
                            // pointing to an element in an array of T.
                            // Alternatively, `ptr' may be pointing to a FAM at
                            // the end of T.  Either way, the offset is
                            // normalized here.
                            EFFECTIVE_BOUNDS adjust = {t->offset_fam, 0};
                            offset -= t->size;
                            unsigned __int128 tmp = (unsigned __int128)offset;
                            tmp *= (unsigned __int128)t->magic;
                            idx = (size_t)(tmp >> EFFECTIVE_RADIX);
                            offset -= idx * t->size_fam;
                            bounds += adjust;
                            offset += t->offset_fam;
                        }

                        offset = offset;

                        // uint64_t layout_idx = 0;
                        // while(t->layout[layout_idx].hash !=
                        // EFFECTIVE_ENTRY_EMPTY_HASH)
                        // {
                        //     *out <<  std::dec  << "(" << t->info->name << ")"
                        //                         " (" << layout_idx << ")"
                        //                         " (" <<
                        //                         t->layout[layout_idx].hash <<
                        //                         ")" " B0(" <<
                        //                         t->layout[layout_idx].bounds[0]
                        //                         << ")" " B1(" <<
                        //                         t->layout[layout_idx].bounds[1]
                        //                         << ")"
                        //                         "\n" << std::flush;
                        //     layout_idx++;
                        // }
                        //  *out <<  std::dec  << "(" << t->info->name << ")"
                        //                         " (" << layout_idx << ")"
                        //                         " (" <<
                        //                         t->layout[layout_idx].hash <<
                        //                         ")" " B0(" <<
                        //                         t->layout[layout_idx].bounds[0]
                        //                         << ")" " B1(" <<
                        //                         t->layout[layout_idx].bounds[1]
                        //                         << ")"
                        //                         "\n" << std::flush;

                        // findPtrType(t->info);

                        srcTypeIds += "_";
                        srcTypeIds += std::string(t->info->name);
                        // ITC[(UINT32)opcode]++;
                        isSrcPointer = true;
                    }
                }
            }
        }
    }

    //*out << '\n' << std::flush << "Writes: " << '\n' << std::flush;
    for (uint64_t i = 0; i < WRegs->size(); i++) {
        const UINT grRegSize = REG_Size((REG)(*WRegs)[i]);
        PIN_REGISTER val;
        PIN_GetContextRegval(ctx, (REG)(*WRegs)[i],
                             reinterpret_cast<UINT8 *>(&val));
        uint64_t addr2 = Val2Str(val, grRegSize);
        addr2 = addr2;
        // *out << "PIN_REGISTER val = " << hex << addr2 << '\n' << std::flush;
        // *out << REG_StringShort((REG)(*WRegs)[i]) << "(" << grRegSize
        //      << ")"
        //      //  << "(" << Val2Str(&val, grRegSize) << ") ";
        //      << "(" << hex << addr2 << ") " << std::flush;
        void *ptr = (void *)addr2;
        if (lowfat_is_ptr(ptr)) {
            if (lowfat_is_heap_ptr(ptr)) {
                //*out << "11111111111111111111\n" << std::flush;
                size_t idx = lowfat_index(ptr);
                //*out << "222222222222222222222\n" << std::flush;
                if (idx > EFFECTIVE_LOWFAT_NUM_REGIONS_LIMIT ||
                    _LOWFAT_MAGICS[idx] == 0) {
                    //*out << "JJJJJJJJJJJJJJJJJJJJJ\n" << std::flush;
                    ;
                } else {
                    void *base = lowfat_base(ptr);
                    // Get the object meta-data and calculate the allocation
                    // bounds.
                    EFFECTIVE_META *meta = (EFFECTIVE_META *)base;
                    // *out << "ADDR: " << addr2 << " " << ptr << " " << base
                    //      << " " << meta->size << '\n'
                    //      << std::flush;
                    base = (void *)(meta + 1);
                    // *out << "AAAAAAAAAAAAAAAAAAAAAAAAaa\n" << std::flush;
                    const EFFECTIVE_TYPE *t = meta->type;
                    // *out << "PPPPPPPPPPPPPPPPPPPPPPPPPPP\n" << std::flush;
                    if (t == NULL) {
                        // *out << "HERE \n" << std::flush;
                        ;
                    } else {
                        // std::string name(t->info->name); ptr_type += name;

                        if (EFFECTIVE_UNLIKELY(t->info == NULL)) {
                            assert(0);
                        }
                        // if (EFFECTIVE_UNLIKELY(t->info == NULL)) {
                        //     assert(0);
                        // }
                        // *out << "HERE2 " << std::dec << t->info->name << "("
                        //      << t->info->tid_info->tid << ")" << std::flush;
                        // ITC[(UINT32)opcode]++;
                        destTypeIds += "_";
                        destTypeIds += std::string(t->info->name);
                        isDestPointer = true;

                        // predict and update LVPT
                        PointerID tid = lvpt->lookup((uint64_t)pc, 0);

                        bool prediction = false;
                        prediction = (t->info->tid_info->tid == tid.getPID());

                        lvpt->update((uint64_t)pc,
                                     PointerID(t->info->tid_info->tid), 0,
                                     prediction);

                        lvpt->LVPTNumOfAccesses++;
                        if (!prediction) lvpt->LVPTMissprediction++;
                    }
                }
            }
        }
    }

    if (isDestPointer && isSrcPointer) {
        for (uint64_t i = 0; i < RRegs->size(); i++) {
            const UINT grRegSize = REG_Size((REG)(*RRegs)[i]);
            PIN_REGISTER val;
            PIN_GetContextRegval(ctx, (REG)(*RRegs)[i],
                                 reinterpret_cast<UINT8 *>(&val));

            // uint64_t addr = Str2Val(Val2Str(&val, grRegSize));
            uint64_t addr2 = Val2Str(val, grRegSize);
            addr2 = addr2;
            // *out << REG_StringShort((REG)(*RRegs)[i]) << "(" << grRegSize
            //  << ")"
            //  //  << "(" << Val2Str(&val, grRegSize)
            //  //  << ") "
            //  << "(" << hex << addr2 << ") "
            //  << " \n" << std::flush;
        }

        for (uint64_t i = 0; i < WRegs->size(); i++) {
            const UINT grRegSize = REG_Size((REG)(*WRegs)[i]);
            PIN_REGISTER val;
            PIN_GetContextRegval(ctx, (REG)(*WRegs)[i],
                                 reinterpret_cast<UINT8 *>(&val));

            // uint64_t addr = Str2Val(Val2Str(&val, grRegSize));
            uint64_t addr2 = Val2Str(val, grRegSize);
            addr2 = addr2;
            // *out << REG_StringShort((REG)(*WRegs)[i]) << "(" << grRegSize
            //  << ")"
            //  //  << "(" << Val2Str(&val, grRegSize)
            //  //  << ") "
            //  << "(" << hex << addr2 << ") "
            //  << " \n" << std::flush;
        }

        std::string key = OPCODE_StringShort(opcode) + srcTypeIds + destTypeIds;

        // *out << "PrintRegistersVectorized: " << where << ": (" << std::hex <<
        // pc << std::dec << ") Opcode: (" << (UINT32)opcode << ") (" <<
        // std::hex << OPCODE_StringShort (opcode) << ") (" << *disass << ") "
        // << " (" << key << ")\n" << std::flush;

        ITC[key]++;
    }

    //*out << '\n' << std::flush;
}

static void PrintRegisters(ADDRINT pc, const CONTEXT *ctxt) {
    *out << "-------------------- " << std::hex << pc << std::dec
         << " ------------------" << '\n'
         << std::flush;
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
             << '\n'
             << std::flush;
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
        //*out << "Function Name: " << rtn_name << '\n' << std::flush;
        return;
    } else {
        //*out << "Function Name: " << rtn_name << '\n' << std::flush;
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
        // INS_InsertCall(ins, IPOINT_BEFORE,
        //                    (AFUNPTR)PrintRegistersVectorized, IARG_ADDRINT,
        //                    "IPOINT_BEFORE", IARG_PTR, dis, IARG_INST_PTR,
        //                    IARG_CONST_CONTEXT, IARG_PTR, maxNumRRegs,
        //                    IARG_PTR, maxNumWRegs, IARG_END);

        if (INS_IsValidForIpointAfter(ins)) {
            INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)PrintRegistersVectorized,
                           IARG_ADDRINT, "IPOINT_AFTER", IARG_PTR, dis,
                           IARG_INST_PTR, IARG_CONST_CONTEXT, IARG_ADDRINT,
                           INS_Opcode(ins), IARG_PTR, maxNumRRegs, IARG_PTR,
                           maxNumWRegs, IARG_END);
        } else {
            INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)PrintRegistersVectorized,
                IARG_ADDRINT, "IPOINT_BEFORE", IARG_PTR, dis, IARG_INST_PTR,
                IARG_CONST_CONTEXT, IARG_ADDRINT, INS_Opcode(ins), IARG_PTR,
                maxNumRRegs, IARG_PTR, maxNumWRegs, IARG_END);
        }
    }
}

#endif
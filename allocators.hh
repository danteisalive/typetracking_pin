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


VOID RecordMemRead(ADDRINT pc, ADDRINT addr, ADDRINT size, string *disass,
                   const CONTEXT *ctx, ADDRINT opcode,
                   std::vector<UINT32> *RRegs, std::vector<UINT32> *WRegs) {


    void *ptr = (void *)addr;

    if (lowfat_is_heap_ptr(ptr)) {
        // *out << "RecordMemRead: "
        //     << ": PC(" << std::hex << pc << ") Addr(" << addr << ") " << std::dec
        //     << "Size(" << size << ") " << std::dec << "Opcode: (" << (UINT32)opcode
        //     << ") (" << std::hex << OPCODE_StringShort(opcode) << ") (" << *disass
        //     << ")\n"
        //     << std::flush;
        // *out << "\n";
        size_t idx = lowfat_index(ptr);
        if (idx > EFFECTIVE_LOWFAT_NUM_REGIONS_LIMIT ||
            _LOWFAT_MAGICS[idx] == 0) {
            *out << "`ptr' is a non-fat-pointer\n";
        } else {
            void *base = lowfat_base(ptr);

            // Get the object meta-data and calculate the allocation bounds.
            EFFECTIVE_META *meta = (EFFECTIVE_META *)base;
            base = (void *)(meta + 1);
            const EFFECTIVE_TYPE *t = meta->type;
            // TYCHE_METADATA_CACHELINE* tm = t->tyche_meta;
            // *out << "tm = " << tm->CacheLine_0 << ", t->size = " << t->size
            //          << '\n';
            // EFFECTIVE_BOUNDS bases = {(intptr_t)base, (intptr_t)base};
            // EFFECTIVE_BOUNDS sizes = {0, meta->size};
            // EFFECTIVE_BOUNDS bounds = bases + sizes;

            if (EFFECTIVE_UNLIKELY(t == NULL)) {
                *out << "Effective type free!!!\n";
            } else {
                TYCHE_METADATA_CACHELINE* tm = t->tyche_meta;
                *out << tm << " " << std::dec << t->size << "\n" << std::flush;
                *out << 
                "tm_0 = " << std::dec << tm->CacheLine_0 << " " <<
                "tm_1 = " << std::dec << tm->CacheLine_1 << " " <<
                "tm_2 = " << std::dec << tm->CacheLine_2 << " " <<
                "tm_3 = " << std::dec << tm->CacheLine_3 << " " <<
                "tm_4 = " << std::dec << tm->CacheLine_4 << " " <<
                "tm_5 = " << std::dec << tm->CacheLine_5 << " " <<
                "tm_6 = " << std::dec << tm->CacheLine_6 << " " <<
                "tm_7 = " << std::dec << tm->CacheLine_7 << " " <<
                "tm_8 = " << std::dec << tm->CacheLine_8 << " " <<
                "tm_9 = " << std::dec << tm->CacheLine_9 << " " <<
                "tm_10 = " << std::dec << tm->CacheLine_10 << " " <<
                "tm_11 = " << std::dec << tm->CacheLine_11 << " " <<
                "tm_12 = " << std::dec << tm->CacheLine_12 << " " <<
                "tm_13 = " << std::dec << tm->CacheLine_13 << " " <<
                "tm_p = "  << std::hex << tm->next_cacheline << " " <<
                '\n' << std::flush;
                // EFFECTIVE_BOUNDS bases = {(intptr_t)base, (intptr_t)base};
                // EFFECTIVE_BOUNDS sizes = {0, meta->size};
                // EFFECTIVE_BOUNDS bounds = bases + sizes;
                // *out << "tm = " << tm->CacheLine_0 << ", t->size = " << t->size
                //         << '\n';
                // size_t offset = (uint8_t *)ptr - (uint8_t *)base;
                // size_t offset_unadjusted = offset;
                // *out << "offset = " << offset << ", t->size = " << t->size
                //      << '\n';

                // if (offset >= t->size) {
                //     // The `offset' is >= sizeof(T).  Thus `ptr' may be
                //     // pointing to an element in an array of T.
                //     // Alternatively, `ptr' may be pointing to a FAM at the
                //     // end of T.  Either way, the offset is normalized here.
                //     // EFFECTIVE_BOUNDS adjust = {t->offset_fam, 0};
                //     offset -= t->size;
                //     unsigned __int128 tmp = (unsigned __int128)offset;
                //     tmp *= (unsigned __int128)t->magic;
                //     idx = (size_t)(tmp >> EFFECTIVE_RADIX);
                //     offset -= idx * t->size_fam;
                //     // bounds += adjust;
                //     offset += t->offset_fam;

                //     *out << "FAM or Array. Offset is adjusted. Offset = "
                //          << offset << ", t->size = " << t->size << '\n';
                // }

                // if (offset_unadjusted > meta->size) {
                //     *out << "out of bound error";
                // }
            }

            // Calculate and normalize the `offset'.
        }

        //*out << "\n";
    } 
    // else if (lowfat_is_global_ptr(ptr)) {
    //     *out << "GLOBAL ADDRESS\n";
    // } else if (lowfat_is_stack_ptr(ptr)) {
    //     *out << "STACK ADDRESS\n";
    // }

    // *out << REG_StringShort((REG)reg) << ": " << std::hex << val << ptr_type
    //      << '\n'
    //      << std::flush;
}

// Print a memory write record
VOID RecordMemWrite(ADDRINT pc, ADDRINT addr, ADDRINT size, string *disass,
                    const CONTEXT *ctx, ADDRINT opcode,
                    std::vector<UINT32> *RRegs, std::vector<UINT32> *WRegs) {


    void *ptr = (void *)addr;

    if (lowfat_is_heap_ptr(ptr)) {
        // *out << "RecordMemWrite: "
        //      << ": PC(" << std::hex << pc << ") Addr(" << addr << ") " << std::dec
        //      << "Size(" << size << ") " << std::dec << "Opcode: (" << (UINT32)opcode
        //      << ") (" << std::hex << OPCODE_StringShort(opcode) << ") (" << *disass
        //      << ")\n"
        //      << std::flush;
        // *out << "\n";
        size_t idx = lowfat_index(ptr);
        if (idx > EFFECTIVE_LOWFAT_NUM_REGIONS_LIMIT ||
            _LOWFAT_MAGICS[idx] == 0) {
            *out << "`ptr' is a non-fat-pointer\n";
        } else {
            void *base = lowfat_base(ptr);

            // Get the object meta-data and calculate the allocation bounds.
            EFFECTIVE_META *meta = (EFFECTIVE_META *)base;
            base = (void *)(meta + 1);
            const EFFECTIVE_TYPE *t = meta->type;

            if (EFFECTIVE_UNLIKELY(t == NULL)) {
                *out << "Effective type free!!!\n";
            } else {
                TYCHE_METADATA_CACHELINE* tm = t->tyche_meta;
                *out << tm << " " << std::dec << t->size << "\n" << std::flush;
                *out << 
                "tm_0 = " << std::dec << tm->CacheLine_0 << " " <<
                "tm_1 = " << std::dec << tm->CacheLine_1 << " " <<
                "tm_2 = " << std::dec << tm->CacheLine_2 << " " <<
                "tm_3 = " << std::dec << tm->CacheLine_3 << " " <<
                "tm_4 = " << std::dec << tm->CacheLine_4 << " " <<
                "tm_5 = " << std::dec << tm->CacheLine_5 << " " <<
                "tm_6 = " << std::dec << tm->CacheLine_6 << " " <<
                "tm_7 = " << std::dec << tm->CacheLine_7 << " " <<
                "tm_8 = " << std::dec << tm->CacheLine_8 << " " <<
                "tm_9 = " << std::dec << tm->CacheLine_9 << " " <<
                "tm_10 = " << std::dec << tm->CacheLine_10 << " " <<
                "tm_11 = " << std::dec << tm->CacheLine_11 << " " <<
                "tm_12 = " << std::dec << tm->CacheLine_12 << " " <<
                "tm_13 = " << std::dec << tm->CacheLine_13 << " " <<
                "tm_p = "  << std::hex << tm->next_cacheline << " " <<
                '\n' << std::flush;

                // TYCHE_METADATA_CACHELINE* tm_1 = (TYCHE_METADATA_CACHELINE*)((void*)tm + 64);
                // *out << 
                // "tm_0 = " << std::dec << tm_1->CacheLine_0 << " " <<
                // "tm_1 = " << std::dec << tm_1->CacheLine_1 << " " <<
                // "tm_2 = " << std::dec << tm_1->CacheLine_2 << " " <<
                // "tm_3 = " << std::dec << tm_1->CacheLine_3 << " " <<
                // "tm_4 = " << std::dec << tm_1->CacheLine_4 << " " <<
                // "tm_5 = " << std::dec << tm_1->CacheLine_5 << " " <<
                // "tm_6 = " << std::dec << tm_1->CacheLine_6 << " " <<
                // "tm_7 = " << std::dec << tm_1->CacheLine_7 << " " <<
                // "tm_8 = " << std::dec << tm_1->CacheLine_8 << " " <<
                // "tm_9 = " << std::dec << tm_1->CacheLine_9 << " " <<
                // "tm_10 = " << std::dec << tm_1->CacheLine_10 << " " <<
                // "tm_11 = " << std::dec << tm_1->CacheLine_11 << " " <<
                // "tm_12 = " << std::dec << tm_1->CacheLine_12 << " " <<
                // "tm_13 = " << std::dec << tm_1->CacheLine_13 << " " <<
                // "tm_p = "  << std::hex << tm_1->next_cacheline << " " <<
                // '\n';

                //  tm_1 = (TYCHE_METADATA_CACHELINE*)((void*)tm_1->next_cacheline + 64);
                // *out << 
                // "tm_0 = " << std::dec << tm_1->CacheLine_0 << " " <<
                // "tm_1 = " << std::dec << tm_1->CacheLine_1 << " " <<
                // "tm_2 = " << std::dec << tm_1->CacheLine_2 << " " <<
                // "tm_3 = " << std::dec << tm_1->CacheLine_3 << " " <<
                // "tm_4 = " << std::dec << tm_1->CacheLine_4 << " " <<
                // "tm_5 = " << std::dec << tm_1->CacheLine_5 << " " <<
                // "tm_6 = " << std::dec << tm_1->CacheLine_6 << " " <<
                // "tm_7 = " << std::dec << tm_1->CacheLine_7 << " " <<
                // "tm_8 = " << std::dec << tm_1->CacheLine_8 << " " <<
                // "tm_9 = " << std::dec << tm_1->CacheLine_9 << " " <<
                // "tm_10 = " << std::dec << tm_1->CacheLine_10 << " " <<
                // "tm_11 = " << std::dec << tm_1->CacheLine_11 << " " <<
                // "tm_12 = " << std::dec << tm_1->CacheLine_12 << " " <<
                // "tm_13 = " << std::dec << tm_1->CacheLine_13 << " " <<
                // "tm_p = "  << std::hex << tm_1->next_cacheline << " " <<
                // '\n';

                // EFFECTIVE_BOUNDS bases = {(intptr_t)base, (intptr_t)base};
                // EFFECTIVE_BOUNDS sizes = {0, static_cast<long int>(meta->size)};
                // EFFECTIVE_BOUNDS bounds = bases + sizes;
                // size_t offset = (uint8_t *)ptr - (uint8_t *)base;
                // size_t offset_unadjusted = offset;

                // *out << "offset = " << std::dec << offset << ", t->size = " << t->size
                //      << '\n';

                // if (offset >= t->size) {
                //     // The `offset' is >= sizeof(T).  Thus `ptr' may be
                //     // pointing to an element in an array of T.
                //     // Alternatively, `ptr' may be pointing to a FAM at the
                //     // end of T.  Either way, the offset is normalized here.
                //     EFFECTIVE_BOUNDS adjust = {t->offset_fam, 0};
                //     offset -= t->size;
                //     unsigned __int128 tmp = (unsigned __int128)offset;
                //     tmp *= (unsigned __int128)t->magic;
                //     idx = (size_t)(tmp >> EFFECTIVE_RADIX);
                //     offset -= idx * t->size_fam;
                //     bounds += adjust; bounds = bounds;
                //     offset += t->offset_fam;

                //     *out << "FAM or Array. Offset is adjusted. Offset = "
                //          << std::dec << offset << ", t->size = " << t->size << '\n';
                // }

                // if (offset_unadjusted > meta->size) {
                //     *out << "out of bound error";
                // }
            }

            // Calculate and normalize the `offset'.
        }

        //*out << "\n";
    } 
    // else if (lowfat_is_global_ptr(ptr)) {
    //     *out << "GLOBAL ADDRESS\n";
    // } else if (lowfat_is_stack_ptr(ptr)) {
    //     *out << "STACK ADDRESS\n";
    // }
}

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
    dis = dis;
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

    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be
    // executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP
    // prefixed instructions appear as predicated instructions in Pin.
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Iterate over each memory operand of the instruction.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++) {
        if (INS_MemoryOperandIsRead(ins, memOp)) {
            INS_InsertCall(
                // INS_InsertIfCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead, IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp, IARG_MEMORYREAD_SIZE, IARG_PTR, dis,
                IARG_CONST_CONTEXT, IARG_ADDRINT, INS_Opcode(ins), IARG_PTR,
                maxNumRRegs, IARG_PTR, maxNumWRegs, IARG_END);
        }
        // Note that in some architectures a single memory operand can be
        // both read and written (for instance incl (%eax) on IA-32)
        // In that case we instrument it once for read and once for write.
        // IARG_MEMORYREAD_SIZE  Type: UINT32. Size in bytes of memory read.
        // IARG_MEMORYOP_EA
        // IARG_MEMORYWRITE_SIZE     Type: UINT32. Size in bytes of memory
        // write.
        if (INS_MemoryOperandIsWritten(ins, memOp)) {
            INS_InsertCall(
                // INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite, IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp, IARG_MEMORYWRITE_SIZE, IARG_PTR, dis,
                IARG_CONST_CONTEXT, IARG_ADDRINT, INS_Opcode(ins), IARG_PTR,
                maxNumRRegs, IARG_PTR, maxNumWRegs, IARG_END);
        }
    }
}

#endif
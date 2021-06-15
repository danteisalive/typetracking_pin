/*
 * Copyright 2002-2019 Intel Corporation.
 *
 * This software is provided to you as Sample Source Code as defined in the
 * accompanying End User License Agreement for the Intel(R) Software Development
 * Products ("Agreement") section 1.L.
 *
 * This software and the related documents are provided as is, with no express
 * or implied warranties, other than those that are expressly stated in the
 * License.
 */

/*! @file
 *  Print a detailed disassemble data on each IMG, loops run forward.
 */

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <vector>

#include "pin.H"
//#include <bits/stdc++.h>
// #include <unordered_map>
// #include <queue>

#include "TypeNode.hh"
#include "allocators.hh"
#include "lowfat-ptr-info.hh"
#include "predictor.hh"

using std::atoi;
using std::dec;
using std::endl;
using std::hex;
using std::ios;
using std::ofstream;
using std::showbase;
using std::string;

//#define FILE 1
#define DIRECTORTY 2
#define NOTEXIST 0

ofstream *out = 0;
FILE* effectiveDumpFile;
UINT64 NumOfCalls = 0;

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
UINT64 icount = 0;

#define EFFECTIVETYPECHECK "effective_type_check"

KNOB<string> KnobTypeDotFile(KNOB_MODE_WRITEONCE, "pintool", "d", "merged.dot",
                             "specify dot file name");

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o",
                            "imageload.out", "specify output file name");

KNOB<BOOL> KnobImageOnly(KNOB_MODE_WRITEONCE, "pintool", "l", "0",
                         "List the loaded images");

TypesCount TC;
InsTypeCount ITC;
InsTypeCount ParrentTypeIDs;
TypesLayout  TyCheTypeLayout;
UINT64 ParentTID;
InsTypeCount TypesUsedInEpoch;

DefaultLVPT *ParentTypePredictor;
DefaultBasicTypePredictor *BasicTypePredictor;

VOID Arg1Before(CHAR *name, ADDRINT arg1, ADDRINT arg2) {
    //*out << "EFFECTIVE_SAN: " << arg1 << " and hex : " << std::hex << arg1 <<
    // std::endl;

    // void *ptr = (void *)arg1;
    // size_t idx = lowfat_index(ptr);
    // if (idx > EFFECTIVE_LOWFAT_NUM_REGIONS_LIMIT || _LOWFAT_MAGICS[idx] == 0) {
    //     return;
    // }

    // void *base = lowfat_base(ptr);

    // // Get the object meta-data and calculate the allocation bounds.
    // EFFECTIVE_META *meta = (EFFECTIVE_META *)base;
    // base = (void *)(meta + 1);
    // const EFFECTIVE_TYPE *t = meta->type;

    // if (lowfat_is_heap_ptr(ptr))
    // {
    //     //*out << std::dec << "HEAP PTR: " << meta->size << "\n";
    // }
    // else if (lowfat_is_global_ptr(ptr))
    // {
    //     //*out << std::dec << "GLOBAL PTR: " << meta->size << "\n";
    // }
    // else if (lowfat_is_stack_ptr(ptr))
    // {
    //     //*out << std::dec << "STACK PTR: " << meta->size << "\n";
    // }
    // else
    //     assert(0);

    //*out << std::dec << "PTR: " << meta->size << "\n";

    // if (EFFECTIVE_UNLIKELY(t == NULL)) return;
    // //     t = &EFFECTIVE_TYPE_FREE;

    // void *t1 = (void *)arg2;
    // EFFECTIVE_TYPE *effective_meta = (EFFECTIVE_TYPE *)arg2;
    // effective_meta = effective_meta;
    // uint64_t *effective_type = (uint64_t *)t1;
    // uint64_t *effective_info = effective_type + 6;  //
    // EFFECTIVE_INFO *effective_tid = (EFFECTIVE_INFO *)(*effective_info);




    NumOfCalls++;
}

VOID ImageLoad(IMG img, VOID *v) {
    *out << "Tool loading " << IMG_Name(img) << " at " << IMG_LoadOffset(img)
         << endl;

    // RTN effective_type_check = RTN_FindByName(img, EFFECTIVETYPECHECK);
    // if (RTN_Valid(effective_type_check)) {
    //     //   *out << "FOUND IT! " << RTN_Address(effective_type_check) <<
    //     endl; RTN_Open(effective_type_check);

    //     RTN_InsertCall(effective_type_check, IPOINT_BEFORE,
    //     (AFUNPTR)Arg1Before,
    //                    IARG_ADDRINT, EFFECTIVETYPECHECK,
    //                    IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
    //                    IARG_FUNCARG_ENTRYPOINT_VALUE, 1, IARG_END);

    //     // RTN_InsertCall(effective_type_check, IPOINT_AFTER,
    //     // (AFUNPTR)MallocAfter,
    //     //                IARG_FUNCRET_EXITPOINT_VALUE, IARG_END);

    //     RTN_Close(effective_type_check);
    // }

    //     for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
    //     {
    //         *out << "  sec " << SEC_Name(sec) << " " << SEC_Address(sec) <<
    //         ":" << SEC_Size(sec) << endl; for (RTN rtn = SEC_RtnHead(sec);
    //         RTN_Valid(rtn); rtn = RTN_Next(rtn))
    //         {
    //             *out << "    rtn " << RTN_Name(rtn) << " " <<
    //             RTN_Address(rtn) << ":" << RTN_Size(rtn) << endl;

    //             RTN_Open(rtn);

    //             for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins =
    //             INS_Next(ins))
    //             {
    //                 // Just print first and last
    //                 if (!INS_Valid(INS_Prev(ins)) ||
    //                 !INS_Valid(INS_Next(ins)))
    //                 {
    //                     *out << "      " << INS_Address(ins);

    // #if 0
    //                     *out << " " << INS_Disassemble(ins) << " read:";

    //                     for (UINT32 i = 0; i < INS_MaxNumRRegs(ins); i++)
    //                     {
    //                         *out << " " << REG_StringShort(INS_RegR(ins, i));
    //                     }
    //                     *out << " writes:";
    //                     for (UINT32 i = 0; i < INS_MaxNumWRegs(ins); i++)
    //                     {
    //                         *out << " " << REG_StringShort(INS_RegW(ins, i));
    //                     }
    // #endif

    //                     *out <<  endl;
    //                 }
    //             }

    //             RTN_Close(rtn);
    //         }
    //     }
}

VOID ImageUnload(IMG img, VOID *v) {
    *out << "Tool unloading " << IMG_Name(img) << " at " << IMG_LoadOffset(img)
         << endl;
}

VOID Trace(TRACE trace, VOID *v) {
    if (!RTN_Valid(TRACE_Rtn(trace))) return;

    if ((!IMG_Valid(SEC_Img(RTN_Sec(TRACE_Rtn(trace)))) ||
         !IMG_IsMainExecutable(SEC_Img(RTN_Sec(TRACE_Rtn(trace))))))
        return;

    std::string rtn_name = RTN_Name(TRACE_Rtn(trace));
    if ((rtn_name.find("lowfat_") != std::string::npos) ||
        (rtn_name.find("effective_") != std::string::npos) ||
        (rtn_name.find("EFFECTIVE_") != std::string::npos) ||
        (rtn_name.find("LOWFAT_") != std::string::npos)) {
        //*out << "Function Name: " << rtn_name << std::endl;
        return;
    } else {
        //*out << "Function Name: " << rtn_name << std::endl;
    }

    // TRACE_InsertCall(trace, IPOINT_BEFORE, (AFUNPTR)PrintRegisters,
    // IARG_CONST_CONTEXT, IARG_END);

    // Insert a call to record the effective address.
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins)) {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)PrintRegisters,
                           IARG_INST_PTR, IARG_CONST_CONTEXT, IARG_END);
        }
    }
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v) {
    // Write to a file since cout and cerr maybe closed by the application
    // out->setf(ios::showbase);
    //*out << "Count " << std::dec << NumOfCalls << endl;

    for (InsTypeCount::iterator it = ITC.begin(); it != ITC.end(); it++) {
        if (it->second != 0) {
            *out << std::dec << it->first << "(" << it->second << ")\n";
            it->second = 0;
        }
    }
    *out << '\n' << std::flush;
    *out << "------------------------------------------------------------------------\n" << std::flush;


    out->close();
}


int main(INT32 argc, CHAR **argv) {
    PIN_InitSymbols();
    PIN_Init(argc, argv);

    effectiveDumpFile = fopen("EffectiveSan.log", "w") ;
    out = new ofstream(KnobOutputFile.Value().c_str());
    *out << hex << showbase;

    ParentTypePredictor = new DefaultLVPT(1024, 16, 0, 1);
    BasicTypePredictor = new DefaultBasicTypePredictor(1024, 16, 0, 1);

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    IMG_AddInstrumentFunction(ImageLoad, 0);
    IMG_AddUnloadFunction(ImageUnload, 0);

    // TRACE_AddInstrumentFunction(Trace, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    ParentTID = 0;

    // Never returns
    PIN_StartProgram();

    return 0;
}

/*
VOID Instruction(INS ins, VOID *v)
{
 ...
 // Instrument REGISTER writes using a predicated call, i.e.
 // the call happens iff the register store will be actually executed
 UINT32 maxNumWRegs;
 maxNumWRegs = INS_MaxNumWRegs(ins);
 for (UINT32 i=0; i<maxNumWRegs; i++)
 {
     // --> BEFORE
     INS_InsertPredicatedCall(
        ins, IPOINT_BEFORE, (AFUNPTR)SaveRegWrite,
        IARG_INST_PTR,
        IARG_UINT32,
        INS_RegW(ins, i),
        IARG_END);

    if (INS_HasFallThrough(ins))
    {
        // --> AFTER
        INS_InsertCall(
            ins, IPOINT_AFTER, (AFUNPTR)LogRegWrite,
            IARG_INST_PTR,
            IARG_UINT32,
            INS_RegW(ins, i),
            IARG_CONTEXT, // to access the register value
            IARG_END);
    }
    if (INS_IsBranchOrCall(ins))
    {
        INS_InsertCall(
            ins, IPOINT_TAKEN_BRANCH, (AFUNPTR)LogRegWrite,
            IARG_INST_PTR,
            IARG_UINT32,
            INS_RegW(ins, i),
            IARG_CONTEXT, // to access the register value
            IARG_END);
    }

    const CONTEXT *ctx; (from the arguments of LogRegWrite...)
    PIN_REGISTER regval;
    PIN_GetContextRegval(ctx, REG_SEG_GS,
reinterpret_cast<UINT8*>(&regval)); PIN_GetContextRegval(ctx, REG_SEG_FS,
reinterpret_cast<UINT8*>(&regval)); PIN_GetContextRegval(ctx,
REG_SEG_GS_BASE, reinterpret_cast<UINT8*>(&regval));
PIN_GetContextRegval(ctx, REG_SEG_FS_BASE,
reinterpret_cast<UINT8*>(&regval));
}
*/

// if(INS_IsMemoryRead(ins) && INS_IsStandardMemop(ins))
//             {
//                 INS_InsertFillBuffer(ins, IPOINT_BEFORE, bufId,
//                                      IARG_INST_PTR, offsetof(struct MEMREF,
//                                      pc), IARG_MEMORYREAD_EA, offsetof(struct
//                                      MEMREF, ea), IARG_END);
//             }

// if (INS_HasMemoryRead2(ins) && INS_IsStandardMemop(ins))
//             {
//                 INS_InsertFillBuffer(ins, IPOINT_BEFORE, bufId,
//                                      IARG_INST_PTR, offsetof(struct MEMREF,
//                                      pc), IARG_MEMORYREAD2_EA,
//                                      offsetof(struct MEMREF, ea), IARG_END);
//             }

// if(INS_IsMemoryWrite(ins) && INS_IsStandardMemop(ins))
//             {
//                 INS_InsertFillBuffer(ins, IPOINT_BEFORE, bufId,
//                                      IARG_INST_PTR, offsetof(struct MEMREF,
//                                      pc), IARG_MEMORYWRITE_EA,
//                                      offsetof(struct MEMREF, ea), IARG_END);
//             }

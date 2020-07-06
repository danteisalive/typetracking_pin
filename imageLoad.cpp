/*
 * Copyright 2002-2019 Intel Corporation.
 * 
 * This software is provided to you as Sample Source Code as defined in the accompanying
 * End User License Agreement for the Intel(R) Software Development Products ("Agreement")
 * section 1.L.
 * 
 * This software and the related documents are provided as is, with no express or implied
 * warranties, other than those that are expressly stated in the License.
 */

/*! @file
 *  Print a detailed disassemble data on each IMG, loops run forward.
 */


#include <iostream>
#include <fstream>
#include <iomanip>
#include "pin.H"
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <vector>
//#include <bits/stdc++.h>
// #include <unordered_map>
// #include <queue>

#include "TypeNode.hh"
#include "lowfat-ptr-info.hh"
#include "allocators.hh"

using std::dec;
using std::string;
using std::ios;
using std::hex;
using std::showbase;
using std::ofstream;
using std::endl;
using std::atoi;


#define FILE 1
#define DIRECTORTY 2
#define NOTEXIST 0




ofstream *out=0;

static UINT64 NumOfCalls = 0; 

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
static UINT64 icount = 0;

#define EFFECTIVETYPECHECK "effective_type_check"


KNOB<string> KnobTypeDotFile(KNOB_MODE_WRITEONCE, "pintool",
    "d", "merged.dot", "specify dot file name");

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "imageload.out", "specify output file name");

KNOB<BOOL> KnobImageOnly(KNOB_MODE_WRITEONCE, "pintool",
    "l", "0", "List the loaded images");


typedef std::map<uint64_t , std::pair<const char*, uint64_t> > TypesCount;
typedef TypesCount TypesCount;


TypesCount                              TC;
std::map<int, std::vector<int> >        TypeTreeTID;
std::map<uint64_t, int>                 HashMapTID;







// This function is called before every instruction is executed
VOID docount() 
{ 
    icount++; 
    uint64_t nonzero_tid = 0;
    if (icount % 100000000 == 0){
        //*out << std::dec << icount/100000000 << " " << NumOfCalls << endl;
        
        for (TypesCount::iterator it = TC.begin(); it != TC.end(); it++)
        {
            if (it->second.second != 0){
                *out << std::dec << it->second.second << " ";
                it->second.second = 0;
                nonzero_tid++;
            }
        }
        *out <<  nonzero_tid  << std::endl;
        NumOfCalls = 0;
    }
}

VOID Arg1Before(CHAR * name, ADDRINT arg1, ADDRINT arg2)
{
   
    
    
    void * ptr = (void*)arg1;
    size_t idx = lowfat_index(ptr);
    if (idx > EFFECTIVE_LOWFAT_NUM_REGIONS_LIMIT || _LOWFAT_MAGICS[idx] == 0)
    {
        return;
    }


    void *base = lowfat_base(ptr);

    // Get the object meta-data and calculate the allocation bounds.
    EFFECTIVE_META *meta = (EFFECTIVE_META *)base;
    base = (void *)(meta + 1);
    const EFFECTIVE_TYPE *t = meta->type;
    
    if (lowfat_is_heap_ptr(ptr))
    {
        //*out << std::dec << "HEAP PTR: " << meta->size << "\n";
    }
    else if (lowfat_is_global_ptr(ptr))
    {
        //*out << std::dec << "GLOBAL PTR: " << meta->size << "\n";
    }
    else if (lowfat_is_stack_ptr(ptr))
    {
        //*out << std::dec << "STACK PTR: " << meta->size << "\n";
    }
    else 
        assert(0);

    //*out << std::dec << "PTR: " << meta->size << "\n";


    if (EFFECTIVE_UNLIKELY(t == NULL))
        return;
    //     t = &EFFECTIVE_TYPE_FREE;

    
    void * t1 = (void*) arg2;
    EFFECTIVE_TYPE * effective_meta = (EFFECTIVE_TYPE*)arg2;
    effective_meta = effective_meta;
    uint64_t * effective_type = (uint64_t *)t1;
    uint64_t * effective_info = effective_type + 6; // 
    EFFECTIVE_INFO * effective_tid = (EFFECTIVE_INFO *)(*effective_info);

    if ( effective_tid->tid_info->num_accesses == 12345)
    {
        //  *out << std::dec   << t->info->name << "(" << t->info->tid_info->tid << ")" <<  
        //                     " " << effective_tid->name << "(" << effective_tid->tid_info->tid << ")" << 
        //                     std::endl; 

        // *out << std::dec << effective_tid->name << " " << 
        //                     effective_meta->hash << " " <<
        //                     effective_meta->hash2 << " " << 
        //                     effective_tid->tid_info->tid << " " << 
        //                     effective_tid->tid_info->num_accesses <<  std::endl;

        // TypesCount::iterator type_count_iter = TC.find(effective_meta->hash);
        // if (type_count_iter == TC.end())
        // {
        //     TC.insert(std::make_pair(effective_meta->hash, std::make_pair(effective_tid->name, 0)));
        // }
        // else 
        // {
        //     if (type_count_iter->second.first != effective_tid->name){
        //         assert(0);
        //     }
        //     else 
        //     {
        //         type_count_iter->second.second++;
        //     }
        // }
        TypesCount::iterator type_count_iter = TC.find(t->hash);
        if (type_count_iter == TC.end())
        {
            TC.insert(std::make_pair(t->hash, std::make_pair(t->info->name, 0)));
        }
        else 
        {
            if (type_count_iter->second.first != t->info->name){
                assert(0);
            }
            else 
            {
                type_count_iter->second.second++;
            }
        }
    }
    else 
        assert(0);


    NumOfCalls++;
}

VOID ImageLoad(IMG img, VOID * v)
{
    *out << "Tool loading " << IMG_Name(img) << " at " << IMG_LoadOffset(img) << endl;

    RTN effective_type_check = RTN_FindByName(img, EFFECTIVETYPECHECK);
    if (RTN_Valid(effective_type_check))
    {

     //   *out << "FOUND IT! " << RTN_Address(effective_type_check) << endl;
        RTN_Open(effective_type_check);

        
        RTN_InsertCall(effective_type_check, IPOINT_BEFORE, (AFUNPTR)Arg1Before,
                       IARG_ADDRINT, EFFECTIVETYPECHECK,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
                       IARG_END);
        
        // RTN_InsertCall(effective_type_check, IPOINT_AFTER, (AFUNPTR)MallocAfter,
        //                IARG_FUNCRET_EXITPOINT_VALUE, IARG_END);

        RTN_Close(effective_type_check);
    }


//     for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
//     {
//         *out << "  sec " << SEC_Name(sec) << " " << SEC_Address(sec) << ":" << SEC_Size(sec) << endl;
//         for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
//         {
//             *out << "    rtn " << RTN_Name(rtn) << " " << RTN_Address(rtn) << ":" << RTN_Size(rtn) << endl;

//             RTN_Open(rtn);
            
//             for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
//             {
//                 // Just print first and last
//                 if (!INS_Valid(INS_Prev(ins)) || !INS_Valid(INS_Next(ins)))
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

VOID ImageUnload(IMG img, VOID * v)
{
    *out << "Tool unloading " << IMG_Name(img) << " at " << IMG_LoadOffset(img) << endl;
}

VOID Trace(TRACE trace, VOID *v)
{
    if (!RTN_Valid(TRACE_Rtn(trace)))
        return; 

    if ((!IMG_Valid(SEC_Img(RTN_Sec(TRACE_Rtn(trace))))
                || !IMG_IsMainExecutable(SEC_Img(RTN_Sec(TRACE_Rtn(trace)))) ))
        return;
        


    std::string rtn_name = RTN_Name(TRACE_Rtn(trace));
    if (
        (rtn_name.find("lowfat_") != std::string::npos)     || 
        (rtn_name.find("effective_") != std::string::npos)  || 
        (rtn_name.find("EFFECTIVE_") != std::string::npos)  ||
        (rtn_name.find("LOWFAT_") != std::string::npos) 
    )
    {
        //*out << "Function Name: " << rtn_name << std::endl; 
        return;
    }
    else 
    {
        //*out << "Function Name: " << rtn_name << std::endl; 
    }

    //TRACE_InsertCall(trace, IPOINT_BEFORE, (AFUNPTR)PrintRegisters, IARG_CONST_CONTEXT, IARG_END);

    // Insert a call to record the effective address.
    for(BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl=BBL_Next(bbl))
    {
        for(INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins=INS_Next(ins))
        {
            
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)PrintRegisters, IARG_INST_PTR , IARG_CONST_CONTEXT, IARG_END);
            // if(INS_IsMemoryRead(ins) && INS_IsStandardMemop(ins))
            // {
            //     INS_InsertFillBuffer(ins, IPOINT_BEFORE, bufId,
            //                          IARG_INST_PTR, offsetof(struct MEMREF, pc),
            //                          IARG_MEMORYREAD_EA, offsetof(struct MEMREF, ea),
            //                          IARG_END);
            // }

            // if (INS_HasMemoryRead2(ins) && INS_IsStandardMemop(ins))
            // {
            //     INS_InsertFillBuffer(ins, IPOINT_BEFORE, bufId,
            //                          IARG_INST_PTR, offsetof(struct MEMREF, pc),
            //                          IARG_MEMORYREAD2_EA, offsetof(struct MEMREF, ea),
            //                          IARG_END);
            // }

            // if(INS_IsMemoryWrite(ins) && INS_IsStandardMemop(ins))
            // {
            //     INS_InsertFillBuffer(ins, IPOINT_BEFORE, bufId,
            //                          IARG_INST_PTR, offsetof(struct MEMREF, pc),
            //                          IARG_MEMORYWRITE_EA, offsetof(struct MEMREF, ea),
            //                          IARG_END);
            // }
        }
    }
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    // Write to a file since cout and cerr maybe closed by the application
    //out->setf(ios::showbase);
    //*out << "Count " << std::dec << NumOfCalls << endl;
    out->close();
}


// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)
{

    // Insert a call to docount before every instruction, no arguments are passed
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_END);

    if (!RTN_Valid(INS_Rtn(ins)))
        return; 

    if ((!IMG_Valid(SEC_Img(RTN_Sec(INS_Rtn(ins))))
                || !IMG_IsMainExecutable(SEC_Img(RTN_Sec(INS_Rtn(ins)))) ))
        return;
        

    // SANITIZING SOME OF THE EFFECTIVE AND LOWFAT FUNCTIONS
    std::string rtn_name = RTN_Name(INS_Rtn(ins));
    if (
        (rtn_name.find("lowfat_") != std::string::npos)     || 
        (rtn_name.find("effective_") != std::string::npos)  || 
        (rtn_name.find("EFFECTIVE_") != std::string::npos)  ||
        (rtn_name.find("LOWFAT_") != std::string::npos) 
    )
    {
        //*out << "Function Name: " << rtn_name << std::endl; 
        return;
    }
    else 
    {
        //*out << "Function Name: " << rtn_name << std::endl; 
    }

    //IN CASE WE NEED THE WHOLE CONTEXT
    //INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)PrintRegisters, IARG_INST_PTR , IARG_CONST_CONTEXT, IARG_END);

    std::vector<UINT32> maxNumRRegs;
    for (UINT32 i = 0; i < INS_MaxNumRRegs(ins); i++)
    {
        if (REG_is_gr(INS_RegR(ins, i))) maxNumRRegs.push_back(i);
    }

    switch (INS_MaxNumRRegs(ins))
    {
        case 1:
        {
            if (REG_is_gr(INS_RegR(ins, 0)))
            {
                std::string * dis = new std::string(INS_Disassemble(ins)); 
                std::string * reg_name = new std::string(REG_StringShort(INS_RegR(ins, 0)));

                INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)PrintRRegisters_1, 
                                IARG_ADDRINT, "Read Before",
                                IARG_PTR, dis, 
                                IARG_INST_PTR, 
                                IARG_PTR, reg_name,
                                IARG_REG_VALUE, INS_RegR(ins, 0),
                                IARG_END);
                if (INS_IsValidForIpointAfter(ins))
                    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)PrintRRegisters_1, 
                                IARG_ADDRINT, "Read After",
                                IARG_PTR, dis, 
                                IARG_INST_PTR, 
                                IARG_PTR, reg_name,
                                IARG_REG_VALUE, INS_RegR(ins, 0),
                                IARG_END);
            }
            break;
        }
        default:
        {
            break;
        }
    }

    switch (INS_MaxNumWRegs(ins))
    {
        case 1:
        {
            if (REG_is_gr(INS_RegW(ins, 0)))
            {
                std::string * dis = new std::string(INS_Disassemble(ins)); 
                std::string * reg_name = new std::string(REG_StringShort(INS_RegW(ins, 0)));

                INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)PrintWRegisters_1, 
                                IARG_ADDRINT, "Write Before",
                                IARG_PTR, dis, 
                                IARG_INST_PTR, 
                                IARG_PTR, reg_name,
                                IARG_REG_VALUE, INS_RegW(ins, 0),
                                IARG_END);
                if (INS_IsValidForIpointAfter(ins))
                    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)PrintWRegisters_1, 
                                IARG_ADDRINT, "Write After",
                                IARG_PTR, dis, 
                                IARG_INST_PTR, 
                                IARG_PTR, reg_name,
                                IARG_REG_VALUE, INS_RegW(ins, 0),
                                IARG_END);
            }
            break;
        }
        default:
        {
            break;
        }
    }


    
}

int main(INT32 argc, CHAR **argv)
{
    PIN_InitSymbols();
    PIN_Init(argc, argv);
    
    out = new ofstream(KnobOutputFile.Value().c_str());
    *out << hex << showbase;
    

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    IMG_AddInstrumentFunction(ImageLoad, 0);
    IMG_AddUnloadFunction(ImageUnload, 0);

    //TRACE_AddInstrumentFunction(Trace, 0);
    

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);

    std::string TIDFileName("merged.hash");
    std::string HashMapFileName("final.hash");


    // Replace 'Plop' with your file name.
    std::ifstream           TIDFile(TIDFileName.c_str());
    std::ifstream           HashMapFile(HashMapFileName.c_str());

    std::string   line;
    // Read one line at a time into the variable line:
    while(std::getline(TIDFile, line))
    {
        std::stringstream  lineStream(line);

        int value, key;
        lineStream >> key;
        // Read an integer at a time from the line
        while(lineStream >> value)
        {
            // Add the integers from a line to a 1D array (vector)
            TypeTreeTID[key].push_back(value);
        }
    }

    while(std::getline(HashMapFile, line))
    {
        std::stringstream  lineStream(line);

        uint64_t  key;
        int value;

        lineStream >> key;
        lineStream >> value;
        // Add the integers from a line to a 1D array (vector)
        HashMapTID[key] = value;

    }


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
    PIN_GetContextRegval(ctx, REG_SEG_GS, reinterpret_cast<UINT8*>(&regval));
    PIN_GetContextRegval(ctx, REG_SEG_FS, reinterpret_cast<UINT8*>(&regval));
    PIN_GetContextRegval(ctx, REG_SEG_GS_BASE, reinterpret_cast<UINT8*>(&regval));
    PIN_GetContextRegval(ctx, REG_SEG_FS_BASE, reinterpret_cast<UINT8*>(&regval));   
}   
*/
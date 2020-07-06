#ifndef __ALLOCATORS_HH__
#define __ALLOCATORS_HH__

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
#include <sstream>


using std::stringstream;

typedef std::map<uint64_t , std::pair<const char*, uint64_t> > TypesCount;
typedef TypesCount TypesCount;


extern ofstream *out;
extern TypesCount                              TC;
extern std::map<int, std::vector<int> >        TypeTreeTID;
extern std::map<uint64_t, int>                 HashMapTID;
extern UINT64 NumOfCalls; 

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
extern UINT64 icount;


string Val2Str(const void* value, unsigned int size)
{
    stringstream sstr;
    sstr << hex;
    const unsigned char* cval = (const unsigned char*)value;
    // Traverse cval from end to beginning since the MSB is in the last block of cval.
    while (size)
    {
        --size;
        sstr << (unsigned int)cval[size];
    }
    return string("0x")+sstr.str();
}

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

// static void PrintRRegisters_1(CHAR* where, string *disass, ADDRINT pc ,  string *reg_name, UINT64 val)
// {
//     *out << "PrintRRegisters_1: " << where << ": " << std::hex << pc << ": "<< *disass << 
//             " => "<< *reg_name << "(" << val << ")" <<  
//             std::endl << std::endl ;
    

// }

// static void PrintRRegisters_2(CHAR* where, string *disass, ADDRINT pc ,  
//                             string *reg_name_0, UINT64 val_0, 
//                             string *reg_name_1, UINT64 val_1)
// {
//     *out << "PrintRRegisters_2: " << where << ": " << std::hex << pc << ": "<< *disass << " => "<< 
//             *reg_name_0 << "(" << val_0 << ") " <<  
//             *reg_name_1 << "(" << val_1 << ") " <<  
//             std::endl << std::endl ;
    
// }

// static void PrintRRegisters_3(CHAR* where, string *disass, ADDRINT pc ,  
//                             string *reg_name_0, UINT64 val_0, 
//                             string *reg_name_1, UINT64 val_1,
//                             string *reg_name_2, UINT64 val_2)
// {
//     *out << "PrintRRegisters_3: " << where << ": " << std::hex << pc << ": "<< *disass << " => "<< 
//             *reg_name_0 << "(" << val_0 << ") " <<  
//             *reg_name_1 << "(" << val_1 << ") " <<  
//             *reg_name_2 << "(" << val_2 << ") " <<  
//             std::endl << std::endl ;
    
// }

// static void PrintWRegisters_1(CHAR* where, string *disass, ADDRINT pc ,  string *reg_name, UINT64 val)
// {
//     *out << "PrintWRegisters_1: "  << where << ": " << std::hex << pc << ": "<< *disass << 
//             " => "<< *reg_name << "(" << val << ")" <<  
//             std::endl << std::endl ;
    

// }

// static void PrintWRegisters_2(CHAR* where, string *disass, ADDRINT pc ,  
//                             string *reg_name_0, UINT64 val_0, 
//                             string *reg_name_1, UINT64 val_1)
// {
//     *out << "PrintWRegisters_2: " << where << ": " << std::hex << pc << ": "<< *disass << " => "<< 
//             *reg_name_0 << "(" << val_0 << ") " <<  
//             *reg_name_1 << "(" << val_1 << ") " <<  
//             std::endl << std::endl ;
    
// }

static void PrintRegistersVectorized(CHAR* where, string *disass, 
                                    ADDRINT pc,
                                    const CONTEXT * ctx,  
                                    std::vector<UINT32>* RRegs,
                                    std::vector<UINT32>* WRegs)
{

    *out << "PrintRegistersVectorized: " << where << ": " << std::hex << pc << ": "<< *disass << std::endl;

    *out << "Reads: " << std::endl;
    for (uint64_t i = 0; i < RRegs->size(); i++)
    {
        const UINT grRegSize = REG_Size((REG)(*RRegs)[i]);
        PIN_REGISTER val;
        PIN_GetContextRegval(ctx, (REG)(*RRegs)[i], reinterpret_cast<UINT8*>(&val));

        *out << REG_StringShort((REG)(*RRegs)[i])  << "(" << Val2Str(&val, grRegSize) << ") ";
    }

    *out << std::endl << "Writes: " << std::endl;
    for (uint64_t i = 0; i < WRegs->size(); i++)
    {
        const UINT grRegSize = REG_Size((REG)(*WRegs)[i]);
        PIN_REGISTER val;
        PIN_GetContextRegval(ctx, (REG)(*WRegs)[i], reinterpret_cast<UINT8*>(&val));
        *out << REG_StringShort((REG)(*WRegs)[i])  << "(" << Val2Str(&val, grRegSize) << ") ";
    }
    
    *out << std::endl ;
    
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

static void PrintRegisters(ADDRINT pc , const CONTEXT * ctxt)
{
    *out << "-------------------- " << std::hex << pc << std::dec << " ------------------" << std::endl;
    for (int reg = (int)REG_GR_BASE; reg <= (int)REG_GR_LAST; ++reg)
    {
        // For the integer registers, it is safe to use ADDRINT. But make sure to pass a pointer to it.
        ADDRINT val;
        PIN_GetContextRegval(ctxt, (REG)reg, reinterpret_cast<UINT8*>(&val));

        void * ptr = (void *) val;
        std::string ptr_type = "";
        
            if (lowfat_is_heap_ptr(ptr))
            {
                ptr_type += "(HEAP)";
            }
            else if (lowfat_is_global_ptr(ptr))
            {
                ptr_type += "(GLOBAL)";
            }
            else if (lowfat_is_stack_ptr(ptr))
            {
                size_t idx = lowfat_index(ptr);
                if (idx > EFFECTIVE_LOWFAT_NUM_REGIONS_LIMIT || _LOWFAT_MAGICS[idx] == 0)
                {
                    ;
                }
                else 
                {
                    ptr_type += "(STACK)";
                }
                
            }
        
        *out << REG_StringShort((REG)reg) << ": " << std::hex << val << ptr_type << endl;
    }

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

    std::string * dis = new std::string(INS_Disassemble(ins)); 
    std::vector<UINT32>* maxNumRRegs = new std::vector<UINT32>();
    std::vector<UINT32>* maxNumWRegs = new std::vector<UINT32>();
    

    for (UINT32 i = 0; i < INS_MaxNumRRegs(ins); i++)
    {
        if      (REG_is_gr64(INS_RegR(ins, i)))     maxNumRRegs->push_back(INS_RegR(ins, i));
        else if (REG_is_gr32(INS_RegR(ins, i)))     maxNumRRegs->push_back(INS_RegR(ins, i));
        else if (REG_is_gr16(INS_RegR(ins, i)))     maxNumRRegs->push_back(INS_RegR(ins, i));
        else if (REG_is_gr8(INS_RegR(ins, i)))      maxNumRRegs->push_back(INS_RegR(ins, i));
        
    }
    for (UINT32 i = 0; i < INS_MaxNumWRegs(ins); i++)
    {
        if      (REG_is_gr64(INS_RegW(ins, i)))     maxNumWRegs->push_back(INS_RegW(ins, i));
        else if (REG_is_gr32(INS_RegW(ins, i)))     maxNumWRegs->push_back(INS_RegW(ins, i));
        else if (REG_is_gr16(INS_RegW(ins, i)))     maxNumWRegs->push_back(INS_RegW(ins, i));
        else if (REG_is_gr8(INS_RegW(ins, i)))      maxNumWRegs->push_back(INS_RegW(ins, i));
    }

    if (maxNumRRegs->size() != 0 || maxNumWRegs->size() != 0){

        if (INS_IsValidForIpointAfter(ins))
        {
            INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)PrintRegistersVectorized, 
                                IARG_ADDRINT, "IPOINT_AFTER",
                                IARG_PTR, dis, 
                                IARG_INST_PTR, 
                                IARG_CONST_CONTEXT,
                                IARG_PTR, maxNumRRegs,
                                IARG_PTR, maxNumWRegs,
                                IARG_END);
        }
        else 
        {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)PrintRegistersVectorized, 
                                IARG_ADDRINT, "IPOINT_BEFORE",
                                IARG_PTR, dis, 
                                IARG_INST_PTR, 
                                IARG_CONST_CONTEXT,
                                IARG_PTR, maxNumRRegs,
                                IARG_PTR, maxNumWRegs,
                                IARG_END);
        }
    }

    
}

#endif
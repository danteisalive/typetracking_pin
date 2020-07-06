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

extern ofstream *out;


static void PrintRRegisters_1(CHAR* where, string *disass, ADDRINT pc ,  string *reg_name, UINT64 val)
{
    *out << where << ": " << std::hex << pc << ": "<< *disass << 
            " => "<< *reg_name << "(" << val << ")" <<  std::endl;
    

}

static void PrintWRegisters_1(CHAR* where, string *disass, ADDRINT pc ,  string *reg_name, UINT64 val)
{
    *out << where << ": " << std::hex << pc << ": "<< *disass << 
            " => "<< *reg_name << "(" << val << ")" <<  std::endl;
    

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


#endif
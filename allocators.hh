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
    *out << "PrintRRegisters_1: " << where << ": " << std::hex << pc << ": "<< *disass << 
            " => "<< *reg_name << "(" << val << ")" <<  
            std::endl << std::endl ;
    

}

static void PrintRRegisters_2(CHAR* where, string *disass, ADDRINT pc ,  
                            string *reg_name_0, UINT64 val_0, 
                            string *reg_name_1, UINT64 val_1)
{
    *out << "PrintRRegisters_2: " << where << ": " << std::hex << pc << ": "<< *disass << " => "<< 
            *reg_name_0 << "(" << val_0 << ") " <<  
            *reg_name_1 << "(" << val_1 << ") " <<  
            std::endl << std::endl ;
    
}

static void PrintRRegisters_3(CHAR* where, string *disass, ADDRINT pc ,  
                            string *reg_name_0, UINT64 val_0, 
                            string *reg_name_1, UINT64 val_1,
                            string *reg_name_2, UINT64 val_2)
{
    *out << "PrintRRegisters_3: " << where << ": " << std::hex << pc << ": "<< *disass << " => "<< 
            *reg_name_0 << "(" << val_0 << ") " <<  
            *reg_name_1 << "(" << val_1 << ") " <<  
            *reg_name_2 << "(" << val_2 << ") " <<  
            std::endl << std::endl ;
    
}

static void PrintWRegisters_1(CHAR* where, string *disass, ADDRINT pc ,  string *reg_name, UINT64 val)
{
    *out << "PrintWRegisters_1: "  << where << ": " << std::hex << pc << ": "<< *disass << 
            " => "<< *reg_name << "(" << val << ")" <<  
            std::endl << std::endl ;
    

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
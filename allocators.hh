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

typedef UINT32 CACHE_STATS; // type of cache hit/miss counters

#include "pin_cache.H"


namespace MC
{
    // 1st level data cache: 4 kB, 64B lines, 8-way associative
    const UINT32 cacheSize = 4*KILO;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 8;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_NO_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR MC::CACHE Meta_Cache("Meta Cache", MC::cacheSize, MC::lineSize, MC::associativity);

using std::stringstream;

//#define ENABLE_TYCHE_LAYOUT_DEBUG 1
#define TYCHE_SECTION_0_START_ADDR 0x4000000
#define TYCHE_SECTION_0_END_ADDR 0x4100000
#define TYCHE_CACHELINE_SIZE 64
#define TYCHE_OFFSETS_IN_EACH_CACHELINE 32

typedef std::map<uint64_t, std::pair<const char *, uint64_t> > TypesCount;
typedef TypesCount TypesCount;

typedef std::map<std::string, uint64_t> InsTypeCount;
typedef InsTypeCount InsTypeCount;

extern ofstream *out;
extern TypesCount TC;
extern InsTypeCount ITC;
extern InsTypeCount TypeIDs;
extern UINT64 TID;

extern UINT64 NumOfCalls;


extern DefaultLVPT *ParentTypePredictor;
extern DefaultLVPT *BasicTypePredictor;


// The running count of instructions is kept here
// make it static to help the compiler optimize docount
extern UINT64 icount;


void dumpEffectiveTypeInfo(const EFFECTIVE_INFO* info)
{
    while (info != NULL)
    {
        *out << "Type Name: " << info->name << "\n";
        for (size_t i = 0; i < info->num_entries; i++)
        {
            dumpEffectiveTypeInfo(info->entries[i].type);
        }
        *out << "------\n";            
        info = info->next;
    }
}

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
        // for (InsTypeCount::iterator it = ITC.begin(); it != ITC.end(); it++) {
        //     if (it->second != 0) {
        //         //*out << std::dec << it->first << "(" << it->second <<  ")\n";
        //         it->second = 0;
        //         numOfRulesUsedDuringThisPeriod++;
        //     }
        // }

        *out << std::dec << numOfRulesUsedDuringThisPeriod << " "
             << (double)ParentTypePredictor->LVPTMissprediction / ParentTypePredictor->LVPTNumOfAccesses
             << '\n'
             << std::flush;

        //*out << Meta_Cache << std::flush;

        // for (InsTypeCount::iterator it = TypeIDs.begin(); it != TypeIDs.end(); it++) {
        //     if (it->second != 0) {
        //         *out << std::dec << it->first << "(" << it->second << ")\n";
        //         it->second = 0;
        //     }
        // }
        // *out << '\n' << std::flush;
        *out << "------------------------------------------------------------------------\n"  << std::flush;

    }
}

bool findPtrType(const EFFECTIVE_INFO *info) {
    assert(info->num_entries >= 0);

    if (info->num_entries == 0) {
        //*out << std::dec << info->name << "\n" << std::flush;
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
    // *out << "-------------------- " << std::hex << pc << std::dec
    //      << " ------------------" << '\n'
    //      << std::flush;
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

        // *out << REG_StringShort((REG)reg) << ": " << std::hex << val << ptr_type
        //      << '\n'
        //      << std::flush;
    }
}


VOID RecordMemRead(ADDRINT pc, ADDRINT addr, ADDRINT size, string *disass,
                   const CONTEXT *ctx, ADDRINT opcode,
                   std::vector<UINT32> *RRegs, std::vector<UINT32> *WRegs) {


    void *ptr = (void *)addr;

    if (lowfat_is_heap_ptr(ptr)) {
        size_t idx = lowfat_index(ptr);
        if (idx > EFFECTIVE_LOWFAT_NUM_REGIONS_LIMIT ||
            _LOWFAT_MAGICS[idx] == 0) {
            *out << "`ptr' is a non-fat-pointer\n";
        } else {
            void *base = lowfat_base(ptr);
            
            // if this memory operation is part of updating the fat pointers, igonore it

            // Get the object meta-data and calculate the allocation bounds.
            EFFECTIVE_META *meta = (EFFECTIVE_META *)base;
            base = (void *)(meta + 1);
            const EFFECTIVE_TYPE *t = meta->type;

            if (EFFECTIVE_UNLIKELY(t == NULL)) {
                *out << "Effective type free!!!\n";
            } else {

                // Update the Type ID
                if (TypeIDs.find(std::string(t->info->name)) == TypeIDs.end())
                    TypeIDs[std::string(t->info->name)] = TID++;
                // How many different types were accesses during this epoch
                ITC[std::string(t->info->name)]++;

                // Verify the type information layout
                TYCHE_METADATA_CACHELINE* tm = t->tyche_meta;
                assert((uint64_t)tm >= TYCHE_SECTION_0_START_ADDR && (uint64_t)tm < TYCHE_SECTION_0_END_ADDR);
                #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                    *out << "Type Name: " << t->info->name << " Size: " << std::dec << t->size << " "<< "\n" << std::flush;
                #endif
                uint64_t number_of_offsets_blocks = t->size / TYCHE_OFFSETS_IN_EACH_CACHELINE;
                for (size_t off = 0; off < number_of_offsets_blocks + 1; off++)
                {
                    uint64_t level = 0;
                    assert((uint64_t)tm >= TYCHE_SECTION_0_START_ADDR && (uint64_t)tm < TYCHE_SECTION_0_END_ADDR);
                    #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                        *out << "Level: " << std::dec  << level << " " << std::hex << tm << "\n" << std::flush;
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
                    #endif


                    TYCHE_METADATA_CACHELINE* tm_next_level = tm->next_cacheline;
                    while (tm_next_level != NULL)
                    {
                        level++;
                        
                        assert((uint64_t)tm_next_level >= (TYCHE_SECTION_0_START_ADDR + level * 0x100000) && (uint64_t)tm_next_level < (TYCHE_SECTION_0_END_ADDR + level * 0x100000)); 
                        #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                            *out << "Level: " << std::dec  << level << " " << std::hex << tm_next_level << "\n" << std::flush;  
                            *out << std::hex << tm_next_level << "\n" << std::flush;
                            *out << 
                            "tm_0 = " << std::dec << tm_next_level->CacheLine_0 << " " <<
                            "tm_1 = " << std::dec << tm_next_level->CacheLine_1 << " " <<
                            "tm_2 = " << std::dec << tm_next_level->CacheLine_2 << " " <<
                            "tm_3 = " << std::dec << tm_next_level->CacheLine_3 << " " <<
                            "tm_4 = " << std::dec << tm_next_level->CacheLine_4 << " " <<
                            "tm_5 = " << std::dec << tm_next_level->CacheLine_5 << " " <<
                            "tm_6 = " << std::dec << tm_next_level->CacheLine_6 << " " <<
                            "tm_7 = " << std::dec << tm_next_level->CacheLine_7 << " " <<
                            "tm_8 = " << std::dec << tm_next_level->CacheLine_8 << " " <<
                            "tm_9 = " << std::dec << tm_next_level->CacheLine_9 << " " <<
                            "tm_10 = " << std::dec << tm_next_level->CacheLine_10 << " " <<
                            "tm_11 = " << std::dec << tm_next_level->CacheLine_11 << " " <<
                            "tm_12 = " << std::dec << tm_next_level->CacheLine_12 << " " <<
                            "tm_13 = " << std::dec << tm_next_level->CacheLine_13 << " " <<
                            "tm_p = "  << std::hex << tm_next_level->next_cacheline << " " <<
                            '\n' << std::flush;
                        #endif
                        tm_next_level = tm_next_level->next_cacheline;  
                              
                    }
                    
                    tm = (TYCHE_METADATA_CACHELINE*)((void*)tm + TYCHE_CACHELINE_SIZE);
                }
                

                // Find the offset for looking at the layout
                // Calculate and normalize the `offset'.
                tm = t->tyche_meta;
                //EFFECTIVE_BOUNDS bases = {(intptr_t)base, (intptr_t)base};
                //EFFECTIVE_BOUNDS sizes = {0, (long int)meta->size};
                //EFFECTIVE_BOUNDS bounds = bases + sizes;
                size_t offset = (uint8_t *)ptr - (uint8_t *)base;
                size_t offset_unadjusted = offset;
                #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                    *out << std::hex << "Type Name: " << t->info->name << " pc: " << pc <<  " ptr: " << ptr << " base: " << base << std::dec << " offset = " << offset << ", t->size = " << t->size << ", meta->size = " << meta->size
                        << '\n' << std::flush;
                #endif
                if (offset >= t->size) {
                    // The `offset' is >= sizeof(T).  Thus `ptr' may be
                    // pointing to an element in an array of T.
                    // Alternatively, `ptr' may be pointing to a FAM at the
                    // end of T.  Either way, the offset is normalized here.
                    // EFFECTIVE_BOUNDS adjust = {t->offset_fam, 0};
                    offset -= t->size;
                    unsigned __int128 tmp = (unsigned __int128)offset;
                    tmp *= (unsigned __int128)t->magic;
                    idx = (size_t)(tmp >> EFFECTIVE_RADIX);
                    offset -= idx * t->size_fam;
                    //bounds += adjust;
                    offset += t->offset_fam;
                    #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                        *out << "FAM or Array. Offset is adjusted. Offset = "
                             << std::dec << offset << ", t->size = " << t->size << ", t->size_fam = " << t->size_fam << ", t->offset_fam = " << t->offset_fam  << '\n' << std::flush;
                    #endif
                }

                if (t->size_fam == t->size)    
                {
                    if (offset > t->size)
                    {
                        *out << std::hex << "Type Name: " << t->info->name << " pc: " << pc <<  " ptr: " << ptr << " base: " << base << std::dec << " offset = " << offset << ", t->size = " << t->size << ", meta->size = " << meta->size
                            << '\n' << std::flush;
                        *out << "FAM or Array. Offset is adjusted. Offset = "
                            << std::dec << offset << ", t->size = " << t->size << ", t->size_fam = " << t->size_fam << ", t->offset_fam = " << t->offset_fam  << '\n' << std::flush;
                    }
                    assert(offset <= t->size);
                }
                else                           
                {
                    if (offset > (t->size_fam + t->size ))
                    {
                        *out << std::hex << "Type Name: " << t->info->name << " pc: " << pc <<  " ptr: " << ptr << " base: " << base << std::dec << " offset = " << offset << ", t->size = " << t->size << ", meta->size = " << meta->size
                            << '\n' << std::flush;
                        *out << "FAM or Array. Offset is adjusted. Offset = "
                            << std::dec << offset << ", t->size = " << t->size << ", t->size_fam = " << t->size_fam << ", t->offset_fam = " << t->offset_fam  << '\n' << std::flush;
                    }
                    assert(offset <= (t->size_fam + t->size ));
                }

                if (offset_unadjusted > meta->size) {
                    *out << "out of bound error";
                    assert(0);
                }

                // Access Meta Cache
                // find the meta cache line that we should fetch
                uint64_t block = offset / TYCHE_OFFSETS_IN_EACH_CACHELINE;
                TYCHE_METADATA_CACHELINE* cl = (TYCHE_METADATA_CACHELINE*)((void*)tm + block * TYCHE_CACHELINE_SIZE);
                #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                    *out << "Meta Cache Access: Offset = " << std::dec << offset << " Block: " << block << " Level: 0 " << " Addr: " << std::hex << cl  << '\n' << std::flush;
                #endif
                Meta_Cache.AccessSingleLine((ADDRINT)cl, CACHE_BASE::ACCESS_TYPE_LOAD); 
                uint64_t level = 0;
                TYCHE_METADATA_CACHELINE* cl_next_level = cl->next_cacheline;
                while (cl_next_level != NULL)
                {
                    level++;            
                    assert((uint64_t)cl_next_level >= (TYCHE_SECTION_0_START_ADDR + level * 0x100000) && (uint64_t)cl_next_level < (TYCHE_SECTION_0_END_ADDR + level * 0x100000)); 
                    #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                        *out << "Meta Cache Access: Offset = " << std::dec << offset << " Block: " << block << " Level: " << level << " Addr: " << std::hex << cl_next_level  << '\n' << std::flush;
                    #endif
                    Meta_Cache.AccessSingleLine((ADDRINT)cl_next_level, CACHE_BASE::ACCESS_TYPE_LOAD); 
                    cl_next_level = cl_next_level->next_cacheline;    
                }


                // Access Type Predictor
                assert(TypeIDs.find(std::string(t->info->name)) != TypeIDs.end());
                PointerID tid = ParentTypePredictor->lookup((uint64_t)pc);
                bool prediction = false;
                prediction = (TypeIDs[std::string(t->info->name)] == tid.getPID());
                ParentTypePredictor->update((uint64_t)pc, PointerID(TypeIDs[std::string(t->info->name)]), prediction);
                ParentTypePredictor->LVPTNumOfAccesses++;
                if (!prediction) ParentTypePredictor->LVPTMissprediction++;
                #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                    *out << std::hex << "Meta Cache Access => PC: " << (uint64_t)pc << " Pred.: " << prediction <<  " Actual TID: " << TypeIDs[std::string(t->info->name)]  << " Pred. TID: " << tid.getPID() << "\n" << std::flush;
                #endif

                const EFFECTIVE_INFO* info = t->info;
                dumpEffectiveTypeInfo(info);
                *out <<  "\n----------------------------------------------------\n";
            }

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

                // Update the Type ID
                if (TypeIDs.find(std::string(t->info->name)) == TypeIDs.end())
                    TypeIDs[std::string(t->info->name)] = TID++;
                // How many different types were accesses during this epoch
                ITC[std::string(t->info->name)]++;

                // Verify the type information layout
                TYCHE_METADATA_CACHELINE* tm = t->tyche_meta;
                assert((uint64_t)tm >= TYCHE_SECTION_0_START_ADDR && (uint64_t)tm < TYCHE_SECTION_0_END_ADDR);
                #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                    *out << "Type Name: " << t->info->name << " Size: " << std::dec << t->size << " "<< "\n" << std::flush;
                #endif
                uint64_t number_of_offsets_blocks = t->size / TYCHE_OFFSETS_IN_EACH_CACHELINE;
                for (size_t off = 0; off < number_of_offsets_blocks + 1; off++)
                {
                    uint64_t level = 0;
                    assert((uint64_t)tm >= TYCHE_SECTION_0_START_ADDR && (uint64_t)tm < TYCHE_SECTION_0_END_ADDR);
                    #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                        *out << "Level: " << std::dec  << level << " " << std::hex << tm << "\n" << std::flush;
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
                    #endif

                    TYCHE_METADATA_CACHELINE* tm_next_level = tm->next_cacheline;
                    
                    while (tm_next_level != NULL)
                    {
                        level++;
                        
                        assert((uint64_t)tm_next_level >= (TYCHE_SECTION_0_START_ADDR + level * 0x100000) && (uint64_t)tm_next_level < (TYCHE_SECTION_0_END_ADDR + level * 0x100000)); 
                        #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                            *out << "Level: " << std::dec  << level << " " << std::hex << tm_next_level << "\n" << std::flush;
                            *out << 
                            "tm_0 = " << std::dec << tm_next_level->CacheLine_0 << " " <<
                            "tm_1 = " << std::dec << tm_next_level->CacheLine_1 << " " <<
                            "tm_2 = " << std::dec << tm_next_level->CacheLine_2 << " " <<
                            "tm_3 = " << std::dec << tm_next_level->CacheLine_3 << " " <<
                            "tm_4 = " << std::dec << tm_next_level->CacheLine_4 << " " <<
                            "tm_5 = " << std::dec << tm_next_level->CacheLine_5 << " " <<
                            "tm_6 = " << std::dec << tm_next_level->CacheLine_6 << " " <<
                            "tm_7 = " << std::dec << tm_next_level->CacheLine_7 << " " <<
                            "tm_8 = " << std::dec << tm_next_level->CacheLine_8 << " " <<
                            "tm_9 = " << std::dec << tm_next_level->CacheLine_9 << " " <<
                            "tm_10 = " << std::dec << tm_next_level->CacheLine_10 << " " <<
                            "tm_11 = " << std::dec << tm_next_level->CacheLine_11 << " " <<
                            "tm_12 = " << std::dec << tm_next_level->CacheLine_12 << " " <<
                            "tm_13 = " << std::dec << tm_next_level->CacheLine_13 << " " <<
                            "tm_p = "  << std::hex << tm_next_level->next_cacheline << " " <<
                            '\n' << std::flush;   
                        #endif
                        tm_next_level = tm_next_level->next_cacheline;  
                    }

                    tm = (TYCHE_METADATA_CACHELINE*)((void*)tm + TYCHE_CACHELINE_SIZE);
                }

                // Find the offset for looking at the layout
                // Calculate and normalize the `offset'.
                tm = t->tyche_meta;

                //EFFECTIVE_BOUNDS bases = {(intptr_t)base, (intptr_t)base};
                //EFFECTIVE_BOUNDS sizes = {0, (long int)meta->size};
                //EFFECTIVE_BOUNDS bounds = bases + sizes;
                size_t offset = (uint8_t *)ptr - (uint8_t *)base;
                size_t offset_unadjusted = offset;
                #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                    *out << std::hex << "Type Name: " << t->info->name << " pc: " << pc <<  " ptr: " << ptr << " base: " << base << std::dec << " offset = " << offset << ", t->size = " << t->size << ", meta->size = " << meta->size
                        << '\n' << std::flush;
                #endif
                if (offset >= t->size) {
                    // The `offset' is >= sizeof(T).  Thus `ptr' may be
                    // pointing to an element in an array of T.
                    // Alternatively, `ptr' may be pointing to a FAM at the
                    // end of T.  Either way, the offset is normalized here.
                    // EFFECTIVE_BOUNDS adjust = {t->offset_fam, 0};
                    offset -= t->size;
                    unsigned __int128 tmp = (unsigned __int128)offset;
                    tmp *= (unsigned __int128)t->magic;
                    idx = (size_t)(tmp >> EFFECTIVE_RADIX);
                    offset -= idx * t->size_fam;
                    //bounds += adjust;
                    offset += t->offset_fam;
                    #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                        *out << "FAM or Array. Offset is adjusted. Offset = "
                             << std::dec << offset << ", t->size = " << t->size << ", t->size_fam = " << t->size_fam << ", t->offset_fam = " << t->offset_fam  << '\n' << std::flush;
                    #endif
                }

                if (t->size_fam == t->size)    
                {
                    if (offset > t->size)
                    {
                        *out << std::hex << "Type Name: " << t->info->name << " pc: " << pc <<  " ptr: " << ptr << " base: " << base << std::dec << " offset = " << offset << ", t->size = " << t->size << ", meta->size = " << meta->size
                            << '\n' << std::flush;
                        *out << "FAM or Array. Offset is adjusted. Offset = "
                            << std::dec << offset << ", t->size = " << t->size << ", t->size_fam = " << t->size_fam << ", t->offset_fam = " << t->offset_fam  << '\n' << std::flush;
                    }
                    assert(offset <= t->size);
                }
                else                           
                {
                    if (offset > (t->size_fam + t->size ))
                    {
                        *out << std::hex << "Type Name: " << t->info->name << " pc: " << pc <<  " ptr: " << ptr << " base: " << base << std::dec << " offset = " << offset << ", t->size = " << t->size << ", meta->size = " << meta->size
                            << '\n' << std::flush;
                        *out << "FAM or Array. Offset is adjusted. Offset = "
                            << std::dec << offset << ", t->size = " << t->size << ", t->size_fam = " << t->size_fam << ", t->offset_fam = " << t->offset_fam  << '\n' << std::flush;
                    }
                    assert(offset <= (t->size_fam + t->size ));
                }

                if (offset_unadjusted > meta->size) {
                    *out << "out of bound error";
                    assert(0);
                }

                // Access Meta Cache
                // find the meta cache line that we should fetch
                uint64_t block = offset / TYCHE_OFFSETS_IN_EACH_CACHELINE;
                TYCHE_METADATA_CACHELINE* cl = (TYCHE_METADATA_CACHELINE*)((void*)tm + block * TYCHE_CACHELINE_SIZE);
                #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                    *out << "Meta Cache Access: Offset = " << std::dec << offset << " Block: " << block << " Level: 0 " << " Addr: " << std::hex << cl  << '\n' << std::flush;
                #endif
                Meta_Cache.AccessSingleLine((ADDRINT)cl, CACHE_BASE::ACCESS_TYPE_LOAD); 
                uint64_t level = 0;
                TYCHE_METADATA_CACHELINE* cl_next_level = cl->next_cacheline;
                while (cl_next_level != NULL)
                {
                    level++;            
                    assert((uint64_t)cl_next_level >= (TYCHE_SECTION_0_START_ADDR + level * 0x100000) && (uint64_t)cl_next_level < (TYCHE_SECTION_0_END_ADDR + level * 0x100000)); 
                    #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                        *out << "Meta Cache Access: Offset = " << std::dec << offset << " Block: " << block << " Level: " << level << " Addr: " << std::hex << cl_next_level  << '\n' << std::flush;
                    #endif
                    Meta_Cache.AccessSingleLine((ADDRINT)cl_next_level, CACHE_BASE::ACCESS_TYPE_LOAD); 
                    cl_next_level = cl_next_level->next_cacheline;    
                }

                // Access Type Predictor
                assert(TypeIDs.find(std::string(t->info->name)) != TypeIDs.end());
                PointerID tid = ParentTypePredictor->lookup((uint64_t)pc);
                bool prediction = false;
                prediction = (TypeIDs[std::string(t->info->name)] == tid.getPID());
                ParentTypePredictor->update((uint64_t)pc, PointerID(TypeIDs[std::string(t->info->name)]) , prediction);
                ParentTypePredictor->LVPTNumOfAccesses++;
                if (!prediction) ParentTypePredictor->LVPTMissprediction++;
                #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                    *out << std::hex << "Meta Cache Access => PC: " << (uint64_t)pc << " Pred.: " << prediction <<  " Actual TID: " << TypeIDs[std::string(t->info->name)]  << " Pred. TID: " << tid.getPID() << "\n" << std::flush;
                #endif

                const EFFECTIVE_INFO* info = t->info;
                dumpEffectiveTypeInfo(info);
                *out <<  "\n----------------------------------------------------\n";
                
            }

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
        (rtn_name.find("LOWFAT_") != std::string::npos) || 
        (rtn_name.find("malloc") != std::string::npos) || 
        (rtn_name.find("_Znwm") != std::string::npos) || 
        (rtn_name.find("_Znam") != std::string::npos) || 
        (rtn_name.find("_ZnwmRKSt9nothrow_t") != std::string::npos) || 
        (rtn_name.find("_ZnamRKSt9nothrow_t") != std::string::npos) ||
        (rtn_name.find("free") != std::string::npos) ||
        (rtn_name.find("_ZdlPv") != std::string::npos) ||
        (rtn_name.find("_ZdaPv") != std::string::npos) ||
        (rtn_name.find("calloc") != std::string::npos) || 
        (rtn_name.find("realloc") != std::string::npos)) {
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
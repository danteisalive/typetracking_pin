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



typedef std::map<uint64_t, std::pair<std::string, uint64_t> > TypesCount;
typedef std::map<uint64_t, std::map<uint64_t, std::pair<std::string, uint64_t> > > TypesLayout;
typedef TypesCount TypesCount;
typedef TypesLayout TypesLayout;

typedef std::map<std::string, uint64_t> InsTypeCount;
typedef std::map<uint64_t, uint64_t> TypeTreeUsage;
typedef InsTypeCount InsTypeCount;

extern FILE* effectiveDumpFile;
extern ofstream *out;
extern TypesCount TC;
extern InsTypeCount ITC;
extern InsTypeCount ParrentTypeIDs;
extern TypesLayout TyCheTypeLayout;
//extern TypeTreeUsage TyCHETypeTreeUsage;
extern TypesLayout AllocationsTypes;
extern UINT64 ParentTID;

extern UINT64 NumOfCalls;

extern InsTypeCount TypesUsedInEpoch;
extern DefaultLVPT *ParentTypePredictor;
extern DefaultBasicTypePredictor *BasicTypePredictor;


// The running count of instructions is kept here
// make it static to help the compiler optimize docount
extern UINT64 icount;
extern size_t total_number_of_allocations;
extern size_t total_number_of_freed_allocations;

extern double overallLayoutUsage;
extern std::vector<uint64_t> overalSpatialCorrelation;
extern uint64_t overallLiveAllocations;


void VerifyTypeLayout(const EFFECTIVE_TYPE* t);
void UpdateStatics(const EFFECTIVE_META * meta);
size_t FindOffset(const EFFECTIVE_TYPE *t, const void* ptr, ADDRINT pc);
void AccessMetaCache(const EFFECTIVE_TYPE* t, const size_t offset);
void AccessTypePredictors(const EFFECTIVE_TYPE *t, size_t offset, ADDRINT pc);

void InsertIntoAllocationsTypes(const EFFECTIVE_META * meta, uint64_t offset)
{

    const EFFECTIVE_TYPE *t = meta->type;
    uint64_t PID = meta->PID;
    uint64_t TID = (uint64_t)t->tyche_meta;
    // we already have an entry for this allocation PID
    if (AllocationsTypes.find(PID) == AllocationsTypes.end())
    {
        // use TID to find the layout of the allocation
        assert(TyCheTypeLayout.find(TID) != TyCheTypeLayout.end());

        if (TyCheTypeLayout[TID].size() < 3) return;

        AllocationsTypes[PID] = TyCheTypeLayout[TID];
    }

    if (AllocationsTypes[PID].find(offset) != AllocationsTypes[PID].end())
    {
        AllocationsTypes[PID][offset].second++;
    }
}


double SpatialCorrelation(std::vector<double>& vals)
{
    assert(vals.size() >= 3);
    double mean;
    double totalValues = 0;
    for (size_t i = 0; i < vals.size(); i++)
    {
        totalValues += vals[i];
    }
    mean = totalValues/(double)vals.size();

    double sigma = (vals[0] - mean) * (vals[0] - mean);
    double nominator = 1.0 * (vals[0] - mean) * (vals[1] - mean);
    for (size_t i = 1; i < vals.size()-1; i++)
    {
        sigma       += (vals[i] - mean) * (vals[i] - mean); 
        nominator   += 0.5 * (vals[i] - mean) * (vals[i-1] - mean);
        nominator   += 0.5 * (vals[i] - mean) * (vals[i+1] - mean);
    }
    sigma += (vals[vals.size()-1] - mean) * (vals[vals.size()-1] - mean);
    nominator += 1.0 * (vals[vals.size()-1] - mean) * (vals[vals.size()-2] - mean);

    double MoransIndex = nominator/sigma;
    return MoransIndex;
    
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

        std::map<std::string, uint64_t> AliveAllocations;
        for (TypesCount::iterator it = TC.begin(); it != TC.end(); it++) 
        {
            if (AliveAllocations.find(it->second.first) == AliveAllocations.end())
            {
                AliveAllocations[it->second.first] = 1;
            }            
            else 
            {
                AliveAllocations[it->second.first]++;
            }
        }
        // *out << std::dec << "AliveAllocations: " << TC.size() << std::endl << std::flush;
        // for (std::map<std::string, uint64_t>::iterator it = AliveAllocations.begin(); it != AliveAllocations.end(); it++)
        // {
        //     *out << it->first << " " << it->second << std::endl << std::flush;
        // }

        std::map<uint64_t, std::string> pairs;
        double totalAllocs = 0.0;
        for (std::map<std::string, uint64_t>::iterator itr = AliveAllocations.begin(); itr != AliveAllocations.end(); ++itr)
        {
            //*out << itr->first << " " << itr->second << std::endl;
            pairs[itr->second] = itr->first;
            totalAllocs += (double)(itr->second);
        }
        
        uint64_t uniqueTypes95Percent = 0;
        double totalAllocs95Percent = 0.0;
        for (std::map<uint64_t, std::string>::reverse_iterator itr = pairs.rbegin(); itr != pairs.rend(); ++itr)
        {
            totalAllocs95Percent += (double)(itr->first);
            if ((double)(itr->first)/totalAllocs >= 0.01) uniqueTypes95Percent++;
        }        




        NumOfCalls = 0;
        

        // find the percentage of type tree
        // double totalTypeTreeUsage = 0.0;
        // for (TypeTreeUsage::iterator it = TyCHETypeTreeUsage.begin(); it != TyCHETypeTreeUsage.end(); it++)
        // {
        //     // assert(TyCheTypeLayout.find(it->first) != TyCheTypeLayout.end());
        //     // TypesLayout::iterator typeTreeIterator = TyCheTypeLayout.find(it->first);
        //     // assert(typeTreeIterator != TyCheTypeLayout.end());
        //     // int subTreeUsed = 0;
        //     for (std::map<uint64_t,std::pair<std::string, uint64_t> >::iterator offsetsIterator = typeTreeIterator->second.begin(); offsetsIterator != typeTreeIterator->second.end(); offsetsIterator++)
        //     {
        //         //*out << "Offset: " << offsetsIterator->first << " Accsses: " << std::dec << offsetsIterator->second.second << "\n";
        //         // if (offsetsIterator->second.second > 0) subTreeUsed++;
        //         offsetsIterator->second.second = 0;
        //     }

        //     // assert(typeTreeIterator->second.size() != 0);
        //     // double percentage = (double)subTreeUsed/(double)typeTreeIterator->second.size();
        //     // totalTypeTreeUsage += percentage;

        // }

        double totalTypeTreeUsage = 0.0;
        uint64_t totalLiveAllocations = 0;
        std::vector<double> dist;
        std::vector<int> spatialCorrelationsInEpochs(3,0);
        for (TypesLayout::iterator it = AllocationsTypes.begin(); it != AllocationsTypes.end(); it++)
        {
            totalLiveAllocations++;
            int subTreeUsed = 0;
            for (std::map<uint64_t,std::pair<std::string, uint64_t> >::iterator offsetsIterator = it->second.begin(); offsetsIterator != it->second.end(); offsetsIterator++)
            {
                if (offsetsIterator->second.second > 0) subTreeUsed++;
                dist.push_back(offsetsIterator->second.second);
            }

            assert(dist.size() >= 3);

            double MoransIndex  = SpatialCorrelation(dist);
            if (MoransIndex > 0.2)                                  spatialCorrelationsInEpochs[0]++;
            else if (MoransIndex >= -0.2 && MoransIndex <= 0.2)     spatialCorrelationsInEpochs[1]++;
            else                                                    spatialCorrelationsInEpochs[2]++;
            //*out << "Layout Size: " << dist.size() << " MoransIndex: " << std::dec << MoransIndex << "\n";
            assert(it->second.size()> 0);
            double percentage = (double)subTreeUsed/(double)it->second.size();
            totalTypeTreeUsage += percentage;
            dist.clear();

        }

        overallLayoutUsage      += totalTypeTreeUsage;
        overallLiveAllocations  += totalLiveAllocations;
        overalSpatialCorrelation[0] += spatialCorrelationsInEpochs[0];
        overalSpatialCorrelation[1] += spatialCorrelationsInEpochs[1];
        overalSpatialCorrelation[2] += spatialCorrelationsInEpochs[2];
        
        double layoutUsage = 0.0;
        std::vector<double> spatialCorrelationsPercentageInEpochs(3,0.0);
        if (totalLiveAllocations != 0)
        {
            layoutUsage = totalTypeTreeUsage * 100.0 / (double)totalLiveAllocations;
            spatialCorrelationsPercentageInEpochs[0] = (double)spatialCorrelationsInEpochs[0] * 100.0 / (double)totalLiveAllocations;
            spatialCorrelationsPercentageInEpochs[1] = (double)spatialCorrelationsInEpochs[1] * 100.0 / (double)totalLiveAllocations;
            spatialCorrelationsPercentageInEpochs[2] = (double)spatialCorrelationsInEpochs[2] * 100.0 / (double)totalLiveAllocations;
        }

        // dist.push_back(100);
        // dist.push_back(0);
        // dist.push_back(100);
        // dist.push_back(0);
        // dist.push_back(100);
        // dist.push_back(0);
        // dist.push_back(100);
        // dist.push_back(0);
        // dist.push_back(100);
        // dist.push_back(0);
        // dist.push_back(100);
        // if (dist.size() >= 3){
        //     double MoransIndex  = SpatialCorrelation(dist);
        //     *out << "Layout Size: " << dist.size() << " MoransIndex: " << std::dec << MoransIndex << "\n";
        // }
        // dist.clear();

        // dist.push_back(100);
        // dist.push_back(100);
        // dist.push_back(100);
        // dist.push_back(100);
        // dist.push_back(100);
        // dist.push_back(100);
        // dist.push_back(100);
        // dist.push_back(100);
        // dist.push_back(0);
        // dist.push_back(0);
        // dist.push_back(0);
        // dist.push_back(0);
        // dist.push_back(0);
        // if (dist.size() >= 3){
        //     double MoransIndex  = SpatialCorrelation(dist);
        //     *out << "Layout Size: " << dist.size() << " MoransIndex: " << std::dec << MoransIndex << "\n";
        // }
        // dist.clear();

        // dist.push_back(100);
        // dist.push_back(0);
        // dist.push_back(0);
        // dist.push_back(40);
        // dist.push_back(100);
        // dist.push_back(10);
        // dist.push_back(0);
        // dist.push_back(0);
        // dist.push_back(10);
        // dist.push_back(0);
        // dist.push_back(100);
        // dist.push_back(100);
        // dist.push_back(0);
        // if (dist.size() >= 3){
        //     double MoransIndex  = SpatialCorrelation(dist);
        //     *out << "Layout Size: " << dist.size() << " MoransIndex: " << std::dec << MoransIndex << "\n";
        // }
        // for (TypesLayout::iterator it = TyCheTypeLayout.begin(); it != TyCheTypeLayout.end(); it++)
        // {
        //     for (std::map<uint64_t,std::pair<std::string, uint64_t> >::iterator offsetsIterator = it->second.begin(); offsetsIterator != it->second.end(); offsetsIterator++)
        //     {
        //         offsetsIterator->second.second = 0;
        //     }

        // }

        *out << std::dec
            << (double)ParentTypePredictor->LVPTMissprediction * 100.0 / ParentTypePredictor->LVPTNumOfAccesses << " "
            << BasicTypePredictor->getMissRate() * 100.0 << " "
            << TypesUsedInEpoch.size() << " "
            << (double)Meta_Cache.Misses() * 100.0 /(double)Meta_Cache.Accesses()  << " "
            << TC.size() << " "
           // << totalAllocs95Percent/totalAllocs << " "
            << uniqueTypes95Percent << " "
            << total_number_of_allocations << " "
            << total_number_of_freed_allocations << " "
            << totalLiveAllocations << " "
            << layoutUsage << " "
            << spatialCorrelationsPercentageInEpochs[0] << " "
            << spatialCorrelationsPercentageInEpochs[1] << " "
            << spatialCorrelationsPercentageInEpochs[2] << " "
            << spatialCorrelationsInEpochs[0] << " "
            << spatialCorrelationsInEpochs[1] << " "
            << spatialCorrelationsInEpochs[2] << " "
            << '\n'
            << std::flush;

        //*out << Meta_Cache << std::flush;

        // for (InsTypeCount::iterator it = ParrentTypeIDs.begin(); it != ParrentTypeIDs.end(); it++) {
        //     if (it->second != 0) {
        //         *out << std::dec << it->first << "(" << it->second << ")\n";
        //         it->second = 0;
        //     }
        // }
        // *out << '\n' << std::flush;
        // *out << "------------------------------------------------------------------------\n"  << std::flush;

        TC.clear();
        TypesUsedInEpoch.clear();
        // TyCHETypeTreeUsage.clear();
        AllocationsTypes.clear();

        

    }
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

                UpdateStatics(meta);

                // Verify the type information layout
                VerifyTypeLayout(t);
            
                // Find the offset for looking at the layout
                // Calculate and normalize the `offset'.
                size_t offset = FindOffset(t, ptr, pc);

                // Access Meta Cache
                AccessMetaCache(t, offset);


                // Access Parent and Basic Type Predictors
                AccessTypePredictors(t, offset, pc);

                InsertIntoAllocationsTypes(meta, offset);

            }

        }

    } 

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
                
                UpdateStatics(meta);

                // Verify the type information layout
                VerifyTypeLayout(t);

                // Find the offset for looking at the layout
                // Calculate and normalize the `offset'.
                size_t offset = FindOffset(t, ptr, pc);

                // Access Meta Cache
                AccessMetaCache(t, offset);

                // Access Parent and Basic Type Predictors
                AccessTypePredictors(t, offset, pc);

                InsertIntoAllocationsTypes(meta, offset);
                
            }

        }

    } 

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
        (rtn_name.find("realloc") != std::string::npos) ||
        (rtn_name.find("__libc_realloc") != std::string::npos) ||
        (rtn_name.find("__libc_malloc") != std::string::npos) ||
        (rtn_name.find("__libc_memalign") != std::string::npos) ||
        (rtn_name.find("__​libc_calloc") != std::string::npos) ||
        (rtn_name.find("__libc_free") != std::string::npos)) {
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

void VerifyTypeLayout(const EFFECTIVE_TYPE* t)
{
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
}


size_t FindOffset(const EFFECTIVE_TYPE *t, const void* ptr, ADDRINT pc)
{
    size_t idx = lowfat_index(ptr);
    void *base = lowfat_base(ptr);
    EFFECTIVE_META *meta = (EFFECTIVE_META *)base;
    base = (void *)(meta + 1);
    //TYCHE_METADATA_CACHELINE*  tm = t->tyche_meta;
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

    return offset;
}

void AccessMetaCache(const EFFECTIVE_TYPE *t, const size_t offset)
{
    TYCHE_METADATA_CACHELINE*  tm = t->tyche_meta;
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

}

void AccessTypePredictors(const EFFECTIVE_TYPE *t, size_t offset, ADDRINT pc)
{
    assert(ParrentTypeIDs.find(std::string(t->info->name)) != ParrentTypeIDs.end());
    PointerID tid = ParentTypePredictor->lookup((uint64_t)pc);
    bool prediction = false;
    prediction = (ParrentTypeIDs[std::string(t->info->name)] == tid.getPID());
    ParentTypePredictor->update((uint64_t)pc, PointerID(ParrentTypeIDs[std::string(t->info->name)]), prediction);
    ParentTypePredictor->LVPTNumOfAccesses++;
    if (!prediction) ParentTypePredictor->LVPTMissprediction++;
    #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
        *out << std::hex << "Meta Cache Access => PC: " << (uint64_t)pc << " Pred.: " << prediction <<  " Actual ParentTID: " << ParrentTypeIDs[std::string(t->info->name)]  << " Pred. ParentTID: " << tid.getPID() << "\n" << std::flush;
    #endif


    // add it if it's not added already
    if (TyCheTypeLayout.find((uint64_t)t->tyche_meta) == TyCheTypeLayout.end())
    {    
       #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
            *out << std::dec << "Parent Name: " << t->info->name << " Length: " << t->length << std::endl << std::flush;
        #endif
        for (size_t type_entry_id = 0; type_entry_id < t->length-1 ; type_entry_id++)
        {
            assert(t->layout[type_entry_id].name != NULL);
            TyCheTypeLayout[(uint64_t)t->tyche_meta][t->layout[type_entry_id].offset] = std::make_pair(std::string(t->layout[type_entry_id].name), 0);
            #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
                *out << std::dec << "Offset: " <<  t->layout[type_entry_id].offset << " Name: " <<  t->layout[type_entry_id].name << " " <<  std::flush;
            #endif
        }
        assert(t->layout[t->length-1].offset == UINT64_MAX);
        #ifdef ENABLE_TYCHE_LAYOUT_DEBUG
            *out <<  "\n----------------------------------------------------\n";
       #endif
    }

    //Basic Type Predictor 
    TypesLayout::iterator typeTreeIterator = TyCheTypeLayout.find((uint64_t)t->tyche_meta);
    assert(typeTreeIterator != TyCheTypeLayout.end());
    std::map<uint64_t,std::pair<std::string, uint64_t> >::iterator offsetsIterator = typeTreeIterator->second.find(offset);
    //assert(offsetsIterator != typeTreeIterator->second.end());
    if (offsetsIterator != typeTreeIterator->second.end())
    {
        // TyCHETypeTreeUsage[(uint64_t)t->tyche_meta] = 0;
        // offsetsIterator->second.second++;
        BasicTypePredictor->lookup((uint64_t)pc, offsetsIterator->second.first);
    }
}

void UpdateStatics(const EFFECTIVE_META * meta)
{   
    const EFFECTIVE_TYPE *t = meta->type;

    total_number_of_allocations = *((size_t*)meta->ALIVE_ALLOCATION);
    total_number_of_freed_allocations = *((size_t*)meta->FREED_ALLOCATIONS);
    // Count number of types used during this epoch
    if (TypesUsedInEpoch.find(std::string(t->info->name)) == TypesUsedInEpoch.end())
    {
        TypesUsedInEpoch[std::string(t->info->name)] = 1;
    }
    else 
    {
        TypesUsedInEpoch[std::string(t->info->name)] += 1;
    }

    // Update the Type ID
    if (ParrentTypeIDs.find(std::string(t->info->name)) == ParrentTypeIDs.end())
    {
        ParrentTypeIDs[std::string(t->info->name)] = ParentTID++;
    }
    // How many different types were accesses during this epoch
    ITC[std::string(t->info->name)]++;

    if (TC.find(meta->PID) == TC.end())
    {
        TC[meta->PID] = std::make_pair(std::string(t->info->name), 1);
    }
    else 
    {
        TC[meta->PID].second++;
    }
}



#endif
#ifndef __PREDICTOR_HH__
#define __PREDICTOR_HH__

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

#include <algorithm>
#include <numeric>

using std::stringstream;

/*
 * Copyright (c) 2004-2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Kevin Lim
 */







typedef uint64_t ThreadID ;


class SatCounter
{
  public:
    /**
     * Constructor for the counter.
     */
    SatCounter()
        : initialVal(0), counter(0)
    { }

    /**
     * Constructor for the counter.
     * @param bits How many bits the counter will have.
     */
    SatCounter(unsigned bits)
        : initialVal(0), maxVal((1 << bits) - 1), counter(0)
    { }

    /**
     * Constructor for the counter.
     * @param bits How many bits the counter will have.
     * @param initial_val Starting value for each counter.
     */
    SatCounter(unsigned bits, uint8_t initial_val)
        : initialVal(initial_val), maxVal((1 << bits) - 1),
          counter(initial_val)
    {
        // Check to make sure initial value doesn't exceed the max
        // counter value.
        if (initial_val > maxVal) {
            //fatal("BP: Initial counter value exceeds max size.");
            assert(0);
        }
    }

    /**
     * Sets the number of bits.
     */
    void setBits(unsigned bits) { maxVal = (1 << bits) - 1; }
    void setInitial(uint8_t _inital_val) { initialVal = _inital_val; }
    void reset() { counter = initialVal; }

    /**
     * Increments the counter's current value.
     */
    void increment()
    {
        if (counter < maxVal) {
            ++counter;
        }
    }

    /**
     * Decrements the counter's current value.
     */
    void decrement()
    {
        if (counter > 0) {
            --counter;
        }
    }

    /**
     * Read the counter's value.
     */
    uint8_t read() const
    { return counter; }

    void write(uint8_t cnt){
      counter = cnt;
    }

  private:
    uint8_t initialVal;
    uint8_t maxVal;
    uint8_t counter;
};


class PointerID
  {
  private:
      uint64_t id;

  public:

      PointerID(){
          id = 0;
      }

      PointerID(uint64_t _id){
          id = _id;
      }


      ~PointerID()
      {

      }

      bool operator != (const PointerID& _pid){
          return (id != _pid.id);
      }

      bool operator == (const PointerID& _pid){
          return (id == _pid.id);
      }


          // A better implementation of operator=
      PointerID& operator = (const PointerID& _pid)
          {
              // self-assignment guard
              if (this == &_pid)
                  return *this;

              // do the copy
              this->id = _pid.id;
              // return the existing object so we can chain this operator
              return *this;
      }

      bool operator < (const PointerID& rhs) const {

          return (this->id < rhs.id);
      }

      PointerID operator + (uint64_t _val)
      {
          this->id += _val;
          PointerID temp(*this);
          //temp.id += _val;

          return temp;

      }

      PointerID& operator += (const uint64_t& _rhs)
      {
          this->id += _rhs;
          return *this;
      }

      PointerID(const PointerID& _PID)
      {
          this->id = _PID.id;
      }

      void        setPID(uint64_t _id) { id = _id; }
      uint64_t    getPID() const { return id; }


  };




class DefaultLVPT
{
  private:
    struct LVPTEntry
    {
        LVPTEntry()
        {
          this->tag = 0;
          this->target = PointerID(0);
          this->valid = false;
        }

        /** The entry's tag. */
        uint64_t tag;

        /** The entry's target. */
        PointerID target;

        /** The entry's thread id. */
        ThreadID tid;

        /** Whether or not the entry is valid. */
        bool valid;
    };

  public:
    unsigned leastSigBit(unsigned n)
    {
        return n & ~(n - 1);
    }

    int floorLog2(unsigned x)
    {
            assert(x > 0);

            int y = 0;

            if (x & 0xffff0000) { y += 16; x >>= 16; }
            if (x & 0x0000ff00) { y +=  8; x >>=  8; }
            if (x & 0x000000f0) { y +=  4; x >>=  4; }
            if (x & 0x0000000c) { y +=  2; x >>=  2; }
            if (x & 0x00000002) { y +=  1; }

            return y;
    }

    bool isPowerOf2(unsigned n)
    {
        return n != 0 && leastSigBit(n) == n;
    }
    /** Creates a LVPT with the given number of entries, number of bits per
     *  tag, and instruction offset amount.
     *  @param numEntries Number of entries for the LVPT.
     *  @param tagBits Number of bits for each tag in the LVPT.
     *  @param instShiftAmt Offset amount for instructions to ignore alignment.
     */
    DefaultLVPT(unsigned numEntries, unsigned tagBits,
               unsigned instShiftAmt, unsigned numThreads)
    {
        std::cout << "creating LVPT " << numEntries << std::endl;
        this->numEntries = numEntries;
        this->tagBits = tagBits;
        this->instShiftAmt = instShiftAmt;
        this->log2NumThreads = floorLog2(numThreads);
        
        if (!isPowerOf2(numEntries)) {
            assert(0);
        }

        lvpt.resize(numEntries);

        for (unsigned i = 0; i < numEntries; ++i) {
            lvpt[i].valid = false;
        }

        this->idxMask = numEntries - 1;

        this->tagMask = (1 << tagBits) - 1;

        this->tagShiftAmt = instShiftAmt + floorLog2(numEntries); // 12 + 2

        //Setup the array of counters for the local predictor
        localBiases.resize(numEntries);

        for (unsigned i = 0; i < numEntries; ++i){
            localBiases[i] = 0;
        }

        localCtrs.resize(numEntries);
        for (size_t i = 0; i < numEntries; i++) {
            localCtrs[i].setBits(2);
            localCtrs[i].setInitial(0x3); // initial value is 0x11
            localCtrs[i].reset();
        }

        confLevel.resize(numEntries);
        for (size_t i = 0; i < numEntries; i++) {
            confLevel[i].setBits(4);
            confLevel[i].setInitial(0); // initial value is 0
            confLevel[i].reset();
        }

        localPointerPredictor.resize(numEntries);
        for (size_t i = 0; i < numEntries; i++) {
            localPointerPredictor[i].setBits(4);
            localPointerPredictor[i].setInitial(0); // initial value is 0
            localPointerPredictor[i].reset();
        }

     
        predictorMissCount.resize(numEntries);
        for (size_t i = 0; i < numEntries; i++) {
        predictorMissCount[i] = 0;
        }
        blackList.resize(numEntries);

        this->LVPTMissprediction = 0;
        this->LVPTNumOfAccesses = 0;
    }

    void reset()
    {
        for (unsigned i = 0; i < numEntries; ++i) {
            lvpt[i].valid = false;
        }
        
    }

    
    uint64_t getIndex(uint64_t instPC, ThreadID tid)
    {
        // Need to shift PC over by the word offset.
        return ((instPC >> instShiftAmt)
                ^ (tid << (tagShiftAmt - instShiftAmt - log2NumThreads)))
                & idxMask;
    }

    uint64_t getTag(uint64_t instPC)
    {
        return (instPC >> tagShiftAmt) & tagMask;
    }

    bool valid(uint64_t instPC, ThreadID tid)
    {
        uint64_t lvpt_idx = getIndex(instPC, tid);

        uint64_t inst_tag = getTag(instPC);

        assert(lvpt_idx < numEntries);

        if (lvpt[lvpt_idx].valid
            && inst_tag == lvpt[lvpt_idx].tag
            && lvpt[lvpt_idx].tid == tid) {
            return true;
        } else {
            return false;
        }
    }



    PointerID lookup( uint64_t instPC, ThreadID tid)
    {


        uint64_t lvpt_idx = getIndex(instPC, tid);

        // if the instPC is in the banList then just return zero;
        // if (banList[lvpt_idx].find(instPC) != banList[lvpt_idx].end()){
        //     return TheISA::PointerID(0);
        // }

        uint64_t inst_tag = getTag(instPC);

        assert(lvpt_idx < numEntries);

        if (lvpt[lvpt_idx].valid)
        {

            PointerID pred_pid = PointerID(0);
            if (inst_tag == lvpt[lvpt_idx].tag)
            {
                switch (localCtrs[lvpt_idx].read()) {
                    case 0x0:
                        //pred_pid = PointerID(lvpt[lvpt_idx].target.getPID());
                        pred_pid = PointerID(0);
                        //LVPTStridePredictions++;
                    
                        break;
                    
                    case 0x1:
                        //pred_pid = PointerID(lvpt[lvpt_idx].target.getPID());
                        pred_pid = PointerID(0);
                        //LVPTStridePredictions++;
                        
                        break;
                    case 0x2:
                    case 0x3:
                        //LVPTConstantPredictions++;
                        
                        pred_pid = lvpt[lvpt_idx].target;
                        break;

                    default:
                        assert(0);
                   
                }
            }
            // else {
            //     return TheISA::PointerID(0);
            // }
            // if (localPointerPredictor[lvpt_idx].read() > 0 &&
            //     pred_pid == TheISA::PointerID(0) &&
            //     confLevel[lvpt_idx].read() > 1)
            // {
            //     return TheISA::PointerID(0x1000000000000-1);
            // }

            // set the confidence level of this prediction

          
            return pred_pid;

        }
        else
        {
            
            //LVPTConstantPredictions++;
            return PointerID(0);
        }
    }


    void update(uint64_t instPC,
                const PointerID &target,
                ThreadID tid, bool predict
               )
    {
        uint64_t lvpt_idx = getIndex(instPC, tid);

        assert(lvpt_idx < numEntries);

        // if prediction is true, we just update the localCtrs
        // std::cout << std::hex << "Update localCtrs: " <<
        // localCtrs[lvpt_idx].read() << std::endl;
        switch (localCtrs[lvpt_idx].read())
        {
            case 0x0:
            case 0x1:
            case 0x2:
            case 0x3:
                if (predict){ localCtrs[lvpt_idx].increment(); }
                else        { localCtrs[lvpt_idx].decrement(); }
                break;

            default:
                assert(0);
        }



        // std::cout << std::hex << "Inst. " << instPC << " updated the LVPT." <<
        // " Before: " <<   lvpt[lvpt_idx].target << " After: " << target << "\n";

        localBiases[lvpt_idx] = target.getPID() - lvpt[lvpt_idx].target.getPID();
        lvpt[lvpt_idx].tid = tid;
        lvpt[lvpt_idx].valid = true;
        lvpt[lvpt_idx].target = target;
        lvpt[lvpt_idx].tag = getTag(instPC);
    }




    float getAverageConfidenceLevel() {

        float avg = 0.0;

        for (size_t i = 0; i < numEntries; i++) {
          avg += float(confLevel[i].read());
        }

        return avg/numEntries;
    }

    void dumpStat()
    {
    }


  private:



    /** The actual LVPT. */
    std::vector<LVPTEntry>  lvpt;
    std::vector<int>        localBiases;
    std::vector<SatCounter> localCtrs;
    std::vector<SatCounter> confLevel;
    std::vector<SatCounter> localPointerPredictor;

    // use predictor execpt for these addresses
    std::vector<std::map<uint64_t,uint64_t> > blackList;

    //for logs
    std::vector<uint64_t> predictorMissCount;



    /** The number of entries in the LVPT. */
    unsigned numEntries;

    /** The index mask. */
    unsigned idxMask;

    /** The number of tag bits per entry. */
    unsigned tagBits;

    /** The tag mask. */
    unsigned tagMask;

    /** Number of bits to shift PC when calculating index. */
    unsigned instShiftAmt;

    /** Number of bits to shift PC when calculating tag. */
    unsigned tagShiftAmt;

    /** Log2 NumThreads used for hashing threadid */
    unsigned log2NumThreads;

    public:
        uint64_t LVPTMissprediction;
        uint64_t LVPTNumOfAccesses;
};


#endif
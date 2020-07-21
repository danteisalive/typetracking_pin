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
    /** Creates a LVPT with the given number of entries, number of bits per
     *  tag, and instruction offset amount.
     *  @param numEntries Number of entries for the LVPT.
     *  @param tagBits Number of bits for each tag in the LVPT.
     *  @param instShiftAmt Offset amount for instructions to ignore alignment.
     */
    DefaultLVPT(unsigned numEntries, unsigned tagBits,
               unsigned instShiftAmt, unsigned numThreads);

    void reset();

/** Looks up an address in the LVPT. Must call valid() first on the address.
     *  @param inst_PC The address of the branch to look up.
     *  @param tid The thread id.
     *  @return Returns the target of the branch.
     */
    PointerID lookup( uint64_t instPC, ThreadID tid);

    /** Checks if a branch is in the LVPT.
     *  @param inst_PC The address of the branch to look up.
     *  @param tid The thread id.
     *  @return Whether or not the branch exists in the LVPT.
     */
    bool valid(uint64_t instPC, ThreadID tid);

    void update(uint64_t instPC,
                const PointerID &target,
                ThreadID tid, bool predict
               );




    float getAverageConfidenceLevel() {

        float avg = 0.0;

        for (size_t i = 0; i < numEntries; i++) {
          avg += float(confLevel[i].read());
        }

        return avg/numEntries;
    }

    void dumpStat(){
    //   auto v = predictorMissCount;
    //   std::vector<std::size_t> result(v.size());
    //   std::iota(std::begin(result), std::end(result), 0);
    //   std::sort(std::begin(result), std::end(result),
    //           [&v](const uint64_t & lhs, const uint64_t & rhs)
    //           {
    //               return v[lhs] > v[rhs];
    //           }
    //   );

    //   for (auto &idx : result) {
    //         std::cout << std::dec << "INDEX[" << idx << "]: " << std::endl;
    //         for (auto& elem1 : predictorMissHistory[idx]){
    //             std::cout << "FUNCTION[" <<elem1.first << "]: " << std::endl;
    //             for (auto& elem2 : elem1.second) {
    //                 std::cout << std::hex <<"["<< elem2.first << "] => ";
    //                 for (auto& elem3 : elem2.second)
    //                     std::cout << std::dec << elem3 << ",";
    //                 std::cout << std::endl;
    //             }
    //         }
    //   }

    }


  private:


    /** Returns the index into the LVPT, based on the branch's PC.
     *  @param inst_PC The branch to look up.
     *  @return Returns the index into the LVPT.
     */
    inline unsigned getIndex(uint64_t instPC, ThreadID tid);

    /** Returns the tag bits of a given address.
     *  @param inst_PC The branch's address.
     *  @return Returns the tag bits.
     */
    inline uint64_t getTag(uint64_t instPC);

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


    uint64_t LVPTStridePredictions;
    uint64_t LVPTConstantPredictions;
};


#endif
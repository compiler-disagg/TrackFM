#pragma once
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <stdint.h>
#include "Allocation.hpp"

using namespace std;

class AllocationMap{
  public:
    
    //TODO: We basically need to implement the allocator
    uint64_t memoryCapacity = 0;
    uint64_t totalSizeOfAllocations = 0;
    
};

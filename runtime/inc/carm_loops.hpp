#pragma once 
#define MAX_LOOPS 1024
class CARMLoop {
  public:
    uint64_t obj_index;
    uint64_t static_prefetch;
    bool active;
    bool carm_ptr;
    bool step_value_constant;
    uint64_t carm_start_addr;
    uint64_t stack_addr;
    uint64_t start_obj_index;
    uint64_t start_obj_offset;
    uint64_t region_addr;
    uint64_t stride;
};
void init_loops();

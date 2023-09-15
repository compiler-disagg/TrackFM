#pragma once
#include "array.hpp"
#include "deref_scope.hpp"
#include "device.hpp"
#include "manager.hpp"
#include "carm_loops.hpp"
#include "carm_object_config.hpp"
#include <mutex> 
#define NOT_MUT 0
#define MUT 1
using namespace far_memory;

class CARM_AIFM_DS {
  public:
    static int64_t induce_fn(uint64_t idx_0, uint64_t idx_1);
    static uint64_t infer_fn(uint64_t idx, int64_t stride);
    static GenericUniquePtr *mapping_fn(uint8_t *&state, uint64_t idx);

    CARM_AIFM_DS(FarMemManager * manager, uint32_t item_size);

    uint8_t  ds_id;
    uint64_t obj_size;
    uint64_t datalen;
    uint64_t num_objects;
    uint64_t total_obj_cnt;

    GenericUniquePtr ** ptrs_;

    std::unordered_map<uint64_t, uint64_t> allocation_map;

    uint64_t * free_list_map;

    uint64_t max_free_list;

    constexpr static auto kInduceFn = [](uint64_t idx_0,
        uint64_t idx_1) -> int64_t {
      return CARM_AIFM_DS::induce_fn(idx_0, idx_1);
    };
    constexpr static auto kInferFn = [](uint64_t idx,
        int64_t stride) -> uint64_t {
      return CARM_AIFM_DS::infer_fn(idx, stride);
    };
    constexpr static auto kMappingFn = [](uint8_t *&state,
        uint64_t idx) -> GenericUniquePtr * {
      return CARM_AIFM_DS::mapping_fn(state, idx);
    };

    Prefetcher<decltype(kInduceFn), decltype(kInferFn), decltype(kMappingFn)> * prefetcher_[MAX_LOOPS + 1];
};

class CARMRT {
  public:
    FarMemManager * manager;
    uint64_t aspace_start;
    uint64_t aspace_end;
    CARM_AIFM_DS * carm_ds_array;
    std::mutex carm_mtx;
};

extern CARMRT * carm_rt;
extern bool   carm_instrument;
constexpr uint64_t kNumConnections = 400;
constexpr uint64_t kFarMemSize = 17179869184;
//Working set size is the amount  memory required by application should be lesser than far memory size
constexpr uint64_t kWorkingSetSize = kFarMemSize - (1ULL<<30);
constexpr uint64_t local_mem_cache_size = 2147483648;
constexpr uint64_t kNumGCThreads = 3;

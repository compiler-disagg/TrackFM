#include <iostream>
#include "carm.hpp"
#include "thread.h"

#include "carm.hpp"
#include "carm_loops.hpp"
#include "carm_runtime.hpp"
#include "array.hpp"
#include "deref_scope.hpp"
#include "device.hpp"
#include "manager.hpp"
#include "pointer.hpp"
#include "carm_runtime_api.hpp"
#include <math.h>

using namespace far_memory;

FORCE_INLINE int64_t CARM_AIFM_DS::induce_fn(uint64_t idx_0,
                                                       uint64_t idx_1) {
  return idx_1 - idx_0;
}

FORCE_INLINE uint64_t CARM_AIFM_DS::infer_fn(uint64_t idx, int64_t stride) {
  return idx + stride;
}

FORCE_INLINE GenericUniquePtr *CARM_AIFM_DS::mapping_fn(uint8_t *&state,
                                                        uint64_t idx) {

  auto & ptrs = *reinterpret_cast<std::unique_ptr<GenericUniquePtr *[]> *>(state);
  auto num_items =
	  *reinterpret_cast<uint64_t *>(state + offsetof(CARM_AIFM_DS, total_obj_cnt) -
			  offsetof(CARM_AIFM_DS, ptrs_));

			if (idx < num_items)
				return ptrs[idx];
			return nullptr;
  //return idx < num_items ? ptrs[idx] : nullptr;
}

CARM_AIFM_DS::CARM_AIFM_DS(FarMemManager * manager, uint32_t item_size) 
{
  for (int i = 0; i < MAX_LOOPS + 1; i++) {
    this->prefetcher_[i] = new Prefetcher<decltype(kInduceFn), decltype(kInferFn),
    decltype(kMappingFn)>(manager->get_device(), reinterpret_cast<uint8_t *>(&ptrs_),item_size); 
  }
} 


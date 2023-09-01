#include <iostream>
#include "carm.hpp"
#include "thread.h"
#include "carm_runtime.hpp"
#include "array.hpp"
#include "deref_scope.hpp"
#include "device.hpp"
#include "manager.hpp"
#include "pointer.hpp"
#include "carm_runtime_api.hpp"
#include "carm_libc.hpp"
#include <math.h>
#include <stdarg.h>
#include <wchar.h>
#include "carm_loops.hpp"

#define DEFAULT_PREFETCHER 0

uint64_t remote_fetches = 0;
uint64_t local_fetches = 0;
using namespace far_memory;
uint64_t prev_obj_index[MAX_LOOPS + 1];

extern void mark_region_not_evacutable(uint64_t object_addr);
extern void invalidate_marked_region(); 
struct CARM_OBJ_STATE_TRACKER  * carm_obj_state;

uint64_t carm_loop_prev_addr = 0;
uint64_t carm_loop_prev_index = FLUSH_CACHE;

volatile bool in_scope = false;

static inline void acquire() {
	DerefScope::enter();
}

static inline void release() {
	DerefScope::exit();
}


void init_carm_meta_obj() {
	carm_obj_state = (CARM_OBJ_STATE_TRACKER *)aligned_alloc(CACHE_LINE_SIZE, sizeof(struct CARM_OBJ_STATE_TRACKER) * TOTAL_OBJECTS);
}


void flush_carn_meta_obj() {
	for (unsigned int i = 0; i < TOTAL_OBJECTS; i++) { 
		carm_obj_state[i].obj_state = FLUSH_CACHE;
	}
}

void set_carm_obj_state(FarMemPtrMeta & metaptr, uint64_t obj_index) {
		carm_obj_state[obj_index].obj_state = metaptr.to_uint64_t();
}

static inline void carm_add_prefetch_trace(uint64_t obj_index, bool nt, uint64_t pf_id) {
	if (prev_obj_index[pf_id] != obj_index) {
		carm_rt->carm_ds_array->prefetcher_[pf_id]->add_trace(nt, obj_index);
		prev_obj_index[pf_id] = obj_index;
	}
}
void carm_static_prefetch(uint64_t start, uint64_t step, uint64_t num, uint64_t loop_id) {
  carm_rt->carm_ds_array->prefetcher_[loop_id]->static_prefetch(start, step, num);
}

static inline GenericUniquePtr * allocate_object(uint64_t obj_index) {
    GenericUniquePtr  * obj = new GenericUniquePtr();
    *obj = carm_rt->manager->allocate_generic_unique_ptr(carm_rt->carm_ds_array->ds_id, carm_rt->carm_ds_array->obj_size);
    carm_rt->carm_ds_array->ptrs_[obj_index] = obj;
    carm_rt->carm_ds_array->ptrs_[obj_index]->set_carm_obj_index(obj_index);
    return carm_rt->carm_ds_array->ptrs_[obj_index];
}

static inline uint64_t carmDeref_read(uint64_t obj_index) {
	GenericUniquePtr * obj = carm_rt->carm_ds_array->ptrs_[obj_index];
  if (unlikely(obj == nullptr)) 
    obj = allocate_object(obj_index);
	uint64_t metadata  = (uint64_t)obj->__deref<false>();
	carm_obj_state[obj_index].obj_state = metadata;
	carm_add_prefetch_trace(obj_index, false, DEFAULT_PREFETCHER);
	return metadata >> kObjectDataAddrBitPos;
}

static inline uint64_t carmDeref_write(uint64_t obj_index) {
	GenericUniquePtr * obj = carm_rt->carm_ds_array->ptrs_[obj_index];
  if (unlikely(obj == nullptr)) 
    obj = allocate_object(obj_index);
	uint64_t metadata = obj->__deref_mut<false>();
	carm_obj_state[obj_index].obj_state = metadata;
	carm_add_prefetch_trace(obj_index, false, DEFAULT_PREFETCHER);
	return metadata >> kObjectDataAddrBitPos;
}

static inline uint64_t carmDeref_write_sp(uint64_t obj_index, uint64_t pf_id) {
	GenericUniquePtr * obj = carm_rt->carm_ds_array->ptrs_[obj_index];
  if (unlikely(obj == nullptr)) 
    obj = allocate_object(obj_index);
	uint64_t metadata = obj->__deref_mut<false>();
	carm_obj_state[obj_index].obj_state = metadata;
	carm_add_prefetch_trace(obj_index, false, pf_id);
	return metadata >> kObjectDataAddrBitPos;
}

static inline uint64_t carmDeref_read_sp(uint64_t obj_index, uint64_t pf_id) {
	GenericUniquePtr * obj = carm_rt->carm_ds_array->ptrs_[obj_index];
  if (unlikely(obj == nullptr)) 
    obj = allocate_object(obj_index);
	uint64_t metadata = obj->__deref<false>();
	carm_obj_state[obj_index].obj_state = metadata;
  carm_add_prefetch_trace(obj_index, false, pf_id); 
	return metadata >> kObjectDataAddrBitPos;
}
static inline uint64_t carmDeref_write_not_temporal(uint64_t obj_index) {
	GenericUniquePtr * obj = carm_rt->carm_ds_array->ptrs_[obj_index];
	if (unlikely(obj == nullptr)) 
		obj = allocate_object(obj_index);
	uint64_t metadata = obj->__deref_mut<true>();
	carm_obj_state[obj_index].obj_state = metadata;
	return metadata >> kObjectDataAddrBitPos;
}

static inline uint64_t carmDeref_read_not_temporal(uint64_t obj_index) {
	GenericUniquePtr * obj = carm_rt->carm_ds_array->ptrs_[obj_index];
	if (unlikely(obj == nullptr)) 
		obj = allocate_object(obj_index);
	uint64_t metadata = obj->__deref<true>();
	carm_obj_state[obj_index].obj_state = metadata;
	return metadata >> kObjectDataAddrBitPos;
}

/*
 */
static inline void carm_deref(uint64_t index) {

#ifdef TRACKFM_TRACE
        auto metadata = carm_obj_state[index].obj_state; 	
	auto exceptions = kPresentClear;
	if (exceptions & metadata)
		remote_fetches++;
	else
		local_fetches++;
#endif

	if (very_likely(in_scope))
		release();
	in_scope = true;
	acquire();
}
static inline void carm_deref_not_temporal(uint64_t index) {
	if (very_likely(in_scope))
		release();
	in_scope = true;
	acquire();
}
uint64_t carm_deref_read(uint64_t obj_index) {
  	//std::cout<<"\ncarm deref read "<<obj_index<<"\n";  

	carm_deref(obj_index);
	uint64_t deref_read = (uint64_t)carmDeref_read(obj_index);
  	//printf("deref read %p\n", deref_read);  
	return deref_read;
}

uint64_t carm_deref_write_not_temporal(uint64_t obj_index) {
	carm_deref(obj_index);
	return (uint64_t)carmDeref_write_not_temporal(obj_index);
}

uint64_t carm_deref_read_not_temporal(uint64_t obj_index) {
	carm_deref(obj_index);
	return (uint64_t)carmDeref_read_not_temporal(obj_index);
}
uint64_t carm_deref_write(uint64_t obj_index) {
  	//std::cout<<"carm deref write "<<obj_index<<"\n";  

	carm_deref(obj_index);
	uint64_t deref_write = (uint64_t)carmDeref_write(obj_index);
		
  	//printf("deref write %p\n", deref_write);  
	return deref_write;
}
//static prefetching
uint64_t carm_deref_write_sp(uint64_t obj_index, uint64_t loop_id) {
  //std::cout<<"carm deref write "<<obj_index<<"\n";  
	carm_deref(obj_index);
	return (uint64_t)carmDeref_write_sp(obj_index, loop_id);
}
uint64_t carm_deref_read_sp(uint64_t obj_index,  uint64_t loop_id) {
  //std::cout<<"\ncarm deref read "<<obj_index<<"\n";  
	carm_deref(obj_index);
	return (uint64_t)carmDeref_read_sp(obj_index, loop_id);
}
//called by evacuation threads when object is evacuated
void update_cache_object(GenericFarMemPtr * metaptr) {
	uint64_t old_index = metaptr->get_carm_obj_index();
  if (old_index != INDEX_INVALID)
		carm_obj_state[old_index].obj_state = (uint64_t)metaptr->meta().to_uint64_t();
}


void carm_trace() {
	std::cout<<"Remote fetches :"<<remote_fetches<<"\n";
	std::cout<<"Local fetches  :"<<local_fetches<<"\n";
}

void carm_debug(uint64_t buf) {
	//exit(0);
	if (carm_rt) {


#if 0
	uint64_t addr1 = carm_cache.cache_addr ^ CARM_PTR;
	uint64_t addr  =  (buf ^ CARM_PTR);
	uint64_t off  =  addr1 + OBJ_SIZE;
	if (addr < carm_rt->aspace_start && addr > off) {
		DEBUG_PRINT("carm_debug CARM CACHE %lx\n", carm_cache.cache_addr);
		DEBUG_PRINT("Escape BUF %lx\n", buf);
		DEBUG_PRINT("DIFF %lx\n", off);
	}
#endif
	}
}

#if 0
void * ptrDeref(void * buf, uint64_t mut) {
  struct Carm_Ptr carm_ptr;
  uint64_t src_addr = (uint64_t)buf;
  get_carm_ptr(&carm_ptr, src_addr, mut);
  return _ptrDeref(buf, &carm_ptr);
}
#endif

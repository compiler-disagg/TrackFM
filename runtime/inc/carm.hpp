#pragma once
#include "carm_object_config.hpp"
#include "array.hpp"
#include "deref_scope.hpp"
#include "device.hpp"
#include "manager.hpp"
#include <math.h>
#include "carm_libc.hpp"
using namespace far_memory;

#define barrier() asm volatile("" ::: "memory")

#define DEBUG 0
#if DEBUG
#define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( false )
#else
#define DEBUG_PRINT(...) do{ } while ( false )
#endif

#define INFO 0
#if INFO
#define INFO_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( false )
#else
#define INFO_PRINT(...) do{ } while ( false )
#endif

#define CARM_ALLOC_SIZE_MULTIPLE 8
#define  START_ADDR  0x1000000000000000 
#define MUTABLE 1
#define NOT_MUTABLE 0
//also update AIFm inc/pointer.hpp cache_index far mem ptr
#define FLUSH_CACHE 0xfffffffffffff
#define TIME_TO_PREFETCH_SIZE 1ULL<<20
//this should be a multiple of CACHE_LINE_SIZE
//#define CACHE_SIZE (OBJ_CACHE_SIZE/OBJ_SIZE) 

#define check_carm_object(addr) !!((addr) & CARM_PTR)

struct Carm_Ptr {
  uint64_t  addr;
  uint64_t offset;
  uint64_t obj_index;
  uint64_t mut;
  uint64_t cache_id;
  bool is_carmptr;
};
struct  CARM_OBJ_STATE_TRACKER {
  uint64_t obj_state;
};
inline void get_carm_ptr(struct Carm_Ptr * cptr, uint64_t addr, uint64_t mut) {
	if (!check_carm_object(addr)) {
		cptr->is_carmptr = false;
		cptr->addr   =  0;
		cptr->obj_index  =  0;
		cptr->offset =  0; //never possible as rem always less than obj size
		cptr->mut  =  0; 
	}
	else {
		cptr->is_carmptr = true;
		cptr->addr   =  (addr ^ CARM_PTR);
		cptr->obj_index  =  (cptr->addr / OBJ_SIZE);
		cptr->offset =  (cptr->addr % OBJ_SIZE);
		cptr->mut  =  mut; 
	}
}
extern struct CARM_OBJ_STATE_TRACKER  * carm_obj_state;

//bool check_carm_object(uint64_t addr); 
//void get_carm_ptr(struct Carm_Ptr * cptr, uint64_t addr, uint64_t mut);
//void * ptrDeref(void * buf, uint64_t mut); 
//void * _ptrDeref(void * buf, struct Carm_Ptr * carm_ptr); 
void init_cache(); 
void set_carm_obj_state(FarMemPtrMeta & metaptr, uint64_t obj_index);
void init_carm_meta_obj();
void flush_carn_meta_obj();
uint64_t carm_deref_write(uint64_t obj_index); 
uint64_t carm_deref_read(uint64_t obj_index); 
uint64_t carm_deref_write_not_temporal(uint64_t index);
uint64_t carm_deref_read_not_temporal(uint64_t index);
uint64_t carm_deref_write_sp(uint64_t index, uint64_t loop_id);
uint64_t carm_deref_read_sp(uint64_t index, uint64_t loop_id);
void carm_static_prefetch(uint64_t start, uint64_t step, uint64_t num, uint64_t loop_id); 

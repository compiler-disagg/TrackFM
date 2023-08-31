#include <iostream>
#include "carm.hpp"
#include "thread.h"
#include "carm_runtime.hpp"
#include "array.hpp"
#include "deref_scope.hpp"
#include "carm_loops.hpp"
#include "device.hpp"
#include "manager.hpp"
#include "pointer.hpp"
#include "carm_runtime_api.hpp"
#include "carm_libc.hpp"
#include <math.h>
#include <stdarg.h>
#include <wchar.h>
#include <time.h>
#include <unistd.h>

using namespace far_memory;

uint64_t loop_id_cnt = 0;


//these two API not needed for now, as loop hoisting fetches individual objects, 
//the API will be required if we start to fetch regions but this is not yet supported, 
//extern void mark_region_not_evacutable(uint64_t object_addr);
//extern void invalidate_marked_region(); 

#define MICRO 1000000
double get_time_us() {
	struct timespec start;
	clock_gettime( CLOCK_BOOTTIME, &start );
	return (start.tv_sec * MICRO) + (start.tv_nsec/1000);
}
CARMLoop loopstate[MAX_LOOPS]; 


void init_loops() {
  memset(loopstate, 0, sizeof(CARMLoop) * MAX_LOOPS);
}

double carm_time = 0.0;
double loop_time = 0.0;
double deref_time = 0.0;
static inline uint64_t _carm_deref_loop_write(uint64_t loop_id) {
//  carm_time = get_time_us();
  if (loopstate[loop_id].carm_ptr) {
    carm_rt->manager->invalidate_marked_region(loopstate[loop_id].region_addr); 
    uint64_t * loop_ptr = (uint64_t *)carm_deref_write_sp(loopstate[loop_id].obj_index, loop_id + 1);
//    deref_time += get_time_us() - carm_time;
    loopstate[loop_id].region_addr = carm_rt->manager->mark_region_not_evacutable((uint64_t)loop_ptr);
    if (unlikely(loopstate[loop_id].start_obj_index == loopstate[loop_id].obj_index)) {
      loopstate[loop_id].obj_index += loopstate[loop_id].stride;
  //    loop_time += get_time_us() - carm_time;
      return (uint64_t)loop_ptr + loopstate[loop_id].start_obj_offset;
    }
    //loopstate[loop_id].obj_index++;
    loopstate[loop_id].obj_index +=  loopstate[loop_id].stride;
//    loopstate[loop_id_cnt].static_prefetch--;
  //  if (!loopstate[loop_id_cnt].static_prefetch) {
  //    carm_static_prefetch(loopstate[loop_id_cnt].obj_index, 1, 256);
 //     loopstate[loop_id_cnt].static_prefetch = 256;
//    }
   // loop_time += get_time_us() - carm_time;

    return (uint64_t)loop_ptr;
  }
  if (loopstate[loop_id].obj_index > 0) {
    std::cout<<"Stack address should call this method only once \n";
    //abort();
 }
  //just in case it is a stack pointer handle it
  uint64_t loop_ptr = (uint64_t)loopstate[loop_id].stack_addr + (loopstate[loop_id].obj_index * OBJ_SIZE);
  loopstate[loop_id].obj_index++;
  return loop_ptr;
}

uint64_t carm_deref_loop_write(uint64_t loopid) {
  return _carm_deref_loop_write(loopid);
}

uint64_t _carm_deref_loop_read(uint64_t loop_id) {
  //carm_time = get_time_us();
  if (loopstate[loop_id].carm_ptr) {
    carm_rt->manager->invalidate_marked_region(loopstate[loop_id].region_addr); 
    uint64_t * loop_ptr = (uint64_t *)carm_deref_read_sp(loopstate[loop_id].obj_index, loop_id + 1);
    //deref_time += get_time_us() - carm_time;
    loopstate[loop_id].region_addr = carm_rt->manager->mark_region_not_evacutable((uint64_t)loop_ptr);
    if (unlikely(loopstate[loop_id].start_obj_index == loopstate[loop_id].obj_index)) {
      loopstate[loop_id].obj_index += loopstate[loop_id].stride;
      //loop_time += get_time_us() - carm_time;
			uint64_t addr =  (uint64_t)loop_ptr + loopstate[loop_id].start_obj_offset;
      return addr;
    }
    loopstate[loop_id].obj_index +=  loopstate[loop_id].stride;
  //  loopstate[loop_id_cnt].static_prefetch--;
  //  if (!loopstate[loop_id_cnt].static_prefetch) {
   //   carm_static_prefetch(loopstate[loop_id_cnt].obj_index, 1, 256);
    //  loopstate[loop_id_cnt].static_prefetch = 256;
   // }
    //loop_time += get_time_us() - carm_time;
    return (uint64_t)loop_ptr;
  }
  if (loopstate[loop_id].obj_index > 0) {

		printf("stack addr %p\n", loopstate[loop_id].stack_addr);
    std::cout<<"Stack address should call this method only once \n";
    //abort();
 }
  //just in case it is a stack pointer handle it
  uint64_t loop_ptr = (uint64_t)loopstate[loop_id].stack_addr + (loopstate[loop_id].obj_index * OBJ_SIZE);
  loopstate[loop_id].obj_index++;
  return loop_ptr;
}

uint64_t carm_deref_loop_read(uint64_t loopid) {
  return _carm_deref_loop_read(loopid);
}

uint64_t stack_address = 0;
uint64_t carm_address = 0;
//side effect returns object size
uint64_t carm_loop_init(uint64_t src_addr, uint64_t step_value_const) {
//			printf("%s %d loopd id %d\n", __func__, __LINE__,  loop_id_cnt);
  uint64_t retval = 0;
  uint64_t obj_s = 0;
#if 1
  if (loopstate[loop_id_cnt].active){
    std::cout<<"WARNING nested loops not supported yet disable loop optimization\n";
    //loop_id_cnt++;
    abort();
  }
#endif
 
  if (!step_value_const)
    loopstate[loop_id_cnt].step_value_constant = true; 
  else
    loopstate[loop_id_cnt].step_value_constant = false; 

  struct Carm_Ptr carm_ptr;
  //mut flag does not matter
  get_carm_ptr(&carm_ptr, src_addr, true);
  if (carm_ptr.is_carmptr) {
    loopstate[loop_id_cnt].carm_start_addr = src_addr;
    loopstate[loop_id_cnt].active = true;
    loopstate[loop_id_cnt].carm_ptr = true;
    loopstate[loop_id_cnt].obj_index = carm_ptr.obj_index;
    loopstate[loop_id_cnt].region_addr = 0;
    loopstate[loop_id_cnt].start_obj_index = carm_ptr.obj_index;
    loopstate[loop_id_cnt].start_obj_offset = carm_ptr.offset;
    
    if (!step_value_const)
    loopstate[loop_id_cnt].stride =  0;
    else if (step_value_const < OBJ_SIZE)
    loopstate[loop_id_cnt].stride =  1;
		else
    loopstate[loop_id_cnt].stride =  step_value_const/OBJ_SIZE;

		//this is a loop ptr whose induction variabel is in an outer loop only
		//prefetch in header
		if (!step_value_const) {
      carm_static_prefetch(loopstate[loop_id_cnt].obj_index, 1, 8, loop_id_cnt + 1);
		}
    obj_s = (OBJ_SIZE - carm_ptr.offset);
   // if (!step_value_const) {
     // carm_static_prefetch(loopstate[loop_id_cnt].obj_index, 1, 512, loop_id_cnt + 1);
   //   loopstate[loop_id_cnt].static_prefetch = 256;
   // }
  }
  else {
    loopstate[loop_id_cnt].stack_addr = src_addr;
    loopstate[loop_id_cnt].active = true;
    loopstate[loop_id_cnt].carm_ptr = false;
    //Not a carm ptr found a stack address;
    loopstate[loop_id_cnt].obj_index = 0;
    loopstate[loop_id_cnt].region_addr = 0;
    // stack_address++;
    //we dont care about stack addresses for now, ensure its a large enough number so that loop_deref is not called
    obj_s = OBJ_SIZE + (512ULL<<40);
  }

  retval = obj_s | ( loop_id_cnt << LOOP_ID_POS);

  loop_id_cnt++;
  return retval;
}

void invalidate_loop_index(uint64_t loop_id) {
  if (loopstate[loop_id].active) {
    //std::cout<<"Loop time "<<loop_time<<"\n";
    //std::cout<<"Deref time "<<deref_time<<"\n";
    //loop_time = 0;
    if (loopstate[loop_id].carm_ptr)
      carm_rt->manager->invalidate_marked_region(loopstate[loop_id].region_addr); 
    loopstate[loop_id].active = false;
    loopstate[loop_id].static_prefetch = 0;
    loopstate[loop_id].carm_ptr = false;
    loopstate[loop_id].stack_addr = 0;
    loopstate[loop_id].carm_start_addr = 0;
    loopstate[loop_id].obj_index = 0;
    loopstate[loop_id].start_obj_index = 0;
    loopstate[loop_id].start_obj_offset = 0;
    loopstate[loop_id].step_value_constant = false; 
    loopstate[loop_id].region_addr = 0;
    if (loop_id_cnt > 0) {
      loop_id_cnt--;
    }
  }
}

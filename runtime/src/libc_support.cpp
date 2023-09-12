#include <iostream>
#include <errno.h>
#include <string.h>

#include "carm.hpp"
#include "thread.h"
#include "carm_runtime.hpp"
#include "array.hpp"
#include "deref_scope.hpp"
#include "device.hpp"
#include "manager.hpp"
#include "pointer.hpp"
#include "carm_runtime_api.hpp"
#include <math.h>
#include <stdarg.h>
#include <wchar.h>
#include <unordered_map>
#include <fstream>
#include "carm_libc.hpp"
#include<unistd.h>
using namespace std;

ofstream myfile;
ofstream myfile1;
inline bool valid_size (int size) {
  return size ? true:false;
}
static inline void * _ptrDeref(void * buf,  struct Carm_Ptr * carm_ptr) {
	auto metadata = carm_obj_state[carm_ptr->obj_index].obj_state;
	auto exceptions = (kHotClear | kPresentClear |
			kEvacuationSet);

	if (carm_ptr->mut) {
		exceptions |= kDirtyClear;
	}

	uint64_t addr;
	if (metadata & exceptions) {

		if (carm_ptr->mut) 
			addr = carm_deref_write(carm_ptr->obj_index);
		else 
			addr = carm_deref_read(carm_ptr->obj_index);

	}
	else
		addr = metadata >>  kObjectDataAddrBitPos;
	//rel_cache->cache_index = carm_ptr->index;

	return ((void *)(addr + carm_ptr->offset));
}
// multiple should be a power of 2 so that allocation spanning more than 1 object is handled correctly 
// example SIZE MULTPLE = 8 then alloc(4094) ->  alloc (16) would allocate 4096 in first object and the rest in the second object
double copy_time = 0.0;
#define MICRO 1000000
double get_itime_us() {
	struct timespec start;
	clock_gettime( CLOCK_BOOTTIME, &start );
	return (start.tv_sec * MICRO) + (start.tv_nsec/1000);
}
static inline uint64_t enforce_size_constraints(uint64_t size) {
  if (size <  CARM_ALLOC_SIZE_MULTIPLE)
    return CARM_ALLOC_SIZE_MULTIPLE;
  uint64_t rem = size % CARM_ALLOC_SIZE_MULTIPLE;
  return rem ? (size - rem) + CARM_ALLOC_SIZE_MULTIPLE : size;
}


static inline void  _carm_memcpy_dst_carm_ptr(struct Carm_Ptr * carm_dst, int8_t * src, uint64_t size, int8_t * dst) {
  unsigned int bytes_transfered = 0;
  int chunk_size = 0;
  while (bytes_transfered < size) {
    void * d = _ptrDeref((void *)((uint64_t)dst + bytes_transfered), carm_dst);
    void * s = (void *)((uint64_t)src + bytes_transfered);
    chunk_size = OBJ_SIZE - carm_dst->offset;
    if ((bytes_transfered + chunk_size) < size)
      memmove(d, s, chunk_size);
    else {
      memmove(d, s, size - bytes_transfered);
      break;
    }
    bytes_transfered += chunk_size;
    carm_dst->obj_index++;
    carm_dst->offset = 0;
  }
}
static inline void _carm_memcpy_src_carm_ptr(int8_t * dst, struct Carm_Ptr * carm_src, uint64_t size, int8_t * src) {
  unsigned int bytes_transfered = 0;
  int chunk_size = 0;
  while (bytes_transfered < size) {
    void * s = _ptrDeref((void *)((uint64_t)src + bytes_transfered), carm_src);
    void * d = (void *)((uint64_t)dst + bytes_transfered);
    chunk_size = OBJ_SIZE - carm_src->offset;
    if ((bytes_transfered + chunk_size) < size)
      memmove(d, s, chunk_size);
    else {
      memmove(d, s, size - bytes_transfered);
      break;
    }
    bytes_transfered += chunk_size;
    carm_src->obj_index++;
    carm_src->offset = 0;
  }
}
bool trace_init_memcpy = false;
long local_memcpy = 0;
long dst_memcpy = 0;
long src_memcpy = 0;
long src_dst_memcpy = 0;
void * carm_memcpy(int8_t * dst, int8_t * src, uint64_t size) {
  //copy_time = 0;
  //double time =  get_itime_us();
#if 0
  if (!trace_init_memcpy) {
  myfile.open ("/home/CARM/runtime/exp/fig9/carm/memcpy_trace",  ios::out | ios::app);
  trace_init_memcpy = true;
  }
#endif
  struct Carm_Ptr carm_dst;
  struct Carm_Ptr carm_src;
  get_carm_ptr(&carm_dst, (uint64_t)dst, 1);
  get_carm_ptr(&carm_src, (uint64_t)src, 0);
  if (!carm_dst.is_carmptr && !carm_src.is_carmptr) {
      //myfile << "total : "<<local_memcpy++<<" : "<<size <<" :0" << "\n";
      return memmove(dst, src, size);
   //   copy_time += (get_itime_us() - time);
      //printf("memcpy local copy time : %lf\n" , copy_time);
  }
  else if (carm_dst.is_carmptr && !carm_src.is_carmptr) {
#if 0
      myfile << "total : "<<dst_memcpy++<<" : " <<size<<" :1"<< "\n";
#endif
	_carm_memcpy_dst_carm_ptr(&carm_dst, src, size, dst);
	return (void *)dst;

  }
  else if (!carm_dst.is_carmptr && carm_src.is_carmptr) {
#if 0
      myfile << "total : "<<src_memcpy++<<" :" <<size<<" :2"<< "\n";
#endif
	_carm_memcpy_src_carm_ptr(dst, &carm_src, size, src);
	return (void *)dst;

  }

#if 0
  myfile << "total : "<<src_dst_memcpy++<<" : "<<size<<" :3"<< "\n";
#endif
  //DEBUG_PRINT("Carm memcpy dst %lx src %lx\n", dst, src);

  unsigned int bytes_transfered = 0;
  int size_dst = 0;
  int size_src = 0;
  int chunk_size = 0;
  while (bytes_transfered < size) {
    void * d = _ptrDeref((void *)((uint64_t)dst + bytes_transfered), &carm_dst);
    void * s = _ptrDeref((void *)((uint64_t)src + bytes_transfered), &carm_src);
    size_dst = OBJ_SIZE - carm_dst.offset;
    size_src = OBJ_SIZE - carm_src.offset;

    chunk_size = min(size_dst, size_src);
#if 0
    if ((size - bytes_transfered)  > TIME_TO_PREFETCH_SIZE) { 
      if (carm_ptr.is_carmptr)
        carm_add_prefetch_trace(carm_ptr.index);
      if (carm_ptr1.is_carmptr)
        carm_add_prefetch_trace(carm_ptr1.index);
    }
#endif

    if (!valid_size(chunk_size)) { 
      chunk_size = max(size_dst, size_src);
      if (!valid_size(chunk_size))  
        DEBUG_PRINT("carm_memcpy Not valid chunk size ");
      exit(0);
    }
    //memove safer than memcpy when d = s
    if ((bytes_transfered + chunk_size) < size)
      memmove(d, s, chunk_size);
    else {
      memmove(d, s, size - bytes_transfered);
      break;
    }
    bytes_transfered += chunk_size;
    get_carm_ptr(&carm_dst, (uint64_t)dst + bytes_transfered, 1);
    get_carm_ptr(&carm_src, (uint64_t)src + bytes_transfered, 0);
  }
  //copy_time += (get_itime_us() - time);
  //printf("memcpy remote copy time : %lf\n" , copy_time);
  //DEBUG_PRINT("Carm memcpy success\n");
  return (void *)dst;
}

void * carm_wmemcpy(int32_t * dst, int32_t * src, uint64_t size) {
  DEBUG_PRINT("Carm wmemcpy dst %lx src %lx\n", dst, src);
  exit(0);
  return carm_memcpy((int8_t *) dst, (int8_t *) src, size * sizeof(int32_t)); 
  //	DEBUG_PRINT("Carm wmemcpy init\n");
}

void * carm_memset(void * dst, int src, uint64_t size) {
  //DEBUG_PRINT("Carm memset  \n" );
  struct Carm_Ptr carm_dst;
  uint64_t mut = 1;
  get_carm_ptr(&carm_dst, (uint64_t)dst, mut);
  if (!carm_dst.is_carmptr) {
    return memset(dst, src, size);
  }
  unsigned int bytes_set = 0;
  int chunk_size = 0;
  while (bytes_set < size) {
#if 0
    if ((size - bytes_set)  > TIME_TO_PREFETCH_SIZE) { 
      if (carm_dst.is_carmptr)
        carm_add_prefetch_trace(carm_dst.index);
    }
#endif
    void * d = _ptrDeref((void *)((uint64_t)dst + bytes_set), &carm_dst);
    chunk_size = OBJ_SIZE - carm_dst.offset;
    if (unlikely(!valid_size(chunk_size)))  {
      DEBUG_PRINT("carm_memset Not valid chunk size ");
      exit(0);
      break;
    }
    if ((bytes_set + chunk_size) < size)
      memset(d, src, chunk_size);
    else
      memset(d, src, size - bytes_set);
    bytes_set += chunk_size;
    carm_dst.obj_index++;
    carm_dst.offset = 0;
  }
  return dst;
}
//local malloc
void * carm_local_malloc(uint64_t size) {
  uint64_t ret = 0;
  ret = (uint64_t)malloc(size);
  carm_rt->carm_ds_array->allocation_map[ret] = size;
  return (void *)ret; 
}

void * carm_remote_malloc(uint64_t size) {
  uint64_t ret = 0;
  //return (void *)malloc(size);
  if (carm_rt) {
    uint64_t asize = enforce_size_constraints(size);
    carm_rt->carm_mtx.lock();
    ret = carm_rt->aspace_start;
    INFO_PRINT("carm malloc %ld ADDR %lx\n", asize, carm_rt->aspace_start);  

    //carm_memset((void*)ret, 0 , asize);
    carm_rt->carm_ds_array->allocation_map[ret] = asize;
    carm_rt->aspace_start += asize;
    carm_rt->carm_mtx.unlock();
    return (void *)ret;
    //	carm_rt->carm_mtx.unlock();
  }
  return (void *)carm_local_malloc(size); 
}

void * carm_calloc(uint64_t elem, uint64_t elem_size) {
  uint64_t size = elem * elem_size;
  uint64_t ret = 0;
  //myfile.open ("/home/CARM/runtime/exp/fig7/carm/allocation_trace.txt",  ios::out | ios::app);
  //return (void *)calloc(elem, elem_size);
  if (carm_rt) {
    uint64_t asize = enforce_size_constraints(size);
    carm_rt->carm_mtx.lock();
    //		myfile << asize<<"\n";
    //		myfile.close();
    ret = carm_rt->aspace_start;
    INFO_PRINT("carm calloc %ld ADDR %lx\n", asize, carm_rt->aspace_start);  
    carm_rt->carm_ds_array->allocation_map[ret] = asize;
    carm_rt->aspace_start += asize;
    carm_memset((void*)ret, 0 , asize);
    carm_rt->carm_mtx.unlock();
    return (void *)ret; 
    //		carm_rt->carm_mtx.unlock();
  }
  //myfile << size<<"\n";
  //myfile.close();
  ret = (uint64_t)calloc(elem, elem_size);
  carm_rt->carm_ds_array->allocation_map[ret] = size;
  return (void *)ret; 
}

//add freed objects to end of list
static inline void update_object_list(uint64_t old_index) {
  uint64_t new_index = carm_rt->carm_ds_array->total_obj_cnt;
  carm_rt->carm_ds_array->ptrs_[new_index] = carm_rt->carm_ds_array->ptrs_[old_index];
  carm_rt->carm_ds_array->ptrs_[new_index]->set_carm_obj_index(new_index);
  carm_rt->carm_ds_array->total_obj_cnt++;
}

void  carm_free(void * ptr) {

  uint64_t addr = (uint64_t) ptr;
  //free(ptr);
  if (check_carm_object(addr)) {

    INFO_PRINT("carm free %lx\n", ptr);  
    uint64_t mut = 0;
    struct Carm_Ptr carm_ptr;
    uint64_t size = carm_rt->carm_ds_array->allocation_map[addr];
    get_carm_ptr(&carm_ptr, addr, mut);
    uint64_t freed_size = 0;
    unsigned int chunk_size = 0;

    chunk_size = OBJ_SIZE - carm_ptr.offset;
    carm_rt->carm_mtx.lock();
    while (freed_size < size) {
      if (!valid_size(chunk_size)) {
        carm_rt->carm_mtx.unlock();
        exit(0);
        break;
      }

#if 0
      if ((size - freed_size)  > TIME_TO_PREFETCH_SIZE) { 
        if (carm_ptr.is_carmptr)
          carm_add_prefetch_trace(carm_ptr.index);
      }
#endif
      if ((freed_size + chunk_size) < size)
        carm_rt->carm_ds_array->free_list_map[carm_ptr.obj_index] += chunk_size;
      else
        carm_rt->carm_ds_array->free_list_map[carm_ptr.obj_index] += (size - freed_size);

#if 1
      if (carm_rt->carm_ds_array->free_list_map[carm_ptr.obj_index] == OBJ_SIZE) { 
        if (carm_rt->carm_ds_array->ptrs_[carm_ptr.obj_index])
          update_object_list(carm_ptr.obj_index);
        else
          break; //contigous data no point going ahead uninitialized data
      }
      if (unlikely(carm_rt->carm_ds_array->free_list_map[carm_ptr.obj_index] > OBJ_SIZE)) { 
        std::cout<<"Should not happen carm free having size > OBJ size\n";
        carm_rt->carm_mtx.unlock();
        abort();
      }
#endif

      freed_size += chunk_size;
      chunk_size = OBJ_SIZE;
      carm_ptr.obj_index++;
    }
      carm_rt->carm_mtx.unlock();
  }
  else {
      free(ptr);
  }

}


uint8_t * carm_realloc(void * ptr, uint64_t size) {


  if (!check_carm_object((uint64_t)ptr)) {

      return (uint8_t *)realloc(ptr, size);
  }

  if (size == 0) {
    carm_free(ptr);
    return NULL;
  }

  void * rptr = carm_remote_malloc(size); 
  if ((uint64_t)ptr && (size > 0)) {
    carm_memcpy((int8_t*)rptr, (int8_t*)ptr, carm_rt->carm_ds_array->allocation_map[(uint64_t)ptr]);
    carm_free(ptr);
  }

  return (uint8_t*)rptr; 
}


uint64_t carm_strlen(void * s1)
{
  //DEBUG_PRINT("carm_strlen \n");
  if (!check_carm_object((uint64_t)s1)) {
    return strlen((char *)s1);
  }

  struct Carm_Ptr carm_ptr;
  uint64_t len = 0;
  unsigned int checked_bytes = 0;
  unsigned int chunk_size = 0;
  char * dest  = nullptr;
  uint64_t mut = 0;

  get_carm_ptr(&carm_ptr, (uint64_t)s1 + checked_bytes, mut);

  while (true) {
    dest = (char *)_ptrDeref((void *)((uint64_t)s1 + checked_bytes), &carm_ptr);
    chunk_size = OBJ_SIZE - carm_ptr.offset;
    for (unsigned int j = 0; j < chunk_size; j++) {
      if (dest[j] == '\0') {
        return len;
      }
      len++;
    }
    if (!valid_size(chunk_size)) {
      DEBUG_PRINT("carm_strlen not valid chunksize \n");
      exit(0);
      break;
    }
    checked_bytes += chunk_size;
    carm_ptr.obj_index++;
    carm_ptr.offset = 0;
  }
  return 0;
}

void * _carm_strcpy_dst_carm_ptr(void * dst, void * src) {
	int n = strlen((char *)src) + 1;
	return carm_memcpy((int8_t *)dst, (int8_t *)src, n);
}

void * _carm_strcpy_src_carm_ptr(void * dst, void * src) {
	int n = carm_strlen(src) + 1;
	return carm_memcpy((int8_t *)dst, (int8_t *)src, n);
}

void * carm_strcpy(void * d, void * s)
{

  if (!check_carm_object((uint64_t)d) && !check_carm_object((uint64_t)s)) {
      return strcpy((char *)d, (char *) s);
  }
  else if (check_carm_object((uint64_t)d) && !check_carm_object((uint64_t)s)) {
	return _carm_strcpy_dst_carm_ptr(d, s);

  }
  else if (!check_carm_object((uint64_t)d) && check_carm_object((uint64_t)s)) {
	return _carm_strcpy_src_carm_ptr(d, s);

  }
  int n = carm_strlen(s) + 1;
  return carm_memcpy((int8_t *)d, (int8_t *)s, n);
}

void * carm_strncpy(void * s1, void * s2, uint64_t n)
{
  DEBUG_PRINT("Carm strncpy \n");
  if (!check_carm_object((uint64_t)s1) && !check_carm_object((uint64_t)s2)) {
      return strncpy((char *)s1, (char *) s2, n);
  }

  return  carm_memcpy((int8_t *)s1, (int8_t *)s2, n);
}


static inline int _carm_memcmp_src_carm_ptr(void * dst, struct Carm_Ptr * carm_src, uint64_t n, void * src) {

  uint64_t size = n;
  unsigned int bytes_read = 0;
  int chunk_size = 0;
  int ret = 0;
  while (bytes_read < size) {
    void * d = (void *)((uint64_t)dst + bytes_read);
    void * s = _ptrDeref((void *)((uint64_t)src + bytes_read), carm_src);

    chunk_size = OBJ_SIZE - carm_src->offset;

    if ((bytes_read + chunk_size) < size)
      ret = memcmp(d, s, chunk_size);
    else {
      ret = memcmp(d, s, size - bytes_read);
      return ret;
    }
    if (ret != 0)
      return ret;
    bytes_read += chunk_size;
    carm_src->obj_index++;
    carm_src->offset = 0;
  }
  return 0;

}

static inline int _carm_memcmp_dst_carm_ptr(struct Carm_Ptr * carm_dst, void * src, uint64_t n, void * dst) {

  uint64_t size = n;
  unsigned int bytes_read = 0;
  int chunk_size = 0;
  int ret = 0;
  while (bytes_read < size) {
    void * d = _ptrDeref((void *)((uint64_t)dst + bytes_read), carm_dst);
    void * s = (void *)((uint64_t)src + bytes_read);

    chunk_size = OBJ_SIZE - carm_dst->offset;

    if ((bytes_read + chunk_size) < size)
      ret = memcmp(d, s, chunk_size);
    else {
      ret = memcmp(d, s, size - bytes_read);
      return ret;
    }
    if (ret != 0)
      return ret;
    bytes_read += chunk_size;
    carm_dst->obj_index++;
    carm_dst->offset = 0;
  }
  return 0;

}
bool trace_init_memcmp = false;
long local_memcmp = 0;
long dst_memcmp = 0;
long src_memcmp = 0;
long src_dst_memcmp = 0;
int carm_memcmp(void * dst, void * src, uint64_t n)
{
  struct Carm_Ptr carm_dst;
  struct Carm_Ptr carm_src;
#if 0
  if (!trace_init_memcmp) {
  myfile1.open ("/home/CARM/runtime/exp/fig9/carm/memcmp_trace",  ios::out | ios::app);
  trace_init_memcmp = true;;
  }
#endif
  DEBUG_PRINT("Carm memcmp dst %lx src %lx\n", dst, src);
  if (!check_carm_object((uint64_t)dst) && !check_carm_object((uint64_t)src)) {
#if 0
      myfile1 << "total : "<<local_memcmp++<<" : "<<n <<" :0" << "\n";
#endif
      return memcmp(dst, src, n);
  }
  else if (check_carm_object((uint64_t)dst) && !check_carm_object((uint64_t)src)) {
#if 0
      myfile1 << "total : "<<dst_memcmp++<<" : "<<n <<" :1" << "\n";
#endif
      get_carm_ptr(&carm_dst, (uint64_t)dst, 0);
      return _carm_memcmp_dst_carm_ptr(&carm_dst, src, n, dst);
  }
  else if (!check_carm_object((uint64_t)dst) && check_carm_object((uint64_t)src)) {
#if 0
      myfile1 << "total : "<<src_memcmp++<<" : "<<n <<" :2" << "\n";
#endif
      get_carm_ptr(&carm_src, (uint64_t)src, 0);
      return _carm_memcmp_src_carm_ptr(dst, &carm_src, n, src);
  }

#if 0
  myfile1 << "total : "<<src_dst_memcmp++<<" : "<<n <<" :3" << "\n";
#endif
  uint64_t size = n;
  unsigned int bytes_read = 0;
  int size_dst = 0;
  int size_src = 0;
  int chunk_size = 0;
  int ret = 0;
  uint64_t mut = 0;
  while (bytes_read < size) {
    get_carm_ptr(&carm_dst, (uint64_t)dst + bytes_read, mut);
    get_carm_ptr(&carm_src, (uint64_t)src + bytes_read, mut);
    void * d = _ptrDeref((void *)((uint64_t)dst + bytes_read), &carm_dst);
    void * s = _ptrDeref((void *)((uint64_t)src + bytes_read), &carm_src);
#if 0
    if ((size - bytes_read)  > TIME_TO_PREFETCH_SIZE) { 
      if (carm_m1.is_carmptr)
        carm_add_prefetch_trace(carm_m1.index);
      if (carm_m2.is_carmptr)
        carm_add_prefetch_trace(carm_m2.index);
    }
#endif
    size_dst = OBJ_SIZE - carm_dst.offset;
    size_src = OBJ_SIZE - carm_src.offset;
    //if both are -1 is a noy possible as one of them is always a carm ptr
    chunk_size = min(size_dst, size_src);

    if (!valid_size(chunk_size)) { 
      chunk_size = max(size_dst, size_src);
      if (!valid_size(chunk_size)) { 
        exit(0);
        break;
      }
    }

    if ((bytes_read + chunk_size) < size)
      ret = memcmp(d, s, chunk_size);
    else
      ret = memcmp(d, s, size - bytes_read);
    if (ret != 0)
      return ret;
    bytes_read += chunk_size;
  }

  return 0;	
}


unsigned long carm_strtoul( char *nptr, char **endptr, int base) {

  if (!check_carm_object((uint64_t)nptr)) {
      return strtoul(nptr, endptr, base);
  }

  if (endptr) {
	  DEBUG_PRINT("non null end ptr not handled");
	  exit(0);
  }

  struct Carm_Ptr carm_ptr;
  char * dest  = nullptr;
  uint64_t mut = 0;

  get_carm_ptr(&carm_ptr, (uint64_t)nptr, mut);

  //minimum alloc size is 8 bytes so we should be safe
  dest = (char *)_ptrDeref((void *)((uint64_t)nptr), &carm_ptr);

  return strtoul(dest, endptr, base);

}

long carm_strtol( char *nptr, char **endptr, int base) {

  if (!check_carm_object((uint64_t)nptr)) {
      return strtol(nptr, endptr, base);
  }

  if (endptr) {
	  DEBUG_PRINT("non null end ptr not handled");
	  exit(0);
  }

  struct Carm_Ptr carm_ptr;
  char * dest  = nullptr;
  uint64_t mut = 0;

  get_carm_ptr(&carm_ptr, (uint64_t)nptr, mut);

  //minimum alloc size is 8 bytes so we should be safe
  dest = (char *)_ptrDeref((void *)((uint64_t)nptr), &carm_ptr);

  return strtol(dest, endptr, base);

}
//should be sizeof(wchar_t)
int carm_wmemcmp(int32_t * s1, int32_t * s2, uint64_t n)
{
  DEBUG_PRINT("Carm wmemcmp dst %lx src %lx\n", s1, s2);
  if (!check_carm_object((uint64_t)s1)) {
    if (!check_carm_object((uint64_t)s2)) {
      return wmemcmp( (const wchar_t*)s1, (const wchar_t*)s2, n );
    }
  }
  DEBUG_PRINT("carm_wmemcmp NOT IMPLEMENTED");
  exit(0);
  return carm_memcmp((void *) s1, (void * )s2, n);
}
void * carm_memchr(void *str, int c, size_t n) {
  if (!check_carm_object((uint64_t)str)) {
    return memchr(str, c, n);
  }


  struct Carm_Ptr carm_ptr;
  uint64_t len = 0;
  unsigned int checked_bytes = 0;
  unsigned int chunk_size = 0;
  unsigned char * dest  = nullptr;
  uint64_t mut = 0;

  get_carm_ptr(&carm_ptr, (uint64_t)str + checked_bytes, mut);

  while (len < n) {
    dest = (unsigned char *)_ptrDeref((void *)((uint64_t)str + checked_bytes), &carm_ptr);
    chunk_size = OBJ_SIZE - carm_ptr.offset;
    for (unsigned int j = 0; j < chunk_size; j++) {
      if (dest[j] == c) {
        return (void *)((uint64_t)str + len);
      }
      len++;
    }
    checked_bytes += chunk_size;
    carm_ptr.obj_index++;
    carm_ptr.offset = 0;
  }


  return NULL; 
}

void * carm_wmemchr(int32_t * s1, int c, uint64_t size)
{
  if (!check_carm_object((uint64_t)s1)) {
    return memchr(s1, c, size);
  }
  DEBUG_PRINT("carm_wmemchr NOT IMPLEMENTED");
  exit(0);
  return 0;
}

int _carm_strcmp_dst_carm_ptr(void * dst, void * src) {
	int n = strlen((char *)src) + 1;
	return carm_memcmp(dst, src, n);
}

int _carm_strcmp_src_carm_ptr(void * dst, void * src) {
	int n = carm_strlen(src) + 1;
	return carm_memcmp(dst, src, n);
}
int carm_strcmp(void * d, void * s)
{
  DEBUG_PRINT("Carm strcmp dst %lx src %lx\n", d, s);
  if (!check_carm_object((uint64_t)d) && !check_carm_object((uint64_t)s)) {
	  return strcmp((char *)d, (char *) s);
  }
  else if (check_carm_object((uint64_t)d) && !check_carm_object((uint64_t)s)) {
	return _carm_strcmp_dst_carm_ptr(d, s);

  }
  else if (!check_carm_object((uint64_t)d) && check_carm_object((uint64_t)s)) {
	return _carm_strcmp_src_carm_ptr(d, s);

  }
  int n = carm_strlen(s) + 1;
  return carm_memcmp(d, s, n);
}
int carm_vfprintf(void *str, void * format, va_list cva_list)
{
  if (!check_carm_object((uint64_t)str)) 
    return vfprintf((FILE *)str, (const char *)format, cva_list);
  DEBUG_PRINT("carm_vfprintf NOT IMPLEMENTED");
  exit(0);
  return 0;
}
/*
 * d or i	Signed decimal integer	392
 u	Unsigned decimal integer	7235
 o	Unsigned octal	610
 x	Unsigned hexadecimal integer	7fa
 X	Unsigned hexadecimal integer (uppercase)	7FA
 f	Decimal floating point, lowercase	392.65
 F	Decimal floating point, uppercase	392.65
 e	Scientific notation (mantissa/exponent), lowercase	3.9265e+2
 E	Scientific notation (mantissa/exponent), uppercase	3.9265E+2
 g	Use the shortest representation: %e or %f	392.65
 G	Use the shortest representation: %E or %F	392.65
 a	Hexadecimal floating point, lowercase	-0xc.90fep-2
 A	Hexadecimal floating point, uppercase	-0XC.90FEP-2
 c	Character	a
 s	String of characters	sample
 p	Pointer address	b8000000
 n	Nothing printed.
 %	A % followed by another % character will write a single % to the stream.	%
 */

enum fcodes {Cd, Cu, Co, Cx, CX, Cf, CF, Ce, CE, Cg, CG, Ca, CA, Cc, Cs, Cp, Cn, Cps, Cl};

const string fp = "duoxXfFeEgGaAcspn%l";

static int inline getType(char a) {
  return fp.find(a); 
}

struct Numeric
{
  int type;
  int i;
  double f;
  unsigned int ui;
  uint64_t p;
  long l;
};


static inline void get_va_args_value(Numeric * ns, int i, va_list args1, bool * vargs_is_cptr) {

  switch (i) {
    case Cd:
      ns->i = va_arg (args1, int);
      ns->type = Cd;
      break;
    case Cu: 
      ns->ui = va_arg (args1, unsigned int);
      ns->type = Cu;
      break;
    case Co: 
      ns->ui = va_arg (args1, unsigned int);
      ns->type = Cu;
      break;
    case Cx: 
      ns->ui = va_arg (args1, unsigned int);
      ns->type = Cu;
      break;
    case CX: 
      ns->ui = va_arg (args1, unsigned int);
      ns->type = Cu;
      break;
    case Cf: 
      ns->f = va_arg (args1, double);  //float - double promotion
      ns->type = Cf;
      break;
    case CF: 
      ns->i = va_arg (args1, int);
      ns->type = Cd;
      break;
    case Ce: 
      ns->i = va_arg (args1, int);
      ns->type = Cd;
      break;
    case CE: 
      ns->i = va_arg (args1, int);
      ns->type = Cd;
      break;
    case Cg: 
      ns->i = va_arg (args1, int);
      ns->type = Cd;
      break;
    case CG: 
      ns->i = va_arg (args1, int);
      ns->type = Cd;
      break;
    case Ca: 
      ns->f = va_arg (args1, double);
      ns->type = Cf;
      break;
    case CA: 
      ns->f = va_arg (args1, double);
      ns->type = Cf;
      break;
    case Cc:
      ns->i = va_arg (args1, int); //minnimum size int
      ns->type = Cd;
      break;
    case Cs: 
      ns->p = (uint64_t) va_arg (args1, char*); 
      ns->type = Cp;
      if (check_carm_object(ns->p)) 
	      *vargs_is_cptr = true;
      DEBUG_PRINT("###########################\n"); 
      DEBUG_PRINT("get arg value %lx \n", ns->p); 
      DEBUG_PRINT("###########################\n"); 
      break;
    case Cp: 
      ns->p = (uint64_t)va_arg (args1, void*); 
      ns->type = Cp;
      if (check_carm_object(ns->p)) 
	      *vargs_is_cptr = true;
      break;
    case Cl: 
      ns->l = (uint64_t)va_arg (args1, long); 
      ns->type = Cl;
      break;
  };

}

static inline int parse_va_arg_types(int * vargs_type, int size, char * fmt) {

  int nargs = 0;
  int type = 0;
  for (int i = 0; i < size; i++) {
    if ((i > 0) && (fmt[i-1] == '%')) {
      type = getType(fmt[i]);
      DEBUG_PRINT("parse_va_arg_types %d\n", type);
      if ((type != Cps) && (type != Cn) && (type != -1))
      {
        vargs_type[nargs] = type;
        nargs++;
      }
    }
  }
  return nargs;

}

#define MAX_VA_ARGS 32
#define MAX_VA_STR_SIZE 4096 
Numeric ns_array[MAX_VA_ARGS];

#define PARSE_VA_LIST(arg) \
  nargs = parse_va_arg_types(vargs_type, strlen(fmt), fmt);\
  for (int i = 0; i < nargs ; i++) { \
    Numeric * ns = (struct Numeric *)&ns_array[i];\
    get_va_args_value(ns, vargs_type[i], arg, &vargs_is_cptr);\
    vargs_values[i] = ns;\
  }

static inline int __vsn_printf(void * str, char * format, Numeric ** vargs_values) {


	int size = 0;
	char * temp = (char *)malloc(MAX_VA_ARGS * MAX_VA_STR_SIZE);
	char * ctemp = temp;
	memset(temp, 0, MAX_VA_ARGS * MAX_VA_STR_SIZE);
	int size_varg;
	char numchar[100];

	int i = 0;
	while (*format != '\0') {
		if (*format == '%') {
			format++;
			size++;
			uint64_t ptr;
			switch (*format) {
				case 's': 
					size_varg = carm_strlen((void *)vargs_values[i]->p) + 1;
					carm_memcpy((int8_t *)temp, (int8_t *)vargs_values[i]->p, size_varg);
					temp += size_varg;
					size += size_varg;
					break;
				case 'd': 
					sprintf(numchar, "%d", vargs_values[i]->i);
					carm_memcpy((int8_t *)temp, (int8_t *)numchar, strlen(numchar));
					temp += strlen(numchar);

					size += strlen(numchar);
					break;
				case 'u': 
					sprintf(numchar, "%u", vargs_values[i]->ui);
					carm_memcpy((int8_t *)temp, (int8_t *)numchar, strlen(numchar));
					temp += strlen(numchar);
					size += strlen(numchar);
					break;
				case 'f': 
					sprintf(numchar, "%f", vargs_values[i]->f);
					carm_memcpy((int8_t *)temp, (int8_t *)numchar, strlen(numchar));
					temp += strlen(numchar);
					size += strlen(numchar);
					break;
				default:
					DEBUG_PRINT("not supported %c in %s\n", format, *format);
					exit(0);
					break;
			};
			i++;
		}
		else {
			*temp = *format;
			temp++;
			size++;
		}
		format++;

	}
	carm_memcpy((int8_t *)str, (int8_t  *)ctemp, size);  
	free(ctemp);
	return size;
}


//use vsnprintf using snprint with va_list leads to bugs
int carm_vsnprintf(void *str, uint64_t size, void * format, va_list cva_list)
{
  DEBUG_PRINT("carm_vsnprintf %lx\n", str);
  bool vargs_is_cptr = false; 
  int nargs = 0;
  int ret = 0;;
  int vargs_type[MAX_VA_ARGS];
  Numeric * vargs_values[MAX_VA_ARGS];
  char * fmt = (char * ) format;
  va_list args1;

  va_copy(args1, cva_list);
  PARSE_VA_LIST(args1)
  va_end (args1);

  char tmp[MAX_VA_STR_SIZE];


  if (!vargs_is_cptr) {
    if (!check_carm_object((uint64_t)str)) {
      ret = vsnprintf((char *)str, size, fmt, cva_list);
      DEBUG_PRINT("carm_snprintf src %s\n", (char *)str);
      DEBUG_PRINT("carm_snprintf format %s\n", (char *)format);
      return ret;
    }
    ret = vsnprintf(tmp, size, fmt, cva_list);
    carm_memcpy((int8_t *)str, (int8_t *)tmp, size);
    DEBUG_PRINT("carm_snprintf carmptr format %s\n", (char *)format);
    return ret;
  }
  else {

      return __vsn_printf(str, (char *)format, vargs_values);
  }

  return ret;
}

static inline int get_size_of_valist(va_list cva_list, char * format) {

  bool vargs_is_cptr = false; 
  int nargs = 0;
  int size = 0;
  int size_varg;
  char numchar[100];
  int i = 0;
  int vargs_type[MAX_VA_ARGS];
  Numeric * vargs_values[MAX_VA_ARGS];
  va_list args1;
  char * fmt = (char * ) format;

  va_copy(args1, cva_list);
  PARSE_VA_LIST(args1)
  va_end (args1);

	while (*format != '\0') {
		if (*format == '%') {
			format++;
			switch (*format) {
				case 's': 
					size_varg = carm_strlen((void *)vargs_values[i]->p) + 1;
					size += size_varg;
					break;
				case 'd': 
					sprintf(numchar, "%d", vargs_values[i]->i);
					size += strlen(numchar);
					break;
				case 'u': 
					sprintf(numchar, "%u", vargs_values[i]->ui);
					size += strlen(numchar);
					break;
				case 'f': 
					sprintf(numchar, "%f", vargs_values[i]->f);
					size += strlen(numchar);
					break;
				default:
					size++;
					DEBUG_PRINT("not supported %c in %s\n", format, *format);
					exit(0);
					break;
			};
			i++;
		}
		else {
			size++;
		}
		format++;
	}
	return size;
}


int get_size_va_list (va_list vlen, void * format) {
  va_list args;
  int size = 0;
  va_copy(args, vlen);
  size += get_size_of_valist(args, (char *)format);
  va_end(args);
  return size;
}

//use vsnprintf using snprint with va_list leads to bugs
int carm_vsprintf(void *str, void * format, va_list cva_list)
{
  DEBUG_PRINT("carm_vsprintf %lx\n", str);
  int ret = 0;
  va_list vlen, vargs;
  va_copy(vlen,cva_list);
  uint64_t size = get_size_va_list(vlen, format);
  va_end(vlen);

  va_copy(vargs,cva_list);
  ret = carm_vsnprintf(str, size, format, vargs);
  va_end(vargs);

  return ret;
}
//use vsnprintf using snprint with va_list leads to bugs
int carm_snprintf(void *str, uint64_t size, void * format, ...)
{
  DEBUG_PRINT("carm_snprintf ");
  int ret = 0;
  va_list vargs;

  va_start(vargs, format);
  ret = carm_vsnprintf(str, size, format, vargs);
  va_end(vargs);

  return ret;
}
//use vsnprintf using snprint with va_list leads to bugs redirect sprintf to vsprintf noth functions are same vsprintf just use va arg
int carm_sprintf(void *str,  void * format,...)
{
  DEBUG_PRINT("carm_sprintf ");
  va_list vlen, vargs;

  int ret = 0;

  va_start(vlen, format);
  uint64_t size = get_size_va_list(vlen, format);
  va_end(vlen);

  va_start(vargs, format);
  ret = carm_vsnprintf(str, size, format, vargs);
  va_end(vargs);
  return ret;

}
int carm_sscanf(void *str,  void * format,...)
{
  DEBUG_PRINT("carm_sscanf ");
  va_list args, args1;
  int n = 0;
  int nargs = 0;
  int vargs_type[MAX_VA_ARGS];
  Numeric * vargs_values[MAX_VA_ARGS];

  char * fmt = (char * ) format;
  bool vargs_is_cptr = false; 

  va_start(args1, (const char *)format);
  PARSE_VA_LIST(args1)
  va_end (args1);

  if (vargs_is_cptr) {
    DEBUG_PRINT("carm_sscanf vargs with carmptr not supported\n");;
    exit(0);
  }

  //hack = https://stackoverflow.com/questions/52350241/determining-size-of-vsprintf-output +1 includes terminating char 
  if (!check_carm_object((uint64_t)str)) {
    va_start(args, format);
    n = vsscanf((char *)str, fmt, args);
    va_end(args);
    return n;
  }

  DEBUG_PRINT("carm_sscanf carm ptr NOT IMPLEMENTED");
  exit(0);
  return 0;
}
void * carm_fopen(void * path,  void * mode)
{
  DEBUG_PRINT("carm_fopen ");

  if (!check_carm_object((uint64_t)path)) {
    return (void *)fopen((char *)path, (const char *)mode);
  }


  int size = carm_strlen(path) + 1;
  char * rptr = (char *)alloca(size); 
  carm_memcpy((int8_t*)rptr, (int8_t*)path, size);
  printf(" \nPath %s\n",rptr);

  return (void *)fopen(rptr, (const char *)mode);
}
int carm_fclose(void * stream)
{
  DEBUG_PRINT("carm_fclose ");

  if (!check_carm_object((uint64_t)stream)) {
    return fclose((FILE *)stream);
  }

  DEBUG_PRINT("carm_fclose carm ptr NOT IMPLEMENTED");
  exit(0);
  return 0;
}
/*
 *
 */
int64_t carm_fwrite(void * ptr, uint64_t size, uint64_t nmemb, void * fptr)
{
  //   DEBUG_PRINT("carm_fwrite ptr %lx, fptr %lx\n", ptr, fptr);
  if (!check_carm_object((uint64_t)ptr)) {
    return fwrite(ptr, size, nmemb, (FILE *)fptr);
  }

  //get number of elements in single object
  uint64_t em_obj           =   OBJ_SIZE/size;
  if ((OBJ_SIZE%size) != 0) {
    DEBUG_PRINT("carm_fwrite em_obj not entirely  divisble\n");
    exit(0);
  }
  if (size > OBJ_SIZE) {
    DEBUG_PRINT("carm_fread size > OBJ_SIZE not supported\n");
    exit(0);
  }
  uint64_t nmem_obj         =   nmemb / em_obj;
  uint64_t nmem_obj_rem     =   nmemb % em_obj;
  uint64_t val =   0;
  char  tmpbuf[OBJ_SIZE];
  int index_offset = 0;
  for (uint64_t i = 0; i < nmem_obj; i++) {
    memset(tmpbuf, 0, OBJ_SIZE);
    carm_memcpy((int8_t *)tmpbuf, (int8_t *)((uint64_t)ptr + (index_offset * OBJ_SIZE)), OBJ_SIZE);
    val += fwrite((void *)tmpbuf, size, em_obj, (FILE *)fptr);
    index_offset++;
  }
  if (nmem_obj_rem > 0) {
    memset(tmpbuf, 0, OBJ_SIZE);
    carm_memcpy((int8_t *)tmpbuf, (int8_t *)((uint64_t)ptr + (index_offset * OBJ_SIZE)), nmem_obj_rem * size);
    val += fwrite((void *)tmpbuf, size, nmem_obj_rem, (FILE *)fptr);
  }
  return val;

}

int64_t carm_fread(void * ptr, uint64_t size, uint64_t nmemb, void * fptr)
{
  //   DEBUG_PRINT("carm_fread ptr %lx, fptr %lx size %lx nmemb %lx\n", ptr, fptr, size, nmemb);
  if (!check_carm_object((uint64_t)ptr)) {
    return fread(ptr, size, nmemb, (FILE *)fptr);
  }
  //get number of elements in single object
  uint64_t em_obj           =   OBJ_SIZE/size;
  if ((OBJ_SIZE%size) != 0) {
    DEBUG_PRINT("carm_fread em_obj not entirely  divisble\n");
    exit(0);
  }
  if (size > OBJ_SIZE) {
    DEBUG_PRINT("carm_fread size > OBJ_SIZE not supported\n");
    exit(0);
  }
  uint64_t nmem_obj         =   nmemb / em_obj;
  uint64_t nmem_obj_rem     =   nmemb % em_obj;
  uint64_t val =   0;

  char  tmpbuf[OBJ_SIZE];
  int index_offset = 0;
  for (uint64_t i = 0; i < nmem_obj; i++) {
    memset(tmpbuf, 0, OBJ_SIZE);
    val += fread((void *)tmpbuf, size, em_obj, (FILE *)fptr);
    carm_memcpy((int8_t *)((uint64_t)ptr + (index_offset * OBJ_SIZE)), (int8_t *)tmpbuf, OBJ_SIZE);
    index_offset++;
  }
  if (nmem_obj_rem > 0) {
    memset(tmpbuf, 0, OBJ_SIZE);
    val += fread((void *)tmpbuf, size, nmem_obj_rem, (FILE *)fptr);
    carm_memcpy((int8_t *)((uint64_t)ptr + (index_offset * OBJ_SIZE)), (int8_t *)tmpbuf, nmem_obj_rem * size);
  }
  return val;

}

int64_t carm_read(int32_t fd, void * ptr, int64_t size)
{
	//   DEBUG_PRINT("carm_fread ptr %lx, fptr %lx size %lx nmemb %lx\n", ptr, fptr, size, nmemb);
	if (!check_carm_object((uint64_t)ptr)) {
		return read(fd, ptr, size);
	}

        char * data_recv_buf = (char *)malloc(size);
        memset(data_recv_buf, 0, size);
	int val = read(fd, data_recv_buf, size);
	if (val > 0)
	carm_memcpy((int8_t *)((uint64_t)ptr), (int8_t *)data_recv_buf, val);

	free(data_recv_buf);
	return val;

}

char * carm_strdup(char * v) {

  if (!check_carm_object((uint64_t)v)) {
    return strdup(v);
  }

  int size = carm_strlen(v) + 1;
  void * rptr = carm_remote_malloc(size); 
  carm_memcpy((int8_t*)rptr, (int8_t*)v, size);
  return (char *)rptr; 
}
//use vsnprintf using snprint with va_list leads to bugs redirect sprintf to vsprintf noth functions are same vsprintf just use va arg
int carm_fprintf(void * fptr,  void * format,...)
{

	DEBUG_PRINT("carm_fprintf ");
	va_list vlen, vargs;

	int ret = 0;

	va_start(vlen, format);
	uint64_t size = get_size_va_list(vlen, format);
	va_end(vlen);

	char * tmp = (char *)malloc(size );
	va_start(vargs, format);
	ret = carm_vsnprintf(tmp, size, format, vargs);
	va_end(vargs);

	carm_fwrite(tmp, 1, size, fptr);

	DEBUG_PRINT("carm_fprintf complete");
	free(tmp);
	return ret;

}

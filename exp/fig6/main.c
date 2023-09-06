
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define TIMER_START {\
  asm volatile("xorl %%eax, %%eax\n\t"\
               "CPUID\n\t"\
               "RDTSC\n\t"\
               "mov %%edx, %0\n\t"\
               "mov %%eax, %1\n\t"\
               : "=r"(cycles_high_start), "=r"(cycles_low_start)::"%rax",\
                 "%rbx", "%rcx", "%rdx");\
}

#define TIMER_END {\
  asm volatile("RDTSCP\n\t"\
               "mov %%edx, %0\n\t"\
               "mov %%eax, %1\n\t"\
               "xorl %%eax, %%eax\n\t"\
               "CPUID\n\t"\
               : "=r"(cycles_high_end), "=r"(cycles_low_end)::"%rax", "%rbx",\
                 "%rcx", "%rdx");\
}
uint64_t get_elapsed_cycles(uint32_t cycles_high_start,
                            uint32_t cycles_low_start,
                            uint32_t cycles_high_end,
                            uint32_t cycles_low_end) {
  uint64_t start, end;
  start = (((uint64_t)cycles_high_start << 32) | cycles_low_start);
  end =   (((uint64_t)cycles_high_end << 32) | cycles_low_end);
  return end - start;
}

const uint64_t kWorkingSetSize = (256ULL << 20);
uint64_t kNumEntries = kWorkingSetSize/2;


void prepare_data(uint8_t * arr, uint64_t val) {

	for (uint64_t i = 0; i < kNumEntries; i++) {
		arr[i] = val;
	}
}
void do_warm_up(uint8_t * arr1, int iterations) {
  uint64_t sum = 0;
	for (uint64_t i = 0; i < iterations; i++) {
									sum += arr1[i];
	}
  printf(" Sum : %d\n");
}

void do_single_pointer_access(uint8_t * arr, int Nb) {
  uint64_t sum = 0;
  uint32_t cycles_high_start, cycles_low_start;
  uint32_t cycles_high_end, cycles_low_end;
	uint32_t start, end;
  uint64_t OBJ_SIZE = 32768;
  uint64_t E = OBJ_SIZE;
	for (uint64_t e = 1; e <= E; e++) {
  int iters = OBJ_SIZE/e;
  TIMER_START
	for (uint64_t i = 0; i < iters; i++) {
									sum += arr[i];
	}
  TIMER_END
	uint64_t cycles = get_elapsed_cycles(cycles_high_start, cycles_low_start,  cycles_high_end, cycles_low_end);
  printf("Iterations : %d : Memaccess : %d : e : %d :Cycles : %d  \n", iters, sum, e, cycles); 
 // prepare_data(arr, 1);
  cycles_high_start = 0;
  cycles_low_start = 0;
  cycles_high_end = 0;
  cycles_low_end = 0;
	}
}

void _Z7do_workPv(void *arg) {

  uint16_t * arr1 = (uint16_t *)malloc(kWorkingSetSize);

  prepare_data(arr1, 1);


  //uint64_t kIterations = kWorkingSetSize/sizeof(long); 
  uint64_t Nb = 1; 

  //do_warm_up(arr1, kWorkingSetSize/sizeof(long));

	for (uint64_t i = 1; i <= Nb; i++) {
									do_single_pointer_access(arr1, i);
	}

}

int __attribute((optnone)) main() {
  _Z7do_workPv(NULL);
	return 0;
}

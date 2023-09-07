
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <string.h>

using namespace std;

constexpr uint64_t kWorkingSetSize = 12ULL << 30;
constexpr uint64_t kNumEntries = kWorkingSetSize;


void prepare_data(uint8_t * arr, uint64_t val) {

	for (uint64_t i = 0; i < kNumEntries; i++) {
		arr[i] = val;
	}
}

void do_sum(uint8_t * arr) {
  uint64_t sum = 0;
  auto start_ts = chrono::steady_clock::now();
  for (uint64_t i = 0; i < kNumEntries; i++) {
    sum += arr[i];
  }
  auto end_ts = chrono::steady_clock::now();
  auto time_US =
      chrono::duration_cast<chrono::microseconds>(end_ts - start_ts).count();
  std::cout << "Sum : "<<sum<<" : Time :" << time_US << std::endl;
}

int main(int argc, char ** argv) {

  uint8_t * arr1 = (uint8_t *)malloc(kWorkingSetSize);

  prepare_data(arr1, 1);

  do_sum(arr1);

}

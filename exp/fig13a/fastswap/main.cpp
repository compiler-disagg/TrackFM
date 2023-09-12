#include <unordered_map>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>


using namespace std;

#define SIZE 50000000

void set_values(unordered_map<int, int> & umap) {
	for (int i = 0; i < SIZE; i++) {
			umap[i] = i;
	}
}

void load_keys(int * zipf ) {
        int i = 0;
        fstream myfile("zipf.txt", std::ios_base::in);

				int a;
				while (myfile >> a)
				{
								zipf[i++] = a;
				}
}

int main(int argc, char ** argv) {

  unordered_map<int, int> umap;
  int * zipf_keys = (int *)malloc((SIZE + 1) * sizeof(int));
 
  set_values(umap);

  load_keys(zipf_keys);
  cout<< "zipf key distribution loaded \n";

  auto start_ts = chrono::steady_clock::now();
  for (int i = 0; i < SIZE; i++) {
	  int key = zipf_keys[i];
	  int value = umap[key];
	  if (value != key) {
		  printf("Expected %d,  Found %d\n", key, value);
	  }
  }
  auto end_ts = chrono::steady_clock::now();

  auto time_US =
      chrono::duration_cast<chrono::microseconds>(end_ts - start_ts).count();
  std::cout <<"Time :"<< time_US << std::endl;
}

CARM_LLVM=/home/llvm-project/build/bin
CARMCXXFLAGS=-O3

all: main_lib

main_lib:
	$(CARM_LLVM)/clang++ $(CARMCXXFLAGS) main.cpp -o main

.PHONY: clean
clean:
	rm -f *.bc *.ll *.so *.o main


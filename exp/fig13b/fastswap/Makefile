CARM_LLVM:=/home/llvm-project/build/bin
override CARMCXXFLAGS += -O3 

all: main

main_lib:
	$(CARM_LLVM)/clang++ $(CARMCXXFLAGS) main.cpp

.PHONY: clean
clean:
	rm -f *.bc *.ll *.so *.o main


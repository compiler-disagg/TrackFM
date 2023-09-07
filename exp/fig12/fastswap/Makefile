CARM_LLVM:=/home/llvm-project/build/bin
CARM_LDX_FLAGS=-filetype=obj -relocation-model=pic
CARM_CC= $(CARM_LLVM)/clang++
CARM_C= $(CARM_LLVM)/clang

main_src = main.cpp
main_obj = $(main_src:.cpp=.o)

src = $(main_src) 
obj = $(src:.cpp=.o)
dep = $(obj:.o=.d)

CXXFLAGS += -O3 

#must be first
all: main_lib	 

main_lib:
	$(CARM_CC) $(CXXFLAGS) main.cpp -o main

.PHONY: clean
clean:
	rm -f *.bc *.ll *.so *.o $(dep) main

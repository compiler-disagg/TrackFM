CARM_LLVM:=/home/llvm-project/build/bin
AIFM_PATH:=../../runtime/AIFM/aifm
CARM_PATH:=../../runtime
CARM_LDX_FLAGS=-filetype=obj -relocation-model=pic
CARM_CC= $(CARM_LLVM)/clang++
CARM_C= $(CARM_LLVM)/clang
SHENANGO_PATH=$(AIFM_PATH)/../shenango
include $(SHENANGO_PATH)/shared.mk

librt_libs = $(SHENANGO_PATH)/bindings/cc/librt++.a
INC += -I$(SHENANGO_PATH)/bindings/cc -I$(AIFM_PATH)/inc -I$(CARM_PATH)/inc  -I$(SHENANGO_PATH)/ksched

main_src = main.cpp
main_obj = $(main_src:.cpp=.o)

lib_src = $(wildcard $(CARM_PATH)/src/*.cpp $(AIFM_PATH)/src/*.cpp)
lib_src := $(filter-out $(AIFM_PATH)/src/tcp_device_server.cpp,$(lib_src))
lib_obj = $(lib_src:.cpp=.o)

src = $(main_src) $(lib_src) 
obj = $(src:.cpp=.o)
dep = $(obj:.o=.d)

CXXFLAGS := $(filter-out -std=gnu++17,$(CXXFLAGS))
override CXXFLAGS += -std=gnu++2a -fconcepts -Wno-unused-function -mcmodel=medium -g

#override CARMCXXFLAGS += -O1 -mllvm -disable-llvm-optzns -disable-llvm-passes -march=native -fPIC -fno-builtin -fno-vectorize
override CARMCXXFLAGS += -O1 -march=native -fPIC -fno-builtin -fno-vectorize -fno-exceptions 

#must be first
all: main

main_lib:
	$(CARM_LLVM)/clang++ $(CARMCXXFLAGS)  -I$(CARM_PATH)/inc/ -c  main.cpp  -nostdinc++ -nostdlib++ -isystem /proj/rmt-PG0/noelle_setup/llvm-project/build/include/c++/v1 -emit-llvm
	#opt -O1 -inline -inline-threshold=10000 main.bc -o mainin.bc
	noelle-load -load ~/CAT/lib/TRACKFMINIT.so  -CAT main.bc -o main_init.bc
	noelle-norm main_init.bc -o main_norm.bc 
	noelle-load -load ~/CAT/lib/TRACKFMCHECKS.so -CAT main_norm.bc -o main_check.bc 
	noelle-norm main_check.bc -o main_check_norm.bc 
	noelle-load -load ~/CAT/lib/TRACKFMTRANSFORM.so -CAT main_check_norm.bc -o main_transform.bc 
	noelle-norm main_transform.bc -o main_transform_norm.bc 
	noelle-load -load ~/CAT/lib/TRACKFMLIBCTRANSFORM.so -CAT main_transform_norm.bc -o main_libctransform.bc 
	noelle-norm main_libctransform.bc -o main_libctransform_norm.bc 
	$(CARM_LLVM)/opt -O3 -disable-simplify-libcalls main_libctransform_norm.bc -o opt.bc
	$(CARM_LLVM)/llc $(CARM_LDX_FLAGS) opt.bc -o maint.o

main:  $(librt_libs) $(RUNTIME_DEPS)  $(lib_obj) main_lib
	objcopy --prefix-symbols=carm maint.o
	./symbol_redefine.sh
	$(CARM_LLVM)/clang++ -g -shared -o libcarmapp.so maint.o  libc++.so.1.0.o  -nostdlib++ 
	$(LDXX) -o $@ $(LDFLAGS)  $(lib_obj) $(librt_libs) $(RUNTIME_LIBS)  -L. -lcarmapp   

ifneq ($(MAKECMDGOALS),clean)
-include $(dep)   # include all dep files in the makefile
endif

#rule to generate a dep file by using the C preprocessor
#(see man cpp for details on the - MM and - MT options)
%.d: %.cpp
	@$(CXX) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.bc *.ll *.so *.o $(dep) main $(AIFM_PATH)/src/*.o $(CARM_PATH)/src/*.o


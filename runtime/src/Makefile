AIFM_PATH=../AIFM/aifm/
CARM_PATH=../
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
override CXXFLAGS += -std=gnu++2a -fconcepts -Wno-unused-function -mcmodel=medium

#must be first
all: main

main: $(librt_libs) $(RUNTIME_DEPS) $(lib_obj)
	ar rcs libcarm.a $(lib_obj) $(librt_libs) 

ifneq ($(MAKECMDGOALS),clean)
-include $(dep)   # include all dep files in the makefile
endif

#rule to generate a dep file by using the C preprocessor
#(see man cpp for details on the - MM and - MT options)
%.d: %.cpp
	@$(CXX) $(CXXFLAGS) -fPIC  $< -MM -MT $(@:.d=.o) >$@
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o $(dep) main $(AIFM_PATH)/src/*.o $(CARM_PATH)/src/*.o


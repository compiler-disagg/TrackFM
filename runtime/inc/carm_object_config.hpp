#pragma once
#include <iostream>
#define  OBJ_SIZE  4096
#define TOTAL_OBJECTS 41943040
#define  OBJ_SIZE_MASK  (OBJ_SIZE - 1)
#define  OBJ_SIZE_LOG  log2(OBJ_SIZE)
#define  CARM_PTR  (1ULL<<60)
#define CACHE_LINE_SIZE 64
#define INDEX_INVALID 0xffffffffffffffff
#define OBJECTHEADERSIZE (10)
//TODO automate this based on AIFM  24 includes object header size + DS size and 8 byte alignment
#define OBJECTHEADERSIZE (10)
//Check AIFM object size alignment
#define OBJECTALIGN (14)
#define TOBJECTSIZE (OBJ_SIZE + OBJECTHEADERSIZE + OBJECTALIGN)
#define OBJ_STATE 0
#define CARM_PREV_OBJ_INDEX 1
#define CARM_OBJ_DATA_ADDR 2
#define CARM_PREV_OBJ_MUT_INDEX 3
#define CARM_OBJ_MUT_DATA_ADDR 4
#define LOOP_ID_POS 50 
#define LOOP_OBJ_MASK  ((1ULL<<50) - 1)
//#define TRACKFM_TRACE 1
#define LOOP_PREFETCH 1
#define PREFETCH 1
constexpr static uint32_t kObjectDataAddrPos = 2;
constexpr static uint32_t kObjectDataAddrSize = 6;
constexpr static uint32_t kDirtyClear = 0x400U;
constexpr static uint32_t kPresentClear = 0x100U;
constexpr static uint32_t kHotClear = 0x80U;
constexpr static uint32_t kEvacuationSet = 0x10000U;
constexpr static uint32_t kObjIDLenPosShift = 9;
constexpr static uint32_t kObjectDataAddrBitPos = 17;
constexpr static uint32_t kObjectSizeBitPos = 10;
const std::string LOCAL_MALLOC = "local_malloc";

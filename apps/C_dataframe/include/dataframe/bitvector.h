#ifndef GUARD_A3282442_D74C_4432_861A_884A67C58835
#define GUARD_A3282442_D74C_4432_861A_884A67C58835

#include "dataframe/config.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    uint8_t* data;
    size_t size;
    size_t byte_size;
} DataFrame_BitVector;

DATAFRAME_EXPORT void
DataFrame_BitVector_Init(DataFrame_BitVector* v);
DATAFRAME_EXPORT const char*
DataFrame_BitVector_Add(DataFrame_BitVector* v, bool value);
DATAFRAME_EXPORT void
DataFrame_BitVector_Remove(DataFrame_BitVector* v, size_t index);
DATAFRAME_EXPORT bool
DataFrame_BitVector_Get(DataFrame_BitVector* v, size_t index);
DATAFRAME_EXPORT void
DataFrame_BitVector_Set(DataFrame_BitVector* v, size_t index, bool value);
DATAFRAME_EXPORT void
DataFrame_BitVector_Clear(DataFrame_BitVector* v);
DATAFRAME_EXPORT void
DataFrame_BitVector_Destroy(DataFrame_BitVector* v);

#ifdef __cplusplus
}
#endif


#endif

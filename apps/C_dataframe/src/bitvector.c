#include "dataframe/bitvector.h"
#include <memory.h>

void DataFrame_BitVector_Init(DataFrame_BitVector* v)
{
    memset(v, 0, sizeof(DataFrame_BitVector));
}

const char* DataFrame_BitVector_Add(
    DataFrame_BitVector* v, bool value)
{
    size_t byte = v->size / 8;
    size_t bit = v->size & 7;
    uint8_t* d;
    size_t newSize;

    if(v->byte_size == byte)
    {
        newSize = v->byte_size * 2;
        if(!newSize)
            newSize = 1;
        d = realloc(v->data, newSize);
        if(!d) return "DataFrame_BitVector_Add: Out of memory";
        v->data = d;
        v->byte_size = newSize;
    }

    if(value)
        v->data[byte] |= 1 << bit;
    else
        v->data[byte] &= ~(1 << bit);
    v->size++;
    return NULL;
}
void DataFrame_BitVector_Remove(
    DataFrame_BitVector* v, size_t index)
{
    size_t byte = index / 8;
    size_t bit = index & 7;
    size_t i;

    for(i = byte; i < v->byte_size;i++)
    {
        v->data[i] >>= 1;
        if(i < v->byte_size - 1)
            v->data[i] |= v->data[i+1] >> 7;
    }
    v->size--;
}

bool DataFrame_BitVector_Get(DataFrame_BitVector* v, size_t index)
{
    size_t byte = index / 8;
    size_t bit = index & 7;
    if(byte >= v->size)
        return false;
    return v->data[byte] & bit;
}

void DataFrame_BitVector_Set(DataFrame_BitVector* v, size_t index, bool value)
{
    size_t byte = index / 8;
    size_t bit = index & 7;

    if(value)
        v->data[byte] |= 1 << bit;
    else
        v->data[byte] &= ~(1 << bit);
}



void DataFrame_BitVector_Clear(DataFrame_BitVector* v)
{
    v->size = 0;
}

void DataFrame_BitVector_Destroy(DataFrame_BitVector* v)
{
    free(v->data);
    memset(v, 0, sizeof(DataFrame_BitVector));
}

#include "dataframe/config.h"
#include "dataframe/column.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DataFrame_ColumnUInt64 DataFrame_ColumnUInt64;
typedef struct
{
    /* shared */
    DataFrame_Type _typeID;
    void* (*Cast)(DataFrame_ColumnUInt64* self, DataFrame_Type type);
    bool (*IncRef)(DataFrame_ColumnUInt64* self);
    bool (*DecRef)(DataFrame_ColumnUInt64* self);
    size_t (*Size)(DataFrame_ColumnUInt64* self);
    void (*Remove)(DataFrame_ColumnUInt64* self, size_t i);
    void (*Clear)(DataFrame_ColumnUInt64* self);
    char* (*GetName)(DataFrame_ColumnUInt64* self);
    char* (*SetName)(DataFrame_ColumnUInt64* self, char* name);
    bool (*HasValue)(DataFrame_ColumnUInt64* self, size_t index);
    uint8_t* (*GetNAs)(DataFrame_ColumnUInt64* self);


    /* type specific */
    bool (*TryGet)(DataFrame_ColumnUInt64* self, size_t index, uint64_t* v);
    /* returns a reference */
    uint64_t* (*Get)(DataFrame_ColumnUInt64* self, size_t index);

    const char* (*Add)(DataFrame_ColumnUInt64* self, uint64_t v);

    const char* (*AddNA)(DataFrame_ColumnUInt64* self);

    void (*Set)(DataFrame_ColumnUInt64* self, size_t i, uint64_t v);

    void (*SetNA)(DataFrame_ColumnUInt64* self, size_t i);

    uint64_t* (*GetUnique)(DataFrame_ColumnUInt64* self);
   
    int (*GetUniqueSize)(DataFrame_ColumnUInt64* self);
     uint64_t* (*GetAll)(DataFrame_ColumnUInt64* self);
    int (*GetColSize)(DataFrame_ColumnUInt64* self);
    uint64_t* (*GetBySelInt32)(DataFrame_ColumnUInt64* self, int32_t * selection_column, int32_t  selection);
    int (*GetSelSize)(DataFrame_ColumnUInt64* self);
    uint64_t* (*GetBySelString)(DataFrame_ColumnUInt64* self, char ** selection_column, char * selection); 

    uint64_t* (*GetBySelStringUnique)(DataFrame_ColumnUInt64* self, char ** selection_column, char * selection);
    const char*
      (*AddColValues)(DataFrame_ColumnUInt64* self, uint64_t * v, uint64_t size);
    uint64_t (*GetValue)(DataFrame_ColumnUInt64* self, size_t index);
    uint64_t* (*GetBySelInt8)(DataFrame_ColumnUInt64* self, int8_t * selection_column, int8_t  selection);
    uint64_t* (*GetBySelInt8Unique)(DataFrame_ColumnUInt64* self, int8_t * selection_column, int8_t  selection);
    const char* (*AddDateTime)(DataFrame_ColumnUInt64* self, long datetime);
       uint64_t** (*GetBySelInt8Group)(DataFrame_ColumnUInt64* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, uint64_t * copy_data); 
    int64_t * (*GetBySelInt8GroupSize)(DataFrame_ColumnUInt64* self);
    uint64_t* (*GetCopy)(DataFrame_ColumnUInt64* self);
    uint64_t * (*GetBySelGE)(DataFrame_ColumnUInt64* self, uint64_t selection);
} DataFrame_ColumnUInt64Methods;

struct DataFrame_ColumnUInt64
{
    DataFrame_ColumnUInt64Methods* methods;
    int type;
};


DATAFRAME_EXPORT DataFrame_ColumnUInt64* DataFrame_ColumnUInt64_New(int type);


#ifdef __cplusplus
}
#endif
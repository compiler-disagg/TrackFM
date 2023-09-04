#include "dataframe/config.h"
#include "dataframe/column.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DataFrame_ColumnInt64 DataFrame_ColumnInt64;
typedef struct
{
    /* shared */
    DataFrame_Type _typeID;
    void* (*Cast)(DataFrame_ColumnInt64* self, DataFrame_Type type);
    bool (*IncRef)(DataFrame_ColumnInt64* self);
    bool (*DecRef)(DataFrame_ColumnInt64* self);
    size_t (*Size)(DataFrame_ColumnInt64* self);
    void (*Remove)(DataFrame_ColumnInt64* self, size_t i);
    void (*Clear)(DataFrame_ColumnInt64* self);
    char* (*GetName)(DataFrame_ColumnInt64* self);
    char* (*SetName)(DataFrame_ColumnInt64* self, char* name);
    bool (*HasValue)(DataFrame_ColumnInt64* self, size_t index);
    uint8_t* (*GetNAs)(DataFrame_ColumnInt64* self);


    /* type specific */
    bool (*TryGet)(DataFrame_ColumnInt64* self, size_t index, int64_t* v);
    /* returns a reference */
    int64_t* (*Get)(DataFrame_ColumnInt64* self, size_t index);

    const char* (*Add)(DataFrame_ColumnInt64* self, int64_t v);

    const char* (*AddNA)(DataFrame_ColumnInt64* self);

    void (*Set)(DataFrame_ColumnInt64* self, size_t i, int64_t v);

    void (*SetNA)(DataFrame_ColumnInt64* self, size_t i);

    int64_t* (*GetUnique)(DataFrame_ColumnInt64* self);
   
    int (*GetUniqueSize)(DataFrame_ColumnInt64* self);
     int64_t* (*GetAll)(DataFrame_ColumnInt64* self);
    int (*GetColSize)(DataFrame_ColumnInt64* self);
    int64_t* (*GetBySelInt32)(DataFrame_ColumnInt64* self, int32_t * selection_column, int32_t  selection);
    int (*GetSelSize)(DataFrame_ColumnInt64* self);
    int64_t* (*GetBySelString)(DataFrame_ColumnInt64* self, char ** selection_column, char * selection); 

    int64_t* (*GetBySelStringUnique)(DataFrame_ColumnInt64* self, char ** selection_column, char * selection);
    const char*
      (*AddColValues)(DataFrame_ColumnInt64* self, int64_t * v, uint64_t size);
    int64_t (*GetValue)(DataFrame_ColumnInt64* self, size_t index);
    int64_t* (*GetBySelInt8)(DataFrame_ColumnInt64* self, int8_t * selection_column, int8_t  selection);
    int64_t* (*GetBySelInt8Unique)(DataFrame_ColumnInt64* self, int8_t * selection_column, int8_t  selection);
    const char* (*AddDateTime)(DataFrame_ColumnInt64* self, long datetime);
       int64_t** (*GetBySelInt8Group)(DataFrame_ColumnInt64* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, int64_t * copy_data); 
    int64_t * (*GetBySelInt8GroupSize)(DataFrame_ColumnInt64* self);
    int64_t* (*GetCopy)(DataFrame_ColumnInt64* self);
    int64_t * (*GetBySelGE)(DataFrame_ColumnInt64* self, int64_t selection);
} DataFrame_ColumnInt64Methods;

struct DataFrame_ColumnInt64
{
    DataFrame_ColumnInt64Methods* methods;
    int type;
};


DATAFRAME_EXPORT DataFrame_ColumnInt64* DataFrame_ColumnInt64_New(int type);


#ifdef __cplusplus
}
#endif
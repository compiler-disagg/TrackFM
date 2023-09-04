#include "dataframe/config.h"
#include "dataframe/column.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DataFrame_ColumnUInt8 DataFrame_ColumnUInt8;
typedef struct
{
    /* shared */
    DataFrame_Type _typeID;
    void* (*Cast)(DataFrame_ColumnUInt8* self, DataFrame_Type type);
    bool (*IncRef)(DataFrame_ColumnUInt8* self);
    bool (*DecRef)(DataFrame_ColumnUInt8* self);
    size_t (*Size)(DataFrame_ColumnUInt8* self);
    void (*Remove)(DataFrame_ColumnUInt8* self, size_t i);
    void (*Clear)(DataFrame_ColumnUInt8* self);
    char* (*GetName)(DataFrame_ColumnUInt8* self);
    char* (*SetName)(DataFrame_ColumnUInt8* self, char* name);
    bool (*HasValue)(DataFrame_ColumnUInt8* self, size_t index);
    uint8_t* (*GetNAs)(DataFrame_ColumnUInt8* self);


    /* type specific */
    bool (*TryGet)(DataFrame_ColumnUInt8* self, size_t index, uint8_t* v);
    /* returns a reference */
    uint8_t* (*Get)(DataFrame_ColumnUInt8* self, size_t index);

    const char* (*Add)(DataFrame_ColumnUInt8* self, uint8_t v);

    const char* (*AddNA)(DataFrame_ColumnUInt8* self);

    void (*Set)(DataFrame_ColumnUInt8* self, size_t i, uint8_t v);

    void (*SetNA)(DataFrame_ColumnUInt8* self, size_t i);

    uint8_t* (*GetUnique)(DataFrame_ColumnUInt8* self);
   
    int (*GetUniqueSize)(DataFrame_ColumnUInt8* self);
     uint8_t* (*GetAll)(DataFrame_ColumnUInt8* self);
    int (*GetColSize)(DataFrame_ColumnUInt8* self);
    uint8_t* (*GetBySelInt32)(DataFrame_ColumnUInt8* self, int32_t * selection_column, int32_t  selection);
    int (*GetSelSize)(DataFrame_ColumnUInt8* self);
    uint8_t* (*GetBySelString)(DataFrame_ColumnUInt8* self, char ** selection_column, char * selection); 

    uint8_t* (*GetBySelStringUnique)(DataFrame_ColumnUInt8* self, char ** selection_column, char * selection);
    const char*
      (*AddColValues)(DataFrame_ColumnUInt8* self, uint8_t * v, uint64_t size);
    uint8_t (*GetValue)(DataFrame_ColumnUInt8* self, size_t index);
    uint8_t* (*GetBySelInt8)(DataFrame_ColumnUInt8* self, int8_t * selection_column, int8_t  selection);
    uint8_t* (*GetBySelInt8Unique)(DataFrame_ColumnUInt8* self, int8_t * selection_column, int8_t  selection);
    const char* (*AddDateTime)(DataFrame_ColumnUInt8* self, long datetime);
       uint8_t** (*GetBySelInt8Group)(DataFrame_ColumnUInt8* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, uint8_t * copy_data); 
    int64_t * (*GetBySelInt8GroupSize)(DataFrame_ColumnUInt8* self);
    uint8_t* (*GetCopy)(DataFrame_ColumnUInt8* self);
    uint8_t * (*GetBySelGE)(DataFrame_ColumnUInt8* self, uint8_t selection);
} DataFrame_ColumnUInt8Methods;

struct DataFrame_ColumnUInt8
{
    DataFrame_ColumnUInt8Methods* methods;
    int type;
};


DATAFRAME_EXPORT DataFrame_ColumnUInt8* DataFrame_ColumnUInt8_New(int type);


#ifdef __cplusplus
}
#endif
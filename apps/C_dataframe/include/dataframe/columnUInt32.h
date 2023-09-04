#include "dataframe/config.h"
#include "dataframe/column.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DataFrame_ColumnUInt32 DataFrame_ColumnUInt32;
typedef struct
{
    /* shared */
    DataFrame_Type _typeID;
    void* (*Cast)(DataFrame_ColumnUInt32* self, DataFrame_Type type);
    bool (*IncRef)(DataFrame_ColumnUInt32* self);
    bool (*DecRef)(DataFrame_ColumnUInt32* self);
    size_t (*Size)(DataFrame_ColumnUInt32* self);
    void (*Remove)(DataFrame_ColumnUInt32* self, size_t i);
    void (*Clear)(DataFrame_ColumnUInt32* self);
    char* (*GetName)(DataFrame_ColumnUInt32* self);
    char* (*SetName)(DataFrame_ColumnUInt32* self, char* name);
    bool (*HasValue)(DataFrame_ColumnUInt32* self, size_t index);
    uint8_t* (*GetNAs)(DataFrame_ColumnUInt32* self);


    /* type specific */
    bool (*TryGet)(DataFrame_ColumnUInt32* self, size_t index, uint32_t* v);
    /* returns a reference */
    uint32_t* (*Get)(DataFrame_ColumnUInt32* self, size_t index);

    const char* (*Add)(DataFrame_ColumnUInt32* self, uint32_t v);

    const char* (*AddNA)(DataFrame_ColumnUInt32* self);

    void (*Set)(DataFrame_ColumnUInt32* self, size_t i, uint32_t v);

    void (*SetNA)(DataFrame_ColumnUInt32* self, size_t i);

    uint32_t* (*GetUnique)(DataFrame_ColumnUInt32* self);
   
    int (*GetUniqueSize)(DataFrame_ColumnUInt32* self);
     uint32_t* (*GetAll)(DataFrame_ColumnUInt32* self);
    int (*GetColSize)(DataFrame_ColumnUInt32* self);
    uint32_t* (*GetBySelInt32)(DataFrame_ColumnUInt32* self, int32_t * selection_column, int32_t  selection);
    int (*GetSelSize)(DataFrame_ColumnUInt32* self);
    uint32_t* (*GetBySelString)(DataFrame_ColumnUInt32* self, char ** selection_column, char * selection); 

    uint32_t* (*GetBySelStringUnique)(DataFrame_ColumnUInt32* self, char ** selection_column, char * selection);
    const char*
      (*AddColValues)(DataFrame_ColumnUInt32* self, uint32_t * v, uint64_t size);
    uint32_t (*GetValue)(DataFrame_ColumnUInt32* self, size_t index);
    uint32_t* (*GetBySelInt8)(DataFrame_ColumnUInt32* self, int8_t * selection_column, int8_t  selection);
    uint32_t* (*GetBySelInt8Unique)(DataFrame_ColumnUInt32* self, int8_t * selection_column, int8_t  selection);
    const char* (*AddDateTime)(DataFrame_ColumnUInt32* self, long datetime);
       uint32_t** (*GetBySelInt8Group)(DataFrame_ColumnUInt32* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, uint32_t * copy_data); 
    int64_t * (*GetBySelInt8GroupSize)(DataFrame_ColumnUInt32* self);
    uint32_t* (*GetCopy)(DataFrame_ColumnUInt32* self);
    uint32_t * (*GetBySelGE)(DataFrame_ColumnUInt32* self, uint32_t selection);
} DataFrame_ColumnUInt32Methods;

struct DataFrame_ColumnUInt32
{
    DataFrame_ColumnUInt32Methods* methods;
    int type;
};


DATAFRAME_EXPORT DataFrame_ColumnUInt32* DataFrame_ColumnUInt32_New(int type);


#ifdef __cplusplus
}
#endif
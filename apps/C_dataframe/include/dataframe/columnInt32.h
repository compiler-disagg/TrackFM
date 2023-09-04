#include "dataframe/config.h"
#include "dataframe/column.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DataFrame_ColumnInt32 DataFrame_ColumnInt32;
typedef struct
{
    /* shared */
    DataFrame_Type _typeID;
    void* (*Cast)(DataFrame_ColumnInt32* self, DataFrame_Type type);
    bool (*IncRef)(DataFrame_ColumnInt32* self);
    bool (*DecRef)(DataFrame_ColumnInt32* self);
    size_t (*Size)(DataFrame_ColumnInt32* self);
    void (*Remove)(DataFrame_ColumnInt32* self, size_t i);
    void (*Clear)(DataFrame_ColumnInt32* self);
    char* (*GetName)(DataFrame_ColumnInt32* self);
    char* (*SetName)(DataFrame_ColumnInt32* self, char* name);
    bool (*HasValue)(DataFrame_ColumnInt32* self, size_t index);
    uint8_t* (*GetNAs)(DataFrame_ColumnInt32* self);


    /* type specific */
    bool (*TryGet)(DataFrame_ColumnInt32* self, size_t index, int32_t* v);
    /* returns a reference */
    int32_t* (*Get)(DataFrame_ColumnInt32* self, size_t index);

    const char* (*Add)(DataFrame_ColumnInt32* self, int32_t v);

    const char* (*AddNA)(DataFrame_ColumnInt32* self);

    void (*Set)(DataFrame_ColumnInt32* self, size_t i, int32_t v);

    void (*SetNA)(DataFrame_ColumnInt32* self, size_t i);

    int32_t* (*GetUnique)(DataFrame_ColumnInt32* self);
   
    int (*GetUniqueSize)(DataFrame_ColumnInt32* self);
     int32_t* (*GetAll)(DataFrame_ColumnInt32* self);
    int (*GetColSize)(DataFrame_ColumnInt32* self);
    int32_t* (*GetBySelInt32)(DataFrame_ColumnInt32* self, int32_t * selection_column, int32_t  selection);
    int (*GetSelSize)(DataFrame_ColumnInt32* self);
    int32_t* (*GetBySelString)(DataFrame_ColumnInt32* self, char ** selection_column, char * selection); 

    int32_t* (*GetBySelStringUnique)(DataFrame_ColumnInt32* self, char ** selection_column, char * selection);
    const char*
      (*AddColValues)(DataFrame_ColumnInt32* self, int32_t * v, uint64_t size);
    int32_t (*GetValue)(DataFrame_ColumnInt32* self, size_t index);
    int32_t* (*GetBySelInt8)(DataFrame_ColumnInt32* self, int8_t * selection_column, int8_t  selection);
    int32_t* (*GetBySelInt8Unique)(DataFrame_ColumnInt32* self, int8_t * selection_column, int8_t  selection);
    const char* (*AddDateTime)(DataFrame_ColumnInt32* self, long datetime);
       int32_t** (*GetBySelInt8Group)(DataFrame_ColumnInt32* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, int32_t * copy_data); 
    int64_t * (*GetBySelInt8GroupSize)(DataFrame_ColumnInt32* self);
    int32_t* (*GetCopy)(DataFrame_ColumnInt32* self);
    int32_t * (*GetBySelGE)(DataFrame_ColumnInt32* self, int32_t selection);
} DataFrame_ColumnInt32Methods;

struct DataFrame_ColumnInt32
{
    DataFrame_ColumnInt32Methods* methods;
    int type;
};


DATAFRAME_EXPORT DataFrame_ColumnInt32* DataFrame_ColumnInt32_New(int type);


#ifdef __cplusplus
}
#endif
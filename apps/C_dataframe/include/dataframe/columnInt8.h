#include "dataframe/config.h"
#include "dataframe/column.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DataFrame_ColumnInt8 DataFrame_ColumnInt8;
typedef struct
{
    /* shared */
    DataFrame_Type _typeID;
    void* (*Cast)(DataFrame_ColumnInt8* self, DataFrame_Type type);
    bool (*IncRef)(DataFrame_ColumnInt8* self);
    bool (*DecRef)(DataFrame_ColumnInt8* self);
    size_t (*Size)(DataFrame_ColumnInt8* self);
    void (*Remove)(DataFrame_ColumnInt8* self, size_t i);
    void (*Clear)(DataFrame_ColumnInt8* self);
    char* (*GetName)(DataFrame_ColumnInt8* self);
    char* (*SetName)(DataFrame_ColumnInt8* self, char* name);
    bool (*HasValue)(DataFrame_ColumnInt8* self, size_t index);
    uint8_t* (*GetNAs)(DataFrame_ColumnInt8* self);


    /* type specific */
    bool (*TryGet)(DataFrame_ColumnInt8* self, size_t index, int8_t* v);
    /* returns a reference */
    int8_t* (*Get)(DataFrame_ColumnInt8* self, size_t index);

    const char* (*Add)(DataFrame_ColumnInt8* self, int8_t v);

    const char* (*AddNA)(DataFrame_ColumnInt8* self);

    void (*Set)(DataFrame_ColumnInt8* self, size_t i, int8_t v);

    void (*SetNA)(DataFrame_ColumnInt8* self, size_t i);

    int8_t* (*GetUnique)(DataFrame_ColumnInt8* self);
   
    int (*GetUniqueSize)(DataFrame_ColumnInt8* self);
     int8_t* (*GetAll)(DataFrame_ColumnInt8* self);
    int (*GetColSize)(DataFrame_ColumnInt8* self);
    int8_t* (*GetBySelInt32)(DataFrame_ColumnInt8* self, int32_t * selection_column, int32_t  selection);
    int (*GetSelSize)(DataFrame_ColumnInt8* self);
    int8_t* (*GetBySelString)(DataFrame_ColumnInt8* self, char ** selection_column, char * selection); 

    int8_t* (*GetBySelStringUnique)(DataFrame_ColumnInt8* self, char ** selection_column, char * selection);
    const char*
      (*AddColValues)(DataFrame_ColumnInt8* self, int8_t * v, uint64_t size);
    int8_t (*GetValue)(DataFrame_ColumnInt8* self, size_t index);
    int8_t* (*GetBySelInt8)(DataFrame_ColumnInt8* self, int8_t * selection_column, int8_t  selection);
    int8_t* (*GetBySelInt8Unique)(DataFrame_ColumnInt8* self, int8_t * selection_column, int8_t  selection);
    const char* (*AddDateTime)(DataFrame_ColumnInt8* self, long datetime);
       int8_t** (*GetBySelInt8Group)(DataFrame_ColumnInt8* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, int8_t * copy_data); 
    int64_t * (*GetBySelInt8GroupSize)(DataFrame_ColumnInt8* self);
    int8_t* (*GetCopy)(DataFrame_ColumnInt8* self);
    int8_t * (*GetBySelGE)(DataFrame_ColumnInt8* self, int8_t selection);
} DataFrame_ColumnInt8Methods;

struct DataFrame_ColumnInt8
{
    DataFrame_ColumnInt8Methods* methods;
    int type;
};


DATAFRAME_EXPORT DataFrame_ColumnInt8* DataFrame_ColumnInt8_New(int type);


#ifdef __cplusplus
}
#endif
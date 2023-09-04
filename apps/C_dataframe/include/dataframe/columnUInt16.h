#include "dataframe/config.h"
#include "dataframe/column.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DataFrame_ColumnUInt16 DataFrame_ColumnUInt16;
typedef struct
{
    /* shared */
    DataFrame_Type _typeID;
    void* (*Cast)(DataFrame_ColumnUInt16* self, DataFrame_Type type);
    bool (*IncRef)(DataFrame_ColumnUInt16* self);
    bool (*DecRef)(DataFrame_ColumnUInt16* self);
    size_t (*Size)(DataFrame_ColumnUInt16* self);
    void (*Remove)(DataFrame_ColumnUInt16* self, size_t i);
    void (*Clear)(DataFrame_ColumnUInt16* self);
    char* (*GetName)(DataFrame_ColumnUInt16* self);
    char* (*SetName)(DataFrame_ColumnUInt16* self, char* name);
    bool (*HasValue)(DataFrame_ColumnUInt16* self, size_t index);
    uint8_t* (*GetNAs)(DataFrame_ColumnUInt16* self);


    /* type specific */
    bool (*TryGet)(DataFrame_ColumnUInt16* self, size_t index, uint16_t* v);
    /* returns a reference */
    uint16_t* (*Get)(DataFrame_ColumnUInt16* self, size_t index);

    const char* (*Add)(DataFrame_ColumnUInt16* self, uint16_t v);

    const char* (*AddNA)(DataFrame_ColumnUInt16* self);

    void (*Set)(DataFrame_ColumnUInt16* self, size_t i, uint16_t v);

    void (*SetNA)(DataFrame_ColumnUInt16* self, size_t i);

    uint16_t* (*GetUnique)(DataFrame_ColumnUInt16* self);
   
    int (*GetUniqueSize)(DataFrame_ColumnUInt16* self);
     uint16_t* (*GetAll)(DataFrame_ColumnUInt16* self);
    int (*GetColSize)(DataFrame_ColumnUInt16* self);
    uint16_t* (*GetBySelInt32)(DataFrame_ColumnUInt16* self, int32_t * selection_column, int32_t  selection);
    int (*GetSelSize)(DataFrame_ColumnUInt16* self);
    uint16_t* (*GetBySelString)(DataFrame_ColumnUInt16* self, char ** selection_column, char * selection); 

    uint16_t* (*GetBySelStringUnique)(DataFrame_ColumnUInt16* self, char ** selection_column, char * selection);
    const char*
      (*AddColValues)(DataFrame_ColumnUInt16* self, uint16_t * v, uint64_t size);
    uint16_t (*GetValue)(DataFrame_ColumnUInt16* self, size_t index);
    uint16_t* (*GetBySelInt8)(DataFrame_ColumnUInt16* self, int8_t * selection_column, int8_t  selection);
    uint16_t* (*GetBySelInt8Unique)(DataFrame_ColumnUInt16* self, int8_t * selection_column, int8_t  selection);
    const char* (*AddDateTime)(DataFrame_ColumnUInt16* self, long datetime);
       uint16_t** (*GetBySelInt8Group)(DataFrame_ColumnUInt16* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, uint16_t * copy_data); 
    int64_t * (*GetBySelInt8GroupSize)(DataFrame_ColumnUInt16* self);
    uint16_t* (*GetCopy)(DataFrame_ColumnUInt16* self);
    uint16_t * (*GetBySelGE)(DataFrame_ColumnUInt16* self, uint16_t selection);
} DataFrame_ColumnUInt16Methods;

struct DataFrame_ColumnUInt16
{
    DataFrame_ColumnUInt16Methods* methods;
    int type;
};


DATAFRAME_EXPORT DataFrame_ColumnUInt16* DataFrame_ColumnUInt16_New(int type);


#ifdef __cplusplus
}
#endif
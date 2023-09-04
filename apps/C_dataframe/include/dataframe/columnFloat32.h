#include "dataframe/config.h"
#include "dataframe/column.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DataFrame_ColumnFloat32 DataFrame_ColumnFloat32;
typedef struct
{
    /* shared */
    DataFrame_Type _typeID;
    void* (*Cast)(DataFrame_ColumnFloat32* self, DataFrame_Type type);
    bool (*IncRef)(DataFrame_ColumnFloat32* self);
    bool (*DecRef)(DataFrame_ColumnFloat32* self);
    size_t (*Size)(DataFrame_ColumnFloat32* self);
    void (*Remove)(DataFrame_ColumnFloat32* self, size_t i);
    void (*Clear)(DataFrame_ColumnFloat32* self);
    char* (*GetName)(DataFrame_ColumnFloat32* self);
    char* (*SetName)(DataFrame_ColumnFloat32* self, char* name);
    bool (*HasValue)(DataFrame_ColumnFloat32* self, size_t index);
    uint8_t* (*GetNAs)(DataFrame_ColumnFloat32* self);


    /* type specific */
    bool (*TryGet)(DataFrame_ColumnFloat32* self, size_t index, float* v);
    /* returns a reference */
    float* (*Get)(DataFrame_ColumnFloat32* self, size_t index);

    const char* (*Add)(DataFrame_ColumnFloat32* self, float v);

    const char* (*AddNA)(DataFrame_ColumnFloat32* self);

    void (*Set)(DataFrame_ColumnFloat32* self, size_t i, float v);

    void (*SetNA)(DataFrame_ColumnFloat32* self, size_t i);

    float* (*GetUnique)(DataFrame_ColumnFloat32* self);
   
    int (*GetUniqueSize)(DataFrame_ColumnFloat32* self);
     float* (*GetAll)(DataFrame_ColumnFloat32* self);
    int (*GetColSize)(DataFrame_ColumnFloat32* self);
    float* (*GetBySelInt32)(DataFrame_ColumnFloat32* self, int32_t * selection_column, int32_t  selection);
    int (*GetSelSize)(DataFrame_ColumnFloat32* self);
    float* (*GetBySelString)(DataFrame_ColumnFloat32* self, char ** selection_column, char * selection); 

    float* (*GetBySelStringUnique)(DataFrame_ColumnFloat32* self, char ** selection_column, char * selection);
    const char*
      (*AddColValues)(DataFrame_ColumnFloat32* self, float * v, uint64_t size);
    float (*GetValue)(DataFrame_ColumnFloat32* self, size_t index);
    float* (*GetBySelInt8)(DataFrame_ColumnFloat32* self, int8_t * selection_column, int8_t  selection);
    float* (*GetBySelInt8Unique)(DataFrame_ColumnFloat32* self, int8_t * selection_column, int8_t  selection);
    const char* (*AddDateTime)(DataFrame_ColumnFloat32* self, long datetime);
       float** (*GetBySelInt8Group)(DataFrame_ColumnFloat32* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, float * copy_data); 
    int64_t * (*GetBySelInt8GroupSize)(DataFrame_ColumnFloat32* self);
    float* (*GetCopy)(DataFrame_ColumnFloat32* self);
    float * (*GetBySelGE)(DataFrame_ColumnFloat32* self, float selection);
} DataFrame_ColumnFloat32Methods;

struct DataFrame_ColumnFloat32
{
    DataFrame_ColumnFloat32Methods* methods;
    int type;
};


DATAFRAME_EXPORT DataFrame_ColumnFloat32* DataFrame_ColumnFloat32_New(int type);


#ifdef __cplusplus
}
#endif
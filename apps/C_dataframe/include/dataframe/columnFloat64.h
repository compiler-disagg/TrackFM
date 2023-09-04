#include "dataframe/config.h"
#include "dataframe/column.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DataFrame_ColumnFloat64 DataFrame_ColumnFloat64;
typedef struct
{
    /* shared */
    DataFrame_Type _typeID;
    void* (*Cast)(DataFrame_ColumnFloat64* self, DataFrame_Type type);
    bool (*IncRef)(DataFrame_ColumnFloat64* self);
    bool (*DecRef)(DataFrame_ColumnFloat64* self);
    size_t (*Size)(DataFrame_ColumnFloat64* self);
    void (*Remove)(DataFrame_ColumnFloat64* self, size_t i);
    void (*Clear)(DataFrame_ColumnFloat64* self);
    char* (*GetName)(DataFrame_ColumnFloat64* self);
    char* (*SetName)(DataFrame_ColumnFloat64* self, char* name);
    bool (*HasValue)(DataFrame_ColumnFloat64* self, size_t index);
    uint8_t* (*GetNAs)(DataFrame_ColumnFloat64* self);


    /* type specific */
    bool (*TryGet)(DataFrame_ColumnFloat64* self, size_t index, double* v);
    /* returns a reference */
    double* (*Get)(DataFrame_ColumnFloat64* self, size_t index);

    const char* (*Add)(DataFrame_ColumnFloat64* self, double v);

    const char* (*AddNA)(DataFrame_ColumnFloat64* self);

    void (*Set)(DataFrame_ColumnFloat64* self, size_t i, double v);

    void (*SetNA)(DataFrame_ColumnFloat64* self, size_t i);

    double* (*GetUnique)(DataFrame_ColumnFloat64* self);
   
    int (*GetUniqueSize)(DataFrame_ColumnFloat64* self);
     double* (*GetAll)(DataFrame_ColumnFloat64* self);
    int (*GetColSize)(DataFrame_ColumnFloat64* self);
    double* (*GetBySelInt32)(DataFrame_ColumnFloat64* self, int32_t * selection_column, int32_t  selection);
    int (*GetSelSize)(DataFrame_ColumnFloat64* self);
    double* (*GetBySelString)(DataFrame_ColumnFloat64* self, char ** selection_column, char * selection); 

    double* (*GetBySelStringUnique)(DataFrame_ColumnFloat64* self, char ** selection_column, char * selection);
    const char*
      (*AddColValues)(DataFrame_ColumnFloat64* self, double * v, uint64_t size);
    double (*GetValue)(DataFrame_ColumnFloat64* self, size_t index);
    double* (*GetBySelInt8)(DataFrame_ColumnFloat64* self, int8_t * selection_column, int8_t  selection);
    double* (*GetBySelInt8Unique)(DataFrame_ColumnFloat64* self, int8_t * selection_column, int8_t  selection);
    const char* (*AddDateTime)(DataFrame_ColumnFloat64* self, long datetime);
       double** (*GetBySelInt8Group)(DataFrame_ColumnFloat64* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, double * copy_data); 
    int64_t * (*GetBySelInt8GroupSize)(DataFrame_ColumnFloat64* self);
    double* (*GetCopy)(DataFrame_ColumnFloat64* self);
    double * (*GetBySelGE)(DataFrame_ColumnFloat64* self, double selection);
} DataFrame_ColumnFloat64Methods;

struct DataFrame_ColumnFloat64
{
    DataFrame_ColumnFloat64Methods* methods;
    int type;
};


DATAFRAME_EXPORT DataFrame_ColumnFloat64* DataFrame_ColumnFloat64_New(int type);


#ifdef __cplusplus
}
#endif
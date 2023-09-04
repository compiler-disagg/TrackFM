#include "dataframe/config.h"
#include "dataframe/column.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DataFrame_ColumnString DataFrame_ColumnString;
typedef struct
{
    /* shared */
    DataFrame_Type _typeID;
    void* (*Cast)(DataFrame_ColumnString* self, DataFrame_Type type);
    bool (*IncRef)(DataFrame_ColumnString* self);
    bool (*DecRef)(DataFrame_ColumnString* self);
    size_t (*Size)(DataFrame_ColumnString* self);
    void (*Remove)(DataFrame_ColumnString* self, size_t i);
    void (*Clear)(DataFrame_ColumnString* self);
    char* (*GetName)(DataFrame_ColumnString* self);
    char* (*SetName)(DataFrame_ColumnString* self, char* name);
    bool (*HasValue)(DataFrame_ColumnString* self, size_t index);
    uint8_t* (*GetNAs)(DataFrame_ColumnString* self);


    /* type specific */
    bool (*TryGet)(DataFrame_ColumnString* self, size_t index, char** v);
    /* returns a reference */
    char** (*Get)(DataFrame_ColumnString* self, size_t index);

    const char* (*Add)(DataFrame_ColumnString* self, const char* v);

    const char* (*AddNA)(DataFrame_ColumnString* self);

    void (*Set)(DataFrame_ColumnString* self, size_t i, const char* v);

    void (*SetNA)(DataFrame_ColumnString* self, size_t i);

    char ** (*GetUnique)(DataFrame_ColumnString* self);
   
    int (*GetUniqueSize)(DataFrame_ColumnString* self);
     char** (*GetAll)(DataFrame_ColumnString* self);
    int (*GetColSize)(DataFrame_ColumnString* self);
    char ** (*GetBySelInt32)(DataFrame_ColumnString* self, int32_t * selection_column, int32_t  selection);
    int (*GetSelSize)(DataFrame_ColumnString* self);
    char ** (*GetBySelString)(DataFrame_ColumnString* self, char ** selection_column, char * selection); 

    char ** (*GetBySelStringUnique)(DataFrame_ColumnString* self, char ** selection_column, char * selection);
    const char*
      (*AddColValues)(DataFrame_ColumnString* self, char ** v, uint64_t size);
    char * (*GetValue)(DataFrame_ColumnString* self, size_t index);
    char ** (*GetBySelInt8)(DataFrame_ColumnString* self, int8_t * selection_column, int8_t  selection);
    char ** (*GetBySelInt8Unique)(DataFrame_ColumnString* self, int8_t * selection_column, int8_t  selection);
    const char* (*AddDateTime)(DataFrame_ColumnString* self, long datetime);
	char ** (*GetBySelInt8Group)(DataFrame_ColumnString* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, char ** copy_data);
    int64_t * (*GetBySelInt8GroupSize)(DataFrame_ColumnString* self);
    char ** (*GetCopy)(DataFrame_ColumnString* self);
    char ** (*GetBySelGE)(DataFrame_ColumnString* self, char * selection);
} DataFrame_ColumnStringMethods;

struct DataFrame_ColumnString
{
    DataFrame_ColumnStringMethods* methods;
    int type;
};


DATAFRAME_EXPORT DataFrame_ColumnString* DataFrame_ColumnString_New(int type);


#ifdef __cplusplus
}
#endif
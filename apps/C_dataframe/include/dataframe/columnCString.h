#include "dataframe/config.h"
#include "dataframe/column.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DataFrame_ColumnCString DataFrame_ColumnCString;
typedef struct
{
    /* shared */
    DataFrame_Type _typeID;
    void* (*Cast)(DataFrame_ColumnCString* self, DataFrame_Type type);
    bool (*IncRef)(DataFrame_ColumnCString* self);
    bool (*DecRef)(DataFrame_ColumnCString* self);
    size_t (*Size)(DataFrame_ColumnCString* self);
    void (*Remove)(DataFrame_ColumnCString* self, size_t i);
    void (*Clear)(DataFrame_ColumnCString* self);
    char* (*GetName)(DataFrame_ColumnCString* self);
    const char* (*SetName)(DataFrame_ColumnCString* self, const char* name);
    bool (*HasValue)(DataFrame_ColumnCString* self, size_t index);
    uint8_t* (*GetNAs)(DataFrame_ColumnCString* self);


    /* type specific */
    bool (*TryGet)(DataFrame_ColumnCString* self, size_t index, char** v);
    /* returns a reference */
    char** (*Get)(DataFrame_ColumnCString* self, size_t index);

    const char* (*Add)(DataFrame_ColumnCString* self, const char* v);

    const char* (*AddNA)(DataFrame_ColumnCString* self);

    void (*Set)(DataFrame_ColumnCString* self, size_t i, const char* v);

    void (*SetNA)(DataFrame_ColumnCString* self, size_t i);
} DataFrame_ColumnCStringMethods;

struct DataFrame_ColumnCString
{
    DataFrame_ColumnCStringMethods* methods;
};


DATAFRAME_EXPORT DataFrame_ColumnCString* DataFrame_ColumnCString_New();


#ifdef __cplusplus
}
#endif
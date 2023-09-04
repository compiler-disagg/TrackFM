#include "dataframe/config.h"
#include "dataframe/column.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DataFrame_Column{{name}} DataFrame_Column{{name}};
typedef struct
{
    /* shared */
    DataFrame_Type _typeID;
    void* (*Cast)(DataFrame_Column{{name}}* self, DataFrame_Type type);
    bool (*IncRef)(DataFrame_Column{{name}}* self);
    bool (*DecRef)(DataFrame_Column{{name}}* self);
    size_t (*Size)(DataFrame_Column{{name}}* self);
    void (*Remove)(DataFrame_Column{{name}}* self, size_t i);
    void (*Clear)(DataFrame_Column{{name}}* self);
    char* (*GetName)(DataFrame_Column{{name}}* self);
    char* (*SetName)(DataFrame_Column{{name}}* self, char* name);
    bool (*HasValue)(DataFrame_Column{{name}}* self, size_t index);
    uint8_t* (*GetNAs)(DataFrame_Column{{name}}* self);


    /* type specific */
    bool (*TryGet)(DataFrame_Column{{name}}* self, size_t index, {{type}}* v);
    /* returns a reference */
    {{type}}* (*Get)(DataFrame_Column{{name}}* self, size_t index);

    {% if type == "char*" %}
    const char* (*Add)(DataFrame_Column{{name}}* self, const {{type}} v);
    {% else %}
    const char* (*Add)(DataFrame_Column{{name}}* self, {{type}} v);
    {% endif %}

    const char* (*AddNA)(DataFrame_Column{{name}}* self);

    {% if type == "char*" %}
    void (*Set)(DataFrame_Column{{name}}* self, size_t i, const {{type}} v);
    {% else %}
    void (*Set)(DataFrame_Column{{name}}* self, size_t i, {{type}} v);
    {% endif %}

    void (*SetNA)(DataFrame_Column{{name}}* self, size_t i);

    {% if type == "char*" %}
    char ** (*GetUnique)(DataFrame_Column{{name}}* self);
    {% else %}
    {{type}}* (*GetUnique)(DataFrame_Column{{name}}* self);
    {% endif %}
   
    int (*GetUniqueSize)(DataFrame_Column{{name}}* self);
    {% if type == "char*" %}
     {{type}}* (*GetAll)(DataFrame_Column{{name}}* self);
    {% else %}
     {{type}}* (*GetAll)(DataFrame_Column{{name}}* self);
    {% endif %}
    int (*GetColSize)(DataFrame_Column{{name}}* self);
    {% if type == "char*" %}
    char ** (*GetBySelInt32)(DataFrame_Column{{name}}* self, int32_t * selection_column, int32_t  selection);
    {% else %}
    {{type}}* (*GetBySelInt32)(DataFrame_Column{{name}}* self, int32_t * selection_column, int32_t  selection);
    {% endif %}
    int (*GetSelSize)(DataFrame_Column{{name}}* self);
    {% if type == "char*" %}
    char ** (*GetBySelString)(DataFrame_Column{{name}}* self, char ** selection_column, char * selection); 
    {% else %}
    {{type}}* (*GetBySelString)(DataFrame_Column{{name}}* self, char ** selection_column, char * selection); 
    {% endif %}

    {% if type == "char*" %}
    char ** (*GetBySelStringUnique)(DataFrame_Column{{name}}* self, char ** selection_column, char * selection);
    {% else %}
    {{type}}* (*GetBySelStringUnique)(DataFrame_Column{{name}}* self, char ** selection_column, char * selection);
    {% endif %}
    {% if type == "char*" %}
    const char*
      (*AddColValues)(DataFrame_Column{{name}}* self, char ** v, uint64_t size);
    {% else %}
    const char*
      (*AddColValues)(DataFrame_Column{{name}}* self, {{type}} * v, uint64_t size);
    {% endif %}
    {% if type == "char*" %}
    char * (*GetValue)(DataFrame_Column{{name}}* self, size_t index);
    {% else %}
    {{type}} (*GetValue)(DataFrame_Column{{name}}* self, size_t index);
    {% endif %}
    {% if type == "char*" %}
    char ** (*GetBySelInt8)(DataFrame_Column{{name}}* self, int8_t * selection_column, int8_t  selection);
    {% else %}
    {{type}}* (*GetBySelInt8)(DataFrame_Column{{name}}* self, int8_t * selection_column, int8_t  selection);
    {% endif %}
    {% if type == "char*" %}
    char ** (*GetBySelInt8Unique)(DataFrame_Column{{name}}* self, int8_t * selection_column, int8_t  selection);
    {% else %}
    {{type}}* (*GetBySelInt8Unique)(DataFrame_Column{{name}}* self, int8_t * selection_column, int8_t  selection);
    {% endif %}
    {% if type == "char*" %}
    const char* (*AddDateTime)(DataFrame_Column{{name}}* self, long datetime);
    {% else %}
    const char* (*AddDateTime)(DataFrame_Column{{name}}* self, long datetime);
    {% endif %}
    {% if type == "char*" %}
	char ** (*GetBySelInt8Group)(DataFrame_Column{{name}}* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, char ** copy_data);
    {% else %}
       {{type}}** (*GetBySelInt8Group)(DataFrame_Column{{name}}* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, {{type}} * copy_data); 
    {% endif %}
    int64_t * (*GetBySelInt8GroupSize)(DataFrame_Column{{name}}* self);
    {% if type == "char*" %}
    char ** (*GetCopy)(DataFrame_Column{{name}}* self);
    {% else %}
    {{type}}* (*GetCopy)(DataFrame_Column{{name}}* self);
    {% endif %}
    {% if type == "char*" %}
    char ** (*GetBySelGE)(DataFrame_Column{{name}}* self, char * selection);
    {% else %}
    {{type}} * (*GetBySelGE)(DataFrame_Column{{name}}* self, {{type}} selection);
    {% endif %}
} DataFrame_Column{{name}}Methods;

struct DataFrame_Column{{name}}
{
    DataFrame_Column{{name}}Methods* methods;
    int type;
};


DATAFRAME_EXPORT DataFrame_Column{{name}}* DataFrame_Column{{name}}_New(int type);


#ifdef __cplusplus
}
#endif

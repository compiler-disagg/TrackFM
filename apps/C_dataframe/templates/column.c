#include "dataframe/column{{name}}.h"
#include "dataframe/atomic.h"
#include <string.h>
#include <stdio.h>

#define SELF ((DataFrame_Column{{name}}Impl*)self)

#ifdef _MSC_VER
   #define strdup _strdup
#endif

typedef struct
{
    /* public */
    DataFrame_Column{{name}}Methods* methods;
    int   type;

    /* private */
    volatile uint32_t ref_count;
    char* name;
    {{type}}* data;
    size_t size;
    size_t col_size;
    size_t capacity;
    DataFrame_BitVector na;
    size_t unique_size;
    size_t selection_size;
    int64_t * group_size;
} DataFrame_Column{{name}}Impl;

static void*
Cast(DataFrame_Column{{name}}* self, DataFrame_Type type)
{
    if(SELF->methods->_typeID == type)
        return self;
    return NULL;
}

static bool
IncRef(DataFrame_Column{{name}}* self)
{
    return InterlockedIncrement(&SELF->ref_count) > 0;
}

static bool
DecRef(DataFrame_Column{{name}}* self)
{
    if(!InterlockedDecrement(&SELF->ref_count))
    {
        free(SELF->data);
        DataFrame_BitVector_Destroy(&SELF->na);
        free(SELF);
        return false;
    }
    return true;
}

static size_t
Size(DataFrame_Column{{name}}* self)
{
    return SELF->size;
}

static void
Clear(DataFrame_Column{{name}}* self)
{
    SELF->size = 0;
    SELF->na.size = 0;
    SELF->col_size = 0;
}

static bool
TryGet(
    DataFrame_Column{{name}}* self, size_t index, {{type}}* v)
{
    bool na = DataFrame_BitVector_Get(&SELF->na, index);
    if(na) return false;
    *v = SELF->data[index];
    return true;
}

static const char*
Resize(DataFrame_Column{{name}}Impl* self)
{
    {{type}}* d;
    size_t newSize;

    newSize = SELF->size * 2;
    if(!newSize) newSize = 4;

    d = realloc(SELF->data, newSize * sizeof({{type}}));
    if(!d) return "DataFrame_ColumnInt8_Add: Out of memory";
    SELF->data = d;
    SELF->capacity = newSize;

    return NULL;
}

{% if type == "char*" %}

static const char*
Add(DataFrame_Column{{name}}* self, const char* v)
{
    char * v2;
    bool na = v ? false : true;
    const char* e = DataFrame_BitVector_Add(&SELF->na, na);
    if(e) return e;

    if(SELF->size == SELF->capacity)
    {
        e = Resize(SELF);
        if(e) return e;
    }

    if(na)
        SELF->data[SELF->size++] = NULL;
    else
    {
        v2 = strdup(v);
        if(!v2)
            return "DataFrame_ColumnCString_Add: Out of memory";
        SELF->data[SELF->size++] = v2;
        SELF->col_size++;
    }
    return NULL;
}

static const char*
AddDateTime(DataFrame_Column{{name}}* self, long datetime)
{
    long * v2;
    bool na = datetime ? false : true;
    const char* e = DataFrame_BitVector_Add(&SELF->na, na);
    if(e) return e;

    if(SELF->size == SELF->capacity)
    {
        e = Resize(SELF);
        if(e) return e;
    }

    if(na)
        SELF->data[SELF->size++] = NULL;
    else
    {
        v2 = malloc(8);
        if(!v2)
            return "DataFrame_ColumnCString_Add: Out of memory";
        *v2 = datetime;

        SELF->data[SELF->size++] = (char *)v2;
        SELF->col_size++;
    }
    return NULL;
}

static const char*
AddColValues(DataFrame_Column{{name}}* self, char ** v, uint64_t size)
{
        SELF->data = v;
	      SELF->col_size = size;
	      SELF->size = size;
	      SELF->capacity = size;
        return NULL;
}

static void
Set(DataFrame_Column{{name}}* self, size_t i, const char* v)
{
    bool na = v ? false : true;
    if(na)
    {
        DataFrame_BitVector_Set(&SELF->na, i, na);
        SELF->data[i] = NULL;
    }
    else
    {
        SELF->data[i] = strdup(v);
        DataFrame_BitVector_Set(&SELF->na, i, !na && SELF->data[i]);
    }
}

static bool is_unique(char ** seen_val, size_t size, char * new_val) {  
  int i = 0; 
  for (i = 0; i < size; i++) { 
    if (!strcmp(new_val, seen_val[i])) 
      return false;  
  }   
  return true;   
}

static char **
GetUnique(DataFrame_Column{{name}}* self)
{
    int j = 0; 
    int i = 0;
    char ** dt = (char **) SELF->data;
     //__attribute__((annotate("local_malloc")))
    char ** val = (char **) malloc(SELF->size * sizeof(char *));
    for (i = 0; i < SELF->col_size; i++) {
      if (is_unique(val, j, dt[i])) {
        val[j] = (char *)malloc(strlen(dt[i]));
        strcpy(val[j], dt[i]);
        j++;
      } 
    }
    SELF->unique_size = j;
    return val;
}
static char **
GetBySelInt32(DataFrame_Column{{name}}* self, int32_t * selection_column, int32_t  selection)
{
    int j = 0; 
    int i = 0;
    
    char ** dt = (char **) SELF->data;
    //__attribute__((annotate("local_malloc")))
    char ** val = (char **) malloc(SELF->size * sizeof (char *));
    for (i = 0; i < SELF->col_size; i++) {
      if (selection_column[i] == selection) {
        val[j] = (char *)malloc(strlen(dt[i]));
        strcpy(val[j], dt[i]);
        j++;
      } 
    }
    SELF->selection_size = j;
    return val;
}
static char **
GetBySelInt8(DataFrame_Column{{name}}* self, int8_t * selection_column, int8_t  selection)
{
    int j = 0; 
    int i = 0;
    
    char ** dt = (char **) SELF->data;
    //__attribute__((annotate("local_malloc")))
    char ** val = (char **) malloc(SELF->size * sizeof (char *));
    for (i = 0; i < SELF->col_size; i++) {
      if (selection_column[i] == selection) {
        val[j] = (char *)malloc(strlen(dt[i]));
        strcpy(val[j], dt[i]);
        j++;
      } 
    }
    SELF->selection_size = j;
    return val;
}
static char ** 
GetBySelInt8Group(DataFrame_Column{{name}}* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, char ** copy_data) {
	return NULL;
}
static char **
GetBySelString(DataFrame_Column{{name}}* self, char ** selection_column, char *  selection)
{
    int j = 0; 
    
    char ** dt = (char **) SELF->data;
    //__attribute__((annotate("local_malloc")))
    char ** val = (char **) malloc(SELF->size * sizeof (char *));
    for (int i = 0; i < SELF->col_size; i++) {
      if (strcmp(selection_column[i], selection) == 0) {
        val[j] = (char *)malloc(strlen(dt[i]));
        strcpy(val[j], dt[i]);
        j++;
      } 
    }
    SELF->selection_size = j;
    return val;
}

static char **
GetBySelInt8Unique(DataFrame_Column{{name}}* self, int8_t * selection_column, int8_t   selection)
{
    int j = 0; 
    
    char ** dt = (char **) SELF->data;
     //__attribute__((annotate("local_malloc")))
    char ** val = (char **) malloc(SELF->size * sizeof (char *));
    for (int i = 0; i < SELF->col_size; i++) {
      if (selection_column[i] == selection) {
        if (is_unique(val, j, dt[i])) {
          val[j] = (char *)malloc(strlen(dt[i]));
          strcpy(val[j], dt[i]);
          j++;
        }
      } 
    }
    SELF->selection_size = j;
    return val;
}
static char **
GetBySelStringUnique(DataFrame_Column{{name}}* self, char ** selection_column, char *  selection)
{
    int j = 0; 
    
    char ** dt = (char **) SELF->data;
     //__attribute__((annotate("local_malloc")))
    char ** val = (char **) malloc(SELF->size * sizeof (char *));
    for (int i = 0; i < SELF->col_size; i++) {
      if (strcmp(selection_column[i], selection) == 0) {
        if (is_unique(val, j, dt[i])) {
          val[j] = (char *)malloc(strlen(dt[i]));
          strcpy(val[j], dt[i]);
          j++;
        }
      } 
    }
    SELF->selection_size = j;
    return val;
}
static {{type}}*
GetAll(DataFrame_Column{{name}}* self)
{
    return (char **)SELF->data;
}
static char **
GetCopy(DataFrame_Column{{name}}* self)
{
    return (char **)SELF->data;
}
static char *
GetValue(DataFrame_Column{{name}}* self, size_t index)
{
    return SELF->data[index];
}
static char ** 
GetBySelGE(DataFrame_Column{{name}}* self, char * selection) {

    //NOT IMPLEMENTED
    SELF->selection_size = 0;
    abort();
    return SELF->data;
}
{% else %}
static int setSelInt8({{type}} * data, int8_t * selection_column, int8_t  selection, size_t size, {{type}} * dst) {
    int j = 0;
    int i = 0;
    {{type}} dat;
    {{type}} select_val;
    for (i = 0; i < size; i++) {
      dat = data[i];
      select_val = selection_column[i];
      if (selection == select_val) {
        dst[j] = dat;
        j++;
      } 
    }
    return j;
}
static int setSelInt32({{type}} * data, int32_t * selection_column, int32_t  selection, size_t size, {{type}} * dst) {
    int j = 0;
    int i = 0;
    {{type}} dat;
    {{type}} select_val;
    for (i = 0; i < size; i++) {
      dat = data[i];
      select_val = selection_column[i];
      if (selection == select_val) {
        dst[j] = dat;
        j++;
      } 
    }
    return j;
}
static const char*
AddDateTime(DataFrame_Column{{name}}* self, long datetime)
{
   
    return NULL;
}
static {{type}} 
GetValue(DataFrame_Column{{name}}* self, size_t index)
{
    return SELF->data[index];
}
static const char*
Add(DataFrame_Column{{name}}* self, {{type}} v)
{
    const char* e = DataFrame_BitVector_Add(&SELF->na, false);
    if(e) return e;

    if(SELF->size == SELF->capacity)
    {
        e = Resize(SELF);
        if(e) return e;
    }


    SELF->data[SELF->size++] = v;
    SELF->col_size++;
    return NULL;
}

static const char*
AddColValues(DataFrame_Column{{name}}* self, {{type}} * v, uint64_t size)
{
        SELF->data = v;
	      SELF->col_size = size;
	      SELF->size = size;
	      SELF->capacity = size;
        return NULL;
}

static void
Set(DataFrame_Column{{name}}* self, size_t i, {{type}} v)
{
    DataFrame_BitVector_Set(&SELF->na, i, false);
    SELF->data[i] = v;
}

static bool iis_unique({{type}} * seen_val, {{type}} new_val, size_t size) { 
	for(int k = 0; k < size; k++) { 
		if (new_val == seen_val[k]) {
			return false;
		}
	}
	return true;
}
static int setUnique({{type}} * val, {{type}} * data, size_t size) {

    int j = 0; 
    int i = 0;
    {{type}} dt;
    for (i = 0; i < size; i++) {
      dt = data[i];
      if (iis_unique(val, dt, j)) {
        val[j] = dt;
        j++;
      } 
    }
    return j;
}
static {{type}}*
GetUnique(DataFrame_Column{{name}}* self)
{
     //__attribute__((annotate("local_malloc")))
    {{type}} * val = ({{type}} *) malloc(SELF->size * sizeof({{type}}));

    SELF->unique_size = setUnique(val, SELF->data, SELF->col_size);

    return val;
}
static {{type}}* 
GetBySelInt32(DataFrame_Column{{name}}* self, int32_t * selection_column, int32_t  selection) {

//     __attribute__((annotate("local_malloc")))
	{{type}} * val = ({{type}} *) malloc(SELF->size * sizeof({{type}}));
	{{type}} * data = SELF->data;

	SELF->selection_size = setSelInt32(data, selection_column, selection, SELF->size, val); 
	return val;
}

static {{type}}* 
GetBySelInt8(DataFrame_Column{{name}}* self, int8_t * selection_column, int8_t  selection) {

     //__attribute__((annotate("local_malloc")))
    {{type}} * val = ({{type}} *) malloc(SELF->size * sizeof({{type}}));
    {{type}} * data = SELF->data;

    SELF->selection_size = setSelInt8(data, selection_column, selection, SELF->size,  val); 

    return val;
}


static size_t GetGE({{type}}  selection, {{type}} * dst,  {{type}} * data, size_t size) {
    int i = 0;
    int j = 0;

    {{type}} dat;
    {{type}} select_val;

    for (i = 0; i < size; i++) {
      dat = data[i];
      if (dat >= selection) {
        dst[j] = dat;
        j++;
      } 
    }
    return j;
}

static {{type}}* 
GetBySelGE(DataFrame_Column{{name}}* self, {{type}}  selection) {

     //__attribute__((annotate("local_malloc")))
    {{type}} * val = ({{type}} *) malloc(SELF->size * sizeof({{type}}));
    {{type}} * data = SELF->data;

    SELF->selection_size = GetGE(selection,  val,  data, SELF->col_size);

    return val;
}

static void group_BY_col({{type}} * data, int8_t * group_column, 
		{{type}} ** groupby, int64_t  * group_size, int8_t * groups, size_t size, size_t ngroups) { 

    int group_index;
    int group_item_size;
    {{type}} dat;
    int8_t select_val;
    {{type}} * group_item;
    int64_t  group_len;
    for (int j = 0; j < size; j++) {
	    dat = data[j];
	    select_val = group_column[j];
	    group_index = select_val % ngroups;
	    group_len = group_size[group_index];
	    group_item = groupby[group_index];
	    group_item[group_len] = dat;
	    group_size[group_index] = group_len + 1;
    }
}
#if 1
static {{type}}** 
GetBySelInt8Group(DataFrame_Column{{name}}* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, {{type}} * copy_data) {

    {{type}} * data = copy_data;

    int i = 0;
//     __attribute__((annotate("local_malloc")))
    {{type}} ** groupby = ({{type}} **) malloc(ngroups * sizeof({{type}} *));

   int64_t * group_size = (int64_t *) malloc(ngroups * sizeof(int64_t));

   for (i = 0; i < ngroups; i++) {
	groupby[i] = malloc(SELF->size * sizeof({{type}}));
	group_size[i] = 0;
   }

    group_BY_col(data, group_column, groupby, group_size, groups, SELF->col_size, ngroups); 

    SELF->group_size = group_size; 
    return groupby;
}
#endif

static {{type}}* 
GetBySelString(DataFrame_Column{{name}}* self, char ** selection, char * select_val) {

    //__attribute__((annotate("local_malloc")))
    {{type}} * val = ({{type}} *) malloc(SELF->col_size * sizeof({{type}}));

    int j = 0;
    int i = 0;
    for (i = 0; i < SELF->size; i++) {
      if (!strcmp(selection[i], select_val)) {
        val[j] = SELF->data[i];
        j++;
      } 
    }
    SELF->selection_size = j ;
    return val;
}

static {{type}}* 
GetBySelStringUnique(DataFrame_Column{{name}}* self, char ** selection_column, char * selection) {

    int j = 0; 
   // __attribute__((annotate("local_malloc")))
    {{type}} * val = ({{type}} *) malloc(SELF->col_size * sizeof({{type}}));

    for (int i = 0; i < SELF->col_size; i++) {
      if (strcmp(selection, selection_column[i]) == 0) {
        if (iis_unique(val, SELF->data[i], SELF->size)) {
          val[j] = SELF->data[i];
          j++;
        }
	 } 
    }
    SELF->selection_size = j;
    return val;
}

static int setSelInt8Unique({{type}} * val, {{type }} * data, int8_t * selection_column, int8_t selection, size_t size) {

    {{type}} dat;
    int8_t  select_val;
    int j = 0;

    for (int i = 0; i < size; i++) {
      dat = data[i];
      select_val = selection_column[i];

      if (selection == select_val) {
	      if (iis_unique(val, dat, j)) {
		      val[j] = dat;
		      j++;
	      }
      } 
    }
    return j;
}
static {{type}}* 
GetBySelInt8Unique(DataFrame_Column{{name}}* self, int8_t * selection_column, int8_t selection) {

    {{type}} * val = ({{type}} *) malloc(SELF->col_size * sizeof({{type}}));
    {{type}} * data = SELF->data;
   // __attribute__((annotate("local_malloc")))
    SELF->selection_size = setSelInt8Unique(val, data, selection_column, selection, SELF->size);
    return val;
}
static {{type}}*
GetAll(DataFrame_Column{{name}}* self)
{
    return SELF->data;
}

static void copy_col({{type}} * dst, {{type}} * src, uint64_t size) {
	for (int i = 0; i < size; i++) {
		dst[i] = src[i]; 
	}
}

static {{type}}*
GetCopy(DataFrame_Column{{name}}* self)
{
    int i = 0;
    {{type}} * copy = ({{type}} *) malloc(SELF->col_size * sizeof({{type}}));
    {{type}} * data = SELF->data;

    copy_col(copy, data, SELF->size);
    return copy;
}

{% endif %}

static int64_t * 
GetBySelInt8GroupSize(DataFrame_Column{{name}}* self) {
    return SELF->group_size;
}
static const char*
AddNA(DataFrame_Column{{name}}* self)
{
    const char* e = DataFrame_BitVector_Add(&SELF->na, true);
    if(e) return e;

    if(SELF->size == SELF->capacity)
    {
        e = Resize(SELF);
        if(e) return e;
    }

    SELF->size++;
    return NULL;
}

static void
Remove(DataFrame_Column{{name}}* self, size_t i)
{
    DataFrame_BitVector_Remove(&SELF->na, i);
    memmove(&SELF->data[i], &SELF->data[i+1], SELF->size - i - 1);
    SELF->size--;
    SELF->col_size--;
}


static void
SetNA(DataFrame_Column{{name}}* self, size_t i)
{
    DataFrame_BitVector_Set(&SELF->na, i, true);
}

static char*
GetName(DataFrame_Column{{name}}* self)
{ 
    return (char *)SELF->name;
}

static char*
SetName(DataFrame_Column{{name}}* self, char* name)
{
    char* n;

    if(!name)
    {
        SELF->name = NULL;
        return NULL;
    }

    n = strdup(name);
    if(!n) return "DataFrame_Column{{name}}_SetName: Out of memory";

    SELF->name = n;
    return NULL;
}

static bool
HasValue(DataFrame_Column{{name}}* self, size_t index)
{
    bool na = DataFrame_BitVector_Get(&SELF->na, index);
    return !na;
}

static {{type}} *
Get(DataFrame_Column{{name}}* self, size_t index)
{
    return &SELF->data[index];
}

static uint8_t*
GetNAs(DataFrame_Column{{name}}* self)
{
    return SELF->na.data;
}

static int
GetUniqueSize(DataFrame_Column{{name}}* self)
{
    return SELF->unique_size;
}

static int
GetColSize(DataFrame_Column{{name}}* self)
{
    return SELF->col_size;
}
static int
GetSelSize(DataFrame_Column{{name}}* self)
{
    return SELF->selection_size;
}
static DataFrame_Column{{name}}Methods {{name}}Methods =
{
/* shared */
    DataFrame_Type_Column{{name}},
    Cast,
    IncRef,
    DecRef,
    Size,
    Remove,
    Clear,
    GetName,
    SetName,
    HasValue,
    GetNAs,

/* type specific */
    TryGet,
    Get,
    Add,
    AddNA,
    Set,
    SetNA,
    GetUnique,
    GetUniqueSize,
    GetAll,
    GetColSize,
    GetBySelInt32,
    GetSelSize,
    GetBySelString,
    GetBySelStringUnique,
    AddColValues,
    GetValue,
    GetBySelInt8,
    GetBySelInt8Unique,
    AddDateTime,
    GetBySelInt8Group,
    GetBySelInt8GroupSize,
    GetCopy,
    GetBySelGE
};


DataFrame_Column{{name}}* DataFrame_Column{{name}}_New(int type)
{
    DataFrame_Column{{name}}Impl* c;

    c = calloc(1, sizeof(DataFrame_Column{{name}}Impl));
    if(!c) return NULL;
    c->methods = &{{name}}Methods;
    c->ref_count = 1;
    c->type = type;
    return (DataFrame_Column{{name}}*)c;
}

#include "dataframe/columnString.h"
#include "dataframe/atomic.h"
#include <string.h>
#include <stdio.h>

#define SELF ((DataFrame_ColumnStringImpl*)self)

#ifdef _MSC_VER
   #define strdup _strdup
#endif

typedef struct
{
    /* public */
    DataFrame_ColumnStringMethods* methods;
    int   type;

    /* private */
    volatile uint32_t ref_count;
    char* name;
    char** data;
    size_t size;
    size_t col_size;
    size_t capacity;
    DataFrame_BitVector na;
    size_t unique_size;
    size_t selection_size;
    int64_t * group_size;
} DataFrame_ColumnStringImpl;

static void*
Cast(DataFrame_ColumnString* self, DataFrame_Type type)
{
    if(SELF->methods->_typeID == type)
        return self;
    return NULL;
}

static bool
IncRef(DataFrame_ColumnString* self)
{
    return InterlockedIncrement(&SELF->ref_count) > 0;
}

static bool
DecRef(DataFrame_ColumnString* self)
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
Size(DataFrame_ColumnString* self)
{
    return SELF->size;
}

static void
Clear(DataFrame_ColumnString* self)
{
    SELF->size = 0;
    SELF->na.size = 0;
    SELF->col_size = 0;
}

static bool
TryGet(
    DataFrame_ColumnString* self, size_t index, char** v)
{
    bool na = DataFrame_BitVector_Get(&SELF->na, index);
    if(na) return false;
    *v = SELF->data[index];
    return true;
}

static const char*
Resize(DataFrame_ColumnStringImpl* self)
{
    char** d;
    size_t newSize;

    newSize = SELF->size * 2;
    if(!newSize) newSize = 4;

    d = realloc(SELF->data, newSize * sizeof(char*));
    if(!d) return "DataFrame_ColumnInt8_Add: Out of memory";
    SELF->data = d;
    SELF->capacity = newSize;

    return NULL;
}


static const char*
Add(DataFrame_ColumnString* self, const char* v)
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
AddDateTime(DataFrame_ColumnString* self, long datetime)
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
AddColValues(DataFrame_ColumnString* self, char ** v, uint64_t size)
{
        SELF->data = v;
	      SELF->col_size = size;
	      SELF->size = size;
	      SELF->capacity = size;
        return NULL;
}

static void
Set(DataFrame_ColumnString* self, size_t i, const char* v)
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
GetUnique(DataFrame_ColumnString* self)
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
GetBySelInt32(DataFrame_ColumnString* self, int32_t * selection_column, int32_t  selection)
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
GetBySelInt8(DataFrame_ColumnString* self, int8_t * selection_column, int8_t  selection)
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
GetBySelInt8Group(DataFrame_ColumnString* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, char ** copy_data) {
	return NULL;
}
static char **
GetBySelString(DataFrame_ColumnString* self, char ** selection_column, char *  selection)
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
GetBySelInt8Unique(DataFrame_ColumnString* self, int8_t * selection_column, int8_t   selection)
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
GetBySelStringUnique(DataFrame_ColumnString* self, char ** selection_column, char *  selection)
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
static char**
GetAll(DataFrame_ColumnString* self)
{
    return (char **)SELF->data;
}
static char **
GetCopy(DataFrame_ColumnString* self)
{
    return (char **)SELF->data;
}
static char *
GetValue(DataFrame_ColumnString* self, size_t index)
{
    return SELF->data[index];
}
static char ** 
GetBySelGE(DataFrame_ColumnString* self, char * selection) {

    //NOT IMPLEMENTED
    SELF->selection_size = 0;
    abort();
    return SELF->data;
}

static int64_t * 
GetBySelInt8GroupSize(DataFrame_ColumnString* self) {
    return SELF->group_size;
}
static const char*
AddNA(DataFrame_ColumnString* self)
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
Remove(DataFrame_ColumnString* self, size_t i)
{
    DataFrame_BitVector_Remove(&SELF->na, i);
    memmove(&SELF->data[i], &SELF->data[i+1], SELF->size - i - 1);
    SELF->size--;
    SELF->col_size--;
}


static void
SetNA(DataFrame_ColumnString* self, size_t i)
{
    DataFrame_BitVector_Set(&SELF->na, i, true);
}

static char*
GetName(DataFrame_ColumnString* self)
{ 
    return (char *)SELF->name;
}

static char*
SetName(DataFrame_ColumnString* self, char* name)
{
    char* n;

    if(!name)
    {
        SELF->name = NULL;
        return NULL;
    }

    n = strdup(name);
    if(!n) return "DataFrame_ColumnString_SetName: Out of memory";

    SELF->name = n;
    return NULL;
}

static bool
HasValue(DataFrame_ColumnString* self, size_t index)
{
    bool na = DataFrame_BitVector_Get(&SELF->na, index);
    return !na;
}

static char* *
Get(DataFrame_ColumnString* self, size_t index)
{
    return &SELF->data[index];
}

static uint8_t*
GetNAs(DataFrame_ColumnString* self)
{
    return SELF->na.data;
}

static int
GetUniqueSize(DataFrame_ColumnString* self)
{
    return SELF->unique_size;
}

static int
GetColSize(DataFrame_ColumnString* self)
{
    return SELF->col_size;
}
static int
GetSelSize(DataFrame_ColumnString* self)
{
    return SELF->selection_size;
}
static DataFrame_ColumnStringMethods StringMethods =
{
/* shared */
    DataFrame_Type_ColumnString,
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


DataFrame_ColumnString* DataFrame_ColumnString_New(int type)
{
    DataFrame_ColumnStringImpl* c;

    c = calloc(1, sizeof(DataFrame_ColumnStringImpl));
    if(!c) return NULL;
    c->methods = &StringMethods;
    c->ref_count = 1;
    c->type = type;
    return (DataFrame_ColumnString*)c;
}
#include "dataframe/columnUInt64.h"
#include "dataframe/atomic.h"
#include <string.h>
#include <stdio.h>

#define SELF ((DataFrame_ColumnUInt64Impl*)self)

#ifdef _MSC_VER
   #define strdup _strdup
#endif

typedef struct
{
    /* public */
    DataFrame_ColumnUInt64Methods* methods;
    int   type;

    /* private */
    volatile uint32_t ref_count;
    char* name;
    uint64_t* data;
    size_t size;
    size_t col_size;
    size_t capacity;
    DataFrame_BitVector na;
    size_t unique_size;
    size_t selection_size;
    int64_t * group_size;
} DataFrame_ColumnUInt64Impl;

static void*
Cast(DataFrame_ColumnUInt64* self, DataFrame_Type type)
{
    if(SELF->methods->_typeID == type)
        return self;
    return NULL;
}

static bool
IncRef(DataFrame_ColumnUInt64* self)
{
    return InterlockedIncrement(&SELF->ref_count) > 0;
}

static bool
DecRef(DataFrame_ColumnUInt64* self)
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
Size(DataFrame_ColumnUInt64* self)
{
    return SELF->size;
}

static void
Clear(DataFrame_ColumnUInt64* self)
{
    SELF->size = 0;
    SELF->na.size = 0;
    SELF->col_size = 0;
}

static bool
TryGet(
    DataFrame_ColumnUInt64* self, size_t index, uint64_t* v)
{
    bool na = DataFrame_BitVector_Get(&SELF->na, index);
    if(na) return false;
    *v = SELF->data[index];
    return true;
}

static const char*
Resize(DataFrame_ColumnUInt64Impl* self)
{
    uint64_t* d;
    size_t newSize;

    newSize = SELF->size * 2;
    if(!newSize) newSize = 4;

    d = realloc(SELF->data, newSize * sizeof(uint64_t));
    if(!d) return "DataFrame_ColumnInt8_Add: Out of memory";
    SELF->data = d;
    SELF->capacity = newSize;

    return NULL;
}

static int setSelInt8(uint64_t * data, int8_t * selection_column, int8_t  selection, size_t size, uint64_t * dst) {
    int j = 0;
    int i = 0;
    uint64_t dat;
    uint64_t select_val;
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
static int setSelInt32(uint64_t * data, int32_t * selection_column, int32_t  selection, size_t size, uint64_t * dst) {
    int j = 0;
    int i = 0;
    uint64_t dat;
    uint64_t select_val;
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
AddDateTime(DataFrame_ColumnUInt64* self, long datetime)
{
   
    return NULL;
}
static uint64_t 
GetValue(DataFrame_ColumnUInt64* self, size_t index)
{
    return SELF->data[index];
}
static const char*
Add(DataFrame_ColumnUInt64* self, uint64_t v)
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
AddColValues(DataFrame_ColumnUInt64* self, uint64_t * v, uint64_t size)
{
        SELF->data = v;
	      SELF->col_size = size;
	      SELF->size = size;
	      SELF->capacity = size;
        return NULL;
}

static void
Set(DataFrame_ColumnUInt64* self, size_t i, uint64_t v)
{
    DataFrame_BitVector_Set(&SELF->na, i, false);
    SELF->data[i] = v;
}

static bool iis_unique(uint64_t * seen_val, uint64_t new_val, size_t size) { 
	for(int k = 0; k < size; k++) { 
		if (new_val == seen_val[k]) {
			return false;
		}
	}
	return true;
}
static int setUnique(uint64_t * val, uint64_t * data, size_t size) {

    int j = 0; 
    int i = 0;
    uint64_t dt;
    for (i = 0; i < size; i++) {
      dt = data[i];
      if (iis_unique(val, dt, j)) {
        val[j] = dt;
        j++;
      } 
    }
    return j;
}
static uint64_t*
GetUnique(DataFrame_ColumnUInt64* self)
{
     //__attribute__((annotate("local_malloc")))
    uint64_t * val = (uint64_t *) malloc(SELF->size * sizeof(uint64_t));

    SELF->unique_size = setUnique(val, SELF->data, SELF->col_size);

    return val;
}
static uint64_t* 
GetBySelInt32(DataFrame_ColumnUInt64* self, int32_t * selection_column, int32_t  selection) {

//     __attribute__((annotate("local_malloc")))
	uint64_t * val = (uint64_t *) malloc(SELF->size * sizeof(uint64_t));
	uint64_t * data = SELF->data;

	SELF->selection_size = setSelInt32(data, selection_column, selection, SELF->size, val); 
	return val;
}

static uint64_t* 
GetBySelInt8(DataFrame_ColumnUInt64* self, int8_t * selection_column, int8_t  selection) {

     //__attribute__((annotate("local_malloc")))
    uint64_t * val = (uint64_t *) malloc(SELF->size * sizeof(uint64_t));
    uint64_t * data = SELF->data;

    SELF->selection_size = setSelInt8(data, selection_column, selection, SELF->size,  val); 

    return val;
}


static size_t GetGE(uint64_t  selection, uint64_t * dst,  uint64_t * data, size_t size) {
    int i = 0;
    int j = 0;

    uint64_t dat;
    uint64_t select_val;

    for (i = 0; i < size; i++) {
      dat = data[i];
      if (dat >= selection) {
        dst[j] = dat;
        j++;
      } 
    }
    return j;
}

static uint64_t* 
GetBySelGE(DataFrame_ColumnUInt64* self, uint64_t  selection) {

     //__attribute__((annotate("local_malloc")))
    uint64_t * val = (uint64_t *) malloc(SELF->size * sizeof(uint64_t));
    uint64_t * data = SELF->data;

    SELF->selection_size = GetGE(selection,  val,  data, SELF->col_size);

    return val;
}

static void group_BY_col(uint64_t * data, int8_t * group_column, 
		uint64_t ** groupby, int64_t  * group_size, int8_t * groups, size_t size, size_t ngroups) { 

    int group_index;
    int group_item_size;
    uint64_t dat;
    int8_t select_val;
    uint64_t * group_item;
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
static uint64_t** 
GetBySelInt8Group(DataFrame_ColumnUInt64* self, int8_t * group_column, int8_t  * groups, int8_t ngroups, uint64_t * copy_data) {

    uint64_t * data = copy_data;

    int i = 0;
//     __attribute__((annotate("local_malloc")))
    uint64_t ** groupby = (uint64_t **) malloc(ngroups * sizeof(uint64_t *));

   int64_t * group_size = (int64_t *) malloc(ngroups * sizeof(int64_t));

   for (i = 0; i < ngroups; i++) {
	groupby[i] = malloc(SELF->size * sizeof(uint64_t));
	group_size[i] = 0;
   }

    group_BY_col(data, group_column, groupby, group_size, groups, SELF->col_size, ngroups); 

    SELF->group_size = group_size; 
    return groupby;
}
#endif

static uint64_t* 
GetBySelString(DataFrame_ColumnUInt64* self, char ** selection, char * select_val) {

    //__attribute__((annotate("local_malloc")))
    uint64_t * val = (uint64_t *) malloc(SELF->col_size * sizeof(uint64_t));

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

static uint64_t* 
GetBySelStringUnique(DataFrame_ColumnUInt64* self, char ** selection_column, char * selection) {

    int j = 0; 
   // __attribute__((annotate("local_malloc")))
    uint64_t * val = (uint64_t *) malloc(SELF->col_size * sizeof(uint64_t));

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

static int setSelInt8Unique(uint64_t * val, uint64_t * data, int8_t * selection_column, int8_t selection, size_t size) {

    uint64_t dat;
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
static uint64_t* 
GetBySelInt8Unique(DataFrame_ColumnUInt64* self, int8_t * selection_column, int8_t selection) {

    uint64_t * val = (uint64_t *) malloc(SELF->col_size * sizeof(uint64_t));
    uint64_t * data = SELF->data;
   // __attribute__((annotate("local_malloc")))
    SELF->selection_size = setSelInt8Unique(val, data, selection_column, selection, SELF->size);
    return val;
}
static uint64_t*
GetAll(DataFrame_ColumnUInt64* self)
{
    return SELF->data;
}

static void copy_col(uint64_t * dst, uint64_t * src, uint64_t size) {
	for (int i = 0; i < size; i++) {
		dst[i] = src[i]; 
	}
}

static uint64_t*
GetCopy(DataFrame_ColumnUInt64* self)
{
    int i = 0;
    uint64_t * copy = (uint64_t *) malloc(SELF->col_size * sizeof(uint64_t));
    uint64_t * data = SELF->data;

    copy_col(copy, data, SELF->size);
    return copy;
}


static int64_t * 
GetBySelInt8GroupSize(DataFrame_ColumnUInt64* self) {
    return SELF->group_size;
}
static const char*
AddNA(DataFrame_ColumnUInt64* self)
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
Remove(DataFrame_ColumnUInt64* self, size_t i)
{
    DataFrame_BitVector_Remove(&SELF->na, i);
    memmove(&SELF->data[i], &SELF->data[i+1], SELF->size - i - 1);
    SELF->size--;
    SELF->col_size--;
}


static void
SetNA(DataFrame_ColumnUInt64* self, size_t i)
{
    DataFrame_BitVector_Set(&SELF->na, i, true);
}

static char*
GetName(DataFrame_ColumnUInt64* self)
{ 
    return (char *)SELF->name;
}

static char*
SetName(DataFrame_ColumnUInt64* self, char* name)
{
    char* n;

    if(!name)
    {
        SELF->name = NULL;
        return NULL;
    }

    n = strdup(name);
    if(!n) return "DataFrame_ColumnUInt64_SetName: Out of memory";

    SELF->name = n;
    return NULL;
}

static bool
HasValue(DataFrame_ColumnUInt64* self, size_t index)
{
    bool na = DataFrame_BitVector_Get(&SELF->na, index);
    return !na;
}

static uint64_t *
Get(DataFrame_ColumnUInt64* self, size_t index)
{
    return &SELF->data[index];
}

static uint8_t*
GetNAs(DataFrame_ColumnUInt64* self)
{
    return SELF->na.data;
}

static int
GetUniqueSize(DataFrame_ColumnUInt64* self)
{
    return SELF->unique_size;
}

static int
GetColSize(DataFrame_ColumnUInt64* self)
{
    return SELF->col_size;
}
static int
GetSelSize(DataFrame_ColumnUInt64* self)
{
    return SELF->selection_size;
}
static DataFrame_ColumnUInt64Methods UInt64Methods =
{
/* shared */
    DataFrame_Type_ColumnUInt64,
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


DataFrame_ColumnUInt64* DataFrame_ColumnUInt64_New(int type)
{
    DataFrame_ColumnUInt64Impl* c;

    c = calloc(1, sizeof(DataFrame_ColumnUInt64Impl));
    if(!c) return NULL;
    c->methods = &UInt64Methods;
    c->ref_count = 1;
    c->type = type;
    return (DataFrame_ColumnUInt64*)c;
}
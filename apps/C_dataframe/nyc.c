#include "dataframe/dataframe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define assert(x) do { if(!(x)) printf("%s:%d Assertion failed %s", \
                                       __FILE__, __LINE__, #x); } while(0)

#define set_columns(df)  \
    DataFrame_ColumnInt32* c1 = DataFrame_ColumnInt32_New(INT32); \
    DataFrame_ColumnInt64 * c2 = DataFrame_ColumnInt64_New(DATETIME);\
    DataFrame_ColumnInt64 * c3 = DataFrame_ColumnInt64_New(DATETIME);\
    DataFrame_ColumnInt32* c4 = DataFrame_ColumnInt32_New(INT32);\
    DataFrame_ColumnFloat64* c5 = DataFrame_ColumnFloat64_New(FLOAT64);\
    DataFrame_ColumnFloat64* c6 = DataFrame_ColumnFloat64_New(FLOAT64);\
    DataFrame_ColumnFloat64* c7 = DataFrame_ColumnFloat64_New(FLOAT64);\
    DataFrame_ColumnInt32* c8 = DataFrame_ColumnInt32_New(INT32);\
    DataFrame_ColumnInt8 * c9 = DataFrame_ColumnInt8_New(CHAR); \
    DataFrame_ColumnFloat64* c10 = DataFrame_ColumnFloat64_New(FLOAT64);\
    DataFrame_ColumnFloat64* c11 = DataFrame_ColumnFloat64_New(FLOAT64);\
    DataFrame_ColumnInt32* c12 = DataFrame_ColumnInt32_New(INT32);\
    DataFrame_ColumnFloat64* c13 = DataFrame_ColumnFloat64_New(FLOAT64);\
    DataFrame_ColumnFloat64* c14 = DataFrame_ColumnFloat64_New(FLOAT64);\
    DataFrame_ColumnFloat64* c15 = DataFrame_ColumnFloat64_New(FLOAT64);\
    DataFrame_ColumnFloat64* c16 = DataFrame_ColumnFloat64_New(FLOAT64);\
    DataFrame_ColumnFloat64* c17 = DataFrame_ColumnFloat64_New(FLOAT64);\
    DataFrame_ColumnFloat64* c18 = DataFrame_ColumnFloat64_New(FLOAT64);\
    DataFrame_ColumnFloat64* c19 = DataFrame_ColumnFloat64_New(FLOAT64);\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c1));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c2));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c3));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c4));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c5));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c6));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c7));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c8));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c9));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c10));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c11));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c12));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c13));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c14));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c15));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c16));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c17));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c18));\
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c19));


#define set_column_names(col_cnt)  \
				switch (col_cnt) {\
					case 0:\
						c1->methods->SetName(c1, tok);\
						break;\
					case 1:\
						c2->methods->SetName(c2, tok);\
						break;\
					case 2:\
						c3->methods->SetName(c3, tok);\
						break;\
					case 3:\
						c4->methods->SetName(c4, tok);\
						break;\
					case 4:\
						c5->methods->SetName(c5, tok);\
						break;\
					case 5:\
						c6->methods->SetName(c6, tok);\
						break;\
					case 6:\
						c7->methods->SetName(c7, tok);\
						break;\
					case 7:\
						c8->methods->SetName(c8, tok);\
						break;\
					case 8:\
						c9->methods->SetName(c9, tok);\
						break;\
					case 9:\
						c10->methods->SetName(c10, tok);\
						break;\
					case 10:\
						c11->methods->SetName(c11, tok);\
						break;\
					case 11:\
						c12->methods->SetName(c12, tok);\
						break;\
					case 12:\
						c13->methods->SetName(c13, tok);\
						break;\
					case 13:\
						c14->methods->SetName(c14, tok);\
						break;\
					case 14:\
						c15->methods->SetName(c15, tok);\
						break;\
					case 15:\
						c16->methods->SetName(c16, tok);\
						break;\
					case 16:\
						c17->methods->SetName(c17, tok);\
						break;\
					case 17:\
						c18->methods->SetName(c18, tok);\
						break;\
					case 18:\
						c19->methods->SetName(c19, tok);\
						break;\
				}
#define parse_column_values(col_cnt)  \
				switch (col_cnt) {\
					case 0:\
						c1->methods->Add(c1, atoi(tok));\
						break;\
					case 1: {\
            parse_date(tok, &dat);\
            ld = (long *)&dat;\
						c2->methods->Add(c2, *ld);\
						break;\
                  }\
					case 2:{\
            parse_date(tok, &dat);\
            ld = (long *)&dat;\
						c3->methods->Add(c3, *ld);\
						break;\
                 }\
					case 3:\
						c4->methods->Add(c4, atoi(tok));\
						break;\
					case 4:\
						c5->methods->Add(c5, atof(tok));\
						break;\
					case 5:\
						c6->methods->Add(c6, atof(tok));\
						break;\
					case 6:\
						c7->methods->Add(c7, atof(tok));\
						break;\
					case 7:\
						c8->methods->Add(c8, atoi(tok));\
						break;\
					case 8:\
						c9->methods->Add(c9, (int8_t)tok[0]);\
						break;\
					case 9:\
						c10->methods->Add(c10, atof(tok));\
						break;\
					case 10:\
						c11->methods->Add(c11, atof(tok));\
						break;\
					case 11:\
						c12->methods->Add(c12, atoi(tok));\
						break;\
					case 12:\
						c13->methods->Add(c13, atof(tok));\
						break;\
					case 13:\
						c14->methods->Add(c14, atof(tok));\
						break;\
					case 14:\
						c15->methods->Add(c15, atof(tok));\
						break;\
					case 15:\
						c16->methods->Add(c16, atof(tok));\
						break;\
					case 16:\
						c17->methods->Add(c17, atof(tok));\
						break;\
					case 17:\
						c18->methods->Add(c18, atof(tok));\
						break;\
					case 18:\
						c19->methods->Add(c19, atof(tok));\
						break;\
				}

#define parse_time(col_cnt) {\
			ret = time->year;\
			ret = (12 * ret) + time->month;\
			ret = (30 * ret) + time->day;\
			ret = (24 * ret) + time->hour;\
			ret = (60 * ret) + time->minute;\
			ret = (60 * ret) + time->second;\
}



// Utility function to swapping of element
#define swap(c,d)\
{\
    temp = *c;\
    *c = *d;\
    *d = temp;\
}
 
#define YEAR   0
#define MONTH  1
#define DAY    2
#define HOUR   3
#define MINUTE 4
#define SECOND 5
struct date {
  short year;
  char day;
  char month;
  char hour;
  char minute;
  char second;
};
#define BYTES_MASK(bytes) ((1<<(bytes * 8)) - 1)
#define BYTE_MASK BYTES_MASK(1)

static uint64_t inline get_datetime(struct date * dt) {
  return   (((uint64_t)dt->year << 40ULL) | ((uint64_t)dt->month << 32ULL) | ((uint64_t)dt->day << 24ULL) | ((uint64_t)dt->hour << 16ULL) | ((uint64_t)dt->minute << 8ULL) | (dt->second));
}

#define MICRO 1000000
double get_time_us() {

	struct timespec start;
	clock_gettime( CLOCK_BOOTTIME, &start );
	return (start.tv_sec * MICRO) + (start.tv_nsec/1000);
}
 static inline void parse_date_time(uint64_t date_time, struct date * dt)  {
    dt->year = (date_time >> 40ULL) & BYTES_MASK(3);
    dt->month  = (date_time >> 32ULL) & BYTE_MASK;
    dt->day  = (date_time >> 24ULL) & BYTE_MASK;
    dt->hour = (date_time >> 16ULL) & BYTE_MASK;
    dt->minute = (date_time >> 8ULL) & BYTE_MASK;
    dt->second = date_time & BYTE_MASK;
//    dt->date_time = date_time;
}
static inline void print_date_time(struct date * datetime, bool newline) {
  printf("%d-%d-%d %d:%d:%d ", datetime->year, datetime->month, datetime->day, datetime->hour, datetime->minute, datetime->second);
  if (newline)
    printf("\n");
}
// Returns the correct position of
// pivot element
static inline int Partition(int64_t arr[], int l, int r)
{
    int64_t temp;
    int64_t lst = arr[r], i = l, j = l;
    while (j < r) {
        if (arr[j] < lst) {
            swap(&arr[i], &arr[j]);
            i++;
        }
        j++;
    }
    swap(&arr[i], &arr[r]);
    return i;
}
 
// Picks a random pivot element between
// l and r and partitions arr[l..r]
// around the randomly picked element
// using partition()
static inline int randomPartition(int64_t arr[], int l,
                    int r)
{
    int64_t temp;
    int n = r - l + 1;
    int64_t pivot = rand() % n;
    swap(&arr[l + pivot], &arr[r]);
    return Partition(arr, l, r);
}
 
// Utility function to find median
void MedianUtil(int64_t arr[], int l, int r, int k, int64_t * a, int64_t * b)
{
 
    // if l < r
    if (l <= r) {
 
        // Find the partition index
        int partitionIndex
            = randomPartition(arr, l, r);
 
        // If partition index = k, then
        // we found the median of odd
        // number element in arr[]
        if (partitionIndex == k) {
            *b = arr[partitionIndex];
            if (*a != -1)
                return;
        }
 
        // If index = k - 1, then we get
        // a & b as middle element of
        // arr[]
        else if (partitionIndex == k - 1) {
            *a = arr[partitionIndex];
            if (*b != -1)
                return;
        }
 
        // If partitionIndex >= k then
        // find the index in first half
        // of the arr[]
        if (partitionIndex >= k)
            return MedianUtil(arr, l,partitionIndex - 1,k, a, b);
 
        // If partitionIndex <= k then
        // find the index in second half
        // of the arr[]
        else
            return MedianUtil(arr,partitionIndex + 1,r, k, a, b);
    }
 
    return;
}
 
// Function to find Median
int64_t get_median(int64_t arr[], int n)
{
    int64_t median, a = -1, b = -1;
 
    // If n is odd
    if (n % 2 == 1) {
        MedianUtil(arr, 0, n - 1, n / 2, &a, &b);
        median = b;
    }
 
    // If n is even
    else {
        MedianUtil(arr, 0, n - 1, n / 2, &a, &b);
        median = (a + b) / 2;
    }
 
    // Print the Median of arr[]
    return median;
}


static inline void PRINT_X_ROWS_INT64(DataFrame_ColumnInt64* col, int x) {

  char pstr[4096];
  char * colname = col->methods->GetName(col);

  int64_t * colval = col->methods->GetAll(col);
  strcpy(pstr, col->methods->GetName(col));
	printf("%s:{", pstr);
	for (int i = 0; i < x; i++) {
		printf("%d,", colval[i]);
	}
	printf("},\n");

}

static inline void add_FLOAT64_values(DataFrame_ColumnFloat64* newcol, DataFrame_ColumnFloat64* oldcol, int * index, int size) {

	double * col_values = malloc(size * sizeof(double));

	double * oldcolval = oldcol->methods->GetAll(oldcol); 

	for (int i = 0; i < size; i++) {
		col_values[i] = oldcolval[index[i]]; 
	}
	
	newcol->methods->AddColValues(newcol, col_values, size); 
}
static inline void add_INT64_values(DataFrame_ColumnInt64* newcol, DataFrame_ColumnInt64* oldcol, int * index, int size) {

	int64_t * col_values = malloc(size * sizeof(int64_t));

	 int64_t * oldcolval = oldcol->methods->GetAll(oldcol); 

	for (int i = 0; i < size; i++) {
		col_values[i] = oldcolval[index[i]]; 
	}
	
	newcol->methods->AddColValues(newcol, col_values, size); 
}

static inline void add_DATETIME_values(DataFrame_ColumnInt64 * newcol, DataFrame_ColumnInt64* oldcol, int * index, int size) {

	long * col_values = malloc(size * sizeof(long));

	long * oldcolval = (long *)oldcol->methods->GetAll(oldcol); 

	for (int i = 0; i < size; i++) {
		col_values[i] = oldcolval[index[i]]; 
	}
	
	newcol->methods->AddColValues(newcol, (int64_t *)col_values, size); 
}
static inline void add_INT8_values(DataFrame_ColumnInt8* newcol, DataFrame_ColumnInt8* oldcol, int * index, int size) {

	int8_t * col_values = malloc(size * sizeof(int8_t));

	 int8_t * oldcolval = oldcol->methods->GetAll(oldcol); 

	for (int i = 0; i < size; i++) {
		col_values[i] = oldcolval[index[i]]; 
	}
	
	newcol->methods->AddColValues(newcol, col_values, size); 
}

static inline void add_INT32_values(DataFrame_ColumnInt32* newcol, DataFrame_ColumnInt32* oldcol, int * index, int size) {

	int32_t * col_values = malloc(size * sizeof(int32_t));

	 int32_t * oldcolval = oldcol->methods->GetAll(oldcol); 

	for (int i = 0; i < size; i++) {
		col_values[i] = oldcolval[index[i]]; 
	}
	
	newcol->methods->AddColValues(newcol, col_values, size); 
}
static inline void PRINT_X_ROWS_DATETIME(DataFrame_ColumnInt64* col, int x) {

  char pstr[4096];

  int64_t * colval = col->methods->GetAll(col);
  strcpy(pstr, col->methods->GetName(col));
	printf("%s:{", pstr);
  struct date * date_time;
	for (int i = 0; i < x; i++) {
    //parse_date_time(colval[i], &date_time);
    date_time = (struct date *)&colval[i];
    print_date_time(date_time, false);
	}
	printf("},\n");

}
static inline void PRINT_X_ROWS_INT(DataFrame_ColumnInt32* col, int x) {

  char pstr[4096];

  int * colval = col->methods->GetAll(col);

  char * colname = col->methods->GetName(col);
  strcpy(pstr, colname);

	printf("%s:{", pstr);

	for (int i = 0; i < x; i++) {
		printf("%d,", colval[i]);
	}
	printf("},\n");

}
static inline void PRINT_X_ROWS_CHAR(DataFrame_ColumnInt8* col, int x) {

  char pstr[4096];

  int8_t * colval = col->methods->GetAll(col);
  strcpy(pstr, col->methods->GetName(col));
	printf("%s:{", pstr);
	for (int i = 0; i < x; i++) {
		printf("%c,", (char)colval[i]);
	}
	printf("},\n");

}
static inline void PRINT_X_ROWS_INT8(DataFrame_ColumnInt8* col, int x) {

  char pstr[4096];

  int8_t * colval = col->methods->GetAll(col);
  strcpy(pstr, col->methods->GetName(col));
	printf("%s:{", pstr);
	for (int i = 0; i < x; i++) {
		printf("%d,", colval[i]);
	}
	printf("},\n");

}
static inline void PRINT_X_ROWS_STRING(DataFrame_ColumnString* col, int x) {

  char pstr[4096];

  char ** colval = col->methods->GetAll(col);
  strcpy(pstr, col->methods->GetName(col));
  printf("%s", pstr);

  char ptr[4096];
  printf("%s:{", pstr);
	for (int i = 0; i < x; i++) {
    strcpy(pstr, colval[i]);
    printf("%s", pstr);
	}
	printf("},\n");

}

static inline void PRINT_X_ROWS_FLOAT(DataFrame_ColumnFloat64 * col, int x) {

  char pstr[4096];

  double * colval = col->methods->GetAll(col);
  strcpy(pstr, col->methods->GetName(col));
	printf("%s:{", pstr);
	for (int i = 0; i < x; i++) {
		printf("%f,", colval[i]);
	}
	printf("},\n");

}

static inline void PRINT_COLUMNS(DataFrame * df, int nvalues)
{
    DataFrame_Column * c = NULL;             
    DataFrame_ColumnInt8* c0 = NULL;             
    DataFrame_ColumnInt32* c1 = NULL;             
    DataFrame_ColumnString* c2 = NULL;             
    DataFrame_ColumnFloat64* c3 = NULL;             
    DataFrame_ColumnInt64 * c4 = NULL;             
    int ncol = df->methods->ColumnSize(df);

    for (int j = 0; j < ncol; j++) {
	    c = df->methods->GetColumnRefByIndex(df, j);
	    switch(c->type) {
		    case INT32:
			    c1 = (DataFrame_ColumnInt32*) c;
			    PRINT_X_ROWS_INT(c1, nvalues);
			    break;
		    case INT64:
			    c4 = (DataFrame_ColumnInt64*) c;
			    PRINT_X_ROWS_INT64(c4, nvalues);
			    break;
		    case STRING:
			    c2 = (DataFrame_ColumnString*) c;
			    PRINT_X_ROWS_STRING(c2, nvalues);
			    break;
		    case INT8:
			    c0 = (DataFrame_ColumnInt8*) c;
			    PRINT_X_ROWS_INT8(c0, nvalues);
			    break;
		    case CHAR:
			    c0 = (DataFrame_ColumnInt8*) c;
			    PRINT_X_ROWS_CHAR(c0, nvalues);
			    break;
		    case FLOAT64:
			    c3 = (DataFrame_ColumnFloat64*) c;
			    PRINT_X_ROWS_FLOAT(c3, nvalues);
			    break;
		    case DATETIME:
			    c4 = (DataFrame_ColumnInt64*) c;
			    PRINT_X_ROWS_DATETIME(c4, nvalues);
			    break;
	    }
    }
}

static inline void free_df(DataFrame * df1)
{
	DataFrame_Column * new_column = NULL;             
	DataFrame_ColumnInt8* nc0 = NULL;             
	DataFrame_ColumnInt32* nc1 = NULL;             
	DataFrame_ColumnString* nc2 = NULL;             
	DataFrame_ColumnFloat64* nc3 = NULL;             
	DataFrame_ColumnInt64 * nc4 = NULL;             
	int ncol = df1->methods->ColumnSize(df1);

	for (int k = 0; k < ncol; k++) {
		new_column = df1->methods->GetColumnRefByIndex(df1, k);
		switch(new_column->type) {

			case INT64:
				nc4 = (DataFrame_ColumnInt64*) new_column;
				free(nc4->methods->GetAll(nc4));
				break;
			case INT32:
				nc1 = (DataFrame_ColumnInt32*) new_column;
				free(nc1->methods->GetAll(nc1));
				break;
			case INT8:
				nc0 = (DataFrame_ColumnInt8*) new_column;
				free(nc0->methods->GetAll(nc0));
				break;
#if 1
			case CHAR:
				nc0 = (DataFrame_ColumnInt8*) new_column;
				free(nc0->methods->GetAll(nc0));
				break;
			case FLOAT64:
				nc3 = (DataFrame_ColumnFloat64*) new_column;
				free(nc3->methods->GetAll(nc3));
				break;
			case DATETIME:
				nc2 = (DataFrame_ColumnString*) new_column;
				free(nc2->methods->GetAll(nc2));
				break;
#endif
		}
	}
	free(df1);
}
static inline DataFrame* GET_SELGE(DataFrame * origdf, double select_val, double * selection, size_t size)
{
	DataFrame_Column * orig_column = NULL;             
	DataFrame_Column * new_column = NULL;             
	DataFrame_ColumnInt8* oc0 = NULL;             
	DataFrame_ColumnInt32* oc1 = NULL;             
	DataFrame_ColumnString* oc2 = NULL;             
	DataFrame_ColumnFloat64* oc3 = NULL;             
	DataFrame_ColumnInt64 * oc4 = NULL;             
	DataFrame_ColumnInt8* nc0 = NULL;             
	DataFrame_ColumnInt32* nc1 = NULL;             
	DataFrame_ColumnString* nc2 = NULL;             
	DataFrame_ColumnFloat64* nc3 = NULL;             
	DataFrame_ColumnInt64 * nc4 = NULL;             
	int ncol = origdf->methods->ColumnSize(origdf);

	int j = 0;
	int * indexes = malloc(size * sizeof(int));
	DataFrame_Column cols[ncol];

    	DataFrame* df1 = DataFrame_New();

	set_columns(df1);
    	DataFrame_ColumnInt64* c20 = DataFrame_ColumnInt64_New(INT64);
	assert(!df1->methods->AddColumn(df1, (DataFrame_Column*)c20));
    	DataFrame_ColumnFloat64* c21 = DataFrame_ColumnFloat64_New(FLOAT64);
	assert(!df1->methods->AddColumn(df1, (DataFrame_Column*)c21));

	for (int i = 0; i < size; i++) {

		if (selection[i] > select_val) {

			indexes[j] = i; 
			j++;
		}
	}

//	printf("Col Size %d\n", j);

	for (int k = 0; k < ncol; k++) {
		orig_column = origdf->methods->GetColumnRefByIndex(origdf, k);
		new_column = df1->methods->GetColumnRefByIndex(df1, k);
		switch(new_column->type) {

			case INT64:
				oc4 = (DataFrame_ColumnInt64*) orig_column;
				nc4 = (DataFrame_ColumnInt64*) new_column;
				add_INT64_values(nc4, oc4, indexes, j);
				break;
			case INT32:
				oc1 = (DataFrame_ColumnInt32*) orig_column;
				nc1 = (DataFrame_ColumnInt32*) new_column;
				add_INT32_values(nc1, oc1, indexes, j);
				break;
			case INT8:
				oc0 = (DataFrame_ColumnInt8*) orig_column;
				nc0 = (DataFrame_ColumnInt8*) new_column;
				add_INT8_values(nc0, oc0, indexes, j);
				break;
#if 1
			case CHAR:
				oc0 = (DataFrame_ColumnInt8*) orig_column;
				nc0 = (DataFrame_ColumnInt8*) new_column;
				add_INT8_values(nc0, oc0, indexes, j);
				break;
			case FLOAT64:
				oc3 = (DataFrame_ColumnFloat64*) orig_column;
				nc3 = (DataFrame_ColumnFloat64*) new_column;
				add_FLOAT64_values(nc3, oc3, indexes, j);
				break;
			case DATETIME:
				oc4 = (DataFrame_ColumnInt64*) orig_column;
				nc4 = (DataFrame_ColumnInt64*) new_column;
				add_DATETIME_values(nc4, oc4, indexes, j);
				break;
#endif
		}
	}
	free(indexes);

	return df1;
}
static inline DataFrame* GET_SEL_INT32(DataFrame * origdf, int32_t select_val, int32_t * selection, size_t size)
{
	DataFrame_Column * orig_column = NULL;             
	DataFrame_Column * new_column = NULL;             
	DataFrame_ColumnInt8* oc0 = NULL;             
	DataFrame_ColumnInt32* oc1 = NULL;             
	DataFrame_ColumnString* oc2 = NULL;             
	DataFrame_ColumnFloat64* oc3 = NULL;             
	DataFrame_ColumnInt64 * oc4 = NULL;             
	DataFrame_ColumnInt8* nc0 = NULL;             
	DataFrame_ColumnInt32* nc1 = NULL;             
	DataFrame_ColumnString* nc2 = NULL;             
	DataFrame_ColumnFloat64* nc3 = NULL;             
	DataFrame_ColumnInt64 * nc4 = NULL;             
	int ncol = origdf->methods->ColumnSize(origdf);

	int j = 0;
	int * indexes = malloc(size * sizeof(int));
	DataFrame_Column cols[ncol];

    	DataFrame* df1 = DataFrame_New();

	set_columns(df1);

	for (int i = 0; i < size; i++) {

		if (selection[i] == select_val) {

			indexes[j] = i; 
			j++;
		}
	}

//	printf("Col Size %d\n", j);

	for (int k = 0; k < ncol; k++) {
		orig_column = origdf->methods->GetColumnRefByIndex(origdf, k);
		new_column = df1->methods->GetColumnRefByIndex(df1, k);
		switch(new_column->type) {

			case INT64:
				oc4 = (DataFrame_ColumnInt64*) orig_column;
				nc4 = (DataFrame_ColumnInt64*) new_column;
				add_INT64_values(nc4, oc4, indexes, j);
				break;
			case INT32:
				oc1 = (DataFrame_ColumnInt32*) orig_column;
				nc1 = (DataFrame_ColumnInt32*) new_column;
				add_INT32_values(nc1, oc1, indexes, j);
				break;
			case INT8:
				oc0 = (DataFrame_ColumnInt8*) orig_column;
				nc0 = (DataFrame_ColumnInt8*) new_column;
				add_INT8_values(nc0, oc0, indexes, j);
				break;
#if 1
			case CHAR:
				oc0 = (DataFrame_ColumnInt8*) orig_column;
				nc0 = (DataFrame_ColumnInt8*) new_column;
				add_INT8_values(nc0, oc0, indexes, j);
				break;
			case FLOAT64:
				oc3 = (DataFrame_ColumnFloat64*) orig_column;
				nc3 = (DataFrame_ColumnFloat64*) new_column;
				add_FLOAT64_values(nc3, oc3, indexes, j);
				break;
			case DATETIME:
				oc4 = (DataFrame_ColumnInt64*) orig_column;
				nc4 = (DataFrame_ColumnInt64*) new_column;
				add_DATETIME_values(nc4, oc4, indexes, j);
				break;
#endif
		}
	}
	free(indexes);

	return df1;
}
static inline DataFrame* GET_SEL(DataFrame * origdf, int8_t select_val, int8_t * selection, size_t size)
{
	DataFrame_Column * orig_column = NULL;             
	DataFrame_Column * new_column = NULL;             
	DataFrame_ColumnInt8* oc0 = NULL;             
	DataFrame_ColumnInt32* oc1 = NULL;             
	DataFrame_ColumnString* oc2 = NULL;             
	DataFrame_ColumnFloat64* oc3 = NULL;             
	DataFrame_ColumnInt64 * oc4 = NULL;             
	DataFrame_ColumnInt8* nc0 = NULL;             
	DataFrame_ColumnInt32* nc1 = NULL;             
	DataFrame_ColumnString* nc2 = NULL;             
	DataFrame_ColumnFloat64* nc3 = NULL;             
	DataFrame_ColumnInt64 * nc4 = NULL;             
	int ncol = origdf->methods->ColumnSize(origdf);

	int j = 0;
	int * indexes = malloc(size * sizeof(int));
	DataFrame_Column cols[ncol];

    	DataFrame* df1 = DataFrame_New();

	set_columns(df1);
    	DataFrame_ColumnInt64* c20 = DataFrame_ColumnInt64_New(INT64);
	assert(!df1->methods->AddColumn(df1, (DataFrame_Column*)c20));

	for (int i = 0; i < size; i++) {

		if (selection[i] == select_val) {

			indexes[j] = i; 
			j++;
		}
	}

//	printf("Col Size %d\n", j);

	for (int k = 0; k < ncol; k++) {
		orig_column = origdf->methods->GetColumnRefByIndex(origdf, k);
		new_column = df1->methods->GetColumnRefByIndex(df1, k);
		switch(new_column->type) {

			case INT64:
				oc4 = (DataFrame_ColumnInt64*) orig_column;
				nc4 = (DataFrame_ColumnInt64*) new_column;
				add_INT64_values(nc4, oc4, indexes, j);
				break;
			case INT32:
				oc1 = (DataFrame_ColumnInt32*) orig_column;
				nc1 = (DataFrame_ColumnInt32*) new_column;
				add_INT32_values(nc1, oc1, indexes, j);
				break;
			case INT8:
				oc0 = (DataFrame_ColumnInt8*) orig_column;
				nc0 = (DataFrame_ColumnInt8*) new_column;
				add_INT8_values(nc0, oc0, indexes, j);
				break;
#if 1
			case CHAR:
				oc0 = (DataFrame_ColumnInt8*) orig_column;
				nc0 = (DataFrame_ColumnInt8*) new_column;
				add_INT8_values(nc0, oc0, indexes, j);
				break;
			case FLOAT64:
				oc3 = (DataFrame_ColumnFloat64*) orig_column;
				nc3 = (DataFrame_ColumnFloat64*) new_column;
				add_FLOAT64_values(nc3, oc3, indexes, j);
				break;
			case DATETIME:
				oc4 = (DataFrame_ColumnInt64*) orig_column;
				nc4 = (DataFrame_ColumnInt64*) new_column;
				add_DATETIME_values(nc4, oc4, indexes, j);
				break;
#endif
		}
	}
	free(indexes);

	return df1;
}
 void parse_date(char * time, struct date * dt)  {

	 char ptime[512];
	 char date[512];
	 char * tok;
	 int col_cnt = 0;
	 char ttime[512];
   char * saveptr;
	 strcpy(ttime, time);
	 for (tok = strtok_r(ttime, " ", &saveptr);
			 tok && *tok;
			 tok = strtok_r(NULL, " \n", &saveptr))
	 {
		 if (col_cnt == 0)
	 		strcpy(date, tok);
		 else
	 		strcpy(ptime, tok);
		 col_cnt++;
	 }

	 col_cnt = 0;
	 for (tok = strtok_r(date, "-", &saveptr);
			 tok && *tok;
			 tok = strtok_r(NULL, "-\n", &saveptr))
	 {
		 if (col_cnt == MONTH)
       dt->month = atoi(tok);
		 if (col_cnt == DAY)
       dt->day = atoi(tok);
		 if (col_cnt == YEAR)
       dt->year = atoi(tok);
		 col_cnt++;
	 }
		 for (tok = strtok_r(ptime, ":", &saveptr);
				 tok && *tok;
				 tok = strtok_r(NULL, ":\n", &saveptr))
		 {
			 if (col_cnt == HOUR)
         dt->hour = atoi(tok);
			 if (col_cnt == MINUTE)
         dt->minute = atoi(tok);
			 if (col_cnt == SECOND)
         dt->second = atoi(tok);
			 col_cnt++;
		 }


 }

void read_csv(DataFrame * df) {

    struct date dat;
    FILE* stream = fopen("all.csv", "r");

    char * line;
    int ncol = 19;
    int col_cnt = 0;
    ssize_t read;
    size_t len = 0;
    long * ld;

    char * saveptr;
    DataFrame_Column cols[ncol];

    set_columns(df);
    
    bool column_names = true;
    while ((read = getline(&line, &len, stream)) != -1)
    {
	    char* tok;
	    for (tok = strtok_r(line, ",", &saveptr);
			    tok && *tok;
			    tok = strtok_r(NULL, ",\r\n", &saveptr))
	    {
		    if (column_names) {
			    set_column_names(col_cnt)
		    }
		    else
		    parse_column_values(col_cnt);

		    col_cnt++;
		    if (col_cnt == ncol)
			    column_names = false;
	    }

	    col_cnt = 0;

    }
		
        // NOTE strtok clobbers tmp

}

void print_passage_counts_by_vendor_id(DataFrame * df, int vendor_id)
{
    printf("print_passage_counts_by_vendor_id(vendor_id), vendor_id = %d\n", vendor_id);



    DataFrame_ColumnInt32 * vendors = (DataFrame_ColumnInt32*)df->methods->GetColumnRefByIndex(df, 0);


    DataFrame * df1 = GET_SEL_INT32(df, vendor_id,  vendors->methods->GetAll(vendors) , vendors->methods->GetColSize(vendors));

    DataFrame_ColumnInt32 * passenger_count = (DataFrame_ColumnInt32*)df1->methods->GetColumnRefByIndex(df1, 3);

    size_t size = passenger_count->methods->GetColSize(passenger_count);
    // __attribute__((annotate("local_malloc")))
    int32_t * passage_vec = passenger_count->methods->GetAll(passenger_count);

    uint32_t * passage_map  = (uint32_t *)malloc(size * sizeof(uint32_t));
    memset(passage_map, 0, size * sizeof(uint32_t));

    int key = 0;
    for (int i = 0; i < size; i++)
    {
	key = passage_vec[i] % size;
	passage_map[key]++ ;
    } 


    uint32_t passage_mapc;
    for (int i = 0; i < size; i++)
    {
	    passage_mapc = passage_map[i];
	    if (passage_mapc)
		    printf("passage_count= %d, cnt = %d\n", i, passage_mapc);
    } 

    free_df(df1);
    free(passage_map);
}

  
 static inline uint64_t to_second(struct date * time)  {

	 uint64_t ret = 0;   
	 
   parse_time(time);

	 return ret;
 }



void print_number_vendor_ids_and_unique(DataFrame * df)
{
    printf("print_number_vendor_ids_and_unique() \n");
    DataFrame_ColumnInt32 * c = (DataFrame_ColumnInt32*)df->methods->GetColumnRefByIndex(df, 0);
    printf("Number of vendor_ids in the train dataset: %ld \n", c->methods->Size(c));

     c->methods->GetUnique(c);

    printf("Number of unique vendor_ids in the train dataset: %d \n", c->methods->GetUniqueSize(c));

}

void calculate_trip_duration(DataFrame * df)
{
    printf("calculate_trip_duration()\n");
    struct date  * pickup_datetime;
    struct date  * drop_off_time; 

    DataFrame_ColumnString * pickup_time_col = (DataFrame_ColumnString*)df->methods->GetColumnRefByIndex(df, 1);
    DataFrame_ColumnString * drop_off_time_col = (DataFrame_ColumnString*)df->methods->GetColumnRefByIndex(df, 2);

    long * pickup_time_vec = (long  *)pickup_time_col->methods->GetAll(pickup_time_col);
    long * drop_off_time_vec = (long *)drop_off_time_col->methods->GetAll(drop_off_time_col);

    assert(pickup_time_col->methods->GetColSize(pickup_time_col) == drop_off_time_col->methods->GetColSize(drop_off_time_col));

    uint64_t size = drop_off_time_col->methods->GetColSize(drop_off_time_col);

    DataFrame_ColumnInt64* duration_vec = DataFrame_ColumnInt64_New(INT64);
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)duration_vec));    
    duration_vec->methods->SetName(duration_vec, "duration");

    long val1;
    long val2;
    //__attribute__((annotate("local_malloc")))
    int64_t * duration_a = (int64_t *)malloc(size * sizeof(int64_t));
    for (uint64_t i = 0; i < size; i++) {
      val1 = pickup_time_vec[i];
      val2 = drop_off_time_vec[i];
      pickup_datetime = (struct date *)(&val1);
      drop_off_time = (struct date *)(&val2);
      //printf(" long value %lu\n", *val);
#if 0
      printf(" Year %d ", pickup_datetime->year);
      printf(" Month %d ",(uint8_t) pickup_datetime->month);
      printf(" day %d ", (uint8_t)pickup_datetime->day);
      printf(" hour %d ", (uint8_t)pickup_datetime->hour);
      printf(" minute %d ", (uint8_t)pickup_datetime->minute);
      printf(" second %d \n", (uint8_t)pickup_datetime->second);
#endif
      //parse_date_time(drop_off_time_vec[i],&drop_off_time);
      uint64_t pickup_time_second  = to_second(pickup_datetime);
      uint64_t dropoff_time_second = to_second(drop_off_time);
      duration_a[i] = dropoff_time_second - pickup_time_second;
    }

    duration_vec->methods->AddColValues(duration_vec, duration_a, size);

    uint64_t mean = 0;
    uint64_t min = (1ULL<<63);
    uint64_t max = 0;
    int cnt = 0;

    int64_t duration;
    for (uint64_t i = 0; i < size; i++) {
      duration = duration_a[i];
      mean += duration;
      if (min > duration)
        min = duration;
      if (max < duration)
        max = duration;
    }
    mean = mean/size;
    printf("Mean duration = %ld seconds \n", mean);
    printf("Min duration = %lu seconds\n", min);
    printf("Max duration = %lu seconds\n", max);
}
    
void calculate_distribution_store_and_fwd_flag(DataFrame * df)
{
    printf("calculate_distribution_store_and_fwd_flag()\n");
 

    DataFrame_ColumnInt8 * storage_and_fwd_col = (DataFrame_ColumnInt8*)df->methods->GetColumnRefByIndex(df, 8);
     

    int8_t * storage_and_fwd_vec = storage_and_fwd_col->methods->GetAll(storage_and_fwd_col);

    uint64_t col_size = storage_and_fwd_col->methods->GetColSize(storage_and_fwd_col);

    DataFrame * df1 = GET_SEL(df, (int8_t)'N',  storage_and_fwd_vec , col_size);

    DataFrame_ColumnInt8 * new_storage_and_fwd_col = (DataFrame_ColumnInt8*)df1->methods->GetColumnRefByIndex(df1, 8);
    size_t N_size  =  new_storage_and_fwd_col->methods->GetColSize(new_storage_and_fwd_col);

    double ratio = (double)((double)N_size/(double)col_size);
    printf("%lf\n", ratio);

    DataFrame * df2 = GET_SEL(df, (int8_t)'Y',  storage_and_fwd_vec , col_size);

    DataFrame_ColumnInt32 * new_vendor_col = (DataFrame_ColumnInt32*)df2->methods->GetColumnRefByIndex(df2, 0);

    int * vendor_ids  = (int *) new_vendor_col->methods->GetUnique(new_vendor_col);

    uint64_t uv_size = new_vendor_col->methods->GetUniqueSize(new_vendor_col);

    printf( "{");
    for (int i = 0; i < uv_size; i++) {
        printf("%d,", vendor_ids[i]);
    }
    printf("}\n");

   printf("\n");
   //free(vendor_ids);
  free_df(df1);
  free_df(df2);

}
static double haversine(double lat1, double lon1, double lat2, double lon2)
{
    // Distance between latitudes and longitudes
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;

    // Convert to radians.
    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    // Apply formulae.
    double a   = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2);
    double rad = 6371;
    double c   = 2 * asin(sqrt(a));
    return rad * c;
}
void calculate_haversine_distance_column(DataFrame * df)
{
    double times[10];
    printf("calculate_haversine_distance_column()\n");

    DataFrame_ColumnFloat64 * pickup_longitude_col = (DataFrame_ColumnFloat64*)df->methods->GetColumnRefByIndex(df, 5);
    DataFrame_ColumnFloat64 * pickup_latitude_col = (DataFrame_ColumnFloat64*)df->methods->GetColumnRefByIndex(df, 6);
    DataFrame_ColumnFloat64 * dropoff_longitude_col = (DataFrame_ColumnFloat64*)df->methods->GetColumnRefByIndex(df, 9);
    DataFrame_ColumnFloat64 * dropoff_latitude_col = (DataFrame_ColumnFloat64*)df->methods->GetColumnRefByIndex(df, 10);

    double * pickup_longitude_vec = pickup_longitude_col->methods->GetAll(pickup_longitude_col);
    double * pickup_latitude_vec = pickup_latitude_col->methods->GetAll(pickup_latitude_col);
    double * dropoff_longitude_vec = dropoff_longitude_col->methods->GetAll(dropoff_longitude_col);
    double * dropoff_latitude_vec = dropoff_latitude_col->methods->GetAll(dropoff_latitude_col);

    assert(pickup_longitude_col->methods->GetColSize(pickup_longitude_col) == pickup_latitude_col->methods->GetColSize(pickup_latitude_col));
    assert(pickup_longitude_col->methods->GetColSize(pickup_longitude_col) == dropoff_longitude_col->methods->GetColSize(dropoff_longitude_col));
    assert(pickup_longitude_col->methods->GetColSize(pickup_longitude_col) == dropoff_latitude_col->methods->GetColSize(dropoff_latitude_col));

    DataFrame_ColumnFloat64* haversine_distance = DataFrame_ColumnFloat64_New(FLOAT64);
    assert(!df->methods->AddColumn(df, (DataFrame_Column *)haversine_distance));    
    haversine_distance->methods->SetName(haversine_distance, "haversine_distance");

    uint64_t col_size = pickup_longitude_col->methods->GetColSize(pickup_longitude_col);
    uint64_t ncnt = 0;
     //__attribute__((annotate("local_malloc")))
    double * haversine_distance_vec = malloc(sizeof(double) * col_size);
    //times[0] = get_time_us();
    //printf("Unique us %lf\n",  (times[1] - times[0]));
    for (uint64_t i = 0; i < col_size; i++) {
	    haversine_distance_vec[i] = (haversine(pickup_latitude_vec[i], pickup_longitude_vec[i],dropoff_latitude_vec[i],dropoff_longitude_vec[i]));

    }
   // times[1] = get_time_us();
    
    haversine_distance->methods->AddColValues(haversine_distance, haversine_distance_vec, col_size);

    DataFrame * df1 = GET_SELGE(df, 100.0,  haversine_distance_vec , col_size);


    DataFrame_ColumnFloat64 *  new_haversine_distance = (DataFrame_ColumnFloat64*)df1->methods->GetColumnRefByIndex(df1, 20);

    //printf("loop time in havershine %lf\n",  (times[1] - times[0]));
    printf("Number of rows that have haversine_distance > 100 KM = %ld", new_haversine_distance->methods->GetColSize(new_haversine_distance));

    free_df(df1);
    printf("\n");
}
void analyze_trip_timestamp(DataFrame * df)
{
    printf("analyze_trip_timestamp()\n");


    DataFrame_ColumnString * pickup_time_col = (DataFrame_ColumnString*)df->methods->GetColumnRefByIndex(df, 1);
    DataFrame_ColumnString * drop_off_time_col = (DataFrame_ColumnString*)df->methods->GetColumnRefByIndex(df, 2);

    int64_t * pickup_time_vec = (int64_t *)pickup_time_col->methods->GetAll(pickup_time_col);
    //char ** drop_off_time_vec = drop_off_time_col->methods->GetAll(drop_off_time_col);

    uint64_t size = drop_off_time_col->methods->GetColSize(drop_off_time_col);
    uint64_t min = (1ULL<<63);
    uint64_t max = 0;
    struct date  * mintime;
    struct date  * maxtime;
    long   val1;

    struct date * pickup_date_time;
    for (uint64_t i = 0; i < size; i++) {
      val1 = pickup_time_vec[i];
      pickup_date_time = (struct date *)(&val1);
      uint64_t pickup_time_second  = to_second(pickup_date_time);
      if (min > pickup_time_second) {
        min = pickup_time_second;
        mintime = pickup_date_time;
      }
      if (max < pickup_time_second) {
        max = pickup_time_second;
        maxtime = pickup_date_time;
      }
    }

    print_date_time(mintime, true);
    print_date_time(maxtime, true);

     //__attribute__((annotate("local_malloc")))
    int32_t * pickup_hour_map = malloc (25 * sizeof(int));
    // __attribute__((annotate("local_malloc")))
    int32_t * pickup_day_map = malloc (32 * sizeof(int));
     //__attribute__((annotate("local_malloc")))
    int32_t * pickup_month_map = malloc (13 * sizeof(int));

    memset(pickup_hour_map, 0, 25 * sizeof(int));
    memset(pickup_day_map, 0, 32 * sizeof(int));
    memset(pickup_month_map, 0, 13 * sizeof(int));


    DataFrame_ColumnInt8* pickup_day_vec = DataFrame_ColumnInt8_New(INT8);
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)pickup_day_vec));

    DataFrame_ColumnInt8* pickup_month_vec = DataFrame_ColumnInt8_New(INT8);
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)pickup_month_vec));

    DataFrame_ColumnInt8* pickup_hour_vec = DataFrame_ColumnInt8_New(INT8);
    assert(!df->methods->AddColumn(df, (DataFrame_Column*)pickup_hour_vec));

    pickup_hour_vec->methods->SetName(pickup_hour_vec, "pickup_hour");
    pickup_day_vec->methods->SetName(pickup_day_vec, "pickup_day");
    pickup_month_vec->methods->SetName(pickup_month_vec, "pickup_month");

    int8_t * pickup_hour_a = malloc (size * sizeof(char));
    //__attribute__((annotate("local_malloc")))
    int8_t * pickup_day_a = malloc (size * sizeof(char));
    //__attribute__((annotate("local_malloc")))
    int8_t * pickup_month_a = malloc (size * sizeof(char));

    struct date * dt;
    for (uint64_t i = 0; i < size; i++) {

      val1 = pickup_time_vec[i];
      dt = (struct date *)(&val1);

      pickup_hour_map[dt->hour]++;
      pickup_hour_a[i] = dt->hour;

      pickup_day_map[dt->day]++;
      pickup_day_a[i] = dt->day;

      pickup_month_map[dt->month]++;
      pickup_month_a[i] = dt->month;
    }

      pickup_hour_vec->methods->AddColValues(pickup_hour_vec, pickup_hour_a, size);
      pickup_day_vec->methods->AddColValues(pickup_day_vec, pickup_day_a, size);
      pickup_month_vec->methods->AddColValues(pickup_month_vec, pickup_month_a, size);

    printf("Print top 10 rows.\n{\n");
    
    PRINT_COLUMNS(df, 10);

    printf("}\n");

    for (int i = 0; i < 25; i++) {
	if (pickup_hour_map[i])
        printf("pickup_hour = %d, cnt = %d\n", i, pickup_hour_map[i]);
    }
    printf("\n");
   
    for (int i = 0; i < 32; i++) {
	if (pickup_day_map[i])
        printf("pickup_day = %d, cnt = %d\n", i, pickup_day_map[i]);
    }
    printf("\n");
    for (int i = 0; i < 13; i++) {
	if (pickup_month_map[i])
        printf("pickup_month = %d, cnt = %d\n", i, pickup_month_map[i]);
    }
    printf("\n");
    free(pickup_hour_map);
    free(pickup_day_map);
    free(pickup_month_map);
}


void analyze_trip_durations_of_timestamps( DataFrame * df, char* key_col_name, int col_idx)
{
    double times[10];
    printf("analyze_trip_durations_of_timestamps() on key = %s\n", key_col_name);

    DataFrame_ColumnInt8* group_by_col = (DataFrame_ColumnInt8*)df->methods->GetColumnRefByIndex(df, col_idx);


    DataFrame_ColumnInt64* duration_col = (DataFrame_ColumnInt64*)df->methods->GetColumnRefByIndex(df, 19);

    int8_t * groups = group_by_col->methods->GetUnique(group_by_col);
    int32_t  ngroups = group_by_col->methods->GetUniqueSize(group_by_col);

    int8_t * group_by_col_vec = group_by_col->methods->GetCopy(group_by_col);
    int64_t * duration_col_vec = duration_col->methods->GetCopy(duration_col);

    int64_t ** duration_g  = (int64_t **) duration_col->methods->GetBySelInt8Group(duration_col, group_by_col_vec, groups, ngroups, duration_col_vec);

    int64_t * gsize = duration_col->methods->GetBySelInt8GroupSize(duration_col);
    for (int i = 0; i < ngroups; i++) {
      printf("%d %d\n",groups[i], get_median(duration_g[i], gsize[i]));
    }

    for (int i = 0; i < ngroups; i++) {
	    free(duration_g[i]);
    }
    free(duration_g);
    free(gsize);
    printf("\n");
    free(groups);
    free(group_by_col_vec);
    free(duration_col_vec);
}


void _Z7do_workPv(void * arg)
{
	 
    double times[10];
    DataFrame* df = DataFrame_New();
    read_csv(df);
    times[0] = get_time_us();
    print_number_vendor_ids_and_unique(df); 
    times[1] = get_time_us();
    print_passage_counts_by_vendor_id(df, 1);
    times[2] = get_time_us();
    print_passage_counts_by_vendor_id(df, 2);
    times[3] = get_time_us();
    calculate_trip_duration(df);
    times[4] = get_time_us();
    calculate_distribution_store_and_fwd_flag(df);
    times[5] = get_time_us();
   calculate_haversine_distance_column(df);
    times[6] = get_time_us();
   analyze_trip_timestamp(df);
    times[7] = get_time_us();
    analyze_trip_durations_of_timestamps(df, "pickup_day", 21);
    times[8] = get_time_us();
    analyze_trip_durations_of_timestamps(df, "pickup_month", 22);
    times[9] = get_time_us();

    for (uint32_t i = 1; i < 10; i++) {
        printf("Step %d : %lf us\n", i, (times[i] - times[i - 1]));
                      
    }
    printf("Total: %lf \n", (times[9] - times[0]));
}
int main(int argc, char** argv)
{
  _Z7do_workPv(NULL);
  return 0;
}

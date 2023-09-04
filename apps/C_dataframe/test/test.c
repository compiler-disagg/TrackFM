#include "dataframe/dataframe.h"
#include <stdio.h>

#define assert(x) do { if(!(x)) printf("%s:%d Assertion failed %s", \
                                       __FILE__, __LINE__, #x); } while(0)

int main(int argc, char** argv)
{
    DataFrame* df = DataFrame_New();
    DataFrame_ColumnInt32* c = DataFrame_ColumnInt32_New();
    int32_t v;

    assert(df);
    assert(c);

    assert(df->methods->ColumnSize(df) == 0);

    assert(!df->methods->AddColumn(df, (DataFrame_Column*)c));
    assert(c->methods);
    assert(c->methods->DecRef(c)); /* AddColumn should incref */
    assert(c->methods);
    assert(df->methods->ColumnSize(df) == 1);
    assert(df->columns[0]);
    c = (DataFrame_ColumnInt32*)df->methods->GetColumnRefByIndex(df, 0);
    assert(c);


    c = (DataFrame_ColumnInt32*)df->methods->GetColumnRefByIndex(df, 0);
    assert(c);
    c->methods->Add(c, 1);
    c->methods->Add(c, 4);
    c->methods->Add(c, 16);
    assert(c->methods->Size(c) == 3);

    assert(df->methods->IsRectangular(df));

    c->methods->Remove(c, 1);
    assert(c->methods->Size(c) == 2);

    assert(c->methods->TryGet(c, 0, &v));
    assert(v == 1);
    assert(c->methods->TryGet(c, 1, &v));
    assert(v == 16);

    assert(!df->methods->DecRef(df));

    return 0;
}

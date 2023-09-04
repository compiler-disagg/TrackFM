#include "DataFrameSharp/DataFrame.h"

namespace DataFrameSharp
{

DataFrame::DataFrame()
    : df(nullptr) {}

DataFrame::DataFrame(::DataFrame* df)
    : df(nullptr)
{
    df->methods->IncRef(df);
    this->df = df;
}

int DataFrame::RowCount::get()
{
    return df->methods->RowCount(df);
}

int DataFrame::ColumnCount::get()
{
    return df->methods->ColumnCount(df);
}

DataFrame::~DataFrame()
{
    System::GC::KeepAlive(this);
        Release();
}

DataFrame::!DataFrame()
{
    Release();
}

void DataFrame::Release()
{
    if(df)
    {
        df->methods->DecRef(df);
        df = nullptr;
    }
}


}

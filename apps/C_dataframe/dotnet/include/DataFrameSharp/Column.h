#pragma once

#include "dataframe/column.h"

namespace DataFrameSharp
{

public ref class DataColumn abstract
{
public:
    DataColumn()
        : column(nullptr) {}
    DataColumn(DataFrame_Column* column)
        : column(column) {}
    property System::Type^ Type
    {
        virtual System::Type^ get() = 0;
    }
    property System::String^ Name
    {
        virtual System::String^ get() = 0;
        virtual void set(System::String^ name) = 0;
    }
    property int Count { int get() { return (int)column->methods->Size(column); } }

    ~DataColumn()
    {
        System::GC::KeepAlive(this);
        Release();
    }
    !DataColumn() { Release(); }
protected:
    DataFrame_Column* column;
private:
    void Release()
    {
        if(column)
        {
            column->methods->DecRef(column);
            column = nullptr;
        }
    }
};

}

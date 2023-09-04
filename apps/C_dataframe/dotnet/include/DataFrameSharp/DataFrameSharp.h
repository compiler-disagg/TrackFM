#pragma once

#include "dataframe/dataframe.h"

namespace DataFrameSharp
{

public ref class DataFrame
{
public:
    DataFrame();
    DataFrame(::DataFrame* df);
    property int RowCount { int get(); }
    property int ColumnCount { int get(); }
    void AddColumn(DataColumn^ column);
    void RemoveColumn(System::String^ name);
    void RemoveRow(int rowIndex);
    property DataColumn^ default[int]
    {
        DataColumn^ get(int index);
    }
    property DataColumn^ default[System::String^]
    {
        DataColumn^ get(System::String^ name);
    }
    ~DataFrame();
    !DataFrame();
private:
    ::DataFrame* df;
    void Release();
};


}

#include "DataFrameSharp/DataColumnCString.h"
#include "DataFrameSharp/StringConverter.h"
#include "dataframe/columnCString.h"

#include <string.h>

#define COLUMN ((DataFrame_ColumnCString*)column)

namespace DataFrameSharp
{

DataColumnCString::DataColumnCString()
{
    this->column = (DataFrame_Column*)DataFrame_ColumnCString_New();
}

DataColumnCString::DataColumnCString(DataFrame_ColumnCString* column)
{
    column->methods->IncRef(column);
    this->column = (DataFrame_Column*)column;
}

cli::array<System::String^>^ DataColumnCString::ToArray()
{
    auto a = gcnew cli::array<System::String^>(
        (int)COLUMN->methods->Size(COLUMN));
    for(int i=0;i<a->Length;i++)
    {
        char* c;
        if(COLUMN->methods->TryGet(COLUMN, (size_t)i, &c))
            a[i] = DataFrameSharp::ToString(c);
        else
            a[i] = nullptr;
    }
    return a;
}


System::Collections::BitArray^ DataColumnCString::NAValues()
{
    auto a = gcnew cli::array<uint8_t>((int)COLUMN->methods->Size(COLUMN));
    auto na = COLUMN->methods->GetNAs(COLUMN);
    if(a->Length)
    {
        pin_ptr<uint8_t> p(&a[0]);
        memcpy(&na[0], p, (a->Length + 7) / 8);
    }

    return gcnew System::Collections::BitArray(a);
}

System::String^ DataColumnCString::default::get(int index)
{
    char* c;
    bool found = COLUMN->methods->TryGet(COLUMN, index, &c);
    return found ? DataFrameSharp::ToString(c) : nullptr;
}
void DataColumnCString::default::set(int index, System::String^ value)
{
    if(value)
    {
        char* c = DataFrameSharp::ToString(value);
        COLUMN->methods->Set(COLUMN, index, c);
        free(c);
    }
    else
        COLUMN->methods->SetNA(COLUMN, index);
}
System::Type^ DataColumnCString::Type::get()
{
    return System::String::typeid;
}

System::String^ DataColumnCString::Name::get()
{
    return DataFrameSharp::ToString(COLUMN->methods->GetName(COLUMN));
}


void DataColumnCString::Name::set(System::String^ name)
{
    COLUMN->methods->SetName(COLUMN, DataFrameSharp::ToString(name));
}



}
#include "DataFrameSharp/DataColumnString.h"
#include "DataFrameSharp/StringConverter.h"
#include "dataframe/columnString.h"

#include <string.h>

#define COLUMN ((DataFrame_ColumnString*)column)

namespace DataFrameSharp
{

DataColumnString::DataColumnString()
{
    this->column = (DataFrame_Column*)DataFrame_ColumnString_New();
}

DataColumnString::DataColumnString(DataFrame_ColumnString* column)
{
    column->methods->IncRef(column);
    this->column = (DataFrame_Column*)column;
}

cli::array<System::String^>^ DataColumnString::ToArray()
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


System::Collections::BitArray^ DataColumnString::NAValues()
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

System::String^ DataColumnString::default::get(int index)
{
    char* c;
    bool found = COLUMN->methods->TryGet(COLUMN, index, &c);
    return found ? DataFrameSharp::ToString(c) : nullptr;
}
void DataColumnString::default::set(int index, System::String^ value)
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
System::Type^ DataColumnString::Type::get()
{
    return System::String::typeid;
}

System::String^ DataColumnString::Name::get()
{
    return DataFrameSharp::ToString(COLUMN->methods->GetName(COLUMN));
}


void DataColumnString::Name::set(System::String^ name)
{
    COLUMN->methods->SetName(COLUMN, DataFrameSharp::ToString(name));
}



}
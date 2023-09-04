#include "DataFrameSharp/DataColumnInt8.h"
#include "DataFrameSharp/StringConverter.h"
#include "dataframe/columnInt8.h"

#include <string.h>

#define COLUMN ((DataFrame_ColumnInt8*)column)

namespace DataFrameSharp
{

DataColumnInt8::DataColumnInt8()
{
    this->column = (DataFrame_Column*)DataFrame_ColumnInt8_New();
}

DataColumnInt8::DataColumnInt8(DataFrame_ColumnInt8* column)
{
    column->methods->IncRef(column);
    this->column = (DataFrame_Column*)column;
}

cli::array<int8_t>^ DataColumnInt8::ToArray()
{
    auto a = gcnew cli::array<int8_t>((int)COLUMN->methods->Size(COLUMN));
    if(a->Length)
    {
        pin_ptr<int8_t> p(&a[0]);
        int8_t* t = COLUMN->methods->Get(COLUMN, 0);
        memcpy(p, t, sizeof(int8_t) * a->Length);
    }
    return a;
}


System::Collections::BitArray^ DataColumnInt8::NAValues()
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

Maybe<int8_t> DataColumnInt8::default::get(int index)
{
    int8_t v;
    bool found = COLUMN->methods->TryGet(COLUMN, index, &v);
    if(found)
        return Maybe<int8_t>(v);
    return Maybe<int8_t>();
}
void DataColumnInt8::default::set(int index, Maybe<int8_t> value)
{
    if(value.HasValue)
        COLUMN->methods->Set(COLUMN, index, value.Value);
    else
        COLUMN->methods->SetNA(COLUMN, index);
}
System::Type^ DataColumnInt8::Type::get()
{
    return int8_t::typeid;
}

System::String^ DataColumnInt8::Name::get()
{
    return DataFrameSharp::ToString(COLUMN->methods->GetName(COLUMN));
}


void DataColumnInt8::Name::set(System::String^ name)
{
    COLUMN->methods->SetName(COLUMN, DataFrameSharp::ToString(name));
}



}
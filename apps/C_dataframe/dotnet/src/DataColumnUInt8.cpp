#include "DataFrameSharp/DataColumnUInt8.h"
#include "DataFrameSharp/StringConverter.h"
#include "dataframe/columnUInt8.h"

#include <string.h>

#define COLUMN ((DataFrame_ColumnUInt8*)column)

namespace DataFrameSharp
{

DataColumnUInt8::DataColumnUInt8()
{
    this->column = (DataFrame_Column*)DataFrame_ColumnUInt8_New();
}

DataColumnUInt8::DataColumnUInt8(DataFrame_ColumnUInt8* column)
{
    column->methods->IncRef(column);
    this->column = (DataFrame_Column*)column;
}

cli::array<uint8_t>^ DataColumnUInt8::ToArray()
{
    auto a = gcnew cli::array<uint8_t>((int)COLUMN->methods->Size(COLUMN));
    if(a->Length)
    {
        pin_ptr<uint8_t> p(&a[0]);
        uint8_t* t = COLUMN->methods->Get(COLUMN, 0);
        memcpy(p, t, sizeof(uint8_t) * a->Length);
    }
    return a;
}


System::Collections::BitArray^ DataColumnUInt8::NAValues()
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

Maybe<uint8_t> DataColumnUInt8::default::get(int index)
{
    uint8_t v;
    bool found = COLUMN->methods->TryGet(COLUMN, index, &v);
    if(found)
        return Maybe<uint8_t>(v);
    return Maybe<uint8_t>();
}
void DataColumnUInt8::default::set(int index, Maybe<uint8_t> value)
{
    if(value.HasValue)
        COLUMN->methods->Set(COLUMN, index, value.Value);
    else
        COLUMN->methods->SetNA(COLUMN, index);
}
System::Type^ DataColumnUInt8::Type::get()
{
    return uint8_t::typeid;
}

System::String^ DataColumnUInt8::Name::get()
{
    return DataFrameSharp::ToString(COLUMN->methods->GetName(COLUMN));
}


void DataColumnUInt8::Name::set(System::String^ name)
{
    COLUMN->methods->SetName(COLUMN, DataFrameSharp::ToString(name));
}



}
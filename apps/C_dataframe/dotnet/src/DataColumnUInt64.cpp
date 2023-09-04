#include "DataFrameSharp/DataColumnUInt64.h"
#include "DataFrameSharp/StringConverter.h"
#include "dataframe/columnUInt64.h"

#include <string.h>

#define COLUMN ((DataFrame_ColumnUInt64*)column)

namespace DataFrameSharp
{

DataColumnUInt64::DataColumnUInt64()
{
    this->column = (DataFrame_Column*)DataFrame_ColumnUInt64_New();
}

DataColumnUInt64::DataColumnUInt64(DataFrame_ColumnUInt64* column)
{
    column->methods->IncRef(column);
    this->column = (DataFrame_Column*)column;
}

cli::array<uint64_t>^ DataColumnUInt64::ToArray()
{
    auto a = gcnew cli::array<uint64_t>((int)COLUMN->methods->Size(COLUMN));
    if(a->Length)
    {
        pin_ptr<uint64_t> p(&a[0]);
        uint64_t* t = COLUMN->methods->Get(COLUMN, 0);
        memcpy(p, t, sizeof(uint64_t) * a->Length);
    }
    return a;
}


System::Collections::BitArray^ DataColumnUInt64::NAValues()
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

Maybe<uint64_t> DataColumnUInt64::default::get(int index)
{
    uint64_t v;
    bool found = COLUMN->methods->TryGet(COLUMN, index, &v);
    if(found)
        return Maybe<uint64_t>(v);
    return Maybe<uint64_t>();
}
void DataColumnUInt64::default::set(int index, Maybe<uint64_t> value)
{
    if(value.HasValue)
        COLUMN->methods->Set(COLUMN, index, value.Value);
    else
        COLUMN->methods->SetNA(COLUMN, index);
}
System::Type^ DataColumnUInt64::Type::get()
{
    return uint64_t::typeid;
}

System::String^ DataColumnUInt64::Name::get()
{
    return DataFrameSharp::ToString(COLUMN->methods->GetName(COLUMN));
}


void DataColumnUInt64::Name::set(System::String^ name)
{
    COLUMN->methods->SetName(COLUMN, DataFrameSharp::ToString(name));
}



}
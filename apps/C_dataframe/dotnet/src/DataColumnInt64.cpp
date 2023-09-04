#include "DataFrameSharp/DataColumnInt64.h"
#include "DataFrameSharp/StringConverter.h"
#include "dataframe/columnInt64.h"

#include <string.h>

#define COLUMN ((DataFrame_ColumnInt64*)column)

namespace DataFrameSharp
{

DataColumnInt64::DataColumnInt64()
{
    this->column = (DataFrame_Column*)DataFrame_ColumnInt64_New();
}

DataColumnInt64::DataColumnInt64(DataFrame_ColumnInt64* column)
{
    column->methods->IncRef(column);
    this->column = (DataFrame_Column*)column;
}

cli::array<int64_t>^ DataColumnInt64::ToArray()
{
    auto a = gcnew cli::array<int64_t>((int)COLUMN->methods->Size(COLUMN));
    if(a->Length)
    {
        pin_ptr<int64_t> p(&a[0]);
        int64_t* t = COLUMN->methods->Get(COLUMN, 0);
        memcpy(p, t, sizeof(int64_t) * a->Length);
    }
    return a;
}


System::Collections::BitArray^ DataColumnInt64::NAValues()
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

Maybe<int64_t> DataColumnInt64::default::get(int index)
{
    int64_t v;
    bool found = COLUMN->methods->TryGet(COLUMN, index, &v);
    if(found)
        return Maybe<int64_t>(v);
    return Maybe<int64_t>();
}
void DataColumnInt64::default::set(int index, Maybe<int64_t> value)
{
    if(value.HasValue)
        COLUMN->methods->Set(COLUMN, index, value.Value);
    else
        COLUMN->methods->SetNA(COLUMN, index);
}
System::Type^ DataColumnInt64::Type::get()
{
    return int64_t::typeid;
}

System::String^ DataColumnInt64::Name::get()
{
    return DataFrameSharp::ToString(COLUMN->methods->GetName(COLUMN));
}


void DataColumnInt64::Name::set(System::String^ name)
{
    COLUMN->methods->SetName(COLUMN, DataFrameSharp::ToString(name));
}



}
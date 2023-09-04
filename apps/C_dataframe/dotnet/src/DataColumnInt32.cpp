#include "DataFrameSharp/DataColumnInt32.h"
#include "DataFrameSharp/StringConverter.h"
#include "dataframe/columnInt32.h"

#include <string.h>

#define COLUMN ((DataFrame_ColumnInt32*)column)

namespace DataFrameSharp
{

DataColumnInt32::DataColumnInt32()
{
    this->column = (DataFrame_Column*)DataFrame_ColumnInt32_New();
}

DataColumnInt32::DataColumnInt32(DataFrame_ColumnInt32* column)
{
    column->methods->IncRef(column);
    this->column = (DataFrame_Column*)column;
}

cli::array<int32_t>^ DataColumnInt32::ToArray()
{
    auto a = gcnew cli::array<int32_t>((int)COLUMN->methods->Size(COLUMN));
    if(a->Length)
    {
        pin_ptr<int32_t> p(&a[0]);
        int32_t* t = COLUMN->methods->Get(COLUMN, 0);
        memcpy(p, t, sizeof(int32_t) * a->Length);
    }
    return a;
}


System::Collections::BitArray^ DataColumnInt32::NAValues()
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

Maybe<int32_t> DataColumnInt32::default::get(int index)
{
    int32_t v;
    bool found = COLUMN->methods->TryGet(COLUMN, index, &v);
    if(found)
        return Maybe<int32_t>(v);
    return Maybe<int32_t>();
}
void DataColumnInt32::default::set(int index, Maybe<int32_t> value)
{
    if(value.HasValue)
        COLUMN->methods->Set(COLUMN, index, value.Value);
    else
        COLUMN->methods->SetNA(COLUMN, index);
}
System::Type^ DataColumnInt32::Type::get()
{
    return int32_t::typeid;
}

System::String^ DataColumnInt32::Name::get()
{
    return DataFrameSharp::ToString(COLUMN->methods->GetName(COLUMN));
}


void DataColumnInt32::Name::set(System::String^ name)
{
    COLUMN->methods->SetName(COLUMN, DataFrameSharp::ToString(name));
}



}
#include "DataFrameSharp/DataColumnInt16.h"
#include "DataFrameSharp/StringConverter.h"
#include "dataframe/columnInt16.h"

#include <string.h>

#define COLUMN ((DataFrame_ColumnInt16*)column)

namespace DataFrameSharp
{

DataColumnInt16::DataColumnInt16()
{
    this->column = (DataFrame_Column*)DataFrame_ColumnInt16_New();
}

DataColumnInt16::DataColumnInt16(DataFrame_ColumnInt16* column)
{
    column->methods->IncRef(column);
    this->column = (DataFrame_Column*)column;
}

cli::array<int16_t>^ DataColumnInt16::ToArray()
{
    auto a = gcnew cli::array<int16_t>((int)COLUMN->methods->Size(COLUMN));
    if(a->Length)
    {
        pin_ptr<int16_t> p(&a[0]);
        int16_t* t = COLUMN->methods->Get(COLUMN, 0);
        memcpy(p, t, sizeof(int16_t) * a->Length);
    }
    return a;
}


System::Collections::BitArray^ DataColumnInt16::NAValues()
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

Maybe<int16_t> DataColumnInt16::default::get(int index)
{
    int16_t v;
    bool found = COLUMN->methods->TryGet(COLUMN, index, &v);
    if(found)
        return Maybe<int16_t>(v);
    return Maybe<int16_t>();
}
void DataColumnInt16::default::set(int index, Maybe<int16_t> value)
{
    if(value.HasValue)
        COLUMN->methods->Set(COLUMN, index, value.Value);
    else
        COLUMN->methods->SetNA(COLUMN, index);
}
System::Type^ DataColumnInt16::Type::get()
{
    return int16_t::typeid;
}

System::String^ DataColumnInt16::Name::get()
{
    return DataFrameSharp::ToString(COLUMN->methods->GetName(COLUMN));
}


void DataColumnInt16::Name::set(System::String^ name)
{
    COLUMN->methods->SetName(COLUMN, DataFrameSharp::ToString(name));
}



}
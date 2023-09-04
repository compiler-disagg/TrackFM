#include "DataFrameSharp/DataColumnUInt16.h"
#include "DataFrameSharp/StringConverter.h"
#include "dataframe/columnUInt16.h"

#include <string.h>

#define COLUMN ((DataFrame_ColumnUInt16*)column)

namespace DataFrameSharp
{

DataColumnUInt16::DataColumnUInt16()
{
    this->column = (DataFrame_Column*)DataFrame_ColumnUInt16_New();
}

DataColumnUInt16::DataColumnUInt16(DataFrame_ColumnUInt16* column)
{
    column->methods->IncRef(column);
    this->column = (DataFrame_Column*)column;
}

cli::array<uint16_t>^ DataColumnUInt16::ToArray()
{
    auto a = gcnew cli::array<uint16_t>((int)COLUMN->methods->Size(COLUMN));
    if(a->Length)
    {
        pin_ptr<uint16_t> p(&a[0]);
        uint16_t* t = COLUMN->methods->Get(COLUMN, 0);
        memcpy(p, t, sizeof(uint16_t) * a->Length);
    }
    return a;
}


System::Collections::BitArray^ DataColumnUInt16::NAValues()
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

Maybe<uint16_t> DataColumnUInt16::default::get(int index)
{
    uint16_t v;
    bool found = COLUMN->methods->TryGet(COLUMN, index, &v);
    if(found)
        return Maybe<uint16_t>(v);
    return Maybe<uint16_t>();
}
void DataColumnUInt16::default::set(int index, Maybe<uint16_t> value)
{
    if(value.HasValue)
        COLUMN->methods->Set(COLUMN, index, value.Value);
    else
        COLUMN->methods->SetNA(COLUMN, index);
}
System::Type^ DataColumnUInt16::Type::get()
{
    return uint16_t::typeid;
}

System::String^ DataColumnUInt16::Name::get()
{
    return DataFrameSharp::ToString(COLUMN->methods->GetName(COLUMN));
}


void DataColumnUInt16::Name::set(System::String^ name)
{
    COLUMN->methods->SetName(COLUMN, DataFrameSharp::ToString(name));
}



}
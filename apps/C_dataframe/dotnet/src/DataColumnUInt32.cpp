#include "DataFrameSharp/DataColumnUInt32.h"
#include "DataFrameSharp/StringConverter.h"
#include "dataframe/columnUInt32.h"

#include <string.h>

#define COLUMN ((DataFrame_ColumnUInt32*)column)

namespace DataFrameSharp
{

DataColumnUInt32::DataColumnUInt32()
{
    this->column = (DataFrame_Column*)DataFrame_ColumnUInt32_New();
}

DataColumnUInt32::DataColumnUInt32(DataFrame_ColumnUInt32* column)
{
    column->methods->IncRef(column);
    this->column = (DataFrame_Column*)column;
}

cli::array<uint32_t>^ DataColumnUInt32::ToArray()
{
    auto a = gcnew cli::array<uint32_t>((int)COLUMN->methods->Size(COLUMN));
    if(a->Length)
    {
        pin_ptr<uint32_t> p(&a[0]);
        uint32_t* t = COLUMN->methods->Get(COLUMN, 0);
        memcpy(p, t, sizeof(uint32_t) * a->Length);
    }
    return a;
}


System::Collections::BitArray^ DataColumnUInt32::NAValues()
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

Maybe<uint32_t> DataColumnUInt32::default::get(int index)
{
    uint32_t v;
    bool found = COLUMN->methods->TryGet(COLUMN, index, &v);
    if(found)
        return Maybe<uint32_t>(v);
    return Maybe<uint32_t>();
}
void DataColumnUInt32::default::set(int index, Maybe<uint32_t> value)
{
    if(value.HasValue)
        COLUMN->methods->Set(COLUMN, index, value.Value);
    else
        COLUMN->methods->SetNA(COLUMN, index);
}
System::Type^ DataColumnUInt32::Type::get()
{
    return uint32_t::typeid;
}

System::String^ DataColumnUInt32::Name::get()
{
    return DataFrameSharp::ToString(COLUMN->methods->GetName(COLUMN));
}


void DataColumnUInt32::Name::set(System::String^ name)
{
    COLUMN->methods->SetName(COLUMN, DataFrameSharp::ToString(name));
}



}
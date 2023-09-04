#include "DataFrameSharp/DataColumnFloat32.h"
#include "DataFrameSharp/StringConverter.h"
#include "dataframe/columnFloat32.h"

#include <string.h>

#define COLUMN ((DataFrame_ColumnFloat32*)column)

namespace DataFrameSharp
{

DataColumnFloat32::DataColumnFloat32()
{
    this->column = (DataFrame_Column*)DataFrame_ColumnFloat32_New();
}

DataColumnFloat32::DataColumnFloat32(DataFrame_ColumnFloat32* column)
{
    column->methods->IncRef(column);
    this->column = (DataFrame_Column*)column;
}

cli::array<float>^ DataColumnFloat32::ToArray()
{
    auto a = gcnew cli::array<float>((int)COLUMN->methods->Size(COLUMN));
    if(a->Length)
    {
        pin_ptr<float> p(&a[0]);
        float* t = COLUMN->methods->Get(COLUMN, 0);
        memcpy(p, t, sizeof(float) * a->Length);
    }
    return a;
}


System::Collections::BitArray^ DataColumnFloat32::NAValues()
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

Maybe<float> DataColumnFloat32::default::get(int index)
{
    float v;
    bool found = COLUMN->methods->TryGet(COLUMN, index, &v);
    if(found)
        return Maybe<float>(v);
    return Maybe<float>();
}
void DataColumnFloat32::default::set(int index, Maybe<float> value)
{
    if(value.HasValue)
        COLUMN->methods->Set(COLUMN, index, value.Value);
    else
        COLUMN->methods->SetNA(COLUMN, index);
}
System::Type^ DataColumnFloat32::Type::get()
{
    return float::typeid;
}

System::String^ DataColumnFloat32::Name::get()
{
    return DataFrameSharp::ToString(COLUMN->methods->GetName(COLUMN));
}


void DataColumnFloat32::Name::set(System::String^ name)
{
    COLUMN->methods->SetName(COLUMN, DataFrameSharp::ToString(name));
}



}
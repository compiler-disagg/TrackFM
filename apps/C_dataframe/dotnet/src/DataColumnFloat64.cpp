#include "DataFrameSharp/DataColumnFloat64.h"
#include "DataFrameSharp/StringConverter.h"
#include "dataframe/columnFloat64.h"

#include <string.h>

#define COLUMN ((DataFrame_ColumnFloat64*)column)

namespace DataFrameSharp
{

DataColumnFloat64::DataColumnFloat64()
{
    this->column = (DataFrame_Column*)DataFrame_ColumnFloat64_New();
}

DataColumnFloat64::DataColumnFloat64(DataFrame_ColumnFloat64* column)
{
    column->methods->IncRef(column);
    this->column = (DataFrame_Column*)column;
}

cli::array<double>^ DataColumnFloat64::ToArray()
{
    auto a = gcnew cli::array<double>((int)COLUMN->methods->Size(COLUMN));
    if(a->Length)
    {
        pin_ptr<double> p(&a[0]);
        double* t = COLUMN->methods->Get(COLUMN, 0);
        memcpy(p, t, sizeof(double) * a->Length);
    }
    return a;
}


System::Collections::BitArray^ DataColumnFloat64::NAValues()
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

Maybe<double> DataColumnFloat64::default::get(int index)
{
    double v;
    bool found = COLUMN->methods->TryGet(COLUMN, index, &v);
    if(found)
        return Maybe<double>(v);
    return Maybe<double>();
}
void DataColumnFloat64::default::set(int index, Maybe<double> value)
{
    if(value.HasValue)
        COLUMN->methods->Set(COLUMN, index, value.Value);
    else
        COLUMN->methods->SetNA(COLUMN, index);
}
System::Type^ DataColumnFloat64::Type::get()
{
    return double::typeid;
}

System::String^ DataColumnFloat64::Name::get()
{
    return DataFrameSharp::ToString(COLUMN->methods->GetName(COLUMN));
}


void DataColumnFloat64::Name::set(System::String^ name)
{
    COLUMN->methods->SetName(COLUMN, DataFrameSharp::ToString(name));
}



}
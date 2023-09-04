#include "DataFrameSharp/DataColumn{{name}}.h"
#include "DataFrameSharp/StringConverter.h"
#include "dataframe/column{{name}}.h"

#include <string.h>

#define COLUMN ((DataFrame_Column{{name}}*)column)

namespace DataFrameSharp
{

DataColumn{{name}}::DataColumn{{name}}()
{
    this->column = (DataFrame_Column*)DataFrame_Column{{name}}_New();
}

DataColumn{{name}}::DataColumn{{name}}(DataFrame_Column{{name}}* column)
{
    column->methods->IncRef(column);
    this->column = (DataFrame_Column*)column;
}

{% if type == "char*" %}
cli::array<System::String^>^ DataColumn{{name}}::ToArray()
{
    auto a = gcnew cli::array<System::String^>(
        (int)COLUMN->methods->Size(COLUMN));
    for(int i=0;i<a->Length;i++)
    {
        char* c;
        if(COLUMN->methods->TryGet(COLUMN, (size_t)i, &c))
            a[i] = DataFrameSharp::ToString(c);
        else
            a[i] = nullptr;
    }
    return a;
}
{% else %}
cli::array<{{type}}>^ DataColumn{{name}}::ToArray()
{
    auto a = gcnew cli::array<{{type}}>((int)COLUMN->methods->Size(COLUMN));
    if(a->Length)
    {
        pin_ptr<{{type}}> p(&a[0]);
        {{type}}* t = COLUMN->methods->Get(COLUMN, 0);
        memcpy(p, t, sizeof({{type}}) * a->Length);
    }
    return a;
}
{% endif %}


System::Collections::BitArray^ DataColumn{{name}}::NAValues()
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

{% if type == "char*" %}
System::String^ DataColumn{{name}}::default::get(int index)
{
    char* c;
    bool found = COLUMN->methods->TryGet(COLUMN, index, &c);
    return found ? DataFrameSharp::ToString(c) : nullptr;
}
void DataColumn{{name}}::default::set(int index, System::String^ value)
{
    if(value)
    {
        char* c = DataFrameSharp::ToString(value);
        COLUMN->methods->Set(COLUMN, index, c);
        free(c);
    }
    else
        COLUMN->methods->SetNA(COLUMN, index);
}
System::Type^ DataColumn{{name}}::Type::get()
{
    return System::String::typeid;
}
{% else %}
Maybe<{{type}}> DataColumn{{name}}::default::get(int index)
{
    {{type}} v;
    bool found = COLUMN->methods->TryGet(COLUMN, index, &v);
    if(found)
        return Maybe<{{type}}>(v);
    return Maybe<{{type}}>();
}
void DataColumn{{name}}::default::set(int index, Maybe<{{type}}> value)
{
    if(value.HasValue)
        COLUMN->methods->Set(COLUMN, index, value.Value);
    else
        COLUMN->methods->SetNA(COLUMN, index);
}
System::Type^ DataColumn{{name}}::Type::get()
{
    return {{type}}::typeid;
}
{% endif %}

System::String^ DataColumn{{name}}::Name::get()
{
    return DataFrameSharp::ToString(COLUMN->methods->GetName(COLUMN));
}


void DataColumn{{name}}::Name::set(System::String^ name)
{
    COLUMN->methods->SetName(COLUMN, DataFrameSharp::ToString(name));
}



}

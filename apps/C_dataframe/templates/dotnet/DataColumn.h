#pragma once

#include "DataFrameSharp/Column.h"
#include "DataFrameSharp/Maybe.h"

#include <cstdint>
#include <cstdlib>

struct DataFrame_Column{{name}};

namespace DataFrameSharp
{

public ref class DataColumn{{name}} : public DataColumn
{
public:
    DataColumn{{name}}();
    DataColumn{{name}}(DataFrame_Column{{name}}* column);
    property System::Type^ Type
    {
        virtual System::Type^ get() override;
    }
    property System::String^ Name
    {
        virtual System::String^ get() override;
        virtual void set(System::String^ name) override;
    }

    {% if type == "char*" %}
    cli::array<System::String^>^ ToArray();
    {% else %}
    cli::array<{{type}}>^ ToArray();
    {% endif %}

    System::Collections::BitArray^ NAValues();

    {% if type == "char*" %}
    property System::String^ default[int]
    {
        System::String^ get(int index);
        void set(int index, System::String^ value);
    }
    {% else %}
    property Maybe<{{type}}> default[int]
    {
        Maybe<{{type}}> get(int index);
        void set(int index, Maybe<{{type}}> value);
    }
    {% endif %}
};

}

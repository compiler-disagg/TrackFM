#pragma once

#include "DataFrameSharp/Column.h"
#include "DataFrameSharp/Maybe.h"

#include <cstdint>
#include <cstdlib>

struct DataFrame_ColumnString;

namespace DataFrameSharp
{

public ref class DataColumnString : public DataColumn
{
public:
    DataColumnString();
    DataColumnString(DataFrame_ColumnString* column);
    property System::Type^ Type
    {
        virtual System::Type^ get() override;
    }
    property System::String^ Name
    {
        virtual System::String^ get() override;
        virtual void set(System::String^ name) override;
    }

    cli::array<System::String^>^ ToArray();

    System::Collections::BitArray^ NAValues();

    property System::String^ default[int]
    {
        System::String^ get(int index);
        void set(int index, System::String^ value);
    }
};

}
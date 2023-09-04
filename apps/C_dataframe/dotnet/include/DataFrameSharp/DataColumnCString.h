#pragma once

#include "DataFrameSharp/Column.h"
#include "DataFrameSharp/Maybe.h"

#include <cstdint>
#include <cstdlib>

struct DataFrame_ColumnCString;

namespace DataFrameSharp
{

public ref class DataColumnCString : public DataColumn
{
public:
    DataColumnCString();
    DataColumnCString(DataFrame_ColumnCString* column);
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
#pragma once

#include "DataFrameSharp/Column.h"
#include "DataFrameSharp/Maybe.h"

#include <cstdint>
#include <cstdlib>

struct DataFrame_ColumnInt8;

namespace DataFrameSharp
{

public ref class DataColumnInt8 : public DataColumn
{
public:
    DataColumnInt8();
    DataColumnInt8(DataFrame_ColumnInt8* column);
    property System::Type^ Type
    {
        virtual System::Type^ get() override;
    }
    property System::String^ Name
    {
        virtual System::String^ get() override;
        virtual void set(System::String^ name) override;
    }

    cli::array<int8_t>^ ToArray();

    System::Collections::BitArray^ NAValues();

    property Maybe<int8_t> default[int]
    {
        Maybe<int8_t> get(int index);
        void set(int index, Maybe<int8_t> value);
    }
};

}
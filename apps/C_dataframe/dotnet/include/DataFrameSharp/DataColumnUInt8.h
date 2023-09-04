#pragma once

#include "DataFrameSharp/Column.h"
#include "DataFrameSharp/Maybe.h"

#include <cstdint>
#include <cstdlib>

struct DataFrame_ColumnUInt8;

namespace DataFrameSharp
{

public ref class DataColumnUInt8 : public DataColumn
{
public:
    DataColumnUInt8();
    DataColumnUInt8(DataFrame_ColumnUInt8* column);
    property System::Type^ Type
    {
        virtual System::Type^ get() override;
    }
    property System::String^ Name
    {
        virtual System::String^ get() override;
        virtual void set(System::String^ name) override;
    }

    cli::array<uint8_t>^ ToArray();

    System::Collections::BitArray^ NAValues();

    property Maybe<uint8_t> default[int]
    {
        Maybe<uint8_t> get(int index);
        void set(int index, Maybe<uint8_t> value);
    }
};

}
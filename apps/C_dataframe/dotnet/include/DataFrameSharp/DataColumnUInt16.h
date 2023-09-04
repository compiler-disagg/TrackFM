#pragma once

#include "DataFrameSharp/Column.h"
#include "DataFrameSharp/Maybe.h"

#include <cstdint>
#include <cstdlib>

struct DataFrame_ColumnUInt16;

namespace DataFrameSharp
{

public ref class DataColumnUInt16 : public DataColumn
{
public:
    DataColumnUInt16();
    DataColumnUInt16(DataFrame_ColumnUInt16* column);
    property System::Type^ Type
    {
        virtual System::Type^ get() override;
    }
    property System::String^ Name
    {
        virtual System::String^ get() override;
        virtual void set(System::String^ name) override;
    }

    cli::array<uint16_t>^ ToArray();

    System::Collections::BitArray^ NAValues();

    property Maybe<uint16_t> default[int]
    {
        Maybe<uint16_t> get(int index);
        void set(int index, Maybe<uint16_t> value);
    }
};

}
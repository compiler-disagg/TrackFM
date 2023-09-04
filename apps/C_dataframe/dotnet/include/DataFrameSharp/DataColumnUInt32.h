#pragma once

#include "DataFrameSharp/Column.h"
#include "DataFrameSharp/Maybe.h"

#include <cstdint>
#include <cstdlib>

struct DataFrame_ColumnUInt32;

namespace DataFrameSharp
{

public ref class DataColumnUInt32 : public DataColumn
{
public:
    DataColumnUInt32();
    DataColumnUInt32(DataFrame_ColumnUInt32* column);
    property System::Type^ Type
    {
        virtual System::Type^ get() override;
    }
    property System::String^ Name
    {
        virtual System::String^ get() override;
        virtual void set(System::String^ name) override;
    }

    cli::array<uint32_t>^ ToArray();

    System::Collections::BitArray^ NAValues();

    property Maybe<uint32_t> default[int]
    {
        Maybe<uint32_t> get(int index);
        void set(int index, Maybe<uint32_t> value);
    }
};

}
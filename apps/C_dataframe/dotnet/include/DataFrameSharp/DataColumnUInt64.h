#pragma once

#include "DataFrameSharp/Column.h"
#include "DataFrameSharp/Maybe.h"

#include <cstdint>
#include <cstdlib>

struct DataFrame_ColumnUInt64;

namespace DataFrameSharp
{

public ref class DataColumnUInt64 : public DataColumn
{
public:
    DataColumnUInt64();
    DataColumnUInt64(DataFrame_ColumnUInt64* column);
    property System::Type^ Type
    {
        virtual System::Type^ get() override;
    }
    property System::String^ Name
    {
        virtual System::String^ get() override;
        virtual void set(System::String^ name) override;
    }

    cli::array<uint64_t>^ ToArray();

    System::Collections::BitArray^ NAValues();

    property Maybe<uint64_t> default[int]
    {
        Maybe<uint64_t> get(int index);
        void set(int index, Maybe<uint64_t> value);
    }
};

}
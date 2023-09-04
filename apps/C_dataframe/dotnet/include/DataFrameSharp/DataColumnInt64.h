#pragma once

#include "DataFrameSharp/Column.h"
#include "DataFrameSharp/Maybe.h"

#include <cstdint>
#include <cstdlib>

struct DataFrame_ColumnInt64;

namespace DataFrameSharp
{

public ref class DataColumnInt64 : public DataColumn
{
public:
    DataColumnInt64();
    DataColumnInt64(DataFrame_ColumnInt64* column);
    property System::Type^ Type
    {
        virtual System::Type^ get() override;
    }
    property System::String^ Name
    {
        virtual System::String^ get() override;
        virtual void set(System::String^ name) override;
    }

    cli::array<int64_t>^ ToArray();

    System::Collections::BitArray^ NAValues();

    property Maybe<int64_t> default[int]
    {
        Maybe<int64_t> get(int index);
        void set(int index, Maybe<int64_t> value);
    }
};

}
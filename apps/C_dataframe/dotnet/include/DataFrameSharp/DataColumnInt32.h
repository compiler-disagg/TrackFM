#pragma once

#include "DataFrameSharp/Column.h"
#include "DataFrameSharp/Maybe.h"

#include <cstdint>
#include <cstdlib>

struct DataFrame_ColumnInt32;

namespace DataFrameSharp
{

public ref class DataColumnInt32 : public DataColumn
{
public:
    DataColumnInt32();
    DataColumnInt32(DataFrame_ColumnInt32* column);
    property System::Type^ Type
    {
        virtual System::Type^ get() override;
    }
    property System::String^ Name
    {
        virtual System::String^ get() override;
        virtual void set(System::String^ name) override;
    }

    cli::array<int32_t>^ ToArray();

    System::Collections::BitArray^ NAValues();

    property Maybe<int32_t> default[int]
    {
        Maybe<int32_t> get(int index);
        void set(int index, Maybe<int32_t> value);
    }
};

}
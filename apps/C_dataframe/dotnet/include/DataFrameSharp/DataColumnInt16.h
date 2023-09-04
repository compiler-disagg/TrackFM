#pragma once

#include "DataFrameSharp/Column.h"
#include "DataFrameSharp/Maybe.h"

#include <cstdint>
#include <cstdlib>

struct DataFrame_ColumnInt16;

namespace DataFrameSharp
{

public ref class DataColumnInt16 : public DataColumn
{
public:
    DataColumnInt16();
    DataColumnInt16(DataFrame_ColumnInt16* column);
    property System::Type^ Type
    {
        virtual System::Type^ get() override;
    }
    property System::String^ Name
    {
        virtual System::String^ get() override;
        virtual void set(System::String^ name) override;
    }

    cli::array<int16_t>^ ToArray();

    System::Collections::BitArray^ NAValues();

    property Maybe<int16_t> default[int]
    {
        Maybe<int16_t> get(int index);
        void set(int index, Maybe<int16_t> value);
    }
};

}
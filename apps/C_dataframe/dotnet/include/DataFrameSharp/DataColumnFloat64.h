#pragma once

#include "DataFrameSharp/Column.h"
#include "DataFrameSharp/Maybe.h"

#include <cstdint>
#include <cstdlib>

struct DataFrame_ColumnFloat64;

namespace DataFrameSharp
{

public ref class DataColumnFloat64 : public DataColumn
{
public:
    DataColumnFloat64();
    DataColumnFloat64(DataFrame_ColumnFloat64* column);
    property System::Type^ Type
    {
        virtual System::Type^ get() override;
    }
    property System::String^ Name
    {
        virtual System::String^ get() override;
        virtual void set(System::String^ name) override;
    }

    cli::array<double>^ ToArray();

    System::Collections::BitArray^ NAValues();

    property Maybe<double> default[int]
    {
        Maybe<double> get(int index);
        void set(int index, Maybe<double> value);
    }
};

}
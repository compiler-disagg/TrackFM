#pragma once

#include "DataFrameSharp/Column.h"
#include "DataFrameSharp/Maybe.h"

#include <cstdint>
#include <cstdlib>

struct DataFrame_ColumnFloat32;

namespace DataFrameSharp
{

public ref class DataColumnFloat32 : public DataColumn
{
public:
    DataColumnFloat32();
    DataColumnFloat32(DataFrame_ColumnFloat32* column);
    property System::Type^ Type
    {
        virtual System::Type^ get() override;
    }
    property System::String^ Name
    {
        virtual System::String^ get() override;
        virtual void set(System::String^ name) override;
    }

    cli::array<float>^ ToArray();

    System::Collections::BitArray^ NAValues();

    property Maybe<float> default[int]
    {
        Maybe<float> get(int index);
        void set(int index, Maybe<float> value);
    }
};

}
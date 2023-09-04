#pragma once

namespace DataFrameSharp
{

generic<typename T>
public value struct Maybe
{
    Maybe(T x)
    {
        Value = x;
        HasValue = true;
    }

    T Value;
    bool HasValue;
};

}

#include <cstdint>
#include <cstdlib>
#include <cstring>


namespace DataFrameSharp
{

    System::String^ ToString(const char* c)
    {
        if(!c) return nullptr;

#ifdef NET46
        return System::Text::Encoding::UTF8->GetString((unsigned char*)c, sz);
#else
        auto sz = strlen(c);
        auto bytes = gcnew cli::array<uint8_t>((int)sz);
        if(bytes->Length)
        {
            pin_ptr<uint8_t> p = &bytes[0];
            memcpy(p, c, sz);
        }
        return System::Text::Encoding::UTF8->GetString(bytes);
#endif
    }

    char* ToString(System::String^ s)
    {
        if(!s) return nullptr;
        auto bytes = System::Text::Encoding::UTF8->GetBytes(s);
        char* c = (char*)malloc(bytes->Length + 1);
        if(!c) throw gcnew System::Exception("Out of memory");
        if(bytes->Length)
        {
            pin_ptr<uint8_t> p = &bytes[0];
            memcpy(c, p, bytes->Length);
        }
        c[bytes->Length] = 0;
        return c;
    }

}

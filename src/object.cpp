#include "object.hpp"
#include <zlib.h>
#include <stdexcept>

std::string GitBlob::serialize() const
{
    return this->GitBlob::blob_data; 
}

void GitBlob::deserialize(std::string_view data)
{
    this->GitBlob::blob_data = std::string(data);
}



std::string GitBlob::serialize() const
{
    return blob_data;
}

void GitBlob::deserialize(std::string_view data)
{
    blob_data = std::string(data);
}

namespace
{
    std::string to_hex(const unsigned char* bytes, unsigned int len)
    {
        static const char digits[] = "0123456789abcdef";
        std::string hex;
        hex.reserve(len * 2);
        for (unsigned int i = 0; i < len; ++i)
        {
            hex.push_back(digits[bytes[i] >> 4]);
            hex.push_back(digits[bytes[i] & 0x0F]);
        }
        return hex;
    }

    std::string zlib_compress(const std::string& raw)
    {
        uLongf n = compressBound(raw.size());
        std::string out(n, '\0');
        int rc = compress2(reinterpret_cast<Bytef*>(out.data()), &n,
                           reinterpret_cast<const Bytef*>(raw.data()), raw.size(),
                           Z_DEFAULT_COMPRESSION);
        if (rc != Z_OK)
            throw std::runtime_error("zlib compression failed");
        out.resize(n);
        return out;
    }


}
#include "object.hpp"



std::string GitBlob::serialize() const
{
    return this->GitBlob::blob_data; 
}

void GitBlob::deserialize(std::string_view data)
{
    this->GitBlob::blob_data = std::string(data);
}
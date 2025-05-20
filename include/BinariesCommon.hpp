#ifndef BINARIES_COMMON_HPP
#define BINARIES_COMMON_HPP
#include <cstdint>

namespace binaries
{
    struct Header
    {
        char magic[4];
        uint32_t version;
    };

    struct Test
    {
        uint32_t value;
    };

    void writeBinary()
    {

    }
} //namespace binaries

#endif //BINARIES_COMMON_HPP

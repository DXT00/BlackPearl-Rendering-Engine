#include "pch.h"
#include "Core/Codec/CrcHash.h"
#include "Core/Codec/Crc.h"

namespace BlackPearl {

     uint32_t CrcHash::Crc32(const uint8_t* Input, size_t InputLen)
    {
    
        return FCrc::MemCrc32(Input, InputLen);
    }

}

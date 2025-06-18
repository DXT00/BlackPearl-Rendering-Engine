#pragma once

#include <cstdint>
#include <string>
#include <iomanip>  //  std::setw, std::setfill
#include <ios>      //  std::hex
#include <sstream>
namespace BlackPearl {
    struct CrcHash
    {
       

        CrcHash() {
            //memset(Hash, 0, sizeof(Hash));
            Hash = 0;
        };
        bool IsValid() {
            return !ToString().empty();
        }
        std::string ToString() const
        {
            //// 转换为十六进制
            std::stringstream ss;
            //for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                ss << std::hex << std::setw(2) << std::setfill('0') << Hash;
            //}
            return ss.str();
        }

        uint32_t Hash;

        static uint32_t Crc32(const uint8_t* Input, size_t InputLen);

        bool operator==(const CrcHash& other) const {
            return this->Hash == other.Hash;
        }

        bool operator!=(const CrcHash& other) const {
            return !(*this == other);
        }

        CrcHash& operator=(const CrcHash& other) {
            if (this != &other) {
              
                    this->Hash = other.Hash;
                

            }
            return *this;

        }
        /*  FSHA256& operator=(FSHA256& other) {
              if (this != &other) {
                  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                      this->Hash[i] = other.Hash[i];
                  }

              }
              return *this;

          }*/


    };

}

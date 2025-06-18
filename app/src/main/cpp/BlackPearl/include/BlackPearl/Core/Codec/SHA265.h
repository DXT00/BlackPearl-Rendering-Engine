#pragma once

#include <cstdint>
#include<sstream>

//#ifdef GE_PLATFORM_WINDOWS
//#include <Windows.h>
//#endif
//#include <openssl/hmac.h>
//#include <openssl/sha.h>
//#include <openssl/ssl.h>
#include "Core/Platform.h"
#include "Core/Templates/EnableIf.h"
#include <iomanip>  //  std::setw, std::setfill
#include <ios>      //  std::hex


namespace BlackPearl {
#ifndef  SHA256_DIGEST_LENGTH
#define SHA256_DIGEST_LENGTH 32
#endif // ! SHA256_DIGEST_LENGTH

    ////////////////////////////////////////////////////////////////////////////////
    struct FSHA256
    {
        using ByteArray = uint8_t[SHA256_DIGEST_LENGTH];

        FSHA256() {
            memset(Hash, 0, sizeof(Hash));
        };
        bool IsValid() {
            return !ToString().empty();
        }
        std::string ToString() const
        {
            // 转换为十六进制
            std::stringstream ss;
            for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                ss << std::hex << std::setw(2) << std::setfill('0') << (int)Hash[i];
            }
            return ss.str();
        }

        alignas(uint32_t) ByteArray Hash {};


        bool operator==(const FSHA256& other) const {
            return this->ToString() == other.ToString(); 
        }

        bool operator!=(const FSHA256& other) const {
            return !(*this == other);
        }

         FSHA256& operator=(const FSHA256& other) {
            if (this != &other) {
                for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                    this->Hash[i] = other.Hash[i];
                }
                
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

    FSHA256 Sha256(const uint8_t* Input, size_t InputLen);
    
}

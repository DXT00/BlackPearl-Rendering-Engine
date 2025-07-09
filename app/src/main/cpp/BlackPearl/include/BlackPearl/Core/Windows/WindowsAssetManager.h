//
// Created by DXT00 on 2025/5/7.
//
#pragma once
#include <string>
namespace BlackPearl {
#ifdef LoadImage
#undef LoadImage
#endif
    class WindowsAssetManager {
    public:
        static bool IsFileExist(const std::string& relPath);

        static unsigned char * LoadImage(const std::string &relPath, int &width, int &height, int &channels);
        static std::string  LoadGlslFile(const std::string& relPath);
        static void StoreGLSLShader( const std::string &shaderCode, const std::string &name);

        static const std::vector<uint8_t>& LoadBuffer(const std::string& relPath);


    };



    typedef WindowsAssetManager AssetManager;

}



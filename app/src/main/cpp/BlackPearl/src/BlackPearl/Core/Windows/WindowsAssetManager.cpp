//
// Created by DXT00 on 2025/5/7.
//
#include "pch.h"
#include <fstream>
#include "RHI/Common/stb_util.h"
#include "Core/Windows/WindowsAssetManager.h"
#include "Core/Memory.h"

#ifdef LoadImage
#undef LoadImage
#endif
namespace BlackPearl{

    unsigned char* WindowsAssetManager::LoadImage(const std::string& relPath, int &width ,int& height, int& channels){


        // 示例：用 stb_image 加载图片（假设是 PNG/JPG）
        int _w, _h, _c;

        unsigned char* imageData =  stbi_load_util(relPath.c_str(), &_w, &_h, &_c, 0);


        if (imageData) {
            width = _w;
            height = _h;
            channels = _c;
            GE_CORE_INFO("Loaded image: %dx%d, %d channels", width, height, channels);
        } else {
            GE_CORE_ERROR("Failed to decode image!");
        }

        return imageData;

    }
    std::string WindowsAssetManager::LoadGlslFile(const std::string& relPath)
    {
        std::string result;
        std::ifstream in(relPath, std::ios::in | std::ios::binary);
        if (in) {
            in.seekg(0, std::ios::end);
            int len = in.tellg();
            result.resize(len);
            in.seekg(0, std::ios::beg);
            in.read(&result[0], result.size());
            in.close();
        }
        else {
            GE_ASSERT("Could not open file '{0}'", filepath);
        }
        return result;
    }

    bool WindowsAssetManager::IsFileExist(const std::string &relPath) {


        std::ifstream file(relPath);
        return file.good(); // 文件可打开则存在

    }
    void WindowsAssetManager::StoreGLSLShader(const std::string &shaderCode, const std::string &name) {
        // 写入UTF-8文件
        std::ofstream out(name, std::ios::binary);
        std::string text = shaderCode;
        out.write(text.c_str(), text.size());

    }

    const std::vector<uint8_t>& WindowsAssetManager::LoadBuffer(const std::string& relPath)
    {
        std::ifstream file(relPath, std::ios::binary | std::ios::ate);

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(size);
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            GE_ASSERT(0, "read binary file failed");
        }
        return  buffer;
    }

}
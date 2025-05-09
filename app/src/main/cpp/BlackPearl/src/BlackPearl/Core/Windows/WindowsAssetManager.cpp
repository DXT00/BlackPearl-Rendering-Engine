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





        //unsigned char* data = stbi_load_util(m_Path.c_str(), &width, &height, &nrChannels, 0);

        // 示例：用 stb_image 加载图片（假设是 PNG/JPG）
        int _w, _h, _c;

        unsigned char* imageData =  stbi_load_util(relPath.c_str(), &_w, &_h, &_c, 0);



        //std::vector<unsigned char> TextContents;


            //std::vector<unsigned char> TextContents;

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

    bool WindowsAssetManager::IsFileExist(const std::string &relPath) {


        std::ifstream file(relPath);
        return file.good(); // 文件可打开则存在

    }


}
//
// Created by DXT00 on 2025/5/7.
//

#include <android/asset_manager.h>
#include <android/log.h>

#include "RHI/Common/stb_util.h"
#include "Core/Android/AndroidAssetManager.h"
#include "Core/Memory.h"
#include <string>
namespace BlackPearl{

    extern AAssetManager * AndroidThunkCpp_GetAssetManager();

    // Android 相对路径 是基于 assets目录，需要去除 assets/
    std::string removeAssetsPrefix(const std::string& path) {
        const std::string prefix = "assets/";
        size_t pos = path.find(prefix);
        if (pos == 0) { // 确保前缀在开头
            return path.substr(prefix.length());
        }
        return path; // 如果不是以assets/开头，返回原路径
    }



    unsigned char* AndroidAssetManager::LoadImage(const std::string& relPath, int& width ,int& height, int& channels){

#if USE_ANDROID_JNI
        std::string assetRelPath = removeAssetsPrefix(relPath);
        AAssetManager* AssetMgr = AndroidThunkCpp_GetAssetManager();
        AAsset* asset = AAssetManager_open(AssetMgr, assetRelPath.c_str(), AASSET_MODE_BUFFER);

        if (!asset) {
            GE_CORE_ERROR( "Failed to open asset: %s", relPath.c_str());
            return nullptr;
        }




        // 获取文件数据
        const void* fileData = AAsset_getBuffer(asset);
        off_t fileSize = AAsset_getLength(asset);

        int _w, _h, _c;
        // 示例：用 stb_image 加载图片（假设是 PNG/JPG）
        unsigned char* imageData = stbi_load_from_memory_util(
                (const unsigned char*)fileData, fileSize, &_w, &_h, &_c, 0
        );
        //std::vector<unsigned char> TextContents;

        if (imageData) {
            width = _w;
            height = _h;
            channels = _c;
            GE_CORE_INFO("Loaded image: %dx%d, %d channels", width, height, channels);
//
//            TextContents.resize(fileSize + 1);
//            FMemory::Memcpy(TextContents.data(), imageData, fileSize);
//            TextContents[fileSize] = 0;
//
//
//            stbi_image_free_util(imageData); // 释放内存
        } else {
            GE_CORE_ERROR("Failed to decode image!");
        }

        AAsset_close(asset); // 关闭 asset

        return imageData;
#endif
        return nullptr;
    }

    bool AndroidAssetManager::IsFileExist(const std::string &relPath) {

#if USE_ANDROID_JNI
        std::string assetRelPath = removeAssetsPrefix(relPath);
        AAssetManager* AssetMgr = AndroidThunkCpp_GetAssetManager();
        AAsset* asset = AAssetManager_open(AssetMgr, assetRelPath.c_str(), AASSET_MODE_UNKNOWN);
        if (asset)
        {
            AAsset_close(asset);
            return true;
        }
#endif
        return false;

    }


}
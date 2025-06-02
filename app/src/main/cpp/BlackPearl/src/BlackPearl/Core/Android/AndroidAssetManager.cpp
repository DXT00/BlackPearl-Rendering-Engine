//
// Created by DXT00 on 2025/5/7.
//

#include <android/asset_manager.h>
#include <android/log.h>

#include "RHI/Common/stb_util.h"
#include "Core/Android/AndroidAssetManager.h"
#include "Core/Memory.h"
#include <string>
#include <android/native_activity.h>

namespace BlackPearl{

    extern AAssetManager * AndroidThunkCpp_GetAssetManager();
    extern ANativeActivity *GNativeActivity;

    // Android 相对路径 是基于 assets目录，需要去除 assets/
    std::string removeAssetsPrefix(const std::string& path) {
        const std::string prefix = "assets/";
        size_t pos = path.find(prefix);
        if (pos == 0) { // 确保前缀在开头
            return path.substr(prefix.length());
        }
        return path; // 如果不是以assets/开头，返回原路径
    }


#if USE_ANDROID_JNI
    AAsset* GetAsset(const std::string& relPath){
        std::string assetRelPath = removeAssetsPrefix(relPath);
        AAssetManager* AssetMgr = AndroidThunkCpp_GetAssetManager();
        AAsset* asset = AAssetManager_open(AssetMgr, assetRelPath.c_str(), AASSET_MODE_BUFFER);

        if (!asset) {
            GE_CORE_ERROR( "Failed to open asset: %s", relPath.c_str());
            return nullptr;
        }
        return asset;
    }
#endif


    unsigned char* AndroidAssetManager::LoadImage(const std::string& relPath, int& width ,int& height, int& channels){

#if USE_ANDROID_JNI


        AAsset* asset = GetAsset(relPath);
        if(!asset)
            return nullptr;
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

    std::string AndroidAssetManager::LoadGlslFile(const std::string &relPath) {
#if USE_ANDROID_JNI
         AAsset* asset = GetAsset(relPath);
         if(!asset)
             return nullptr;

        // 获取文件长度
        size_t length = AAsset_getLength(asset);
        std::vector<char> buffer(length + 1);

        // 读取文件内容
        AAsset_read(asset, buffer.data(), length);
        buffer[length] = '\0'; // 添加字符串结束符

        // 关闭资源
        AAsset_close(asset);

        return std::string(buffer.data());
#endif
        return "";


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


    void AndroidAssetManager::StoreGLSLShader( const std::string& shaderCode, const std::string& name) {

#if USE_ANDROID_JNI
        // 获取外部存储路径（需确保有权限）
        const char* extPath = GNativeActivity->externalDataPath;
        if (!extPath) {
            GE_CORE_ERROR("StoreShader: External storage not available");
            return;
        }

        // 拼接完整文件路径
        std::string fullPath = std::string(extPath) + "/" + name;

        // 写入文件（二进制模式，避免换行符转换）
        std::ofstream out(fullPath, std::ios::binary);
        if (out) {
            out.write(shaderCode.c_str(), shaderCode.size());
            out.close();
            GE_CORE_INFO("StoreShader :Saved to: %s", fullPath.c_str());
        } else {
            GE_CORE_ERROR("StoreShader Failed to write file");
        }
#endif
    }

} // BlackPearl
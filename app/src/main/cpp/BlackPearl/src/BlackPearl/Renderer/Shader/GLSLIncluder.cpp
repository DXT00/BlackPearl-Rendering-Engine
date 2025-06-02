#include "pch.h"
#include "Renderer/Shader/GLSLIncluder.h"
#include <filesystem>
#include "Core/AssetManager.h"


namespace fs = std::filesystem;
namespace BlackPearl {
    bool GLSLIncluder::findIncludeFile(const std::string& filename,
        const std::vector<std::string>& searchPaths,
        std::string& resolvedPath,
        std::string& content) {
        /* for (const auto& path : searchPaths) {
             fs::path fullPath;
             if (path.empty()) {
                 fullPath = filename;
             }
             else {
                 fullPath = fs::path(path) / filename;
             }

             if (fs::exists(fullPath)) {
                 std::ifstream file(fullPath.string());
                 if (file.is_open()) {
                     content.assign((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
                     resolvedPath = fullPath.string();
                     return true;
                 }
             }
         }*/
        for (size_t i = 0; i < searchPaths.size(); i++)
        {
            std::string filePath = searchPaths[i].empty()? filename: (searchPaths[i] + "/" + filename);
            if (AssetManager::IsFileExist(filePath)) {
                content = AssetManager::LoadGlslFile(filePath);
                resolvedPath = filename;
                return true;
            }

        }
        
        return false;
    }
    bool isIncludeCommented(const std::string& code, size_t includePos) {
        // 1. 检查单行注释 "//"
        size_t lineStart = code.rfind('\n', includePos);
        lineStart = (lineStart == std::string::npos) ? 0 : lineStart + 1;

        if (code.find("//", lineStart) < includePos) {
            return true; // 被单行注释
        }

        // 2. 检查多行注释 "/* ... */"
        size_t blockCommentStart = code.rfind("/*", includePos);
        if (blockCommentStart != std::string::npos) {
            size_t blockCommentEnd = code.rfind("*/", includePos);
            if (blockCommentEnd == std::string::npos || blockCommentEnd < blockCommentStart) {
                return true; // 在多行注释块内
            }
        }

        return false;
    }
    // 直接处理字符串版本的 GLSL 代码
    std::string GLSLIncluder::processIncludes(const std::string& glslCode, const std::string& currentDir) {
        std::string result;
        size_t pos = 0;
        std::vector<std::string> tempSearchPaths = m_searchPaths;

        if (!currentDir.empty()) {
            tempSearchPaths.insert(tempSearchPaths.begin(), currentDir);
        }

        while (pos < glslCode.length()) {
            // 查找下一个 #include 指令
            size_t includeStart = glslCode.find("#include", pos);
            if (includeStart == std::string::npos) {
                // 没有更多 include 指令了，添加剩余部分
                result += glslCode.substr(pos);
                break;
            }

     

            if (isIncludeCommented(glslCode, includeStart)) {
                // 不是有效的 include 指令，跳过
                result += glslCode.substr(pos, includeStart - pos + 8); // +8 是 "#include" 的长度
                pos = includeStart + 8;
                continue;
            }

            // 添加 #include 之前的内容
            result += glslCode.substr(pos, includeStart - pos);

            // 提取文件名
            size_t fileStart = glslCode.find_first_of("\"<", includeStart);
            if (fileStart == std::string::npos) {
                 size_t fileStart = glslCode.find_first_of("\"", includeStart);
                 if (fileStart == std::string::npos) {
                     // 格式错误，保留原指令
                     result += glslCode.substr(includeStart, 8);
                     pos = includeStart + 8;
                     continue;
                 }
               
            }

            char quoteChar = glslCode[fileStart];
            char endQuoteChar = (quoteChar == '<') ? '>' : '"';
            size_t fileEnd = glslCode.find(endQuoteChar, fileStart + 1);
            if (fileEnd == std::string::npos) {
                // 格式错误，保留原指令
                GE_CORE_WARN("Include format invalid!");
                result += glslCode.substr(includeStart, 8);
                pos = includeStart + 8;
                continue;
            }

            std::string filename = glslCode.substr(fileStart + 1, fileEnd - fileStart - 1);

            // 处理包含文件
            if (m_includedFiles.count(filename) == 0) {
                std::string fileContent;
                std::string resolvedPath;
                if (findIncludeFile(filename, tempSearchPaths, resolvedPath, fileContent)) {
                    m_includedFiles.insert(filename);

                    // 递归处理包含文件中的 include
                    std::string includedDir = fs::path(resolvedPath).parent_path().string();
                    std::string processedContent = processIncludes(fileContent, includedDir);

                    result += "// [Start of include: " + filename + "]\n";
                    result += processedContent;
                    result += "\n// [End of include: " + filename + "]\n";
                }
                else {
                    GE_CORE_ERROR("Error: Could not open include file %s\n" , filename.c_str());
                    //std::cerr << "Error: Could not open include file '" << filename << "'\n";
                    // 保留原始指令
                    result += glslCode.substr(includeStart, fileEnd - includeStart + 1);
                }
            }
            else {
                result += "// [Already included: " + filename + "]\n";
            }

            pos = fileEnd + 1;
        }

        return result;
    }

    std::string GLSLIncluder::loadShader(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filePath);
        }

        std::string content((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());

        std::string currentDir = fs::path(filePath).parent_path().string();
        return processIncludes(content, currentDir);
    }
    void GLSLIncluder::reset()
    {
        m_includedFiles.clear();
    }
}
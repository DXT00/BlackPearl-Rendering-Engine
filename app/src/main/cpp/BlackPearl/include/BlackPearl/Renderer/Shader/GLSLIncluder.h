#pragma once
#include "pch.h"
#include <filesystem>
namespace fs = std::filesystem;
namespace BlackPearl {

    class GLSLIncluder {
    public:
        // 构造函数，可以设置搜索路径
        GLSLIncluder(const std::vector<std::string>& searchPaths = {})
            : m_searchPaths(searchPaths) {
            // 添加当前目录作为默认搜索路径
            if (m_searchPaths.empty()) {
                m_searchPaths.push_back("");
            }
        }
        std::string processIncludes(const std::string& source, const std::string& currentDir = "");
        std::string loadShader(const std::string& filePath);

    private:
        std::vector<std::string> m_searchPaths;
        std::set<std::string> m_includedFiles; // 跟踪已包含的文件，防止循环包含

        // 检查是否是#include指令
        bool isIncludeDirective(const std::string& line) {
            size_t pos = line.find("#include");
            if (pos == std::string::npos) {
                return false;
            }

            // 检查前面是否只有空白字符
            for (size_t i = 0; i < pos; ++i) {
                if (!std::isspace(line[i])) {
                    return false;
                }
            }

            return true;
        }

        // 从#include指令中提取文件名
        std::string extractIncludeFilename(const std::string& line) {
            size_t start = line.find('"');
            if (start == std::string::npos) {
                start = line.find('<');
                if (start == std::string::npos) {
                    return "";
                }
                size_t end = line.find('>', start + 1);
                if (end == std::string::npos) {
                    return "";
                }
                return line.substr(start + 1, end - start - 1);
            }
            else {
                size_t end = line.find('"', start + 1);
                if (end == std::string::npos) {
                    return "";
                }
                return line.substr(start + 1, end - start - 1);
            }
        }

        // 在搜索路径中查找包含文件
        bool findIncludeFile(const std::string& filename,
            const std::vector<std::string>& searchPaths,
            std::string& resolvedPath,
            std::string& content);
    };
}

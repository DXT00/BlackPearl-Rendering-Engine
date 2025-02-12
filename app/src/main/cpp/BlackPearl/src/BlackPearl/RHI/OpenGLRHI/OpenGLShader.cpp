#pragma once
#include"pch.h"
#include <glad/glad.h>
#include "OpenGLShader.h"
#include <BlackPearl/Core.h>
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/LightComponent/SpotLight.h"
#include "BlackPearl/Component/LightComponent/Light.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "OpenGLBoundShaderState.h"
#include "OpenGLShaderResource.h"
#include "BlackPearl/Log.h"

#include "OpenGLDevice.h"
#include "OpenGLState.h"
#include "BlackPearl/RHI/RHIShader.h"


namespace BlackPearl
{
#define STATS 1
    static uint32_t GCurrentDriverProgramBinaryAllocation = 0;
    static uint32_t GNumPrograms = 0;

    extern class Log* g_Log;

    // Helper to verify a compiled shader 
// returns true if shader was compiled without any errors or errors should be ignored
    static bool VerifyShaderCompilation(GLuint Resource, GLenum type)
    {
       // VERIFY_GL_SCOPE();
        // Verify that an OpenGL shader has compiled successfully.
       // SCOPE_CYCLE_COUNTER(STAT_OpenGLShaderCompileVerifyTime);
       // {
            GLint CompileStatus;
            glGetShaderiv(Resource, GL_COMPILE_STATUS, &CompileStatus);
            if (CompileStatus != GL_TRUE)
            {
                
                    GLint LogLength;
                    glGetShaderiv(Resource, GL_INFO_LOG_LENGTH, &LogLength);
                    
#if GE_PLATFORM_ANDROID
                    if (LogLength == 0)
                    {
                        // make it big anyway
                        // there was a bug in android 2.2 where glGetShaderiv would return 0 even though there was a error message
                        // https://code.google.com/p/android/issues/detail?id=9953
                        LogLength = 4096;
                    }
#endif
                    std::vector<GLchar> infoLog(LogLength);
                    glGetShaderInfoLog(Resource, LogLength, &LogLength, &infoLog[0]);
                    GE_ERROR_JUDGE();
                    glDeleteShader(Resource);
                    GE_ERROR_JUDGE();

                    std::string shaderType;
                    if (type == GL_VERTEX_SHADER)shaderType = "vertex shader";
                    else if (type == GL_FRAGMENT_SHADER)shaderType = "fragment shader";
                    else if (type == GL_GEOMETRY_SHADER)shaderType = "geometry shader";
                    else if (type == GL_COMPUTE_SHADER)shaderType = "compute shader";
                    else if (type == GL_TESS_CONTROL_SHADER)shaderType = "tessellation control shader";
                    else if (type == GL_TESS_EVALUATION_SHADER)shaderType = "tessellation evaluation shader";


                    GE_CORE_ERROR("{0} compile failed :{1}", shaderType, infoLog.data());
                    GE_ASSERT(false, "Shader compliation failure!")
                    
                    return false;
            }
        //}
        return true;
    }

    static GLenum ShaderTypeFromString(const std::string &type) {

        if (type == "vertex")
            return GL_VERTEX_SHADER;
        if (type == "fragment" || type == "pixel")
            return GL_FRAGMENT_SHADER;
        if (type == "geometry")
            return GL_GEOMETRY_SHADER;
        if (type == "compute")
            return GL_COMPUTE_SHADER;
        if (type == "tessellation_control_shader")
            return GL_TESS_CONTROL_SHADER;
        if (type == "tessellation_evaluation_shader")
            return GL_TESS_EVALUATION_SHADER;
        //GE_ASSERT(false, "Unknown shader type!");
        return 0;
    }

    void Shader::getBytecode(const void **ppBytecode, size_t *pSize) const {
        // we don't save these for vulkan
        if (ppBytecode) *ppBytecode = nullptr;
        if (pSize) *pSize = 0;
    }

    Shader::Shader(
            const std::string &vertexSrc,
            const std::string &fragmentSrc,
            const std::string &geometrySrc,
            const std::string &tessCtlSrc,
            const std::string &tessEvlSrc
    ) {
        std::unordered_map<GLenum, std::string> shaderSources;
        shaderSources[GL_VERTEX_SHADER] = vertexSrc;
        shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;
        shaderSources[GL_GEOMETRY_SHADER] = geometrySrc;
        shaderSources[GL_TESS_CONTROL_SHADER] = tessCtlSrc;
        shaderSources[GL_TESS_EVALUATION_SHADER] = tessEvlSrc;

        Compile(shaderSources);

    }

    Shader::Shader(const ShaderDesc &_desc, const void *binaryCode, size_t binarySize) 
    {
        desc = _desc;
        // 后续启动时加载二进制数据
        GLuint newProgram = glCreateProgram();
        //load_from_file("shader.bin", &binary, &binaryLength);
        //TODO::  desc.binaryformat 要与第一次编译是时候一样，通常存储binaryCode在第一个GLenum(uint_32t)
        //BinaryFormat is stored at the start of ProgramBinary array
        glProgramBinary(newProgram, desc.binaryformat, binaryCode, binarySize);
        free((void *) binaryCode);

        // 验证加载是否成功
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(newProgram, GL_LINK_STATUS, &linkStatus);
        if (linkStatus == GL_FALSE) {
            // 处理错误
            GLint maxLength = 0;
            glGetProgramiv(newProgram, GL_INFO_LOG_LENGTH, &maxLength);
            GE_ERROR_JUDGE();
            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(newProgram, maxLength, &maxLength, &infoLog[0]);
            GE_ERROR_JUDGE();
            // We don't need the program anymore.
            glDeleteProgram(newProgram);
            GE_ERROR_JUDGE();

            GE_CORE_ERROR("{0}", infoLog.data());
            GE_ASSERT(false, "Shader link failure!")

            return;
        }
    }

    Shader::Shader(const ShaderDesc &_desc, const std::string &filepath) 
        :desc(_desc) {
        if (filepath.empty()) {

            GE_CORE_WARN("no shader path found");
            return;
        }
        m_ShaderPath = filepath;
        m_GlslCode = ReadFile(filepath);
        std::string commonSource = ReadFile(m_CommonStructPath);

        std::unordered_map<GLenum, std::string> shaderSources = PreProcess(m_GlslCode, commonSource);
        Compile(shaderSources);

    }

    Shader::~Shader() {

        if (m_RendererID == -1) {
            return;
        }
        GLint has_deleted = 0;
        glGetProgramiv(m_RendererID, GL_DELETE_STATUS, &has_deleted);
        if (has_deleted != GL_TRUE) {
            glDeleteProgram(m_RendererID);
        }

    }


    std::string Shader::ReadFile(const std::string &filepath) {
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary);
        if (in) {
            in.seekg(0, std::ios::end);
            int len = in.tellg();
            result.resize(len);
            in.seekg(0, std::ios::beg);
            in.read(&result[0], result.size());
            in.close();
        } else {
            GE_ASSERT("Could not open file '{0}'", filepath);
        }
        return result;
    }

    std::unordered_map<GLenum, std::string>
    Shader::PreProcess(const std::string &source, const std::string &commonSource) {

        GE_ASSERT((!source.empty()), "shader source code is empty");
        std::unordered_map<unsigned int, std::string> shaderSources;
        const char *typeToken = "#type";
        size_t typeTockenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0);//find�Ҳ����᷵��npos
        while (pos != std::string::npos) {
            size_t eol = source.find_first_of("\r\n", pos);
            GE_ASSERT(eol != std::string::npos, "Syntax error");
            size_t begin = pos + typeTockenLength + 1;
            std::string type = source.substr(begin, eol - begin);
            GE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            pos = source.find(typeToken, nextLinePos);
            shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos,
                                                                      pos - (nextLinePos == std::string::npos ?
                                                                             source.size() - 1
                                                                                                              : nextLinePos));//string::npos表示source的末尾位置

        }
        //add common struct source
        if (shaderSources.find(GL_FRAGMENT_SHADER) != shaderSources.end()) {
            size_t pos = shaderSources[GL_FRAGMENT_SHADER].find("#version", 0);//find找不到会返回npos
            GE_ASSERT(pos != std::string::npos, "Syntax error");

            size_t eol = shaderSources[GL_FRAGMENT_SHADER].find_first_of("\r\n", pos);
            GE_ASSERT(eol != std::string::npos, "Syntax error");

            std::string front = shaderSources[GL_FRAGMENT_SHADER].substr(pos, eol - pos + 1);
            std::string res = shaderSources[GL_FRAGMENT_SHADER].substr(eol);
            shaderSources[GL_FRAGMENT_SHADER] = front + commonSource + res;
        }
        return shaderSources;

    }

    void Shader::Compile(const std::unordered_map<GLenum, std::string> &shaderSources) {
        GLuint program = glCreateProgram();
        GE_ERROR_JUDGE();

        std::vector<GLuint> ShaderID;
        for (auto &kv: shaderSources) {
            GLenum type = kv.first;
            const std::string &source = kv.second;
            GLuint shader = glCreateShader(type);
            GE_ERROR_JUDGE();
            ShaderID.push_back(shader);

            const GLchar *sourceCstr = source.c_str();
            glShaderSource(shader, 1, &sourceCstr, 0);
            GE_ERROR_JUDGE();
            glCompileShader(shader);
            GE_ERROR_JUDGE();
            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            GE_ERROR_JUDGE();
            if (isCompiled == GL_FALSE) {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
                GE_ERROR_JUDGE();

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
                GE_ERROR_JUDGE();
                glDeleteShader(shader);
                GE_ERROR_JUDGE();

                std::string shaderType;
                if (type == GL_VERTEX_SHADER)shaderType = "vertex shader";
                else if (type == GL_FRAGMENT_SHADER)shaderType = "fragment shader";
                else if (type == GL_GEOMETRY_SHADER)shaderType = "geometry shader";
                else if (type == GL_COMPUTE_SHADER)shaderType = "compute shader";
                else if (type == GL_TESS_CONTROL_SHADER)shaderType = "tessellation control shader";
                else if (type == GL_TESS_EVALUATION_SHADER)shaderType = "tessellation evaluation shader";


                GE_CORE_ERROR("{0} compile failed :{1}", shaderType, infoLog.data());
                GE_ASSERT(false, "Shader compliation failure!")

                break;
            }
            // Attach our shaders to our program
            glAttachShader(program, shader);
            GE_ERROR_JUDGE();
        }


        // Vertex and fragment shaders are successfully compiled.
        // Now time to link them together into a program.
        // Get a program object.

        // Link our program
        glLinkProgram(program);
        GE_ERROR_JUDGE();
        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int *) &isLinked);
        GE_ERROR_JUDGE();
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
            GE_ERROR_JUDGE();
            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
            GE_ERROR_JUDGE();
            // We don't need the program anymore.
            glDeleteProgram(program);
            GE_ERROR_JUDGE();
            // Don't leak shaders either.
            for (auto &shader: ShaderID) {
                glDeleteShader(shader);
                GE_ERROR_JUDGE();
            }

            GE_CORE_ERROR("{0}", infoLog.data());
            GE_ASSERT(false, "Shader link failure!")

            return;
        }

        glValidateProgram(program);
        GE_ERROR_JUDGE();

        // Always detach shaders after a successful link.
        for (auto &shader: ShaderID) {
            glDetachShader(program, shader);
            GE_ERROR_JUDGE();
        }
        m_RendererID = program;
    }


    void Shader::Compile(GLenum shaderType)
    {
        //TODO:: multi thread compile
        //FScopeLock Lock(&GCompiledShaderCacheCS);
        //FOpenGLCompiledShaderValue& FoundShader = GetOpenGLCompiledShaderCache().FindOrAdd(ShaderCodeKey);
       // Resource = FoundShader.Resource;
        if (m_ShaderID == 0)
        {
            //SCOPE_CYCLE_COUNTER(STAT_OpenGLShaderCompileTime);
            m_ShaderID = FOpenGL::CreateShader(shaderType);

           /* TArray<ANSICHAR> UncompressedShaderCode = FoundShader.GetUncompressedShader();
            int32_t GlslCodeLength = UncompressedShaderCode.Num() - 1;
            const ANSICHAR* UncompressedGlslCodeString = UncompressedShaderCode.GetData();*/
            const GLchar* sourceCstr = m_GlslCode.c_str();
            glShaderSource(m_ShaderID, 1, &sourceCstr, nullptr);
            glCompileShader(m_ShaderID);
            const bool bSuccessfullyCompiled = VerifyShaderCompilation(m_ShaderID, shaderType);
            assert(bSuccessfullyCompiled);

        }
    }

    void Shader::SetLightUniform(LightSources lightSources) {
        unsigned int pointLightIndex = 0;
        this->SetUniform1i("u_PointLightNums", lightSources.GetPointLightNum());
        for (auto lightObj: lightSources.Get()) {

            if (lightObj->HasComponent<ParallelLight>()) {
                auto lightSource = lightObj->GetComponent<ParallelLight>();
                this->SetUniform1ui("u_LightType", (unsigned int) LightType::ParallelLight);
                this->SetUniform1i("u_HasParallelLight", 1);
                this->SetUniformVec3f("u_ParallelLight.ambient", lightSource->GetLightProps().ambient);
                this->SetUniformVec3f("u_ParallelLight.diffuse", lightSource->GetLightProps().diffuse);
                this->SetUniformVec3f("u_ParallelLight.specular", lightSource->GetLightProps().specular);
                this->SetUniformVec3f("u_ParallelLight.direction", lightSource->GetDirection());
                this->SetUniform1f("u_ParallelLight.intensity", lightSource->GetLightProps().intensity);

            }
            if (lightObj->HasComponent<PointLight>()) {

                auto lightSource = lightObj->GetComponent<PointLight>();
                this->SetUniform1ui("u_LightType", (unsigned int) LightType::PointLight);

                this->SetUniformVec3f("u_PointLights[" + std::to_string(pointLightIndex) + "].ambient",
                                      lightSource->GetLightProps().ambient);
                this->SetUniformVec3f("u_PointLights[" + std::to_string(pointLightIndex) + "].diffuse",
                                      lightSource->GetLightProps().diffuse);
                this->SetUniformVec3f("u_PointLights[" + std::to_string(pointLightIndex) + "].specular",
                                      lightSource->GetLightProps().specular);
                this->SetUniformVec3f("u_PointLights[" + std::to_string(pointLightIndex) + "].position",
                                      lightObj->GetComponent<Transform>()->GetPosition());
                this->SetUniform1f("u_PointLights[" + std::to_string(pointLightIndex) + "].intensity",
                                   lightSource->GetLightProps().intensity);

                this->SetUniform1f("u_PointLights[" + std::to_string(pointLightIndex) + "].constant",
                                   lightSource->GetAttenuation().constant);
                this->SetUniform1f("u_PointLights[" + std::to_string(pointLightIndex) + "].linear",
                                   lightSource->GetAttenuation().linear);
                this->SetUniform1f("u_PointLights[" + std::to_string(pointLightIndex) + "].quadratic",
                                   lightSource->GetAttenuation().quadratic);

                pointLightIndex++;
            }

            if (lightObj->HasComponent<SpotLight>()) {
                auto lightSource = lightObj->GetComponent<SpotLight>();

                lightSource->UpdatePositionAndDirection(Renderer::GetSceneData()->CameraPosition,
                                                        Renderer::GetSceneData()->CameraFront);//SpotLight��ʱ��ǵø���Camera

                this->SetUniform1ui("u_LightType", (unsigned int) LightType::SpotLight);
                this->SetUniform1i("u_HasSpotLight", 1);

                this->SetUniformVec3f("u_SpotLight.ambient", lightSource->GetLightProps().ambient);
                this->SetUniformVec3f("u_SpotLight.diffuse", lightSource->GetLightProps().diffuse);
                this->SetUniformVec3f("u_SpotLight.specular", lightSource->GetLightProps().specular);
                this->SetUniformVec3f("u_SpotLight.position",
                                      lightSource->GetPosition()); //TODO:Position Ӧ�ô�Transform��
                this->SetUniformVec3f("u_SpotLight.direction", lightSource->GetDirection());

                this->SetUniform1f("u_SpotLight.cutOff", lightSource->GetCutOffAngle());
                this->SetUniform1f("u_SpotLight.outerCutOff", lightSource->GetOuterCutOffAngle());

                this->SetUniform1f("u_SpotLight.constant", lightSource->GetAttenuation().constant);
                this->SetUniform1f("u_SpotLight.linear", lightSource->GetAttenuation().linear);
                this->SetUniform1f("u_SpotLight.quadratic", lightSource->GetAttenuation().quadratic);
            }


        }

    }

    void Shader::Bind() const {
        GE_ASSERT((glIsProgram(m_RendererID) == GL_TRUE), "glIsProgram(m_RendererID) != GL_TRUE");

        GLint compileStat;
        //glGetShaderiv(m_RendererID, GL_COMPILE_STATUS, &compileStat);

        GE_ERROR_JUDGE();

        //glGetShaderInfoLog(m_RendererID, 1024, length, infoLog);
        glUseProgram(m_RendererID);

        GLsizei bufLen = 0;        // length of buffer to allocate
        GLsizei strLen = 0;        // strlen GL actually wrote to buffer

        /*glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &bufLen);
        if (bufLen > 1)
        {*/
        GLchar *infoLog = new GLchar[1024];
        glGetProgramInfoLog(m_RendererID, bufLen, &strLen, infoLog);
        if (strLen > 0) {
            std::string result = reinterpret_cast<char *>(infoLog);
            GE_CORE_INFO(result);

        }
        delete[] infoLog;
        //}


        /*	std::vector<char> v(1024);
            glGetProgramInfoLog(m_RendererID, 1024, NULL, v.data());
            std::string s(begin(v), end(v));
            GE_CORE_INFO(s);*/

        GE_ERROR_JUDGE();
    }

    void Shader::Unbind() const {
        glUseProgram(0);
        GE_ERROR_JUDGE();


    }

    void Shader::SetUniform1i(const std::string &name, int val) const {
        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        GE_ERROR_JUDGE();

        if (loc != -1)
            glUniform1i(loc, val);
        GE_ERROR_JUDGE();

    }

    void Shader::SetUniform1ui(const std::string &name, const unsigned int val) const {
        glUniform1ui(glGetUniformLocation(m_RendererID, name.c_str()), val);
        GE_ERROR_JUDGE();

    }

    void Shader::SetUniform1f(const std::string &name, float val) const {
        glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), val);
        GE_ERROR_JUDGE();

    }

    void Shader::SetUniformMat4f(const std::string &name, const glm::mat4 &mat) const {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
        GE_ERROR_JUDGE();

    }

    void Shader::SetUniformMat3x4f(const std::string &name, const float *mat3x4, uint32_t count) const {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glProgramUniformMatrix3x4fv(m_RendererID, location, count, GL_FALSE, mat3x4);
        GE_ERROR_JUDGE();

    }

    void Shader::SetUniformMat4f(const std::string &name, const float *mat4x4, uint32_t count) const {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glProgramUniformMatrix4fv(m_RendererID, location, count, GL_FALSE, mat4x4);
    }

    void Shader::SetUniformVec3f(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]);
        GE_ERROR_JUDGE();

    }

    void Shader::SetUniformVec2f(const std::string &name, const glm::vec2 &value) const {
        glUniform2fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]);
        GE_ERROR_JUDGE();

    }

    void Shader::SetUniformVec3f(const std::string &name, const math::float3 &value) const {
        glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]);
        GE_ERROR_JUDGE();

    }

    void Shader::SetUniformVec2f(const std::string &name, const math::float2 &value) const {
        glUniform2fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]);
        GE_ERROR_JUDGE();

    }

    void Shader::SetUniformVec2i(const std::string &name, const glm::ivec2 &value) const {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        GE_ERROR_JUDGE();

        glUniform2iv(location, 1, &value[0]);
        GE_ERROR_JUDGE();

    }



    static void SetNewProgramStats(GLuint Program)
    {

#if STATS
        GLint BinaryLength = 0;
        glGetProgramiv(Program, GL_PROGRAM_BINARY_LENGTH, &BinaryLength);
#endif

        GNumPrograms++;
#if STATS
        GCurrentDriverProgramBinaryAllocation += BinaryLength;
#endif
    }


 /**
 * Verify that an OpenGL program has linked successfully.
 */
    static bool VerifyLinkedProgram(GLuint program)
    {
       

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
        GE_ERROR_JUDGE();
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
            GE_ERROR_JUDGE();
            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
            GE_ERROR_JUDGE();
            // We don't need the program anymore.
            glDeleteProgram(program);
            GE_ERROR_JUDGE();
            // Don't leak shaders either.
            for (auto& shader : ShaderID) {
                glDeleteShader(shader);
                GE_ERROR_JUDGE();
            }

            GE_CORE_ERROR("{0}", infoLog.data());
            GE_ASSERT(false, "Shader link failure!")

            return false;
        }

        return true;
    }

    static void ConfigureStageStates(FOpenGLLinkedProgram* LinkedProgram)
    {
        const FOpenGLLinkedProgramConfiguration& Config = LinkedProgram->Config;

        if (Config.Shaders[ShaderType::Vertex].bValid)
        {
            LinkedProgram->ConfigureShaderStage(
                ShaderType::Vertex,
                OGL_FIRST_UNIFORM_BUFFER
            );
            //assert(LinkedProgram->StagePackedUniformInfo[ShaderType::Vertex].PackedUniformInfos.Num() <= Config.Shaders[ShaderType::Vertex].Bindings.PackedGlobalArrays.Num());
        }

        if (Config.Shaders[ShaderType::Pixel].bValid)
        {
            LinkedProgram->ConfigureShaderStage(
                ShaderType::Pixel,
                OGL_FIRST_UNIFORM_BUFFER +
                Config.Shaders[ShaderType::Vertex].Bindings.NumUniformBuffers
            );
           // assert(LinkedProgram->StagePackedUniformInfo[ShaderType::Pixel].PackedUniformInfos.Num() <= Config.Shaders[ShaderType::Pixel].Bindings.PackedGlobalArrays.Num());
        }

        if (Config.Shaders[ShaderType::Geometry].bValid)
        {
            LinkedProgram->ConfigureShaderStage(
                ShaderType::Geometry,
                OGL_FIRST_UNIFORM_BUFFER +
                Config.Shaders[ShaderType::Vertex].Bindings.NumUniformBuffers +
                Config.Shaders[ShaderType::Pixel].Bindings.NumUniformBuffers
            );
           // assert(LinkedProgram->StagePackedUniformInfo[ShaderType::Geometry].PackedUniformInfos.Num() <= Config.Shaders[ShaderType::Geometry].Bindings.PackedGlobalArrays.Num());
        }

        if (Config.Shaders[ShaderType::Compute].bValid)
        {
            LinkedProgram->ConfigureShaderStage(
                ShaderType::Compute,
                OGL_FIRST_UNIFORM_BUFFER
            );
            //assert(LinkedProgram->StagePackedUniformInfo[ShaderType::Compute].PackedUniformInfos.Num() <= Config.Shaders[ShaderType::Compute].Bindings.PackedGlobalArrays.Num());
        }
    }


    template<class TOpenGLStage0RHI, class TOpenGLStage1RHI>
    static void BindShaderStage(FOpenGLLinkedProgramConfiguration& Config, CrossCompiler::EShaderStage NextStage, TOpenGLStage0RHI* NextStageShaderIn, CrossCompiler::EShaderStage PrevStage, TOpenGLStage1RHI* PrevStageShaderIn)
    {
        auto* PrevStageShader = FOpenGLDynamicRHI::ResourceCast(PrevStageShaderIn);
        auto* NextStageShader = FOpenGLDynamicRHI::ResourceCast(NextStageShaderIn);

        assert(NextStageShader && PrevStageShader);

        FOpenGLLinkedProgramConfiguration::ShaderInfo& ShaderInfo = Config.Shaders[NextStage];
        FOpenGLLinkedProgramConfiguration::ShaderInfo& PrevInfo = Config.Shaders[PrevStage];

        GLuint NextStageResource = NextStageShader->Resource;
        FOpenGLShaderBindings NextStageBindings = NextStageShader->Bindings;

        ShaderInfo.Bindings = NextStageBindings;
        ShaderInfo.Resource = NextStageResource;
    }

    static FOpenGLLinkedProgramConfiguration CreateConfig(IShader* VertexShaderRHI, IShader* PixelShaderRHI, IShader* GeometryShaderRHI)
    {
        Shader* VertexShader   = static_cast<Shader*>(VertexShaderRHI);
        Shader* PixelShader    = static_cast<Shader*>(PixelShaderRHI);
        Shader* GeometryShader = static_cast<Shader*>(GeometryShaderRHI);

        FOpenGLLinkedProgramConfiguration Config;

        assert(VertexShaderRHI);
        assert(PixelShaderRHI);

        VertexShader->Compile(GL_VERTEX_SHADER);
        PixelShader->Compile(GL_FRAGMENT_SHADER);

        // Fill-in the configuration
        Config.Shaders[ShaderType::Vertex].Bindings = VertexShader->Bindings;
        Config.Shaders[ShaderType::Vertex].Resource = VertexShader->m_ShaderID;
        Config.Shaders[ShaderType::Vertex].ShaderKey = VertexShader->ShaderCodeKey;
        Config.Shaders[ShaderType::Vertex].bValid = true;
        Config.ProgramKey.ShaderHashes[ShaderType::Vertex] = VertexShaderRHI->GetHash();

        if (GeometryShaderRHI)
        {
            assert(VertexShader);
            GeometryShader->Compile(GL_GEOMETRY_SHADER);
            BindShaderStage(Config, ShaderType::Geometry, GeometryShaderRHI, ShaderType::Vertex, VertexShaderRHI);
            Config.ProgramKey.ShaderHashes[ShaderType::Geometry] = GeometryShaderRHI->GetHash();
            Config.Shaders[ShaderType::Geometry].ShaderKey = GeometryShader->ShaderCodeKey;
            Config.Shaders[ShaderType::Geometry].bValid = true;
        }

        assert(GeometryShaderRHI || VertexShaderRHI);
        if (GeometryShaderRHI)
        {
            BindShaderStage(Config, ShaderType::Pixel, PixelShaderRHI, ShaderType::Geometry, GeometryShaderRHI);
        }
        else
        {
            BindShaderStage(Config, ShaderType::Pixel, PixelShaderRHI, ShaderType::Vertex, VertexShaderRHI);
        }
        Config.ProgramKey.ShaderHashes[ShaderType::Pixel] = PixelShaderRHI->GetHash();
        Config.Shaders[ShaderType::Pixel].ShaderKey = PixelShader->ShaderCodeKey;
        Config.Shaders[ShaderType::Pixel].bValid = true;


        return Config;
    };


    FOpenGLLinkedProgram* Device::LinkProgram(Shader* vertexShader, Shader* pixelShader, Shader* geometryShader)
    {
        // Make sure we have OpenGL context set up, and invalidate the parameters cache and current program (as we'll link a new one soon)
        GetContextStateForCurrentContext().Program = -1;
        //        MarkShaderParameterCachesDirty(PendingState.ShaderParameters, false);
                //PendingState.LinkedProgramAndDirtyFlag = nullptr;
        //
        //        //  SCOPE_CYCLE_COUNTER(STAT_OpenGLShaderLinkTime);
        //        // VERIFY_GL_SCOPE();
        //
        //        // ensure that compute shaders are always alone
        //        check((Config.Shaders[ShaderType::Vertex].Resource == 0) !=
        //              (Config.Shaders[ShaderType::Compute].Resource == 0));
        //        check((Config.Shaders[ShaderType::Pixel].Resource == 0) !=
        //              (Config.Shaders[ShaderType::Compute].Resource == 0));
        //
        GLuint Program = 0;
        FOpenGL::GenProgramPipelines(1, &Program);

        if (vertexShader->m_ShaderID) {
            FOpenGL::UseProgramStages(Program, GL_VERTEX_SHADER_BIT,
                vertexShader->m_ShaderID);
        }
        if (pixelShader->m_ShaderID) {
            FOpenGL::UseProgramStages(Program, GL_FRAGMENT_SHADER_BIT,
                pixelShader->m_ShaderID);
        }
        if (geometryShader->m_ShaderID) {
            FOpenGL::UseProgramStages(Program, GL_GEOMETRY_SHADER_BIT,
                geometryShader->m_ShaderID);
        }
        //        if (Config.Shaders[ShaderType::Compute].Resource) {
        //            FOpenGL::UseProgramStages(Program, GL_COMPUTE_SHADER_BIT,
        //                                      Config.Shaders[ShaderType::Compute].Resource);
        //        }
        //
        //        if (FOpenGLProgramBinaryCache::IsEnabled() || FGLProgramCache::IsUsingLRU()) {
        //            FOpenGL::ProgramParameter(Program, PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
        //        }

                // Link.
        glLinkProgram(Program);

        if (!VerifyLinkedProgram(Program)) {
            //TODO:: delete Shader
            return nullptr;
        }

        SetNewProgramStats(Program);

        FOpenGL::BindProgramPipeline(Program);

        FOpenGLLinkedProgram* LinkedProgram = new FOpenGLLinkedProgram(Config, Program);

        ConfigureStageStates(LinkedProgram);

        //#if ENABLE_UNIFORM_BUFFER_LAYOUT_VERIFICATION
        //        VerifyUniformBufferLayouts(Program);
        //#endif // #if ENABLE_UNIFORM_BUFFER_LAYOUT_VERIFICATION
        return LinkedProgram;
    }


    void Device::CachedBindUniformBuffer(FOpenGLContextState& ContextState, GLuint Buffer)
    {
        //VERIFY_GL_SCOPE();
        //check(IsInRenderingThread() || IsInRHIThread());
        if (ContextState.UniformBufferBound != Buffer)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, Buffer);
            ContextState.UniformBufferBound = Buffer;
        }
    }

    void Device::BindUniformBufferBase(FOpenGLContextState &ContextState, int32_t NumUniformBuffers,
                                       uint32_t **BoundUniformBuffers, uint32_t FirstUniformBuffer, bool ForceUpdate) {
        /*SCOPE_CYCLE_COUNTER_DETAILED(STAT_OpenGLUniformBindTime);
        VERIFY_GL_SCOPE();
        checkSlow(IsInRenderingThread() || IsInRHIThread());*/

//        for (int32_t BufferIndex = 0; BufferIndex < NumUniformBuffers; ++BufferIndex) {
//            GLuint Buffer = 0;
//            uint32_t Offset = 0;
//            uint32_t Size = ZERO_FILLED_DUMMY_UNIFORM_BUFFER_SIZE;
//            int32_t BindIndex = FirstUniformBuffer + BufferIndex;
//
//            if (BoundUniformBuffers[BufferIndex]) {
//                /*			FRHIUniformBuffer* UB = BoundUniformBuffers[BufferIndex];
//                            FOpenGLUniformBuffer* GLUB = ((FOpenGLUniformBuffer*)UB);*/
//                Buffer = (GLuint) BoundUniformBuffers[BufferIndex]; //GLUB->Resource;
//
//                /*if (GLUB->bIsEmulatedUniformBuffer)
//                {
//                    continue;
//                }*/
//
//                Size = GLUB->GetSize();
//#if SUBALLOCATED_CONSTANT_BUFFER
//                Offset = GLUB->Offset;
//#endif
//            } else {
//                if (PendingState.ZeroFilledDummyUniformBuffer == 0) {
//                    void *ZeroBuffer = malloc(ZERO_FILLED_DUMMY_UNIFORM_BUFFER_SIZE);
//                    FMemory::Memzero(ZeroBuffer, ZERO_FILLED_DUMMY_UNIFORM_BUFFER_SIZE);
//                    FOpenGL::GenBuffers(1, &PendingState.ZeroFilledDummyUniformBuffer);
//                    assert(PendingState.ZeroFilledDummyUniformBuffer != 0);
//                    CachedBindUniformBuffer(ContextState, PendingState.ZeroFilledDummyUniformBuffer);
//                    glBufferData(GL_UNIFORM_BUFFER, ZERO_FILLED_DUMMY_UNIFORM_BUFFER_SIZE, ZeroBuffer, GL_STATIC_DRAW);
//                    delete ZeroBuffer;
//
//                    //OpenGLBufferStats::UpdateUniformBufferStats(ZERO_FILLED_DUMMY_UNIFORM_BUFFER_SIZE, true);
//                }
//
//                Buffer = PendingState.ZeroFilledDummyUniformBuffer;
//            }
//
//            if (ForceUpdate || (Buffer != 0 && ContextState.UniformBuffers[BindIndex] != Buffer) ||
//                ContextState.UniformBufferOffsets[BindIndex] != Offset) {
//                FOpenGL::BindBufferRange(GL_UNIFORM_BUFFER, BindIndex, Buffer, Offset, Size);
//                ContextState.UniformBuffers[BindIndex] = Buffer;
//                ContextState.UniformBufferOffsets[BindIndex] = Offset;
//                ContextState.UniformBufferBound = Buffer;    // yes, calling glBindBufferRange also changes uniform buffer binding.
//            }
//        }
    }

    void Device::BindPendingShaderState(FOpenGLContextState &ContextState) {
        bool ForceUniformBindingUpdate = false;

//        GLuint PendingProgram = PendingState.BoundShaderState->LinkedProgram->Program;
//        if (ContextState.Program != PendingProgram) {
//            FOpenGL::BindProgramPipeline(PendingProgram);
//            ContextState.Program = PendingProgram;
//            //MarkShaderParameterCachesDirty(PendingState.ShaderParameters, false);
//            //PendingState.LinkedProgramAndDirtyFlag = nullptr;
//        }
//
//        if (PendingState.bAnyDirtyRealUniformBuffers[(int) ShaderType::Vertex] ||
//            PendingState.bAnyDirtyRealUniformBuffers[(int) ShaderType::Pixel] ||
//            PendingState.bAnyDirtyRealUniformBuffers[(int) ShaderType::Geometry]) {
//            int32_t NextUniformBufferIndex = OGL_FIRST_UNIFORM_BUFFER;
//
//            /*static_assert(SF_NumGraphicsFrequencies == 5 && SF_NumFrequencies == 10, "Unexpected SF_ ordering");
//            static_assert(SF_RayGen > SF_NumGraphicsFrequencies, "SF_NumGraphicsFrequencies be the number of frequencies supported in OpenGL");*/
//
//            int32_t NumUniformBuffers[(int) ShaderType::AllGraphics];
//
//            //PendingState.GraphicsPipline->pipelineBindingLayouts->getNumUniformBuffers(NumUniformBuffers);
//
//            if (PendingState.bAnyDirtyRealUniformBuffers[(int) ShaderType::Vertex]) {
//                BindUniformBufferBase(
//                        ContextState,
//                        NumUniformBuffers[(int) ShaderType::Vertex],
//                        PendingState.BoundUniformBuffers[(int) ShaderType::Vertex],
//                        NextUniformBufferIndex,
//                        ForceUniformBindingUpdate);
//            }
//            NextUniformBufferIndex += NumUniformBuffers[(int) ShaderType::Vertex];
//
//            if (PendingState.bAnyDirtyRealUniformBuffers[(int) ShaderType::Pixel]) {
//                BindUniformBufferBase(
//                        ContextState,
//                        NumUniformBuffers[(int) ShaderType::Pixel],
//                        PendingState.BoundUniformBuffers[(int) ShaderType::Pixel],
//                        NextUniformBufferIndex,
//                        ForceUniformBindingUpdate);
//            }
//            NextUniformBufferIndex += NumUniformBuffers[(int) ShaderType::Pixel];
//
//            if (NumUniformBuffers[(int) ShaderType::Geometry] >= 0 &&
//                PendingState.bAnyDirtyRealUniformBuffers[(int) ShaderType::Geometry]) {
//                BindUniformBufferBase(
//                        ContextState,
//                        NumUniformBuffers[(int) ShaderType::Geometry],
//                        PendingState.BoundUniformBuffers[(int) ShaderType::Geometry],
//                        NextUniformBufferIndex,
//                        ForceUniformBindingUpdate);
//                NextUniformBufferIndex += NumUniformBuffers[(int) ShaderType::Geometry];
//            }
//
//            PendingState.bAnyDirtyRealUniformBuffers[(int) ShaderType::Vertex] = false;
//            PendingState.bAnyDirtyRealUniformBuffers[(int) ShaderType::Pixel] = false;
//            PendingState.bAnyDirtyRealUniformBuffers[(int) ShaderType::Geometry] = false;
//        }

        /*	if (FOpenGL::SupportsBindlessTexture())
            {
                SetupBindlessTextures(ContextState, PendingState.BoundShaderState->LinkedProgram->Samplers);
            }*/
    }



    IBoundShaderState* Device::RHICreateBoundShaderState_Internal(
            IInputLayout *VertexDeclarationRHI,
            IShader *VertexShaderRHI,
            IShader *PixelShaderRHI,
            IShader *GeometryShaderRHI,
            bool bFromPSOFileCache
    ) {
        //VERIFY_GL_SCOPE();
        assert(!bFromPSOFileCache);

       // TODO:: add OpenGLProgramsCache
        // Check if we already have such a program in released programs cache. Use it, if we do.
        FOpenGLLinkedProgram *LinkedProgram = 0;

    
        if (!LinkedProgram) {
            //FOpenGLLinkedProgram* CachedProgram = GetOpenGLProgramsCache().Find(Config.ProgramKey, true);
            //if (!CachedProgram)
            //{
            //	// ensure that pending request for this program has been completed before
            //	if (FOpenGLProgramBinaryCache::CheckSinglePendingGLProgramCreateRequest(Config.ProgramKey))
            //	{
            //		CachedProgram = GetOpenGLProgramsCache().Find(Config.ProgramKey, true);
            //	}
            //}

            //if (CachedProgram)
            //{
            //	LinkedProgram = CachedProgram;
            //	if (!LinkedProgram->bConfigIsInitalized)
            //	{
            //		LinkedProgram->SetConfig(Config);
            //		// We now have the config for this program, we must configure the program for use.
            //		ConfigureGLProgramStageStates(LinkedProgram);
            //	}
            //}
            //else
            //{
                //OGL_BINARYCACHE_STATS_MARKBINARYCACHEMISS(Config.ProgramKey, true);

                Shader* VertexShader = static_cast<Shader*>(VertexShaderRHI);
                Shader* PixelShader = static_cast<Shader*>(PixelShaderRHI);
                Shader* GeometryShader = static_cast<Shader*>(GeometryShaderRHI);

                // Link program, using the data provided in config
                LinkedProgram = LinkProgram(VertexShader, PixelShader, GeometryShader);

                if (LinkedProgram == NULL) {
#if DEBUG_GL_SHADERS
                    if (VertexShader)
                    {
                        UE_LOG(LogRHI, Error, TEXT("Vertex Shader:\n%s"), ANSI_TO_TCHAR(VertexShader->GlslCode.GetData()));
                    }
                    if (PixelShader)
                    {
                        UE_LOG(LogRHI, Error, TEXT("Pixel Shader:\n%s"), ANSI_TO_TCHAR(PixelShader->GlslCode.GetData()));
                    }
                    if (GeometryShader)
                    {
                        UE_LOG(LogRHI, Error, TEXT("Geometry Shader:\n%s"), ANSI_TO_TCHAR(GeometryShader->GlslCode.GetData()));
                    }
#endif //DEBUG_GL_SHADERS
                    /*FName LinkFailurePanic = bFromPSOFileCache ? FName("FailedProgramLinkDuringPrecompile") : FName("FailedProgramLink");
                    RHIGetPanicDelegate().ExecuteIfBound(LinkFailurePanic);
                    UE_LOG(LogRHI, Fatal, TEXT("Failed to link program [%s]. Current total programs: %d, precompile: %d"), *Config.ProgramKey.ToString(), GNumPrograms, (uint32)bFromPSOFileCache);*/
                } else {
                    //TODO:: Cache program
                    //if (ShouldCacheAllProgramBinaries() &&
                    //    FOpenGLProgramBinaryCache::RequiresCaching(Config.ProgramKey)) {
                    //    // In precache mode we can put any newly compiled programs in the binary cache
                    //    FOpenGLProgramBinary CompiledProgram = UE::OpenGL::GetProgramBinaryFromGLProgram(
                    //            LinkedProgram->Program);
                    //    FOpenGLProgramBinaryCache::CacheProgramBinary(Config.ProgramKey,
                    //                                                  TUniqueObj<FOpenGLProgramBinary>(
                    //                                                          MoveTemp(CompiledProgram)));
                    //}

                    //GetOpenGLProgramsCache().Add(Config.ProgramKey, LinkedProgram);
                }

                return new BoundShaderState(
                    VertexDeclarationRHI,
                    VertexShader,
                    PixelShader,
                    GeometryShader
                );
           // }
        }

        //assert(VertexDeclarationRHI);

        //FOpenGLVertexDeclaration* VertexDeclaration = ResourceCast(VertexDeclarationRHI);

      
        //}
    }

    void FOpenGLLinkedProgram::ConfigureShaderStage(int Stage, uint32_t FirstUniformBuffer)
    {
        static const GLint FirstTextureUnit[ShaderType::NUM_COMPILE_SHADER_STAGES] =
        {
            FOpenGL::GetFirstVertexTextureUnit(),
            FOpenGL::GetFirstPixelTextureUnit(),
            FOpenGL::GetFirstGeometryTextureUnit(),
            0,
            0,
            FOpenGL::GetFirstComputeTextureUnit()
        };

        static const GLint MaxTextureUnit[ShaderType::NUM_COMPILE_SHADER_STAGES] =
        {
            FOpenGL::GetMaxVertexTextureImageUnits(),
            FOpenGL::GetMaxTextureImageUnits(),
            FOpenGL::GetMaxGeometryTextureImageUnits(),
            0,
            0,
            FOpenGL::GetMaxComputeTextureImageUnits()
        };

        static const GLint FirstUAVUnit[ShaderType::NUM_COMPILE_SHADER_STAGES] =
        {
            FOpenGL::GetFirstVertexUAVUnit(),
            FOpenGL::GetFirstPixelUAVUnit(),
            OGL_UAV_NOT_SUPPORTED_FOR_GRAPHICS_UNIT,
            OGL_UAV_NOT_SUPPORTED_FOR_GRAPHICS_UNIT,
            OGL_UAV_NOT_SUPPORTED_FOR_GRAPHICS_UNIT,
            FOpenGL::GetFirstComputeUAVUnit()
        };

        //SCOPE_CYCLE_COUNTER(STAT_OpenGLShaderBindParameterTime);
        //VERIFY_GL_SCOPE();

        FOpenGLUniformName Name;
        Name.Buffer[0] = CrossCompiler::ShaderStageIndexToTypeName(Stage);

        GLuint StageProgram = Program;

        // Bind Global uniform arrays (vu_h, pu_i, etc)
        {
            Name.Buffer[1] = 'u';
            Name.Buffer[2] = '_';
            Name.Buffer[3] = 0;
            Name.Buffer[4] = 0;

            TArray<FPackedUniformInfo> PackedUniformInfos;
            for (uint8 Index = 0; Index < CrossCompiler::PACKED_TYPEINDEX_MAX; ++Index)
            {
                uint8 ArrayIndexType = CrossCompiler::PackedTypeIndexToTypeName(Index);
                Name.Buffer[3] = ArrayIndexType;
                GLint Location = glGetUniformLocation(StageProgram, Name.Buffer);
                if ((int32)Location != -1)
                {
                    FPackedUniformInfo Info = { Location, ArrayIndexType, Index };
                    PackedUniformInfos.Add(Info);
                }
            }

            SortPackedUniformInfos(PackedUniformInfos, Config.Shaders[Stage].Bindings.PackedGlobalArrays, StagePackedUniformInfo[Stage].PackedUniformInfos);
        }

        // Bind uniform buffer packed arrays (vc0_h, pc2_i, etc)
        {
            Name.Buffer[1] = 'c';
            Name.Buffer[2] = 0;
            Name.Buffer[3] = 0;
            Name.Buffer[4] = 0;
            Name.Buffer[5] = 0;
            Name.Buffer[6] = 0;

            check(StagePackedUniformInfo[Stage].PackedUniformBufferInfos.Num() == 0);
            int32 NumUniformBuffers = Config.Shaders[Stage].Bindings.NumUniformBuffers;
            StagePackedUniformInfo[Stage].PackedUniformBufferInfos.SetNum(NumUniformBuffers);
            int32 NumPackedUniformBuffers = Config.Shaders[Stage].Bindings.PackedUniformBuffers.Num();
            check(NumPackedUniformBuffers <= NumUniformBuffers);

            for (int32 UB = 0; UB < NumPackedUniformBuffers; ++UB)
            {
                const TArray<CrossCompiler::FPackedArrayInfo>& PackedInfo = Config.Shaders[Stage].Bindings.PackedUniformBuffers[UB];
                TArray<FPackedUniformInfo>& PackedBuffers = StagePackedUniformInfo[Stage].PackedUniformBufferInfos[UB];

                ANSICHAR* Str = SetIndex(Name.Buffer, 2, UB);
                *Str++ = '_';
                Str[1] = 0;
                for (uint8 Index = 0; Index < PackedInfo.Num(); ++Index)
                {
                    Str[0] = PackedInfo[Index].TypeName;
                    GLint Location = glGetUniformLocation(StageProgram, Name.Buffer); // This could be -1 if optimized out
                    FPackedUniformInfo Info = { Location, PackedInfo[Index].TypeName,  PackedInfo[Index].TypeIndex };
                    PackedBuffers.Add(Info);
                }
            }
        }

        // Reserve and setup Space for Emulated Uniform Buffers
        StagePackedUniformInfo[Stage].LastEmulatedUniformBufferSet.Empty(Config.Shaders[Stage].Bindings.NumUniformBuffers);
        StagePackedUniformInfo[Stage].LastEmulatedUniformBufferSet.AddZeroed(Config.Shaders[Stage].Bindings.NumUniformBuffers);

        // Bind samplers.
        Name.Buffer[1] = 's';
        Name.Buffer[2] = 0;
        Name.Buffer[3] = 0;
        Name.Buffer[4] = 0;
        int32 LastFoundIndex = -1;
        for (int32 SamplerIndex = 0; SamplerIndex < Config.Shaders[Stage].Bindings.NumSamplers; ++SamplerIndex)
        {
            SetIndex(Name.Buffer, 2, SamplerIndex);
            GLint Location = glGetUniformLocation(StageProgram, Name.Buffer);
            if (Location == -1)
            {
                if (LastFoundIndex != -1)
                {
                    // It may be an array of samplers. Get the initial element location, if available, and count from it.
                    SetIndex(Name.Buffer, 2, LastFoundIndex);
                    int32 OffsetOfArraySpecifier = (LastFoundIndex > 9) ? 4 : 3;
                    int32 ArrayIndex = SamplerIndex - LastFoundIndex;
                    Name.Buffer[OffsetOfArraySpecifier] = '[';
                    ANSICHAR* EndBracket = SetIndex(Name.Buffer, OffsetOfArraySpecifier + 1, ArrayIndex);
                    *EndBracket++ = ']';
                    *EndBracket = 0;
                    Location = glGetUniformLocation(StageProgram, Name.Buffer);
                }
            }
            else
            {
                LastFoundIndex = SamplerIndex;
            }

            if (Location != -1)
            {
                if (OpenGLConsoleVariables::bBindlessTexture == 0 || !FOpenGL::SupportsBindlessTexture())
                {
                    // Non-bindless, setup the unit info
                    FOpenGL::ProgramUniform1i(StageProgram, Location, FirstTextureUnit[Stage] + SamplerIndex);
                    TextureStageNeeds[FirstTextureUnit[Stage] + SamplerIndex] = true;
                    MaxTextureStage = FMath::Max(MaxTextureStage, FirstTextureUnit[Stage] + SamplerIndex);
                    if (SamplerIndex >= MaxTextureUnit[Stage])
                    {
                        UE_LOG(LogShaders, Error, TEXT("%s has a shader using too many textures (idx %d, max allowed %d) at stage %d"), *Config.ProgramKey.ToString(), SamplerIndex, MaxTextureUnit[Stage] - 1, Stage);
                        checkNoEntry();
                    }
                }
                else
                {
                    //Bindless, save off the slot information
                    FOpenGLBindlessSamplerInfo Info;
                    Info.Handle = Location;
                    Info.Slot = FirstTextureUnit[Stage] + SamplerIndex;
                    Samplers.Add(Info);
                }
            }
        }

        // Bind UAVs/images.
        Name.Buffer[1] = 'i';
        Name.Buffer[2] = 0;
        Name.Buffer[3] = 0;
        Name.Buffer[4] = 0;
        int32 LastFoundUAVIndex = -1;
        for (int32 UAVIndex = 0; UAVIndex < Config.Shaders[Stage].Bindings.NumUAVs; ++UAVIndex)
        {
            ANSICHAR* Str = SetIndex(Name.Buffer, 2, UAVIndex);
            GLint Location = glGetUniformLocation(StageProgram, Name.Buffer);
            if (Location == -1)
            {
                // SSBO
                Str[0] = '_';
                Str[1] = 'V';
                Str[2] = 'A';
                Str[3] = 'R';
                Str[4] = '\0';
                Location = glGetProgramResourceIndex(StageProgram, GL_SHADER_STORAGE_BLOCK, Name.Buffer);
            }

            if (Location == -1)
            {
                if (LastFoundUAVIndex != -1)
                {
                    // It may be an array of UAVs. Get the initial element location, if available, and count from it.
                    SetIndex(Name.Buffer, 2, LastFoundUAVIndex);
                    int32 OffsetOfArraySpecifier = (LastFoundUAVIndex > 9) ? 4 : 3;
                    int32 ArrayIndex = UAVIndex - LastFoundUAVIndex;
                    Name.Buffer[OffsetOfArraySpecifier] = '[';
                    ANSICHAR* EndBracket = SetIndex(Name.Buffer, OffsetOfArraySpecifier + 1, ArrayIndex);
                    *EndBracket++ = ']';
                    *EndBracket = '\0';
                    Location = glGetUniformLocation(StageProgram, Name.Buffer);
                }
            }
            else
            {
                LastFoundUAVIndex = UAVIndex;
            }

            if (Location != -1)
            {
                // compute shaders have layout(binding) for images
                // glUniform1i(Location, FirstUAVUnit[Stage] + UAVIndex);

                // verify that only CS and PS uses UAVs (limitation on MALI GPUs)
                assert(Stage == ShaderType::Compute || Stage == ShaderType::Pixel);

                UAVStageNeeds[FirstUAVUnit[Stage] + UAVIndex] = true;
                MaxUAVUnitUsed = FMath::Max(MaxUAVUnitUsed, FirstUAVUnit[Stage] + UAVIndex);
            }
        }

        // Bind uniform buffers.
        if (FOpenGL::SupportsUniformBuffers())
        {
            Name.Buffer[1] = 'b';
            Name.Buffer[2] = 0;
            Name.Buffer[3] = 0;
            Name.Buffer[4] = 0;
            for (int32 BufferIndex = 0; BufferIndex < Config.Shaders[Stage].Bindings.NumUniformBuffers; ++BufferIndex)
            {
                SetIndex(Name.Buffer, 2, BufferIndex);
                GLint Location = GetOpenGLProgramUniformBlockIndex(StageProgram, Name);
                if (Location >= 0)
                {
                    GetOpenGLProgramUniformBlockBinding(StageProgram, Location, FirstUniformBuffer + BufferIndex);
                }
            }
        }
    }
  

}
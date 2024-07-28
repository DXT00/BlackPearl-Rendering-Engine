#pragma once
#include <memory>
#include <vector>
#include <string>   
#include <cstdint>
#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/Renderer/Buffer/Buffer.h"
#include "BlackPearl/Object/Object.h"
#include "glm/glm.hpp"
#include "d3d12.h"
#include "BlackPearl/RHI/D3D12RHI/d3dx12.h"
#include "BlackPearl/Log.h"
namespace BlackPearl {
    const char s_padding[4096] = {};
	class CommonFunc
	{
	public:
		static void ShowGBuffer(unsigned int row, unsigned int col, Object* quad, std::shared_ptr<GBuffer> gBuffer, std::vector<TextureHandle>textures);
        static void ShowFrameBuffer(unsigned int row, unsigned int col, Object* quad, std::shared_ptr<FrameBuffer> frameBuffer, std::vector<TextureHandle >textures);
        static void ShowFrameBuffer(glm::vec4 viewPort, Object* quad, std::shared_ptr<FrameBuffer> frameBuffer, TextureHandle texture, bool isMipmap, int lod);
        static void ShowTextures(unsigned int row, unsigned int col, Object* quad, std::vector<TextureHandle>textures);
        static void ShowTexture(glm::vec4 viewPort, Object* quad, TextureHandle texture, bool isMipmap, int lod = 0);
    };
    // Assign a name to the object to aid with debugging.
#if defined(_DEBUG) || defined(DBG)
    inline void SetName(ID3D12Object* pObject, LPCWSTR name)
    {
        pObject->SetName(name);
    }
    inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index)
    {
        WCHAR fullName[50];
        if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
        {
            pObject->SetName(fullName);
        }
    }
#else
    inline void SetName(ID3D12Object*, LPCWSTR)
    {
    }
    inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
    {
    }
#endif

    // Naming helper for ComPtr<T>.
    // Assigns the name of the variable as the name of the object.
    // The indexed variant will include the index in the name of the object.
#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)

    class HrException : public std::runtime_error
    {
        inline std::string HrToString(HRESULT hr)
        {
            char s_str[64] = {};
            sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
            return std::string(s_str);
        }
    public:
        HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
        HRESULT Error() const { return m_hr; }
    private:
        const HRESULT m_hr;
    };

    inline UINT Align(UINT size, UINT alignment)
    {
        return (size + (alignment - 1)) & ~(alignment - 1);
    }

    template<typename T> T Align(T size, T alignment)
    {
        return (size + alignment - 1) & ~(alignment - 1);
    }

    inline void AddAlignUp(uint32_t& offset, uint32_t size, uint32_t align = 4096u) {
        offset += size;
        uint32_t pad = (align - offset % align);
        offset += pad;
    }

    inline UINT CalculateConstantBufferByteSize(UINT byteSize)
    {
        // Constant buffer size is required to be aligned.
        return Align(byteSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
    }
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            GE_CORE_ERROR("function throw error");
            throw HrException(hr);
            //OutputDebugString(msg);
        }
    }

    inline void ThrowIfFailed(HRESULT hr, const wchar_t* msg)
    {
        if (FAILED(hr))
        {
            OutputDebugString(msg);
            throw HrException(hr);
        }
    }

    inline void ThrowIfFalse(bool value)
    {
        ThrowIfFailed(value ? S_OK : E_FAIL);
    }

    inline void ThrowIfFalse(bool value, const wchar_t* msg)
    {
        ThrowIfFailed(value ? S_OK : E_FAIL, msg);
    }

    // be careful to use this function, you need to delete wc after function is called, or it will have a memery leak.
    inline const wchar_t* To_Wchar(const char * c) {
        const size_t cSize = strlen(c)+1;
        wchar_t* wc = new wchar_t[cSize];
        mbstowcs(wc, c, cSize);
        return wc;
    }

    inline std::wstring To_WString(const char * c) {
        const size_t cSize = strlen(c)+1;
        std::wstring wc( cSize, L'#' );
        mbstowcs( &wc[0], c, cSize );
        return wc;
    }

    template<typename T>
    class Span
    {
    public:
        Span()
            :m_Data(nullptr),
            m_Count(0)
        { }
        Span(T* data, uint32_t size)
        :m_Data(data),
         m_Count(size)
        { }
        
        T* data() { return m_Data; }
        const T* data() const { return m_Data; }

        T& back() { return *(m_Data + m_Count - 1); }
        const T& back() const { return *(m_Data + m_Count - 1); }

        size_t size() const { return m_Count; }

        // Iterator interface
        T* begin() { return m_Data; }
        T* end() { return m_Data + m_Count; }

        T& operator[](uint32_t i) { return *(m_Data + i); }
        const T& operator[](uint32_t i) const { *(m_Data + i); }
    private:
        T* m_Data;
        uint32_t m_Count;
    };

    template <typename T>
    Span<T> MakeSpan(T* data, uint32_t size) { return Span<T>(data, size); }
    

    template <typename T>
    inline void WriteAlignUp(std::ostream& stream, const T* data, uint32_t count, uint32_t& offset, uint32_t align = 4096u)
    {
        stream.write(reinterpret_cast<const char*>(data), sizeof(T) * count);
        offset += sizeof(T) * count;

        uint32_t pad = (align - offset % align);

        stream.write(s_padding, pad);
        offset += pad;
    }

    template <typename T, typename U>
    constexpr T DivRoundUp(T num, U denom)
    {
        return (num + denom - 1) / denom;
    }
}


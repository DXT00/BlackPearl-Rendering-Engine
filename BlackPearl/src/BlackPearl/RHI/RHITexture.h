#pragma once
#include "RefCountPtr.h"
#include "RHIDefinitions.h"
#include "RHIResources.h"
#include "RHISampler.h"
namespace BlackPearl {


	enum TextureType {
		None,
		DiffuseMap,
		SpecularMap,
		EmissionMap,
		NormalMap,
		HeightMap,
		CubeMap,
		DepthMap,
		AoMap,
		RoughnessMap,
		MentallicMap,
		OpacityMap,
		ImageMap
	};

	struct TextureDesc
	{
		TextureDesc(){}
		TextureType type = TextureType::None;
		uint32_t width = 1;
		uint32_t height = 1;
		uint32_t depth = 1;
		uint32_t arraySize = 1;
		uint32_t mipLevels = 1;
		uint32_t sampleCount = 1;
		uint32_t sampleQuality = 0;
		Format format = Format::UNKNOWN;
		TextureDimension dimension = TextureDimension::Texture2D;
		std::string debugName;

		bool isShaderResource = true; // Note: isShaderResource is initialized to 'true' for backward compatibility
		bool isRenderTarget = false;
		bool isUAV = false;
		bool isTypeless = false;
		bool isShadingRateSurface = false;

		SharedResourceFlags sharedResourceFlags = SharedResourceFlags::None;

		// Indicates that the texture is created with no backing memory,
		// and memory is bound to the texture later using bindTextureMemory.
		// On DX12, the texture resource is created at the time of memory binding.
		bool isVirtual = false;

		Color clearValue;
		bool useClearValue = false;

		ResourceStates initialState = ResourceStates::Unknown;

		// If keepInitialState is true, command lists that use the texture will automatically
		// begin tracking the texture from the initial state and transition it to the initial state 
		// on command list close.
		bool keepInitialState = false;

//TODO:: 放开宏
//#ifdef GE_API_OPENGL
		
		TextureDesc(TextureType _type, const std::string& _path) {
			type = _type;
			path = _path;
		}

		// opengl 不需要sampler， filter在texture desc 里配置
		FilterMode minFilter = FilterMode::Linear;
		FilterMode magFilter = FilterMode::Linear;
		//int internalFormat = GL_RGBA;
		//int glformat = GL_RGBA;
		SamplerAddressMode wrap = SamplerAddressMode::ClampToEdge;
		//unsigned int dataType = GL_UNSIGNED_BYTE;
		bool generateMipmap = false;
		std::string path;
		//for cubemap
		std::vector<std::string> faces;
		float* data = nullptr;
//#endif



		constexpr TextureDesc& setWidth(uint32_t value) { width = value; return *this; }
		constexpr TextureDesc& setHeight(uint32_t value) { height = value; return *this; }
		constexpr TextureDesc& setDepth(uint32_t value) { depth = value; return *this; }
		constexpr TextureDesc& setArraySize(uint32_t value) { arraySize = value; return *this; }
		constexpr TextureDesc& setMipLevels(uint32_t value) { mipLevels = value; return *this; }
		constexpr TextureDesc& setSampleCount(uint32_t value) { sampleCount = value; return *this; }
		constexpr TextureDesc& setSampleQuality(uint32_t value) { sampleQuality = value; return *this; }
		constexpr TextureDesc& setFormat(Format value) { format = value; return *this; }
		constexpr TextureDesc& setDimension(TextureDimension value) { dimension = value; return *this; }
		TextureDesc& setDebugName(const std::string& value) { debugName = value; return *this; }
		constexpr TextureDesc& setIsRenderTarget(bool value) { isRenderTarget = value; return *this; }
		constexpr TextureDesc& setIsUAV(bool value) { isUAV = value; return *this; }
		constexpr TextureDesc& setIsTypeless(bool value) { isTypeless = value; return *this; }
		constexpr TextureDesc& setIsVirtual(bool value) { isVirtual = value; return *this; }
		constexpr TextureDesc& setClearValue(const Color& value) { clearValue = value; useClearValue = true; return *this; }
		constexpr TextureDesc& setUseClearValue(bool value) { useClearValue = value; return *this; }
		constexpr TextureDesc& setInitialState(ResourceStates value) { initialState = value; return *this; }
		constexpr TextureDesc& setKeepInitialState(bool value) { keepInitialState = value; return *this; }
	};
	// describes a 2D section of a single mip level + single slice of a texture
	struct TextureSlice
	{
		uint32_t x = 0;
		uint32_t y = 0;
		uint32_t z = 0;
		// -1 means the entire dimension is part of the region
		// resolve() below will translate these values into actual dimensions
		uint32_t width = uint32_t(-1);
		uint32_t height = uint32_t(-1);
		uint32_t depth = uint32_t(-1);

		uint32_t mipLevel = 0;
		uint32_t arraySlice = 0;

		[[nodiscard]] TextureSlice resolve(const TextureDesc& desc) const;

		constexpr TextureSlice& setOrigin(uint32_t vx = 0, uint32_t vy = 0, uint32_t vz = 0) { x = vx; y = vy; z = vz; return *this; }
		constexpr TextureSlice& setWidth(uint32_t value) { width = value; return *this; }
		constexpr TextureSlice& setHeight(uint32_t value) { height = value; return *this; }
		constexpr TextureSlice& setDepth(uint32_t value) { depth = value; return *this; }
		constexpr TextureSlice& setSize(uint32_t vx = uint32_t(-1), uint32_t vy = uint32_t(-1), uint32_t vz = uint32_t(-1)) { width = vx; height = vy; depth = vz; return *this; }
		constexpr TextureSlice& setMipLevel(uint32_t level) { mipLevel = level; return *this; }
		constexpr TextureSlice& setArraySlice(uint32_t slice) { arraySlice = slice; return *this; }
	};


	struct TextureSubresourceSet
	{
		static constexpr uint32_t AllMipLevels = uint32_t(-1);
		static constexpr uint32_t AllArraySlices = uint32_t(-1);

		uint32_t baseMipLevel = 0;
		uint32_t numMipLevels = 1;
		uint32_t baseArraySlice = 0;
		uint32_t numArraySlices = 1;

		TextureSubresourceSet() = default;

		TextureSubresourceSet(uint32_t _baseMipLevel, uint32_t _numMipLevels, uint32_t _baseArraySlice, uint32_t _numArraySlices)
			: baseMipLevel(_baseMipLevel)
			, numMipLevels(_numMipLevels)
			, baseArraySlice(_baseArraySlice)
			, numArraySlices(_numArraySlices)
		{
		}

		TextureSubresourceSet resolve(const TextureDesc& desc, bool singleMipLevel) const;
		bool isEntireTexture(const TextureDesc& desc) const;

		bool operator ==(const TextureSubresourceSet& other) const
		{
			return baseMipLevel == other.baseMipLevel &&
				numMipLevels == other.numMipLevels &&
				baseArraySlice == other.baseArraySlice &&
				numArraySlices == other.numArraySlices;
		}
		bool operator !=(const TextureSubresourceSet& other) const { return !(*this == other); }

		constexpr TextureSubresourceSet& setBaseMipLevel(uint32_t value) { baseMipLevel = value; return *this; }
		constexpr TextureSubresourceSet& setNumMipLevels(uint32_t value) { numMipLevels = value; return *this; }
		constexpr TextureSubresourceSet& setMipLevels(uint32_t base, uint32_t num) { baseMipLevel = base; numMipLevels = num; return *this; }
		constexpr TextureSubresourceSet& setBaseArraySlice(uint32_t value) { baseArraySlice = value; return *this; }
		constexpr TextureSubresourceSet& setNumArraySlices(uint32_t value) { numArraySlices = value; return *this; }
		constexpr TextureSubresourceSet& setArraySlices(uint32_t base, uint32_t num) { baseArraySlice = base; numArraySlices = num; return *this; }

		// see the bottom of this file for a specialization of std::hash<TextureSubresourceSet>
	};

	static const TextureSubresourceSet AllSubresources = TextureSubresourceSet(0, TextureSubresourceSet::AllMipLevels, 0, TextureSubresourceSet::AllArraySlices);

	


	class ITexture : public IResource
	{
	public:
		
		[[nodiscard]] virtual const TextureType& GetType() const = 0;
		[[nodiscard]] virtual const TextureDesc& getDesc() const = 0;
		virtual void UnBind() = 0;
		virtual void Bind() = 0;

		// Similar to getNativeObject, returns a native view for a specified set of subresources. Returns nullptr if unavailable.
		// TODO: on D3D12, the views might become invalid later if the view heap is grown/reallocated, we should do something about that.
		//virtual Object getNativeView(ObjectType objectType, Format format = Format::UNKNOWN, TextureSubresourceSet subresources = AllSubresources, TextureDimension dimension = TextureDimension::Unknown, bool isReadOnlyDSV = false) = 0;
	};
	typedef RefCountPtr<ITexture> TextureHandle;


	class IStagingTexture : public IResource
	{
	public:
		[[nodiscard]] virtual const TextureDesc& getDesc() const = 0;
	};
	typedef RefCountPtr<IStagingTexture> StagingTextureHandle;

}



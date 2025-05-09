#pragma once
#include "RefCountPtr.h"
#include "RHIDefinitions.h"
#include "RHIResources.h"
#include "RHISampler.h"
#include "BlackPearl\Log.h"
namespace BlackPearl {



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



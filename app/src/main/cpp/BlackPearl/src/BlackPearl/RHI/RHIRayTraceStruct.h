#pragma once
#include<string>
#include "RHIDefinitions.h"
#include "RefCountPtr.h"
#include "RHIResources.h"
#include "RHIBuffer.h"
namespace BlackPearl {


	namespace rt
	{


		//////////////////////////////////////////////////////////////////////////
	// rt::OpacityMicromap
	//////////////////////////////////////////////////////////////////////////
		
		enum class OpacityMicromapFormat
		{
			OC1_2_State = 1,
			OC1_4_State = 2,
		};

		enum class OpacityMicromapBuildFlags : uint8_t
		{
			None = 0,
			FastTrace = 1,
			FastBuild = 2,
		};

		NVRHI_ENUM_CLASS_FLAG_OPERATORS(OpacityMicromapBuildFlags)

			struct OpacityMicromapUsageCount
		{
			// Number of OMMs with the specified subdivision level and format.
			uint32_t count;
			// Micro triangle count is 4^N, where N is the subdivision level.
			uint32_t subdivisionLevel;
			// OMM input sub format.
			OpacityMicromapFormat format;
		};

		struct OpacityMicromapDesc
		{
			std::string debugName;
			bool trackLiveness = true;

			// OMM flags. Applies to all OMMs in array.
			OpacityMicromapBuildFlags flags;
			// OMM counts for each subdivision level and format combination in the inputs.
			std::vector<OpacityMicromapUsageCount> counts;

			// Base pointer for raw OMM input data.
			// Individual OMMs must be 1B aligned, though natural alignment is recommended.
			// It's also recommended to try to organize OMMs together that are expected to be used spatially close together.
			IBuffer* inputBuffer = nullptr;
			uint64_t inputBufferOffset = 0;

			// One NVAPI_D3D12_RAYTRACING_OPACITY_MICROMAP_DESC entry per OMM.
			IBuffer* perOmmDescs = nullptr;
			uint64_t perOmmDescsOffset = 0;

			OpacityMicromapDesc& setDebugName(const std::string& value) { debugName = value; return *this; }
			OpacityMicromapDesc& setTrackLiveness(bool value) { trackLiveness = value; return *this; }
			OpacityMicromapDesc& setFlags(OpacityMicromapBuildFlags value) { flags = value; return *this; }
			OpacityMicromapDesc& setCounts(const std::vector<OpacityMicromapUsageCount>& value) { counts = value; return *this; }
			OpacityMicromapDesc& setInputBuffer(IBuffer* value) { inputBuffer = value; return *this; }
			OpacityMicromapDesc& setInputBufferOffset(uint64_t value) { inputBufferOffset = value; return *this; }
			OpacityMicromapDesc& setPerOmmDescs(IBuffer* value) { perOmmDescs = value; return *this; }
			OpacityMicromapDesc& setPerOmmDescsOffset(uint64_t value) { perOmmDescsOffset = value; return *this; }
		};

		class IOpacityMicromap : public IResource
		{
		public:
			[[nodiscard]] virtual const OpacityMicromapDesc& getDesc() const = 0;
			[[nodiscard]] virtual bool isCompacted() const = 0;
			[[nodiscard]] virtual uint64_t getDeviceAddress() const = 0;
		};

		typedef RefCountPtr<IOpacityMicromap> OpacityMicromapHandle;


		typedef float AffineTransform[12];

		enum class InstanceFlags : unsigned
		{
			None = 0,
			TriangleCullDisable = 1,
			TriangleFrontCounterclockwise = 2,
			ForceOpaque = 4,
			ForceNonOpaque = 8,
			ForceOMM2State = 16,
			DisableOMMs = 32,
		};

		


		constexpr AffineTransform c_IdentityTransform = {
			//  +----+----+---------  rotation and scaling
			//  v    v    v
				1.f, 0.f, 0.f, 0.f,
				0.f, 1.f, 0.f, 0.f,
				0.f, 0.f, 1.f, 0.f
				//                 ^
				//                 +----  translation
		};

		enum class GeometryFlags : uint8_t
		{
			None = 0,
			Opaque = 1,
			NoDuplicateAnyHitInvocation = 2
		};

		NVRHI_ENUM_CLASS_FLAG_OPERATORS(GeometryFlags)

			enum class GeometryType : uint8_t
		{
			Triangles = 0,
			AABBs = 1
		};

		struct GeometryAABB
		{
			float minX;
			float minY;
			float minZ;
			float maxX;
			float maxY;
			float maxZ;
		};

		struct GeometryTriangles
		{
			IBuffer* indexBuffer = nullptr;   // make sure the first fields in both Triangles 
			IBuffer* vertexBuffer = nullptr;  // and AABBs are IBuffer* for easier debugging
			Format indexFormat = Format::UNKNOWN;
			Format vertexFormat = Format::UNKNOWN;
			uint64_t indexOffset = 0;
			uint64_t vertexOffset = 0;
			uint32_t indexCount = 0;
			uint32_t vertexCount = 0;
			uint32_t vertexStride = 0;

			IOpacityMicromap* opacityMicromap = nullptr;
			IBuffer* ommIndexBuffer = nullptr;
			uint64_t ommIndexBufferOffset = 0;
			Format ommIndexFormat = Format::UNKNOWN;
			const OpacityMicromapUsageCount* pOmmUsageCounts = nullptr;
			uint32_t numOmmUsageCounts = 0;

			GeometryTriangles& setIndexBuffer(IBuffer* value) { indexBuffer = value; return *this; }
			GeometryTriangles& setVertexBuffer(IBuffer* value) { vertexBuffer = value; return *this; }
			GeometryTriangles& setIndexFormat(Format value) { indexFormat = value; return *this; }
			GeometryTriangles& setVertexFormat(Format value) { vertexFormat = value; return *this; }
			GeometryTriangles& setIndexOffset(uint64_t value) { indexOffset = value; return *this; }
			GeometryTriangles& setVertexOffset(uint64_t value) { vertexOffset = value; return *this; }
			GeometryTriangles& setIndexCount(uint32_t value) { indexCount = value; return *this; }
			GeometryTriangles& setVertexCount(uint32_t value) { vertexCount = value; return *this; }
			// GeometryTriangles& setVertexStride(uint32_t value) { vertexStride = value; return *this; }
			 //GeometryTriangles& setOpacityMicromap(IOpacityMicromap* value) { opacityMicromap = value; return *this; }
			GeometryTriangles& setOmmIndexBuffer(IBuffer* value) { ommIndexBuffer = value; return *this; }
			GeometryTriangles& setOmmIndexBufferOffset(uint64_t value) { ommIndexBufferOffset = value; return *this; }
			GeometryTriangles& setOmmIndexFormat(Format value) { ommIndexFormat = value; return *this; }
			//GeometryTriangles& setPOmmUsageCounts(const OpacityMicromapUsageCount* value) { pOmmUsageCounts = value; return *this; }
			GeometryTriangles& setNumOmmUsageCounts(uint32_t value) { numOmmUsageCounts = value; return *this; }
		};

		struct GeometryAABBs
		{
			IBuffer* buffer = nullptr;
			IBuffer* unused = nullptr;
			uint64_t offset = 0;
			uint32_t count = 0;
			uint32_t stride = 0;

			GeometryAABBs& setBuffer(IBuffer* value) { buffer = value; return *this; }
			GeometryAABBs& setOffset(uint64_t value) { offset = value; return *this; }
			GeometryAABBs& setCount(uint32_t value) { count = value; return *this; }
			GeometryAABBs& setStride(uint32_t value) { stride = value; return *this; }
		};

		struct GeometryDesc
		{
			union GeomTypeUnion
			{
				GeometryTriangles triangles;
				GeometryAABBs aabbs;
			} geometryData;

			bool useTransform = false;
			AffineTransform transform{};
			GeometryFlags flags = GeometryFlags::None;
			GeometryType geometryType = GeometryType::Triangles;

			GeometryDesc() : geometryData{} { }

			GeometryDesc& setTransform(const AffineTransform& value) { memcpy(&transform, &value, sizeof(AffineTransform)); useTransform = true; return *this; }
			GeometryDesc& setFlags(GeometryFlags value) { flags = value; return *this; }
			GeometryDesc& setTriangles(const GeometryTriangles& value) { geometryData.triangles = value; geometryType = GeometryType::Triangles; return *this; }
			GeometryDesc& setAABBs(const GeometryAABBs& value) { geometryData.aabbs = value; geometryType = GeometryType::AABBs; return *this; }
		};

		enum class AccelStructBuildFlags : uint8_t
		{
			None = 0,
			AllowUpdate = 1,
			AllowCompaction = 2,
			PreferFastTrace = 4,
			PreferFastBuild = 8,
			MinimizeMemory = 0x10,
			PerformUpdate = 0x20,

			// Removes the errors or warnings that NVRHI validation layer issues when a TLAS
			// includes an instance that points at a NULL BLAS or has a zero instance mask.
			// Only affects the validation layer, doesn't translate to Vk/DX12 AS build flags.
			AllowEmptyInstances = 0x80
		};

		NVRHI_ENUM_CLASS_FLAG_OPERATORS(AccelStructBuildFlags)

			struct AccelStructDesc
		{
			size_t topLevelMaxInstances = 0; // only applies when isTopLevel = true
			std::vector<GeometryDesc> bottomLevelGeometries; // only applies when isTopLevel = false
			AccelStructBuildFlags buildFlags = AccelStructBuildFlags::None;
			std::string debugName;
			bool trackLiveness = true;
			bool isTopLevel = false;
			bool isVirtual = false;

			AccelStructDesc& setTopLevelMaxInstances(size_t value) { topLevelMaxInstances = value; isTopLevel = true; return *this; }
			AccelStructDesc& addBottomLevelGeometry(const GeometryDesc& value) { bottomLevelGeometries.push_back(value); isTopLevel = false; return *this; }
			AccelStructDesc& setBuildFlags(AccelStructBuildFlags value) { buildFlags = value; return *this; }
			AccelStructDesc& setDebugName(const std::string& value) { debugName = value; return *this; }
			AccelStructDesc& setTrackLiveness(bool value) { trackLiveness = value; return *this; }
			AccelStructDesc& setIsTopLevel(bool value) { isTopLevel = value; return *this; }
			AccelStructDesc& setIsVirtual(bool value) { isVirtual = value; return *this; }
		};

		//////////////////////////////////////////////////////////////////////////
		// rt::AccelStruct
		//////////////////////////////////////////////////////////////////////////

		class IAccelStruct : public IResource
		{
		public:
			[[nodiscard]] virtual const AccelStructDesc& getDesc() const = 0;
			[[nodiscard]] virtual bool isCompacted() const = 0;
			[[nodiscard]] virtual uint64_t getDeviceAddress() const = 0;
		};

		typedef RefCountPtr<IAccelStruct> AccelStructHandle;


		NVRHI_ENUM_CLASS_FLAG_OPERATORS(InstanceFlags)
			
		struct InstanceDesc
		{
			AffineTransform transform;
			unsigned instanceID : 24;
			unsigned instanceMask : 8;
			unsigned instanceContributionToHitGroupIndex : 24;
			InstanceFlags flags : 8;
			union {
				IAccelStruct* bottomLevelAS; // for buildTopLevelAccelStruct
				uint64_t blasDeviceAddress;  // for buildTopLevelAccelStructFromBuffer - use IAccelStruct::getDeviceAddress()
			};

			InstanceDesc()  // NOLINT(cppcoreguidelines-pro-type-member-init)
				: instanceID(0)
				, instanceMask(0)
				, instanceContributionToHitGroupIndex(0)
				, flags(InstanceFlags::None)
				, bottomLevelAS(nullptr)
			{
				setTransform(c_IdentityTransform);
			}

			InstanceDesc& setInstanceID(uint32_t value) { instanceID = value; return *this; }
			InstanceDesc& setInstanceContributionToHitGroupIndex(uint32_t value) { instanceContributionToHitGroupIndex = value; return *this; }
			InstanceDesc& setInstanceMask(uint32_t value) { instanceMask = value; return *this; }
			InstanceDesc& setTransform(const AffineTransform& value) { memcpy(&transform, &value, sizeof(AffineTransform)); return *this; }
			InstanceDesc& setFlags(InstanceFlags value) { flags = value; return *this; }
			InstanceDesc& setBLAS(IAccelStruct* value) { bottomLevelAS = value; return *this; }
		};

		static_assert(sizeof(InstanceDesc) == 64, "sizeof(InstanceDesc) is supposed to be 64 bytes");







	}
}
#include "pch.h"
#include "RHI/RHIRayTraceStruct.h"
#include "RHI/RHIBuffer.h"
namespace BlackPearl {
	namespace rt {
		GeometryTriangles& GeometryTriangles::setIndexBuffer(IBuffer* value) { indexBuffer = value; return *this; }
		GeometryTriangles& GeometryTriangles::setVertexBuffer(IBuffer* value) { vertexBuffer = value; return *this; }
		GeometryTriangles& GeometryTriangles::setIndexFormat(Format value) { indexFormat = value; return *this; }
		GeometryTriangles& GeometryTriangles::setVertexFormat(Format value) { vertexFormat = value; return *this; }
		GeometryTriangles& GeometryTriangles::setIndexOffset(uint64_t value) { indexOffset = value; return *this; }
		GeometryTriangles& GeometryTriangles::setVertexOffset(uint64_t value) { vertexOffset = value; return *this; }
		GeometryTriangles& GeometryTriangles::setIndexCount(uint32_t value) { indexCount = value; return *this; }
		GeometryTriangles& GeometryTriangles::setVertexCount(uint32_t value) { vertexCount = value; return *this; }
		// GeometryTriangles& setVertexStride(uint32_t value) { vertexStride = value; return *this; }
		 //GeometryTriangles& setOpacityMicromap(IOpacityMicromap* value) { opacityMicromap = value; return *this; }
		GeometryTriangles& GeometryTriangles::setOmmIndexBuffer(IBuffer* value) { ommIndexBuffer = value; return *this; }
		GeometryTriangles& GeometryTriangles::setOmmIndexBufferOffset(uint64_t value) { ommIndexBufferOffset = value; return *this; }
		GeometryTriangles& GeometryTriangles::setOmmIndexFormat(Format value) { ommIndexFormat = value; return *this; }
		//GeometryTriangles& setPOmmUsageCounts(const OpacityMicromapUsageCount* value) { pOmmUsageCounts = value; return *this; }
		GeometryTriangles& GeometryTriangles::setNumOmmUsageCounts(uint32_t value) { numOmmUsageCounts = value; return *this; }
	}
	
}
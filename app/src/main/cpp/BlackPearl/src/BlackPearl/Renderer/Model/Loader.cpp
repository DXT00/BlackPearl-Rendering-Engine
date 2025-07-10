#include "Renderer/Model/Loader.h"
namespace BlackPearl {
 
    static void AppendBufferRange(BufferRange& range, size_t size, uint64_t& currentBufferSize)
    {
        range.byteOffset = currentBufferSize;
        range.byteSize = size;
        currentBufferSize += range.byteSize;
    }


    void Loader::CreateMeshBuffers(std::shared_ptr<Mesh>& mesh)
    {
        auto buffers = mesh->buffers;

        BufferDesc bufferDescIndex;
        bufferDescIndex.isIndexBuffer = true;
        bufferDescIndex.byteSize = buffers->indexData.size() * sizeof(uint32_t);
        bufferDescIndex.debugName = "IndexBuffer";
        bufferDescIndex.canHaveTypedViews = true;
        bufferDescIndex.canHaveRawViews = true;
        bufferDescIndex.format = Format::R32_UINT;
        bufferDescIndex.isAccelStructBuildInput = false;

        BufferDesc bufferDesc;
        bufferDesc.isVertexBuffer = true;
        bufferDesc.byteSize = 0;
        bufferDesc.debugName = "VertexBuffer";
        bufferDesc.canHaveTypedViews = true;
        bufferDesc.canHaveRawViews = true;
        bufferDesc.isAccelStructBuildInput = false;

        if (!buffers->positionData.empty())
        {
            AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::Position),
                buffers->positionData.size() * sizeof(buffers->positionData[0]), bufferDesc.byteSize);
                bufferDesc.vertexCnt = buffers->positionData.size();

        }

        if (!buffers->normalData.empty())
        {
            AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::Normal),
                buffers->normalData.size() * sizeof(buffers->normalData[0]), bufferDesc.byteSize);
            bufferDesc.vertexCnt = buffers->normalData.size();

        }

        if (!buffers->tangentData.empty())
        {
            AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::Tangent),
                buffers->tangentData.size() * sizeof(buffers->tangentData[0]), bufferDesc.byteSize);
            bufferDesc.vertexCnt = buffers->tangentData.size();

        }

        if (!buffers->texcoordData.empty())
        {
            AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::TexCoord),
                buffers->texcoordData.size() * sizeof(buffers->texcoordData[0]), bufferDesc.byteSize);
            bufferDesc.vertexCnt = buffers->texcoordData.size();

        }

        if (!buffers->texcoord1Data.empty())
        {
            AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::TexCoord1),
                buffers->texcoord1Data.size() * sizeof(buffers->texcoord1Data[0]), bufferDesc.byteSize);
            bufferDesc.vertexCnt = buffers->texcoord1Data.size();

        }

        if (!buffers->jointIdData.empty())
        {
            AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::JointIndices),
                buffers->jointIdData.size() * sizeof(buffers->jointIdData[0]), bufferDesc.byteSize);
        }

        if (!buffers->jointId1Data.empty())
        {
            AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::JointIndices),
                buffers->jointId1Data.size() * sizeof(buffers->jointId1Data[0]), bufferDesc.byteSize);
        }

        if (!buffers->jointId2Data.empty())
        {
            AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::JointIndices),
                buffers->jointId2Data.size() * sizeof(buffers->jointId2Data[0]), bufferDesc.byteSize);
        }

        if (!buffers->jointWeightData.empty())
        {
            AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::JointWeights),
                buffers->jointWeightData.size() * sizeof(buffers->jointWeightData[0]), bufferDesc.byteSize);
        }

        if (!buffers->jointWeight1Data.empty())
        {
            AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::JointWeights),
                buffers->jointWeight1Data.size() * sizeof(buffers->jointWeight1Data[0]), bufferDesc.byteSize);
        }

        if (!buffers->jointWeight2Data.empty())
        {
            AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::JointWeights),
                buffers->jointWeight2Data.size() * sizeof(buffers->jointWeight2Data[0]), bufferDesc.byteSize);
        }
        buffers->vertexBufferDesc = bufferDesc;

        buffers->indexBufferDesc = bufferDescIndex;
    }
}

#pragma once
#include <vector>
#include <string>
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Renderer/Buffer/Buffer.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Component/BoundingSphereComponent/BoundingSphere.h"
#include "BlackPearl/Renderer/Material/MaterialColor.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include "BlackPearl/Renderer/VertexArray.h"
#include "BlackPearl/Common/CommonFunc.h"
#include "BlackPearl/Renderer/Mesh/MeshletConfig.h"
#include "BlackPearl/Renderer/Mesh/Meshlet.h"
#include "BlackPearl/Renderer/SceneType.h"
#include <initializer_list>
#include <memory>

namespace BlackPearl {

	class Mesh
	{
	public:
		Mesh() {
			buffers = std::make_shared<BufferGroup>();
		
		};
		/*one vertexBuffer*/
		Mesh(
			float* vertices,
			uint32_t verticesSize,
			unsigned int* indices,
			uint32_t indicesSize,
			std::shared_ptr<Material> material,
			const VertexBufferLayout& layout,
			bool tessellation = false ,/* whether need to do tessellation */
			uint32_t verticesPerTessPatch = 4
		);

		/*one vertexBuffer*/
		Mesh(
			std::vector<float> vertices,
			std::vector<uint32_t> indices,
			std::shared_ptr<Material> material,
			const VertexBufferLayout& layout,
			bool tessellation = false,
			uint32_t verticesPerTessPatch = 4
		);

		/*multiple vertexBuffers*/
		Mesh(
			std::shared_ptr<Material> material,
			std::shared_ptr<IndexBuffer> indexBuffer,
			std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers,
			bool tessellation = false,
			uint32_t verticesPerTessPatch = 4
		);
		
		~Mesh();
		float* GetVertices() const { return m_Vertices; }

		std::shared_ptr<VertexArray> GetVertexArray() const { return m_VertexArray; }
		uint32_t					 GetIndicesSize() const { return m_IndicesSize; }
		uint32_t				     GetVerticesSize(unsigned int vertexBufferId);
		std::shared_ptr<Material>    GetMaterial() const { return material; }
		VertexBufferLayout			 GetVertexBufferLayout() const { return m_VertexBufferLayout; }
		uint32_t					 GetVertexCount() const { return m_VerticeCount; }
		uint32_t					 GetIndicesCount() const { return m_IndicesCount; }

		void SetShader(const std::string& path)				     { material->SetShader(path); }
		void SetShader(const std::shared_ptr<Shader> &shader)    { material->SetShader(shader); }
		void SetTexture(const std::shared_ptr<Texture>& texture) { material->SetTexture(texture); }
		void SetMaterialColor(MaterialColor::Color color)        { material->SetMaterialColor(color); }
		void SetTessellation(uint32_t verticesPerTessPatch);
		void SetVertexBufferLayout(const VertexBufferLayout& layout);

		std::pair<float*, uint32_t> GetPositionBuffer() const { return { m_Positions, m_PositionsSize }; }
		std::pair<float*, uint32_t> GetNormalBuffer() const   { return { m_Normals, m_NormalsSize }; }
		std::pair<float*, uint32_t> GetTexCoordsBuffer() const { return { m_TexCoords, m_TexCoordsSize }; }
		std::pair<float*, uint32_t> GetTangentBuffer() const { return { m_Tangents, m_TangentsSize }; }
		std::pair<float*, uint32_t> GetBitangentBuffer() const { return { m_Bitangents, m_BitangentsSize }; }
		std::pair<uint32_t*, uint32_t> GetJiontBuffer() const { return { m_JointIndices, m_JointIndicesSize }; }
		std::pair<uint32_t*, uint32_t> GetJiont1Buffer() const { return { m_JointIndices, m_JointIndicesSize }; }
		std::pair<float*, uint32_t> GetWeightBuffer() const { return { m_Weight, m_WeightSize }; }
		std::pair<float*, uint32_t> GetWeight1Buffer() const { return { m_Weight1, m_Weight1Size }; }

		std::pair<uint32_t*, uint32_t> GetIndicesBuffer() const { return { m_Indices, m_IndicesSize }; }
		uint32_t GetIndicesConut() const { return m_IndicesCount; }

		math::float3 GetMinPLocal() const { return m_MinLocalP; }
		math::float3 GetMaxPLocal() const { return m_MaxLocalP; }

	public:
		std::string name;
		std::shared_ptr<BufferGroup> buffers;
		//std::shared_ptr<MeshInfo> skinPrototype;
		//std::vector<std::shared_ptr<MeshGeometry>> geometries;
		math::box3 objectSpaceBounds;
		uint32_t indexOffset = 0;
		uint32_t vertexOffset = 0;
		uint32_t totalIndices = 0;
		uint32_t totalVertices = 0;
		int globalMeshIndex = 0;

		std::shared_ptr<Material>    material = nullptr;


	public:
		//temp for directX mseh shader TODO::¼æÈÝopengl ºÍd3d12 mesh½Ó¿Ú
		std::vector<uint32_t>		VertexStrides;
		std::vector<Span<uint8_t>>  Vertices_ml;
		uint32_t					VertexCount_ml;
		Span<Subset>				MeshletSubsets;
		Span<Meshlet>				Meshlets;
		Span<uint8_t>				UniqueVertexIndices;
		Span<PackedTriangle>		PrimitiveIndices;
		Span<CullData>				CullingData;
		BoundingSphere				BoundingSphere;
		Span<uint8_t>				Indices_ml;
		Span<Subset>				IndexSubsets;
		uint32_t					IndexSize_ml;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> VertexResources;
		Microsoft::WRL::ComPtr<ID3D12Resource>              IndexResource;
		Microsoft::WRL::ComPtr<ID3D12Resource>              MeshletResource;
		Microsoft::WRL::ComPtr<ID3D12Resource>              UniqueVertexIndexResource;
		Microsoft::WRL::ComPtr<ID3D12Resource>              PrimitiveIndexResource;
		Microsoft::WRL::ComPtr<ID3D12Resource>              CullDataResource;
		Microsoft::WRL::ComPtr<ID3D12Resource>              MeshInfoResource;


	private:
		void Init(uint32_t verticesSize);
		void ParseAttributes(const VertexBufferLayout& layout);
		std::shared_ptr<VertexArray> m_VertexArray;
		VertexBufferLayout           m_VertexBufferLayout;

		
		//rt::AccelStructHandle accelStruct; // for use by applications
		//rt::AccelStructHandle accelStructOMM; // for use by application
		//std::vector<rt::OpacityMicromapHandle> opacityMicroMaps; // for use by application

		std::unique_ptr<MeshDebugData> debugData;
		bool debugDataDirty = true; // set this to true to make Scene update the debug data


		float*                       m_Vertices = nullptr;
		uint32_t*                    m_Indices = nullptr;
		uint32_t                     m_IndicesSize = 0; //m_IndicesSize = m_indicesCount* sizeof(uint32_t)
		uint32_t					 m_IndicesCount = 0;
		uint32_t					 m_VerticeSize = 0; //m_VerticeSize = m_VerticeArrayCount* sizeof(float)
		uint32_t					 m_VerticeCount = 0; // one vertex has multiple attributes, a vertex = (pos.xyz, normal.xyz, tex.xy..), m_VerticeCount is the number of attribute vertex
		uint32_t					 m_VerticeArrayCount = 0;

		bool m_NeedTessellation;
		//for batch rendering and indirect drawcall
		float* m_Positions		= nullptr;
		float* m_Normals		= nullptr;
		float* m_TexCoords		= nullptr;
		float* m_Tangents		= nullptr;
		float* m_Bitangents		= nullptr;
		uint32_t* m_JointIndices	= nullptr;
		uint32_t* m_JointIndices1	= nullptr;
		float* m_Weight			= nullptr;
		float* m_Weight1		= nullptr;
		
		uint32_t m_PositionsSize = 0;
		uint32_t m_NormalsSize = 0;
		uint32_t m_TexCoordsSize = 0;
		uint32_t m_TangentsSize = 0;
		uint32_t m_BitangentsSize = 0;
		uint32_t m_JointIndicesSize = 0;
		uint32_t m_JointIndices1Size = 0;
		uint32_t m_WeightSize = 0;
		uint32_t m_Weight1Size = 0;

		math::float3 m_MinLocalP = math::float3(FLT_MAX);
		math::float3 m_MaxLocalP = math::float3(-FLT_MAX);;



		MeshInfo m_MeshInfo;
	};

}
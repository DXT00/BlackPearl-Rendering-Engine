#include "pch.h"
#ifdef GE_API_D3D12
#include "MetaBallSDFScene.h"
#include "BlackPearl/RHI/D3D12RHI/D3D12LocalRootSignature.h"
#include "BlackPearl/RHI/D3D12RHI/D3D12GlobalRootSignature.h"
#include "BlackPearl/Renderer/Buffer/D3D12Buffer/D3D12Buffer.h"
namespace BlackPearl {


	MetaBallSDFScene::MetaBallSDFScene()
		: SDFScene(DemoType::MetaBallSDFScene)
	{
		m_AABBWidth = 2;
		m_AABBDistance = 2;

		CreateShaderTable();
		CreateGeometry();
		CreateMaterials();
		CreateRootSignatureDesc();
	}

	MetaBallSDFScene::~MetaBallSDFScene()
	{
	}

	void MetaBallSDFScene::CreateShaderTable()
	{
		m_RaygenShaderName = L"MyRaygenShader";


		m_IntersectionShaderNames[IntersectionShaderType::AnalyticPrimitive] = L"MyIntersectionShader_AnalyticPrimitive";
		m_IntersectionShaderNames[IntersectionShaderType::VolumetricPrimitive] = L"MyIntersectionShader_VolumetricPrimitive";
		m_IntersectionShaderNames[IntersectionShaderType::SignedDistancePrimitive] = L"MyIntersectionShader_SignedDistancePrimitive";


		m_ClosestHitShaderNames[GeometryType::Triangle] = L"MyClosestHitShader_Triangle";
		m_ClosestHitShaderNames[GeometryType::AABB] = L"MyClosestHitShader_AABB";

		m_MissShaderNames[RayType::Radiance] = L"MyMissShader";
		m_MissShaderNames[RayType::Shadow] = L"MyMissShader_ShadowRay";

		m_AABBHitGroupNames[IntersectionShaderType::AnalyticPrimitive][RayType::Radiance] = L"MyHitGroup_AABB_AnalyticPrimitive";
		m_AABBHitGroupNames[IntersectionShaderType::AnalyticPrimitive][RayType::Shadow] = L"MyHitGroup_AABB_AnalyticPrimitive_ShadowRay";
		m_AABBHitGroupNames[IntersectionShaderType::VolumetricPrimitive][RayType::Radiance] = L"MyHitGroup_AABB_VolumetricPrimitive";
		m_AABBHitGroupNames[IntersectionShaderType::VolumetricPrimitive][RayType::Shadow] = L"MyHitGroup_AABB_VolumetricPrimitive_ShadowRay";
		m_AABBHitGroupNames[IntersectionShaderType::SignedDistancePrimitive][RayType::Radiance] = L"MyHitGroup_AABB_SignedDistancePrimitive";
		m_AABBHitGroupNames[IntersectionShaderType::SignedDistancePrimitive][RayType::Shadow] = L"MyHitGroup_AABB_SignedDistancePrimitive_ShadowRay";

		m_TriangleHitGroupNames[RayType::Radiance] = L"MyHitGroup_Triangle";
		m_TriangleHitGroupNames[RayType::Shadow] = L"MyHitGroup_Triangle_ShadowRay";
	}

	void MetaBallSDFScene::CreateMaterials()
	{
		// Root constants
		m_AabbMaterialCB.resize(GetTotalPrimitiveCount());
		auto SetAttributes = [&](
			UINT primitiveIndex,
			const XMFLOAT4& albedo,
			float reflectanceCoef = 0.0f,
			float diffuseCoef = 0.9f,
			float specularCoef = 0.7f,
			float specularPower = 50.0f,
			float stepScale = 1.0f)
		{
			auto& attributes = m_AabbMaterialCB[primitiveIndex];
			attributes.albedo = albedo;
			attributes.reflectanceCoef = reflectanceCoef;
			attributes.diffuseCoef = diffuseCoef;
			attributes.specularCoef = specularCoef;
			attributes.specularPower = specularPower;
			attributes.stepScale = stepScale;
		};
		m_PlaneMaterialCB = { XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f), 0.25f, 1, 0.4f, 50, 1 };

		// Albedos
		XMFLOAT4 green = XMFLOAT4(0.1f, 1.0f, 0.5f, 1.0f);
		XMFLOAT4 red = XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);
		XMFLOAT4 yellow = XMFLOAT4(1.0f, 1.0f, 0.5f, 1.0f);

		UINT offset = 0;
		// Analytic primitives.
		{
			SetAttributes(offset + AABB, red);
			SetAttributes(offset + Spheres, ChromiumReflectance, 1);
			offset += AnalyticPrimitive::Count_AnalyticPrimitive;
		}

		// Volumetric primitives.
		{
			SetAttributes(offset + Metaballs, ChromiumReflectance, 1);
			offset += VolumetricPrimitive::Count_VolumetricPrimitive;
		}

		// Signed distance primitives.
		{
			SetAttributes(offset + MiniSpheres, green);
			SetAttributes(offset + IntersectedRoundCube, green);
			SetAttributes(offset + SquareTorus, ChromiumReflectance, 1);
			SetAttributes(offset + TwistedTorus, yellow, 0, 1.0f, 0.7f, 50, 0.5f);
			SetAttributes(offset + Cog, yellow, 0, 1.0f, 0.1f, 2);
			SetAttributes(offset + Cylinder, red);
			SetAttributes(offset + FractalPyramid, green, 0, 1, 0.1f, 4, 0.8f);
		}


	}

	void MetaBallSDFScene::InitAccelerationStructure()
	{
	}

	void MetaBallSDFScene::CreateGeometry()
	{
		// Set up AABBs on a grid.
		/*

					   -14
					   |
			   base    |
					   |
					   |
					   |
					   |
		------------ -------------------
		-14            |
					   |
					   |
					   |
					   |
					   |


		*/
		{
			XMINT3 aabbGrid = XMINT3(4, 1, 4);
			const XMFLOAT3 basePosition =
			{
				-(aabbGrid.x * m_AABBWidth + (aabbGrid.x - 1) * m_AABBDistance) / 2.0f,
				-(aabbGrid.y * m_AABBWidth + (aabbGrid.y - 1) * m_AABBDistance) / 2.0f,
				-(aabbGrid.z * m_AABBWidth + (aabbGrid.z - 1) * m_AABBDistance) / 2.0f,
			};

			XMFLOAT3 stride = XMFLOAT3(m_AABBWidth + m_AABBDistance, m_AABBWidth + m_AABBDistance, m_AABBWidth + m_AABBDistance);
			auto InitializeAABB = [&](auto& offsetIndex, auto& size)
			{
				return D3D12_RAYTRACING_AABB{
					basePosition.x + offsetIndex.x * stride.x,
					basePosition.y + offsetIndex.y * stride.y,
					basePosition.z + offsetIndex.z * stride.z,
					basePosition.x + offsetIndex.x * stride.x + size.x,
					basePosition.y + offsetIndex.y * stride.y + size.y,
					basePosition.z + offsetIndex.z * stride.z + size.z,
				};
			};
			m_AABBs.resize(GetTotalPrimitiveCount());
			UINT offset = 0;

			// Analytic primitives.
			{
				m_AABBs[offset + AnalyticPrimitive::AABB] = InitializeAABB(XMINT3(3, 0, 0), XMFLOAT3(2, 3, 2));
				m_AABBs[offset + AnalyticPrimitive::Spheres] = InitializeAABB(XMFLOAT3(2.25f, 0, 0.75f), XMFLOAT3(3, 3, 3));
				offset += AnalyticPrimitive::Count_AnalyticPrimitive;
			}

			// Volumetric primitives.
			{
				m_AABBs[offset + VolumetricPrimitive::Metaballs] = InitializeAABB(XMINT3(0, 0, 0), XMFLOAT3(3, 3, 3));
				offset += VolumetricPrimitive::Count_VolumetricPrimitive;
			}

			// Signed distance primitives.
			{
				m_AABBs[offset + SignedDistancePrimitive::MiniSpheres] = InitializeAABB(XMINT3(2, 0, 0), XMFLOAT3(2, 2, 2));
				m_AABBs[offset + SignedDistancePrimitive::IntersectedRoundCube] = InitializeAABB(XMINT3(0, 0, 2), XMFLOAT3(2, 2, 2));
				m_AABBs[offset + SignedDistancePrimitive::SquareTorus] = InitializeAABB(XMFLOAT3(0.75f, -0.1f, 2.25f), XMFLOAT3(3, 3, 3));
				m_AABBs[offset + SignedDistancePrimitive::TwistedTorus] = InitializeAABB(XMINT3(0, 0, 1), XMFLOAT3(2, 2, 2));
				m_AABBs[offset + SignedDistancePrimitive::Cog] = InitializeAABB(XMINT3(1, 0, 0), XMFLOAT3(2, 2, 2));
				m_AABBs[offset + SignedDistancePrimitive::Cylinder] = InitializeAABB(XMINT3(0, 0, 3), XMFLOAT3(2, 3, 2));
				m_AABBs[offset + SignedDistancePrimitive::FractalPyramid] = InitializeAABB(XMINT3(2, 0, 2), XMFLOAT3(6, 6, 6));
			}
		}
	}

	void MetaBallSDFScene::CreateRootSignatureDesc()
	{
		//--------------- set rootsignature ---------------

		//Global Root Signature
		//CD3DX12_DESCRIPTOR_RANGE ranges[2]; // Perfomance TIP: Order from most frequent to least frequent.
		//m_Ranges[0]-- RWTexture2D<float4> g_renderTarget : register(u0);
		m_Ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1 /*numDescriptors*/, 0 /*baseShaderRegister*/);  // 1 output texture
		//m_Ranges[1] --
		//ByteAddressBuffer g_indices : register(t1, space0);
		//StructuredBuffer<Vertex> g_vertices : register(t2, space0);
		m_Ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2 /*numDescriptors*/, 1 /*baseShaderRegister*/);  // 2 static index and vertex buffers.

		//RWTexture2D<float4> g_renderTarget : register(u0);
		m_GlobalRootParameters[GlobalRootSignature::OutputView].InitAsDescriptorTable(1, &m_Ranges[0]);
		//RaytracingAccelerationStructure g_scene : register(t0, space0);
		m_GlobalRootParameters[GlobalRootSignature::AccelerationStructure].InitAsShaderResourceView(0);
		//ConstantBuffer<SceneConstantBuffer> g_sceneCB : register(b0);
		m_GlobalRootParameters[GlobalRootSignature::SceneConstant].InitAsConstantBufferView(0);
		//StructuredBuffer<PrimitiveInstancePerFrameBuffer> g_AABBPrimitiveAttributes : register(t3, space0);
		m_GlobalRootParameters[GlobalRootSignature::AABBattributeBuffer].InitAsShaderResourceView(3);
		//g_indices,g_vertices
		m_GlobalRootParameters[GlobalRootSignature::VertexBuffers].InitAsDescriptorTable(1, &m_Ranges[1]);
		m_GlobalRootSignatureDesc = CD3DX12_ROOT_SIGNATURE_DESC(ARRAYSIZE(m_GlobalRootParameters), m_GlobalRootParameters);

		// Local Root Signature

		m_LocalRootSignatureDesc.resize(2);
		//Triangle geometry
		{
			m_LocalRootSignaturesTriangle = {
				{ SignatureType::Local, GeometryType::Triangle, SlotType::MaterialConstant, 0 ,SizeOfInUint32(PrimitiveConstantBuffer), 1}
			};
			m_LocalRootSignaturesTriangle.SetRootSignatureDesc(m_LocalRootParametersTriangle, ARRAYSIZE(m_LocalRootParametersTriangle), D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);
			m_LocalRootSignatureDesc[0] = m_LocalRootSignaturesTriangle.GetDesc();
			
		}

		{
			//AABB geometry

			m_LocalRootSignaturesAABB = {
				{ SignatureType::Local, GeometryType::AABB, SlotType::MaterialConstant, 0 ,SizeOfInUint32(PrimitiveConstantBuffer), 1},
				{ SignatureType::Local, GeometryType::AABB, SlotType::GeometryIndex, 1 ,SizeOfInUint32(PrimitiveInstanceConstantBuffer), 2}

			};
			m_LocalRootSignaturesAABB.SetRootSignatureDesc(m_LocalRootParametersAABB, ARRAYSIZE(m_LocalRootParametersAABB), D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);
			m_LocalRootSignatureDesc[1] =  m_LocalRootSignaturesAABB.GetDesc();
		}




		////Triangle geometry
		//{
		//	m_LocalRootParametersTriangle[0].InitAsConstants(SizeOfInUint32(PrimitiveConstantBuffer), 1);

		//	CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(m_LocalRootParametersTriangle), m_LocalRootParametersTriangle);
		//	localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

		//	m_LocalRootSignatureDesc[0] = localRootSignatureDesc;

		//}

		//{
		//	//AABB geometry
		//	m_LocalRootParametersAABB[0].InitAsConstants(SizeOfInUint32(PrimitiveConstantBuffer), 1);
		//	m_LocalRootParametersAABB[1].InitAsConstants(SizeOfInUint32(PrimitiveInstanceConstantBuffer), 2);

		//	CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(m_LocalRootParametersAABB), m_LocalRootParametersAABB);
		//	localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
		//	m_LocalRootSignatureDesc[1] = localRootSignatureDesc;
		//}


	}

	//void MetaBallSDFScene::CreatePlane()
	//{
	//	// Plane indices.
	//	Index indices[] =
	//	{
	//		3,1,0,
	//		2,1,3,

	//	};

	//	// Cube vertices positions and corresponding triangle normals.
	//	Vertex vertices[] =
	//	{
	//		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
	//		{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
	//		{ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
	//		{ XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
	//	};
	//}
}
#endif

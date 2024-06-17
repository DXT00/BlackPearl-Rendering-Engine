#pragma once
#ifdef GE_API_D3D12
#include "Scene.h"
#include "BlackPearl/Renderer/Buffer/D3D12Buffer/D3D12Buffer.h"
#include "BlackPearl/RHI/D3D12RHI/DirectXRaytracingHelper.h"
#include "BlackPearl/Math/Math.h"
#include <DirectXMath.h>
using namespace DirectX;

namespace BlackPearl {
	
	// From: http://blog.selfshadow.com/publications/s2015-shading-course/hoffman/s2015_pbs_physics_math_slides.pdf
	static const XMFLOAT4 ChromiumReflectance = XMFLOAT4(0.549f, 0.556f, 0.554f, 1.0f);


	class SDFScene : public Scene
	{
	public:
		enum GeometryType {
			Triangle,
			AABB,
			Count
		};

		enum IntersectionShaderType {
			AnalyticPrimitive = 0,
			VolumetricPrimitive,
			SignedDistancePrimitive,
			Count_IntersectionShaderType
		};

		//ÿ��Triangle������
		struct TriangleRootArguments
		{
			PrimitiveConstantBuffer materialCb;
		};

		//ÿ��AABB������
		struct AABBRootArguments
		{
			PrimitiveConstantBuffer materialCb;
			PrimitiveInstanceConstantBuffer aabbCB;
		};

		TriangleRootArguments GetTriangleRootArgument() { return m_TriangleRootArgument; };
		AABBRootArguments GetAABBRootArgument() {
			return m_AABBRootArgument;
		}
		UINT GetMaxRootArgumentsSize() {
			return std::max(sizeof(TriangleRootArguments), sizeof(AABBRootArguments));
		}
		SDFScene(Scene::DemoType type);
		~SDFScene();

		virtual UINT GetTotalPrimitiveCount() const = 0;
		virtual UINT PerPrimitiveTypeCount(IntersectionShaderType type) = 0;

		//------------------------------------Scene's Shader------------------------------------------------------//
		wchar_t* GetRaygenShaderName() const {
			return m_RaygenShaderName;
		}
		std::unordered_map<IntersectionShaderType, wchar_t*> GetIntersectionShaderNames() const {
			return m_IntersectionShaderNames;
		}
		std::unordered_map<GeometryType, wchar_t*> GetClosestHitShaderNames() const {
			return m_ClosestHitShaderNames;
		}
		std::unordered_map<RayType, wchar_t*> GetMissShaderNames() const {
			return m_MissShaderNames;
		}
		std::unordered_map<IntersectionShaderType, std::unordered_map<RayType, wchar_t*>> GetAABBHitGroupNames() const {
			return m_AABBHitGroupNames;
		}
		std::unordered_map<RayType, wchar_t*> GetTriangleHitGroupNames() const {
			return m_TriangleHitGroupNames;
		}


		//------------------------------------Scene's Gemoetry------------------------------------------------------//

		std::vector<D3D12_RAYTRACING_AABB> GetAABBsGeometry() const {
			return m_AABBs;
		}
		//------------------------------------Scene's RootSignatruesDesc------------------------------------------------------//

		std::vector<CD3DX12_ROOT_SIGNATURE_DESC> GetLocalRootSignatruesDesc() const {
			return m_LocalRootSignatureDesc;
		};
		CD3DX12_ROOT_SIGNATURE_DESC GetGlobalRootSignatureDesc() const {
			return m_GlobalRootSignatureDesc;
		}

	protected:
		virtual void CreateShaderTable() = 0;
		virtual void CreateGeometry() = 0;
		virtual void CreateRootSignatureDesc() = 0;
		virtual void CreateMaterials() = 0;
		virtual void InitAccelerationStructure() = 0;


		void LoadScene(const std::string& sceneName);
		void LoadScene(int sceneId);
	
	protected:

		// Raytracing scene
		//ConstantBuffer<SceneConstantBuffer> m_SceneCB;
		CD3DX12_ROOT_SIGNATURE_DESC m_GlobalRootSignatureDesc;

		std::vector<CD3DX12_ROOT_SIGNATURE_DESC> m_LocalRootSignatureDesc;

		// shader name
		wchar_t* m_RaygenShaderName;
		std::unordered_map<IntersectionShaderType, wchar_t*> m_IntersectionShaderNames;
		std::unordered_map<GeometryType, wchar_t*> m_ClosestHitShaderNames;
		std::unordered_map<RayType, wchar_t*> m_MissShaderNames;
		std::unordered_map<RayType, wchar_t*> m_TriangleHitGroupNames;
		std::unordered_map<IntersectionShaderType, std::unordered_map<RayType, wchar_t*>> m_AABBHitGroupNames;

		//geometry 
		std::vector<D3D12_RAYTRACING_AABB> m_AABBs;

		// root arguments
		TriangleRootArguments m_TriangleRootArgument;
		AABBRootArguments m_AABBRootArgument;

	};

}
#endif
#pragma once
#ifdef GE_API_D3D12
#include "BlackPearl/Scene/SDFScene.h"
#include "BlackPearl/Renderer/Buffer/D3D12Buffer/D3D12Buffer.h"
#include "BlackPearl/RHI/D3D12RHI/D3D12LocalRootSignature.h"

namespace BlackPearl {

	class MetaBallSDFScene: public SDFScene
	{
	public:
		typedef UINT16 Index;
		struct Vertex
		{
			XMFLOAT3 position;
			XMFLOAT3 normal;
		};


		//Object
		enum AnalyticPrimitive {
			AABB = 0,
			Spheres,
			Count_AnalyticPrimitive,
		};

		enum VolumetricPrimitive {
			Metaballs = 0,
			Count_VolumetricPrimitive
		};

		enum SignedDistancePrimitive {
			MiniSpheres = 0,
			IntersectedRoundCube,
			SquareTorus,
			TwistedTorus,
			Cog,
			Cylinder,
			FractalPyramid,
			Count_SignedDistancePrimitive
		};

		enum GlobalRootSignature {
			OutputView = 0,
			AccelerationStructure,
			SceneConstant,
			AABBattributeBuffer,
			VertexBuffers,
			Count_GlobalRootSignature
		};


		MetaBallSDFScene();
		~MetaBallSDFScene();
		virtual void CreateShaderTable() override;
		virtual void CreateRootSignatureDesc() override;
		virtual void CreateGeometry() override;
		virtual void CreateMaterials() override;
		virtual void InitAccelerationStructure() override;

		int GetAABBWidth() const {
			return m_AABBWidth;
		}
		int GetAABBDistance() const {
			return m_AABBDistance;
		}

		std::vector<PrimitiveConstantBuffer> GetMaterialCbAABB() const { return m_AabbMaterialCB; }
		PrimitiveConstantBuffer GetMaterialCbPlane() const {
			return m_PlaneMaterialCB;
		}

		UINT GetTotalPrimitiveCount() const override {
			return (AnalyticPrimitive::Count_AnalyticPrimitive + VolumetricPrimitive::Count_VolumetricPrimitive + SignedDistancePrimitive::Count_SignedDistancePrimitive);
		}

		UINT PerPrimitiveTypeCount(IntersectionShaderType type) override {
			switch (type)
			{
			case BlackPearl::SDFScene::AnalyticPrimitive:
				return AnalyticPrimitive::Count_AnalyticPrimitive;
			case BlackPearl::SDFScene::VolumetricPrimitive:
				return VolumetricPrimitive::Count_VolumetricPrimitive;
			case BlackPearl::SDFScene::SignedDistancePrimitive:
				return SignedDistancePrimitive::Count_SignedDistancePrimitive;
			default:
				break;
			}
			return 0;
		}

		void CreatePlane();
	private:
		int m_AABBWidth;
		int m_AABBDistance;
		// Root constants
		PrimitiveConstantBuffer m_PlaneMaterialCB;
		std::vector<PrimitiveConstantBuffer> m_AabbMaterialCB;
		CD3DX12_ROOT_PARAMETER m_GlobalRootParameters[GlobalRootSignature::Count_GlobalRootSignature];
		CD3DX12_ROOT_PARAMETER m_LocalRootParametersTriangle[1];
		CD3DX12_ROOT_PARAMETER m_LocalRootParametersAABB[2];

		CD3DX12_DESCRIPTOR_RANGE m_Ranges[2];

		D3D12_ROOT_SIGNATURE_DESC m_LocalRootSignaturesDescAABB;
		D3D12_ROOT_SIGNATURE_DESC m_LocalRootSignaturesDescTriangle;

		D3D12LocalRootSignature m_LocalRootSignaturesAABB;
		D3D12LocalRootSignature m_LocalRootSignaturesTriangle;
	};

}
#endif

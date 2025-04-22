#pragma once
#include "d3dx12.h"
#include "BlackPearl/Core.h"
namespace BlackPearl {

#define MAX_ROOTSIGNATRUE_PARAMETERS 100
	enum SignatureType
	{
		Global,
		Local
	};

	enum SlotType {
		//global type
		OutputView,
		AccelerationStructure,
		SceneConstant,
		AABBattributeBuffer,
		VertexBuffers,

		//local type
		MaterialConstant,
		GeometryIndex,
	};

	struct RootSignatureElement
	{
		D3D12_ROOT_PARAMETER_TYPE GetParameterType(SlotType slotType) {
			switch (slotType)
			{
			case BlackPearl::OutputView:
				break;
			case BlackPearl::AccelerationStructure:
				break;
			case BlackPearl::SceneConstant:
				break;
			case BlackPearl::AABBattributeBuffer:
				break;
			case BlackPearl::VertexBuffers:
				break;
			case BlackPearl::MaterialConstant:
				return D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
				break;
			default:
				break;
			}
		}
		int signatrueType;
		int geometryType;
		int slotType;
		int index;
		int constantSize;
		int shaderRegister;
		RootSignatureElement(int signatrueType, int geometryType, int slotType, int index, int constantSize, int shaderRegister)
			:signatrueType(signatrueType),
			geometryType(geometryType),
			slotType(slotType),
			index(index),
			constantSize(constantSize),
			shaderRegister(shaderRegister){}
	};


}


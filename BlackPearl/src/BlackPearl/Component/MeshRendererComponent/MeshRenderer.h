#pragma once
#include "BlackPearl/Component/Component.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Renderer/Model/Model.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/RHI/RHITexture.h"

//MeshRenderer只负责渲染
namespace BlackPearl {
	class MeshRenderer :public Component<MeshRenderer>
	{
	public:
		//virtual node, has no mesh(only material),such as RTXTransformNode
		MeshRenderer(std::shared_ptr<Material> material)
			:Component(Component::Type::MeshRenderer), m_Material(material) {}
	

		MeshRenderer(const std::shared_ptr<Mesh>& mesh)
			:Component(Component::Type::MeshRenderer) {

			m_Meshes.push_back(mesh);
		}
		MeshRenderer(const std::shared_ptr<Model>& model)
			:Component(Component::Type::MeshRenderer), m_Model(model) {
		}
		~MeshRenderer() {};

		//void UpdateTransformMatrix(glm::mat4 transformMatrix);

		inline std::shared_ptr<Model>GetModel()  const { return m_Model; }
		std::vector<std::shared_ptr<Mesh>>GetMeshes() const {
			return m_Model != nullptr ? m_Model->GetMeshes() : m_Meshes;
		}
		std::vector<std::shared_ptr<Mesh>>& GetMeshlets() {
			return m_Model != nullptr ? m_Model->GetMeshlets() : m_Meshes;
		}
		void SetTexture(unsigned int meshIndex, ITexture* texture);
		void SetTextures(ITexture* texture);
		void SetModelTexture(unsigned int meshIndex, const std::shared_ptr<ITexture>& texture);
		void SetModelTextures(const std::shared_ptr<ITexture>& texture);

		void SetShaders(const std::string& image);
		void SetShaders(const std::shared_ptr<Shader>& shader);

		void SetEnableCullFace(bool isEnable) { m_EnableCullFace = isEnable; }
		bool GetEnableCullFace()const { return m_EnableCullFace; }

		void SetPBRTextureSamples(bool isSampled) {
			for (auto mesh : GetMeshes())
				mesh->GetMaterial()->SetPBRTextureSample((int)isSampled);
		}
		void SetTextureDiffuseSamples(bool isSampled) {
			for (auto mesh : GetMeshes())
				mesh->GetMaterial()->SetTextureSampleDiffuse((int)isSampled);
		}
		void SetTexturEmissionSamples(bool isSampled) {
			for (auto mesh : GetMeshes())
				mesh->GetMaterial()->SetTextureSampleEmission((int)isSampled);
		}

		void SetTextureSpecularSamples(bool isSampled) {
			for (auto mesh : GetMeshes())
				mesh->GetMaterial()->SetTextureSampleSpecular((int)isSampled);
		}
		void SetTextureHeightSamples(bool isSampled) {
			for (auto mesh : GetMeshes())
				mesh->GetMaterial()->SetTextureSampleHeight((int)isSampled);
		}
		bool GetEnableRender()const { return m_EnableRender; }
		void SetEnableRender(bool enable) { m_EnableRender = enable; }

		bool GetIsBackGroundObjects() const { return m_IsBackGroundObjects; }
		void SetIsBackGroundObjects(bool isBackGroundObject) { m_IsBackGroundObjects = isBackGroundObject; }

		bool GetIsShadowObjects() const { return m_HasShadow; }
		void SetIsShadowObjects(bool isShadowObj) { m_HasShadow = isShadowObj; }

		bool GetIsPBRObject() const { return m_IsPBRObject; }
		void SetIsPBRObject(bool isPBRObject) { m_IsPBRObject = isPBRObject; }

		std::vector<unsigned int> GetDiffuseProbeChache() const { return m_diffuseProbeCaches; }
		void SetDiffuseProbeChache(std::vector<unsigned int> probesIdx) { m_diffuseProbeCaches = probesIdx; }

		std::vector<unsigned int> GetReflectionProbeChache() const { return m_reflectProbeCaches; }
		void SetReflectionProbeChache(std::vector<unsigned int> probesIdx) { m_reflectProbeCaches = probesIdx; }

		bool GetIsDiffuseProbeCacheSet() const { return m_SetDiffuseProbeCaches; }
		void SetIsDiffuseProbeCacheSet(bool set) { m_SetDiffuseProbeCaches = set; }

		bool GetIsReflectProbeCacheSet() const { return m_SetReflectProbeCaches; }
		void SetIsReflectProbeCacheSet(bool set) { m_SetReflectProbeCaches = set; }

		void SetSingleMaterial(std::shared_ptr<Material> material) {
			m_Material = material;
		}
		std::shared_ptr<Material> GetSingleMaterial() const {
			GE_ASSERT(m_Material != nullptr, "material is nullptr");
			return m_Material;
		}

	private:
		//开启和禁止渲染
		bool m_EnableRender = true;
		bool m_IsPBRObject = false;
		std::vector<std::shared_ptr<Mesh>> m_Meshes;
		std::shared_ptr<Model> m_Model = nullptr;

		//case: all meshes has the same material
		std::shared_ptr<Material> m_Material = nullptr;

		bool m_EnableCullFace = true;
		bool m_IsBackGroundObjects = false;
		bool m_HasShadow = true;

		bool m_SetDiffuseProbeCaches = false;
		bool m_SetReflectProbeCaches = false;

		std::vector<unsigned int> m_diffuseProbeCaches;
		std::vector<unsigned int> m_reflectProbeCaches;

	};

}


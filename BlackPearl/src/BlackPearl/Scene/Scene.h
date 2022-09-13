#pragma once
namespace BlackPearl {
	class Scene
	{
	public:
		enum DemoType {
			Empty,
			SDFScene,
			MetaBallSDFScene,
		};
		Scene(DemoType type = DemoType::Empty);
		~Scene();
		void AddCamera();
		void GetCamera();
		void AddLights();
		void GetLights();
		DemoType GetDemoType() { return m_DemoType; }
	protected:
		DemoType m_DemoType;

	};


}

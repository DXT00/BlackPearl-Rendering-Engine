#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"


//#include "BlackPearl/Scene/Scene.h"
#include "Layers/FrameBufferTestLayer.h"
#include "Layers/AdvanceLightTestLayer.h"
#include "Layers/ShadowMapTestLayer.h"
#include "Layers/SkyBoxTestLayer.h"
#include "BlackPearl/Application.h"

class SandBox :public BlackPearl::Application {
public:
	SandBox() {
		//PushLayer(new ExampleLayer("ExampleLayer"));
		BlackPearl::EntityManager * entityManager = DBG_NEW BlackPearl::EntityManager();
		BlackPearl::ObjectManager *objectManager = DBG_NEW BlackPearl::ObjectManager(entityManager);

		//BlackPearl::Layer* layer = DBG_NEW FrameBufferTestLayer("FrameBufferTest Layer", objectManager);
		BlackPearl::Layer* layer = DBG_NEW ShadowMapTestLayer("SkyBoxTest Layer", objectManager);
		//BlackPearl::Layer* layer = DBG_NEW AdvanceLightTestLayer("AdvanceLight Layer", objectManager);

		GetScene()->PushLayer(layer);
	}
	virtual ~SandBox() = default;

};

BlackPearl::Application* BlackPearl::CreateApplication() {

	return DBG_NEW SandBox();

}


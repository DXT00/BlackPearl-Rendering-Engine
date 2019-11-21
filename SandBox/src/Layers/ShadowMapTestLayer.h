#pragma once
#pragma once
#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
const GLuint SCR_WIDTH = 960, SCR_HEIGHT = 540;
GLuint quadVAO = 0;
GLuint quadVBO = 0;
GLuint cubeVAO = 0;
GLuint cubeVBO = 0;
class ShadowMapTestLayer :public BlackPearl::Layer {

public:

	ShadowMapTestLayer(const std::string& name, BlackPearl::ObjectManager *objectManager)
		: Layer(name, objectManager)
	{

		m_CameraObj = CreateCamera();
		auto cameraComponent = m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>();
		cameraComponent->SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
		m_CameraPosition = cameraComponent->GetPosition();
		m_CameraRotation.Yaw = cameraComponent->Yaw();
		m_CameraRotation.Pitch = cameraComponent->Pitch();

		BlackPearl::Renderer::Init();

		//	m_FrameBuffer.reset(DBG_NEW BlackPearl::FrameBuffer(960, 540, {BlackPearl::FrameBuffer::Attachment::DepthTexture },false));

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		// 设置OpenGL可选项
		glEnable(GL_DEPTH_TEST); // 开启深度测试

			//Shader reset
		m_Shader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/ShadowMaping_ShadowMapLayer.glsl"));
		m_SimpleDepthShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/DepthShader_ShadowMapLayer.glsl"));
		m_QuadDepthShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/Quad_ShadowMapLayer.glsl"));

		m_Shader->Bind();
		m_Shader->SetUniform1i("u_Material.diffuse", 0);
		m_Shader->SetUniform1i("u_Material.depth", 1);
		m_Shader->SetUniform1f("u_Material.shininess",128.0f);

		

		GLfloat planeVertices[] = {
			// Positions          // Normals         // Texture Coords
			 25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
		-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,
		-25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

		25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
		25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,
		-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f
		};
		// Setup plane VAO
		GLuint planeVBO;
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindVertexArray(0);

		

		BlackPearl::Texture wood(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/wood.png");
		woodTexture = wood.GetRendererID();

		glGenFramebuffers(1, &depthMapFBO);
		// - Create depth texture
		//GLuint depthMap;
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			GE_CORE_ERROR("Framebuffer is not complete!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);



		//CreateLight(BlackPearl::LightType::PointLight);
		//m_CubeObj = CreateCube("assets/shaders/DepthShader_ShadowMapLayer.glsl", "assets/texture/1.jpg");
		////m_CubeObj1 = CreateCube();
		//m_CubeObj1 = CreateCube("assets/shaders/DepthShader_ShadowMapLayer.glsl", "assets/texture/1.jpg");
		//CreateCube();
		//CreatePlane();

		//m_CubeObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(m_FrameBuffer->GetDepthTexture());
		//m_CubeObj1->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(m_FrameBuffer->GetDepthTexture());
		//m_QuadObj = CreateQuad("assets/shaders/Quad_ShadowMapLayer.glsl","");
		//m_QuadObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(m_FrameBuffer->GetDepthTexture());




	}

	virtual ~ShadowMapTestLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		//// render

		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());


		// 1. Render depth of scene to texture (from ligth's perspective)
	   // - Get light projection/view matrix.
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		//GLfloat near_plane = -20.0f, far_plane =20.0f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(m_LightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		m_SimpleDepthShader->Bind();
		m_SimpleDepthShader->SetUniformMat4f("u_LightProjectionViewMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
	
	
		RenderScene(m_SimpleDepthShader, lightSpaceMatrix);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. Render scene as normal 
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_Shader->Bind();

		m_Shader->SetUniformMat4f("u_ProjectionView", BlackPearl::Renderer::GetSceneData()->ViewProjectionMatrix);
		m_Shader->SetUniformMat4f("u_Projection", BlackPearl::Renderer::GetSceneData()->ProjectionMatrix);
		m_Shader->SetUniformMat4f("u_View", BlackPearl::Renderer::GetSceneData()->ViewMatrix);
		m_Shader->SetUniformVec3f("u_CameraViewPos", BlackPearl::Renderer::GetSceneData()->CameraPosition);

		// Set light uniforms
		m_Shader->SetUniformVec3f("u_LightPos", m_LightPos);
		m_Shader->SetUniformMat4f("u_LightProjectionViewMatrix", lightSpaceMatrix);

	//m_Shader->SetUniform1i("u_Material.diffuse", 0);

	//	m_Shader->SetUniform1i("u_Material.depth", 1);
//		glBindTexture(GL_TEXTURE_2D, depthMap);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
	////	RenderScene(m_Shader,);
	//	//m_Shader->SetUniform1f("u_Material.shininess", 64.0f);

		RenderScene(m_Shader, lightSpaceMatrix);
		//glBindTexture(GL_TEXTURE_2D, 0);

		//glViewport(0, 0, 240, 135);

		/*m_QuadDepthShader->Bind();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		RenderQuad();*/
		//glm::mat4 lightProjection, lightView;
		//

		//m_FrameBuffer->Bind(960, 540);
		//glClear(GL_DEPTH_BUFFER_BIT| GL_COLOR_BUFFER_BIT );
		//m_CubeObj->GetComponent<BlackPearl::MeshRenderer>()->SetShaders("assets/shaders/DepthShader_ShadowMapLayer.glsl");



		//m_CubeObj1->GetComponent<BlackPearl::MeshRenderer>()->SetShaders("assets/shaders/DepthShader_ShadowMapLayer.glsl");
		//
		//DrawObjectsExcept(m_QuadObj);

		//m_FrameBuffer->UnBind();

		//

		//glViewport(0, 0, 960, 540);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//m_CubeObj->GetComponent<BlackPearl::MeshRenderer>()->SetShaders("assets/shaders/ShadowMaping_ShadowMapLayer.glsl");//ShadowMaping_ShadowMapLayer

		//m_CubeObj1->GetComponent<BlackPearl::MeshRenderer>()->SetShaders("assets/shaders/ShadowMaping_ShadowMapLayer.glsl");
		//DrawObjectsExcept(m_QuadObj);
		//glViewport(0, 0, 240, 135);

		//DrawObject(m_QuadObj);
		//



	}

	void RenderQuad()
	{
		if (quadVAO == 0)
		{
			GLfloat quadVertices[] = {
				// Positions        // Texture Coords
				-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			};
			// Setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	void RenderScene(std::shared_ptr<BlackPearl::Shader> &shader,glm::mat4 lightSpaceMatrix) {

		// Floor
		glm::mat4 model(1.0f);
		shader->SetUniformVec3f("u_LightPos", m_LightPos);
		shader->SetUniformMat4f("u_LightProjectionViewMatrix", lightSpaceMatrix);

		shader->SetUniformMat4f("u_Model", model);
		shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(model)));
	
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		// Cubes
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
		shader->SetUniformMat4f("u_Model", model);
		shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(model)));
		// Set light uniforms
		shader->SetUniformVec3f("u_LightPos", m_LightPos);
		shader->SetUniformMat4f("u_LightProjectionViewMatrix", lightSpaceMatrix);



		RenderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 1.0f, 3.0f));
		shader->SetUniformMat4f("u_Model", model);
		shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(model)));
		// Set light uniforms
		shader->SetUniformVec3f("u_LightPos", m_LightPos);
		shader->SetUniformMat4f("u_LightProjectionViewMatrix", lightSpaceMatrix);


	
		RenderCube();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-20.0f, 3.0f, 2.0f));
		shader->SetUniformMat4f("u_Model", model);
		shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(model)));
		// Set light uniforms
		shader->SetUniformVec3f("u_LightPos", m_LightPos);
		shader->SetUniformMat4f("u_LightProjectionViewMatrix", lightSpaceMatrix);


		RenderCube();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
		model = glm::rotate(model, 60.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		model = glm::scale(model, glm::vec3(0.5));
		shader->SetUniformMat4f("u_Model", model);
		shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(model)));
		
		// Set light uniforms
		shader->SetUniformVec3f("u_LightPos", m_LightPos);
		shader->SetUniformMat4f("u_LightProjectionViewMatrix", lightSpaceMatrix);


		RenderCube();
	}
	void RenderCube() {

		// Initialize (if necessary)
		if (cubeVAO == 0)
		{
			GLfloat vertices[] = {
				// Back face
				-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
				0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
				0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
				0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
				-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
				-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
				// Front face
				-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
				0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
				0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
				0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
				-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
				-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
				// Left face
				-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
				-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
				-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
				-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
				-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
				-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
				// Right face
				0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
				0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
				0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
				0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
				0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
				0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
				// Bottom face
				-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
				0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
				0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
				0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
				-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
				-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
				// Top face
				-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
				0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
				0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
				0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
				-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
				-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
			};
			glGenVertexArrays(1, &cubeVAO);
			glGenBuffers(1, &cubeVBO);
			// Fill buffer
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			// Link vertex attributes
			glBindVertexArray(cubeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		// Render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	void OnImguiRender() override {


		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Suqare Color", glm::value_ptr(m_BackgroundColor));
		ImGui::End();

		static BlackPearl::Object* currentObj = nullptr;//TODO::注意内存泄漏

		if (ImGui::CollapsingHeader("Create")) {

			const char* const entityItems[] = { "Empty","ParallelLight","PointLight","SpotLight","IronMan","Cube","Plane" };
			static int entityIdx = -1;
			if (ImGui::Combo("CreateEntity", &entityIdx, entityItems, 7))
			{
				switch (entityIdx)
				{
				case 0:
					GE_CORE_INFO("Creating Empty...");
					Layer::CreateEmpty();
					break;
				case 1:
					GE_CORE_INFO("Creating PointLight...");
					Layer::CreateLight(BlackPearl::LightType::ParallelLight);
					break;
				case 2:
					GE_CORE_INFO("Creating PointLight...");
					Layer::CreateLight(BlackPearl::LightType::PointLight);
					break;
				case 3:
					GE_CORE_INFO("Creating SpotLight ...");
					Layer::CreateLight(BlackPearl::LightType::SpotLight);
					break;
				case 4:
					GE_CORE_INFO("Creating IronMan ...");
					Layer::CreateModel("assets/models/IronMan/IronMan.obj", "assets/shaders/IronMan.glsl");
					break;
				case 5:
					GE_CORE_INFO("Creating Cube ...");
					Layer::CreateCube();
					break;
				case 6:
					GE_CORE_INFO("Creating Plane ...");
					Layer::CreatePlane();
					break;
				}
			}
		}
		if (ImGui::BeginTabBar("TabBar 0", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Scene")) {
				std::vector<BlackPearl::Object*> objsList = GetObjects();		//TODO::
				ImGui::ListBoxHeader("CurrentEntities", (int)objsList.size(), 7);

				for (int n = 0; n < objsList.size(); n++) {
					//ImGui::Text("%s", objsList[n].c_str());
					bool is_selected = (currentObj != nullptr && currentObj->ToString() == objsList[n]->ToString());
					if (ImGui::Selectable(objsList[n]->ToString().c_str(), is_selected)) {
						currentObj = objsList[n];
						GE_CORE_INFO(objsList[n]->ToString() + "is selected")
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::ListBoxFooter();
				ImGui::EndTabItem();
			}
			//}
		}
		ImGui::EndTabBar();

		////////////////////Inspector/////////////////////////
		ImGui::Begin("Inspector");


		float pos[] = { m_LightPos.x,m_LightPos.y,m_LightPos.z };
		ImGui::DragFloat3("m_LightPos", pos, 0.1f, -100.0f, 100.0f, "%.3f ");
		m_LightPos = { pos[0],pos[1],pos[2] };

		
		ImGui::DragFloat("near_plane", &near_plane, 0.5f, -50.0f, 100.0f, "%.3f ");
		ImGui::DragFloat("far_plane", &far_plane, 0.5f, -50.0f, 100.0f, "%.3f ");

		if (currentObj != nullptr) {

			std::unordered_map<BlackPearl::BaseComponent::Family, std::shared_ptr<BlackPearl::BaseComponent>> componentList = currentObj->GetComponentList();

			for (auto pair : componentList) {
				auto component = pair.second;
				if (component != nullptr) {

					switch (component->GetType()) {
					case BlackPearl::BaseComponent::Type::MeshRenderer: {
						std::shared_ptr<BlackPearl::MeshRenderer> comp = std::dynamic_pointer_cast<BlackPearl::MeshRenderer>(component);
						ShowMeshRenderer(comp);
						break;
					}
					case BlackPearl::BaseComponent::Type::Transform: {
						std::shared_ptr<BlackPearl::Transform> comp = std::dynamic_pointer_cast<BlackPearl::Transform>(component);
						ShowTransform(comp);
						break;
					}
					case BlackPearl::BaseComponent::Type::Light: {

						std::shared_ptr<BlackPearl::Light> comp = std::dynamic_pointer_cast<BlackPearl::Light>(component);
						ShowLight(comp);
						break;
					}
					default:
						break;
					}

				}

			}
		}

		ImGui::End();



		m_fileDialog.Display();




	}
	void OnAttach() override {


	}
	void InputCheck(float ts)
	{
		auto cameraComponent = m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>();
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_W)) {
			m_CameraPosition += cameraComponent->Front() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_S)) {
			m_CameraPosition -= cameraComponent->Front()* m_CameraMoveSpeed * ts;
		}
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_A)) {
			m_CameraPosition -= cameraComponent->Right() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_D)) {
			m_CameraPosition += cameraComponent->Right() * m_CameraMoveSpeed * ts;
		}
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_E)) {
			m_CameraPosition += cameraComponent->Up() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_Q)) {
			m_CameraPosition -= cameraComponent->Up() * m_CameraMoveSpeed * ts;
		}
		// ---------------------Rotation--------------------------------------

		float posx = BlackPearl::Input::GetMouseX();
		float posy = BlackPearl::Input::GetMouseY();
		if (BlackPearl::Input::IsMouseButtonPressed(BP_MOUSE_BUTTON_RIGHT)) {


			if (BlackPearl::Input::IsFirstMouse()) {
				BlackPearl::Input::SetFirstMouse(false);
				m_LastMouseX = posx;
				m_LastMouseY = posy;
			}
			float diffx = posx - m_LastMouseX;
			float diffy = -posy + m_LastMouseY;

			m_LastMouseX = posx;
			m_LastMouseY = posy;
			m_CameraRotation.Yaw += diffx * m_CameraRotateSpeed * ts;
			m_CameraRotation.Pitch += diffy * m_CameraRotateSpeed * ts;

			if (m_CameraRotation.Pitch > 89.0f)
				m_CameraRotation.Pitch = 89.0f;
			if (m_CameraRotation.Pitch < -89.0f)
				m_CameraRotation.Pitch = -89.0f;

			cameraComponent->SetRotation(m_CameraRotation.Yaw, m_CameraRotation.Pitch);

		}
		else {

			m_LastMouseX = posx;//lastMouse时刻记录当前坐标位置，防止再次点击右键时，发生抖动！
			m_LastMouseY = posy;
		}


		cameraComponent->SetPosition(m_CameraPosition);
	}
private:

	std::vector<BlackPearl::Object*> m_LightObjs;
	BlackPearl::Object* m_CameraObj;
	BlackPearl::Object* m_IronManObj;
	BlackPearl::Object* m_QuadObj;
	BlackPearl::Object* m_PlaneObj;
	BlackPearl::Object* m_CubeObj;
	BlackPearl::Object* m_CubeObj1;
	BlackPearl::Object* m_SkyBoxObj;
	glm::vec3 m_CameraPosition = { 0.0f,0.0f,0.0f };
	struct CameraRotation {
		float Yaw;
		float Pitch;

	};
	CameraRotation m_CameraRotation;
	float m_LastMouseX;
	float m_LastMouseY;
	float m_CameraMoveSpeed = 5.0f;
	float m_CameraRotateSpeed = 9.0f;

	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };




	std::shared_ptr<BlackPearl::FrameBuffer> m_FrameBuffer;
	glm::vec3 m_LightPos = { 0.0f, 1.5f, 0.0f };
	std::shared_ptr<BlackPearl::Shader> m_SimpleDepthShader;
	std::shared_ptr<BlackPearl::Shader> m_Shader;
	std::shared_ptr<BlackPearl::Shader> m_QuadDepthShader;


	GLuint woodTexture;
	GLuint planeVAO;
	GLuint depthMapFBO;
	GLuint depthMap;
	GLfloat near_plane = 1.0f, far_plane = 7.5f;
};

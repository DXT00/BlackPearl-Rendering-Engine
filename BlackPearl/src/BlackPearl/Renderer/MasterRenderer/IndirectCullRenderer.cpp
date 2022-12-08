#include "pch.h"
#include "IndirectCullRenderer.h"
#include "BlackPearl/Node/BatchNode.h"
#include "BlackPearl/Node/SingleNode.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Common/CommonFunc.h"
namespace BlackPearl {


	IndirectCullRenderer::IndirectCullRenderer()
	{
	}

	IndirectCullRenderer::~IndirectCullRenderer()
	{
		GE_SAVE_FREE(m_PositionBuffer);
		GE_SAVE_FREE(m_NormalBuffer);
		GE_SAVE_FREE(m_TexCordBuffer);
		GE_SAVE_FREE(m_TangentBuffer);
		GE_SAVE_FREE(m_BitangentBuffer);
		GE_SAVE_FREE(m_JointIndicesBuffer);
		GE_SAVE_FREE(m_JointIndices1Buffer);
		GE_SAVE_FREE(m_WeightBuffer);
		GE_SAVE_FREE(m_Weight1Buffer);
		GE_SAVE_FREE(m_ColorBuffer);
		GE_SAVE_FREE(m_ObjIdBuffer);
		GE_SAVE_FREE(m_ObjsTransformV0Buffer);
		GE_SAVE_FREE(m_ObjsTransformV1Buffer);
		GE_SAVE_FREE(m_ObjsTransformV2Buffer);
		GE_SAVE_DELETE_ARRAY(m_ObjsTransformBuffer);
		GE_SAVE_DELETE_ARRAY(m_InterleaveVertexBuffer);
		GE_SAVE_DELETE_ARRAY(m_IndexBuffer);
	}



	void IndirectCullRenderer::Init(Scene* scene)
	{
		m_Scene = scene;
		m_MeshGatherer = DBG_NEW MeshGatherer(m_Scene, MeshGatherer::GatherSingleNode);
		uint32_t objCnt = m_MeshGatherer->GetObjCnt();
		uint32_t meshCnt = m_MeshGatherer->GetMeshCnt();
		m_Commands.resize(m_MeshGatherer->GetMeshCnt());
		m_CullingShader = std::make_shared<Shader>("assets/shaders/occlusionCulling/cull.glsl");
		m_FillCmdsShader = std::make_shared<Shader>("assets/shaders/occlusionCulling/fillCmds.glsl");
		m_CmdsClearShader = std::make_shared<Shader>("assets/shaders/occlusionCulling/clear.glsl");
		m_DepthShader = std::make_shared<Shader>("assets/shaders/occlusionCulling/depthMap.glsl");
		m_ShowShader = std::make_shared<Shader>("assets/shaders/occlusionCulling/depthShow.glsl");
		m_HizShader = std::make_shared<Shader>("assets/shaders/occlusionCulling/hiz.glsl");

		GenerateIndirectDrawCommands();
		// create SSBO for compte shader
		m_ComputeShaderSSBOs.resize(ComputeShaderSlot::COUNT);
		m_ComputeShaderSSBOs[CountBuffer_Slot] =
			std::make_shared<ShaderStorageBuffer>(m_MeshGatherer->GetMeshCnt() * sizeof(uint32_t), GL_STREAM_DRAW, nullptr);
		m_ComputeShaderSSBOs[DispactchObjThreads_Slot] =
			std::make_shared<ShaderStorageBuffer>(sizeof(DispatchIndirect), GL_STREAM_DRAW, nullptr);
		m_ComputeShaderSSBOs[ObjsInfoData_Slot] =
			std::make_shared<ShaderStorageBuffer>(m_MeshGatherer->GetObjCnt() * sizeof(MeshGatherer::ObjInfo), GL_STREAM_DRAW, (m_MeshGatherer->GetObjInfoBuffer()));
		//m_ComputeShaderSSBOs[MeshInfoData_Slot] = 
		//	std::make_shared<ShaderStorageBuffer>(m_MeshGatherer->GetObjCnt() * sizeof(MeshGatherer::MeshIdxInfo), GL_STREAM_DRAW, reinterpret_cast<float*>(m_MeshGatherer->GetMeshIdBuffer()));
		m_ComputeShaderSSBOs[IndirectCmds_Slot] =
			std::make_shared<ShaderStorageBuffer>(m_MeshGatherer->GetMeshCnt() * sizeof(IndirectCommand), GL_STREAM_DRAW, m_Commands.data());
		//m_ComputeShaderSSBOs[VisibleObjs_Slot] = 
		//	std::make_shared<ShaderStorageBuffer>(m_MeshGatherer->GetObjCnt() * sizeof(MeshGatherer::ObjInfo), GL_STREAM_DRAW, nullptr);
		GE_ERROR_JUDGE();


		m_MeshGatherer->GetVAO()->SetIndirectBuffer(MeshGatherer::LayOutSlot::DRAWID_SLOT, m_ComputeShaderSSBOs[IndirectCmds_Slot]->GetRenderID());
		m_MeshGatherer->GetVAO()->UpdateVertexBuffers();

		m_HizFrameBuffer.reset(DBG_NEW FrameBuffer());
		m_DepthTexture.reset(DBG_NEW Texture(Texture::Type::DepthMap, Configuration::WindowWidth, Configuration::WindowHeight, true/*isDepth*/, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_CLAMP_TO_EDGE, GL_FLOAT, true/*genmipmap*/));
		m_HizFrameBuffer->Bind();
		m_HizFrameBuffer->AttachDepthTexture(m_DepthTexture, 0);
		glDrawBuffer(GL_NONE); // No color buffer is drawn to.
		glReadBuffer(GL_NONE);
		m_HizFrameBuffer->UnBind();


		std::vector<GLfloat> Objtmp;
		m_ComputeShaderSSBOs[ObjsInfoData_Slot]->Bind();
		GLfloat* objBufPtr = (GLfloat*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_MeshGatherer->GetObjCnt() * sizeof(MeshGatherer::ObjInfo), GL_MAP_READ_BIT);
		for (unsigned j = 0; j < m_MeshGatherer->GetObjCnt() * 8; ++j) {

			GLfloat a = objBufPtr[j];
			Objtmp.push_back(a);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		m_ComputeShaderSSBOs[ObjsInfoData_Slot]->UnBind();


	}

	void IndirectCullRenderer::RenderDepthMap(Object* Quad)
	{

		/**************************** Render depth map lod 0 *******************************/

		m_DepthShader->Bind();

		glViewport(0, 0, Configuration::WindowWidth, Configuration::WindowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_HizFrameBuffer->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		//m_HizFrameBuffer->AttachDepthTexture(m_DepthTexture, 0);
		//render scene to depth buffer
		for (size_t i = 0; i < m_Scene->GetSingleNodes().size(); i++)
		{
			auto obj = dynamic_cast<SingleNode*>(m_Scene->GetSingleNodes(i))->GetObj();
			DrawObject(obj, m_DepthShader);
		}

		m_HizFrameBuffer->UnBind();

		glViewport(0, 0, Configuration::WindowWidth, Configuration::WindowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//CommonFunc::ShowFrameBuffer(1, 1, Quad, m_HizFrameBuffer, { m_HizFrameBuffer->GetDepthTexture() });
		//CommonFunc::ShowFrameBuffer(glm::vec4(0, 0, Configuration::WindowWidth, Configuration::WindowHeight), Quad, m_HizFrameBuffer, m_HizFrameBuffer->GetDepthTexture(), true/*isMipmap*/, 0);



		/**************************** Gnererate Hiz depth map *******************************/
		m_HizShader->Bind();
		m_HizFrameBuffer->Bind();


		m_MipmapLevel = 1+ (int)log2f(fmaxf(Configuration::WindowWidth, Configuration::WindowHeight));
		int curWidth = Configuration::WindowWidth;
		int curHeight = Configuration::WindowHeight;

		// disable color buffer as we will render only a depth image
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthFunc(GL_ALWAYS);


		glActiveTexture(GL_TEXTURE0);
		m_HizFrameBuffer->GetDepthTexture()->Bind();
		m_HizShader->SetUniform1i("u_LastMip", 0);

		
		for (size_t i = 1; i < m_MipmapLevel; i++)
		{
			m_HizShader->Bind();
			m_HizShader->SetUniformVec2i("u_LastMipSize", glm::ivec2(curWidth , curHeight));

			curWidth /= 2;
			curHeight /= 2;
			curWidth  = curWidth > 0 ? curWidth : 1;
			curHeight = curHeight > 0 ? curHeight : 1;
			glViewport(0, 0, curWidth, curHeight);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, i - 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, i - 1);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture->GetRendererID(), i);
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
			m_HizShader->SetUniform1i("u_LastMipLevel", i - 1);
			//glActiveTexture(GL_TEXTURE0);
			//m_HizFrameBuffer->GetDepthTexture()->Bind();
			//m_HizShader->SetUniform1i("u_LastMip", 0);

			DrawObject(Quad, m_HizShader);

		}

		//reset mipmap level range, and bind lod 0
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, m_MipmapLevel - 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture->GetRendererID(), 0);


		glDepthFunc(GL_LEQUAL);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);




		m_HizFrameBuffer->UnBind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 0
		uint32_t width = Configuration::WindowWidth;
		uint32_t height = Configuration::WindowHeight;
		glm::vec2 size = glm::vec2(width / 2, height / 2);

		CommonFunc::ShowFrameBuffer(glm::vec4(0, 0, size.x, size.y), Quad, m_HizFrameBuffer, m_HizFrameBuffer->GetDepthTexture(), true/*isMipmap*/, 2);

		CommonFunc::ShowFrameBuffer(glm::vec4(width / 2, 0, size.x,  size.y), Quad, m_HizFrameBuffer, m_HizFrameBuffer->GetDepthTexture(), true/*isMipmap*/, 2);

		CommonFunc::ShowFrameBuffer(glm::vec4(0, height / 2, size.x, size.y), Quad, m_HizFrameBuffer, m_HizFrameBuffer->GetDepthTexture(), true/*isMipmap*/, 3);

		CommonFunc::ShowFrameBuffer(glm::vec4(width / 2, height / 2,size.x, size.y), Quad, m_HizFrameBuffer, m_HizFrameBuffer->GetDepthTexture(), true/*isMipmap*/, 4);
#endif
	}

	
	void IndirectCullRenderer::Render(std::shared_ptr<Shader> indirectShader, MainCamera* cam)
	{


		//UpdateTransform();
		//UpdateDrawCommands();

		Clear();
		GE_ERROR_JUDGE();


		m_CullingShader->Bind();
		m_CullingShader->SetUniform1i("u_DeapthMap", 5);
		glActiveTexture(GL_TEXTURE5);
		m_HizFrameBuffer->GetDepthTexture()->Bind();

		m_CullingShader->SetUniformMat4f("u_ProjectionView", Renderer::GetSceneData()->ProjectionViewMatrix);
		m_CullingShader->SetUniformMat4f("u_Projection", Renderer::GetSceneData()->ProjectionMatrix);
		m_CullingShader->SetUniformMat4f("u_View", Renderer::GetSceneData()->ViewMatrix);

		m_CullingShader->SetUniformVec3f("u_CameraViewPos", Renderer::GetSceneData()->CameraPosition);
		m_CullingShader->SetUniform1f("u_Near", cam->ZNear());
		m_CullingShader->SetUniform1f("u_Far", cam->ZFar());
		m_CullingShader->SetUniform1f("u_MaxLevel", m_MipmapLevel);
		m_CullingShader->SetUniformVec2f("u_ViewPortSize", glm::vec2(Configuration::WindowWidth, Configuration::WindowHeight));

		Cull();
		FillterCommands();
		/*m_ComputeShaderSSBOs[IndirectCmds_Slot]->Bind();
		m_ComputeShaderSSBOs[IndirectCmds_Slot]->BindIndex(MeshGatherer::LayOutSlot::DRAWID_SLOT);*/
		GE_ERROR_JUDGE();

		m_ComputeShaderSSBOs[IndirectCmds_Slot]->Bind();

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_ComputeShaderSSBOs[IndirectCmds_Slot]->GetRenderID());

		GE_ERROR_JUDGE();
		DrawMultiIndirect(m_MeshGatherer->GetVAO(), indirectShader, m_Commands.size());
		GE_ERROR_JUDGE();
		m_ComputeShaderSSBOs[IndirectCmds_Slot]->UnBind();


	}


	void IndirectCullRenderer::Clear()
	{
		m_CmdsClearShader->Bind();
		m_CmdsClearShader->SetUniform1ui("uMeshCounts", m_MeshGatherer->GetMeshCnt());
		m_ComputeShaderSSBOs[DispactchObjThreads_Slot]->BindIndex(DispactchObjThreads_Slot);
		m_ComputeShaderSSBOs[CountBuffer_Slot]->BindIndex(CountBuffer_Slot);
		m_ComputeShaderSSBOs[IndirectCmds_Slot]->BindIndex(IndirectCmds_Slot);
		GE_ERROR_JUDGE();

		
	
		DiscpatchCompute(m_MeshGatherer->GetMeshCnt(), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);
		GE_ERROR_JUDGE();

		std::vector<GLuint> Cnttmp;
		m_ComputeShaderSSBOs[CountBuffer_Slot]->Bind();
		GLuint* countBufPtr = (GLuint*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_MeshGatherer->GetMeshCnt() * sizeof(GLuint), GL_MAP_READ_BIT);
		for (unsigned j = 0; j < m_MeshGatherer->GetMeshCnt(); ++j) {

			GLuint a = countBufPtr[j];
			Cnttmp.push_back(a);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		m_ComputeShaderSSBOs[CountBuffer_Slot]->UnBind();


		std::vector<GLuint> tmp;
		m_ComputeShaderSSBOs[IndirectCmds_Slot]->Bind();
		GLuint* cmdBufPtr = (GLuint*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_MeshGatherer->GetMeshCnt() * sizeof(IndirectCommand), GL_MAP_READ_BIT);
		for (unsigned j = 0; j < m_MeshGatherer->GetMeshCnt() * 5; ++j) {

			GLuint a = cmdBufPtr[j];
			tmp.push_back(a);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		m_ComputeShaderSSBOs[IndirectCmds_Slot]->UnBind();
	}


	void IndirectCullRenderer::Cull()
	{
		m_ComputeShaderSSBOs[CountBuffer_Slot]->BindIndex(CountBuffer_Slot);
		m_ComputeShaderSSBOs[DispactchObjThreads_Slot]->BindIndex(DispactchObjThreads_Slot);
		m_ComputeShaderSSBOs[ObjsInfoData_Slot]->BindIndex(ObjsInfoData_Slot);
		/*m_ComputeShaderSSBOs[MeshInfoData_Slot]->BindIndex(MeshInfoData_Slot);
		m_ComputeShaderSSBOs[VisibleObjs_Slot]->BindIndex(VisibleObjs_Slot);*/

		DiscpatchCompute(m_MeshGatherer->GetObjCnt(), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		std::vector<GLuint> Cnttmp;
		m_ComputeShaderSSBOs[CountBuffer_Slot]->Bind();
		GLuint* countBufPtr = (GLuint*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_MeshGatherer->GetMeshCnt() * sizeof(GLuint), GL_MAP_READ_BIT);
		for (unsigned j = 0; j < m_MeshGatherer->GetMeshCnt(); ++j) {

			GLuint a = countBufPtr[j];
			Cnttmp.push_back(a);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		m_ComputeShaderSSBOs[CountBuffer_Slot]->UnBind();



		std::vector<GLfloat> Objtmp;
		m_ComputeShaderSSBOs[ObjsInfoData_Slot]->Bind();
		GLfloat* objBufPtr = (GLfloat*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_MeshGatherer->GetObjCnt() * sizeof(MeshGatherer::ObjInfo), GL_MAP_READ_BIT);
		for (unsigned j = 0; j < m_MeshGatherer->GetObjCnt() * 8; ++j) {

			GLfloat a = objBufPtr[j];
			Objtmp.push_back(a);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		m_ComputeShaderSSBOs[ObjsInfoData_Slot]->UnBind();
	}

	void IndirectCullRenderer::FillterCommands()
	{
		m_FillCmdsShader->Bind();
		m_ComputeShaderSSBOs[CountBuffer_Slot]->BindIndex(CountBuffer_Slot);
		m_ComputeShaderSSBOs[IndirectCmds_Slot]->BindIndex(IndirectCmds_Slot);
		m_FillCmdsShader->SetUniform1i("uMeshCounts", m_MeshGatherer->GetMeshCnt());
		DiscpatchCompute(m_MeshGatherer->GetMeshCnt(), 1, 1);
		GE_ERROR_JUDGE();

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		GE_ERROR_JUDGE();

		std::vector<GLuint> tmp;
		m_ComputeShaderSSBOs[IndirectCmds_Slot]->Bind();
		GLuint* cmdBufPtr = (GLuint*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_MeshGatherer->GetMeshCnt() * sizeof(IndirectCommand), GL_MAP_READ_BIT);
		for (unsigned j = 0; j < m_MeshGatherer->GetMeshCnt() * 5; ++j) {

			GLuint a = cmdBufPtr[j];
			tmp.push_back(a);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		m_ComputeShaderSSBOs[IndirectCmds_Slot]->UnBind();

	}

	void IndirectCullRenderer::ShowDepthMap(int mipmap)
	{
	}



	void IndirectCullRenderer::UpdateTransform()
	{
		uint32_t baseVert = 0;
		uint32_t baseIndex = 0;
		uint32_t baseInstance = 0;
		uint32_t cmdIdx = 0;


		for (auto node : m_Scene->GetSingleNodes())
		{
			SingleNode* singleNode = dynamic_cast<SingleNode*>(node);
			auto& meshes = singleNode->GetObj()->GetComponent<MeshRenderer>()->GetMeshes();

			AddObjTransformToBuffer(baseInstance, singleNode->GetObj()->GetComponent<Transform>()->GetTransformMatrix());
			baseInstance++;
		}
		m_MatrixVBO->UpdateData(m_ObjsTransformBuffer, m_ObjsCnt * sizeof(glm::mat4));
		m_VertexArray->UpdateVertexBuffer(m_MatrixVBO);

	}


	void IndirectCullRenderer::GenerateIndirectDrawCommands()
	{

		uint32_t baseVert = 0;
		uint32_t baseIndex = 0;
		uint32_t cmdId = 0;
		uint32_t baseInstance = 0;

		for (auto node : m_Scene->GetSingleNodes())
		{
			SingleNode* singleNode = dynamic_cast<SingleNode*>(node);
			auto& meshes = singleNode->GetObj()->GetComponent<MeshRenderer>()->GetMeshes();


			for (auto mesh : meshes)
			{
				uint32_t meshVetCnt = mesh->GetVertexCount();
				uint32_t meshIndexCnt = mesh->GetIndicesConut();

				m_Commands[cmdId].count = meshIndexCnt;		//4 triangles = 12 vertices
				m_Commands[cmdId].instanceCnt = 0;		//Draw 1 instance
				m_Commands[cmdId].firstIndex = 0;			//Draw from index 0 for this instance
				m_Commands[cmdId].startVertex = baseVert;	//Starting from baseVert
				m_Commands[cmdId].startInstance = 0;		//gl_InstanceID

				baseVert += meshVetCnt;
				baseIndex += meshIndexCnt;
				baseInstance++;
				cmdId++;

			}
		}

		//m_IndirectBuffer = std::make_shared<IndirectBuffer>(m_Commands);
		//m_VertexArray->SetIndirectBuffer(DRAWID_SLOT, m_IndirectBuffer);

	}


	void IndirectCullRenderer::UpdateDrawCommands()
	{
		m_IndirectBuffer->UpdateCommands(m_Commands);
		m_VertexArray->SetIndirectBuffer(DRAWID_SLOT, m_IndirectBuffer);

	}

	void IndirectCullRenderer::AddObjTransformToBuffer(uint32_t objId, glm::mat4 modelMat) {
		m_ObjsTransformBuffer[objId] = modelMat;
	}

	void IndirectCullRenderer::GernerateInterleaveIndirectData()
	{
		//InitIndirectData();
		m_VertexCnt = 0;
		m_IndexCnt = 0;
		m_ObjsCnt = 0;
		m_MeshCnt = 0;
		//需要保证每个object 的vertex attribute 属性排列都是一样的，即IndirectVertex中的属性
		for (size_t i = 0; i < m_Scene->GetSingleNodesCnt(); i++)
		{
			SingleNode* singleNode = dynamic_cast<SingleNode*>(m_Scene->GetSingleNodes(i));
			m_VertexCnt += singleNode->GetVertexCount();
			m_IndexCnt += singleNode->GetIndexCount();
			m_MeshCnt += singleNode->GetObj()->GetComponent<MeshRenderer>()->GetMeshes().size();
			m_ObjsCnt++;
		}

		m_Commands.resize(m_MeshCnt);

		m_InterleaveVertexBuffer = DBG_NEW IndirectVertex[m_VertexCnt];
		m_IndexBuffer = DBG_NEW uint32_t[m_IndexCnt];
		m_ObjsTransformBuffer = DBG_NEW glm::mat4[m_ObjsCnt];
		//m_ColorBuffer = (float*)malloc(m_VertexCnt * sizeof(float) * 3);

		uint32_t baseVert = 0;
		uint32_t baseIndex = 0;
		uint32_t baseInstance = 0;
		uint32_t cmdIdx = 0;


		for (auto node : m_Scene->GetSingleNodes())
		{
			SingleNode* singleNode = dynamic_cast<SingleNode*>(node);
			auto& meshes = singleNode->GetObj()->GetComponent<MeshRenderer>()->GetMeshes();


			for (auto mesh : meshes)
			{
				uint32_t meshVetCnt = mesh->GetVertexCount();
				uint32_t meshIndexCnt = mesh->GetIndicesConut();
				memcpy(m_InterleaveVertexBuffer + baseVert, mesh->GetVertices(), meshVetCnt * sizeof(IndirectVertex));
				uint32_t* indexBuffer = mesh->GetIndicesBuffer().first;

				for (size_t i = 0; i < meshIndexCnt; i++)
				{
					m_IndexBuffer[baseIndex + i] = indexBuffer[i];
				}

				AddObjTransformToBuffer(baseInstance, singleNode->GetObj()->GetComponent<Transform>()->GetTransformMatrix());
				baseVert += meshVetCnt;
				baseIndex += meshIndexCnt;
				baseInstance++;

			}

		}

		//PrintData();

		m_VertexArray->Bind();

		m_VertexVBO = std::make_shared<VertexBuffer>(reinterpret_cast<float*>(m_InterleaveVertexBuffer), m_VertexCnt * sizeof(IndirectVertex));
		m_VertexVBO->SetBufferLayout({
			{ ElementDataType::Float3,"aPos",false,POS_SLOT},
			{ ElementDataType::Float3,"aNormal",false,NORMAL_SLOT} ,
			{ ElementDataType::Float2,"aTexCoords",false,TEXCOORD_SLOT}
			});
		m_VertexArray->AddVertexBuffer(m_VertexVBO);

		m_IndexIBO = std::make_shared<IndexBuffer>(m_IndexBuffer, m_IndexCnt * sizeof(uint32_t));
		m_VertexArray->SetIndexBuffer(m_IndexIBO);

		m_MatrixVBO = std::make_shared<VertexBuffer>(m_ObjsTransformBuffer, m_ObjsCnt * sizeof(glm::mat4),true,true,1);
		m_MatrixVBO->SetBufferLayout({
			{ ElementDataType::Float4,"aModel",false,MODELS_SLOT},
			{ ElementDataType::Float4,"aModelV1",false,MODELS_SLOT + 1},
			{ ElementDataType::Float4,"aModelV2",false,MODELS_SLOT + 2},
			{ ElementDataType::Float4,"aModelV3",false,MODELS_SLOT + 3}
			});
		m_VertexArray->AddVertexBuffer(m_MatrixVBO);

	}



	void IndirectCullRenderer::CopyMeshBufferToIndirectBuffer(uint32_t vertexCnt, float* indirectBuffer, const std::pair<float*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, uint32_t itemCnt)
	{
		if (meshBuffer.first) {
			memcpy(indirectBuffer + vertexCnt * itemCnt, mesh->GetPositionBuffer().first, mesh->GetPositionBuffer().second);
		}
		else {
			//GE_CORE_WARN("no attirbute buffer found in mesh");
		}
	}

	void IndirectCullRenderer::DestructTransformMatrix(uint32_t objId, std::pair<float*, uint32_t> batchTransformMatrix)
	{
		uint32_t batchObjCnt = batchTransformMatrix.second / (12.0 * sizeof(float));
		for (size_t i = 0; i < batchObjCnt; i++)
		{
			memcpy(m_ObjsTransformV0Buffer + objId * 4, batchTransformMatrix.first + i * 12, sizeof(float) * 4);
			memcpy(m_ObjsTransformV1Buffer + objId * 4, batchTransformMatrix.first + i * 12 + 4, sizeof(float) * 4);
			memcpy(m_ObjsTransformV2Buffer + objId * 4, batchTransformMatrix.first + i * 12 + 8, sizeof(float) * 4);

		}
	}



	void IndirectCullRenderer::CopyMeshBufferToIndirectBuffer(uint32_t vertexCnt, uint32_t* indirectBuffer, const std::pair<uint32_t*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, uint32_t itemCnt)
	{

		if (meshBuffer.first) {
			memcpy(indirectBuffer + vertexCnt * itemCnt, meshBuffer.first, meshBuffer.second);
		}
		else {
			//GE_CORE_WARN("no attirbute buffer found in mesh");
		}

	}


	void IndirectCullRenderer::CreateAttributesBuffer()
	{
		//	std::shared_ptr<IndexBuffer> indexBuffer;
		//	indexBuffer.reset(DBG_NEW IndexBuffer(m_IndexBuffer, m_IndexCnt * sizeof(uint32_t)));
		//	m_VertexArray->SetIndexBuffer(indexBuffer);

		//	//如果每个vertexbuffer 一种attribute,那么 stride = 0, offset 也 = 0
		//	std::shared_ptr<VertexBuffer> vertexBuffer(DBG_NEW VertexBuffer(m_PositionBuffer, m_VertexCnt * sizeof(float) * 3));
		//	vertexBuffer->SetBufferLayout({ {ElementDataType::Float3, "aPos", false, POS_SLOT} });
		//	std::shared_ptr<VertexBuffer> normalBuffer(DBG_NEW VertexBuffer(m_NormalBuffer, m_VertexCnt * sizeof(float) * 3));
		//	normalBuffer->SetBufferLayout({ { ElementDataType::Float3,"aNormal",false,NORMAL_SLOT} });
		//	std::shared_ptr<VertexBuffer> texcoordsBuffer(DBG_NEW VertexBuffer(m_TexCordBuffer, m_VertexCnt * sizeof(float) * 2));
		//	texcoordsBuffer->SetBufferLayout({ { ElementDataType::Float2,"aTexCoords",false,TEXCOORD_SLOT} });
		//	std::shared_ptr<VertexBuffer> tangentBuffer(DBG_NEW VertexBuffer(m_TangentBuffer, m_VertexCnt * sizeof(float) * 3));
		//	tangentBuffer->SetBufferLayout({ { ElementDataType::Float3,"aTangent",false,TANGENT_SLOT} });
		//	std::shared_ptr<VertexBuffer> bitangentBuffer(DBG_NEW VertexBuffer(m_BitangentBuffer, m_VertexCnt * sizeof(float) * 3));
		//	bitangentBuffer->SetBufferLayout({ { ElementDataType::Float3,"aBitangent",false,BITANGENT_SLOT} });

		//	std::shared_ptr<VertexBuffer> jointBuffer(DBG_NEW VertexBuffer(m_JointIndicesBuffer, m_VertexCnt * sizeof(uint32_t) * 4));
		//	jointBuffer->SetBufferLayout({ { ElementDataType::Int4,"aJointIndices",false,JOINTINDICES_SLOT} });
		//	std::shared_ptr<VertexBuffer> joint1Buffer(DBG_NEW VertexBuffer(m_JointIndices1Buffer, m_VertexCnt * sizeof(uint32_t) * 4));
		//	joint1Buffer->SetBufferLayout({ { ElementDataType::Int4,"aJointIndices1",false,JOINTINDICES1_SLOT} });
		//	std::shared_ptr<VertexBuffer> weightBuffer(DBG_NEW VertexBuffer(m_WeightBuffer, m_VertexCnt * sizeof(float) * 4));
		//	weightBuffer->SetBufferLayout({ { ElementDataType::Float4,"aWeights",false,WEIGHT_SLOT} });
		//	std::shared_ptr<VertexBuffer> weight1Buffer(DBG_NEW VertexBuffer(m_Weight1Buffer, m_VertexCnt * sizeof(float) * 4));
		//	weight1Buffer->SetBufferLayout({ { ElementDataType::Float4,"aWeights1",false,WEIGHT1_SLOT} });

		//	std::shared_ptr<VertexBuffer> objIdBuffer(DBG_NEW VertexBuffer(m_ObjIdBuffer, m_VertexCnt * sizeof(float)));
		//	objIdBuffer->SetBufferLayout({ { ElementDataType::Float,"aObjId",false,OBJID_SLOT} });
		//	std::shared_ptr<VertexBuffer> colorBuffer(DBG_NEW VertexBuffer(m_ColorBuffer, m_VertexCnt * sizeof(float) * 3));
		//	colorBuffer->SetBufferLayout({ { ElementDataType::Float3,"aColor",false,COLOR_SLOT} });

		//	std::shared_ptr<VertexBuffer> matrixBuffer(DBG_NEW VertexBuffer(m_ObjsTransformBuffer, m_ObjsCnt * sizeof(float) * 16));
		//	matrixBuffer->SetBufferLayout({ { ElementDataType::Float4,"aModels",false,MODELS_SLOT},
		//		{ ElementDataType::Float4,"aModels",false,MODELS_SLOT+1} ,
		//		{ ElementDataType::Float4,"aModels",false,MODELS_SLOT+2} ,
		//		{ ElementDataType::Float4,"aModels",false,MODELS_SLOT+3} });

		//	/*std::shared_ptr<VertexBuffer> matrixV0Buffer(DBG_NEW VertexBuffer(m_ObjsTransformV0Buffer, m_ObjsCnt * sizeof(float) * 4));
		//	matrixV0Buffer->SetBufferLayout({ { ElementDataType::Float4,"aModelsVec0",false,MODELSV0_SLOT} });
		//	std::shared_ptr<VertexBuffer> matrixV1Buffer(DBG_NEW VertexBuffer(m_ObjsTransformV1Buffer, m_ObjsCnt * sizeof(float) * 4));
		//	matrixV1Buffer->SetBufferLayout({ { ElementDataType::Float4,"aModelsVec1",false,MODELSV1_SLOT} });
		//	std::shared_ptr<VertexBuffer> matrixV2Buffer(DBG_NEW VertexBuffer(m_ObjsTransformV2Buffer, m_ObjsCnt * sizeof(float) * 4));
		//	matrixV2Buffer->SetBufferLayout({ { ElementDataType::Float4,"aModelsVec2",false,MODELSV2_SLOT} });*/

		//	m_VertexArray->AddAttributeVertexBuffer(vertexBuffer);
		//	m_VertexArray->AddAttributeVertexBuffer(normalBuffer);
		//	m_VertexArray->AddAttributeVertexBuffer(texcoordsBuffer);
		//	//m_VertexArray->AddAttributeVertexBuffer(tangentBuffer);
		//	//m_VertexArray->AddAttributeVertexBuffer(bitangentBuffer);
		//	//m_VertexArray->AddAttributeVertexBuffer(jointBuffer);
		//	//m_VertexArray->AddAttributeVertexBuffer(joint1Buffer);
		//	//m_VertexArray->AddAttributeVertexBuffer(weightBuffer);
		//	//m_VertexArray->AddAttributeVertexBuffer(weight1Buffer);
		//	//m_VertexArray->AddAttributeVertexBuffer(colorBuffer);
		//	//m_VertexArray->AddAttributeVertexBuffer(objIdBuffer);
		//	/*m_VertexArray->AddVertexBuffer(matrixBuffer);
		//	glVertexAttribDivisor(MODELS_SLOT, 1);
		//	glVertexAttribDivisor(MODELS_SLOT+1, 1);
		//	glVertexAttribDivisor(MODELS_SLOT+2, 1);
		//	glVertexAttribDivisor(MODELS_SLOT+3, 1);*/

		///*	m_VertexArray->AddAttributeVertexBuffer(matrixV0Buffer);
		//	glVertexAttribDivisor(MODELSV0_SLOT, 1);

		//	m_VertexArray->AddAttributeVertexBuffer(matrixV1Buffer);
		//	glVertexAttribDivisor(MODELSV1_SLOT, 1);

		//	m_VertexArray->AddAttributeVertexBuffer(matrixV2Buffer);
		//	glVertexAttribDivisor(MODELSV2_SLOT, 1);*/


		//	m_VertexArray->UnBind();
	}

	void IndirectCullRenderer::PrintData() {
		GE_CORE_INFO("sizeof(IndirectVertex)={0},sizeof(IndirectVertex) / sizeof(float) ={1} ", sizeof(IndirectVertex), sizeof(IndirectVertex) / sizeof(float));

		for (size_t i = 0; i < m_IndexCnt; i++)
		{
			//	m_IndexBuffer[ i] = m_IndexBuffer[i];
			GE_CORE_INFO("i = {0}, m_IndexBuffer[{1}] = {2} ", i, i, m_IndexBuffer[i]);
		}


		for (size_t i = 0; i < m_VertexCnt; i++)
		{
			uint32_t base = i;
			GE_CORE_INFO("i = {0}, m_InterleaveVertexBuffer[{1}] = {2},{3},{4},{5},{6},{7},{8},{9} ", i, i,
				m_InterleaveVertexBuffer[i].pos[0],
				m_InterleaveVertexBuffer[i].pos[1],
				m_InterleaveVertexBuffer[i].pos[2],
				m_InterleaveVertexBuffer[i].normal[0],
				m_InterleaveVertexBuffer[i].normal[1],
				m_InterleaveVertexBuffer[i].normal[2],
				m_InterleaveVertexBuffer[i].texcoords[0],
				m_InterleaveVertexBuffer[i].texcoords[1]
			);

		}

	}


	//void IndirectCullRenderer::GernerateIndirectData()
	//{
	//	InitIndirectData();
	//	uint32_t baseVert = 0;
	//	uint32_t baseIndex = 0;
	//	uint32_t baseInstance = 0;

	//	for (auto node : m_Scene->GetBatchNodes())
	//	{
	//		// only support 1 instance batch for now.
	//		BatchNode* batchNode = dynamic_cast<BatchNode*>(node);
	//		//GE_ASSERT(batchNode->GetInstanceCnt() == 1, "only support 1 instance batch for now");
	//		memcpy(m_PositionBuffer + baseVert * 3, batchNode->GetBatch()->GetPositionBuffer().first, batchNode->GetBatch()->GetPositionBuffer().second);
	//		memcpy(m_NormalBuffer + baseVert * 3, batchNode->GetBatch()->GetNormalBuffer().first, batchNode->GetBatch()->GetNormalBuffer().second);
	//		memcpy(m_TexCordBuffer + baseVert * 2, batchNode->GetBatch()->GetTexCoordsBuffer().first, batchNode->GetBatch()->GetTexCoordsBuffer().second);
	//		memcpy(m_TangentBuffer + baseVert * 3, batchNode->GetBatch()->GetTangentBuffer().first, batchNode->GetBatch()->GetTangentBuffer().second);
	//		memcpy(m_BitangentBuffer + baseVert * 3, batchNode->GetBatch()->GetBitangentBuffer().first, batchNode->GetBatch()->GetBitangentBuffer().second);
	//		memcpy(m_JointIndicesBuffer + baseVert * 4, batchNode->GetBatch()->GetJiontBuffer().first, batchNode->GetBatch()->GetJiontBuffer().second);
	//		memcpy(m_JointIndices1Buffer + baseVert * 4, batchNode->GetBatch()->GetJiont1Buffer().first, batchNode->GetBatch()->GetJiont1Buffer().second);
	//		memcpy(m_WeightBuffer + baseVert * 4, batchNode->GetBatch()->GetWeightBuffer().first, batchNode->GetBatch()->GetWeightBuffer().second);
	//		memcpy(m_Weight1Buffer + baseVert * 4, batchNode->GetBatch()->GetWeight1Buffer().first, batchNode->GetBatch()->GetWeight1Buffer().second);
	//		memcpy(m_IndexBuffer + baseIndex, batchNode->GetBatch()->GetIndicesBuffer().first, batchNode->GetBatch()->GetIndicesBuffer().second);
	//		memcpy(m_ObjIdBuffer + baseVert, batchNode->GetBatch()->GetObjIdBuffer().first, batchNode->GetBatch()->GetObjIdBuffer().second);
	//		memcpy(m_ColorBuffer + baseVert * 3, batchNode->GetBatch()->GetColorBuffer().first, batchNode->GetBatch()->GetColorBuffer().second);

	//		//memcpy(m_ObjsTransformBuffer + baseInstance * 12, batchNode->GetBatch()->GetModelTransformBuffer().first, batchNode->GetBatch()->GetModelTransformBuffer().second);
	//		DestructTransformMatrix(baseInstance, batchNode->GetBatch()->GetModelTransformBuffer());

	//		baseVert += batchNode->GetVertexCount();
	//		baseIndex += batchNode->GetIndexCount();
	//		baseInstance += batchNode->GetObjCnt();
	//	}

	//	for (auto node : m_Scene->GetSingleNodes())
	//	{
	//		SingleNode* singleNode = dynamic_cast<SingleNode*>(node);
	//		auto& meshes = singleNode->GetObj()->GetComponent<MeshRenderer>()->GetMeshes();
	//	/*	uint32_t baseMeshVert = 0;
	//		uint32_t baseMeshIndex = 0;*/

	//		for (auto mesh: meshes)
	//		{
	//			uint32_t meshVetCnt = mesh->GetVertexCount();
	//			uint32_t meshIndexCnt = mesh->GetIndicesConut();

	//			CopyMeshBufferToIndirectBuffer(baseVert, m_PositionBuffer, mesh->GetPositionBuffer(), mesh, 3);
	//			CopyMeshBufferToIndirectBuffer(baseVert, m_NormalBuffer, mesh->GetNormalBuffer(), mesh, 3);
	//			CopyMeshBufferToIndirectBuffer(baseVert, m_TexCordBuffer, mesh->GetTexCoordsBuffer(), mesh, 2);
	//			CopyMeshBufferToIndirectBuffer(baseVert, m_TangentBuffer, mesh->GetTangentBuffer(), mesh, 3);
	//			CopyMeshBufferToIndirectBuffer(baseVert, m_BitangentBuffer, mesh->GetBitangentBuffer(), mesh, 3);
	//			CopyMeshBufferToIndirectBuffer(baseVert, m_JointIndicesBuffer, mesh->GetJiontBuffer(), mesh, 4);
	//			CopyMeshBufferToIndirectBuffer(baseVert, m_JointIndices1Buffer, mesh->GetJiont1Buffer(), mesh, 4);
	//			CopyMeshBufferToIndirectBuffer(baseVert, m_WeightBuffer, mesh->GetWeightBuffer(), mesh, 4);
	//			CopyMeshBufferToIndirectBuffer(baseVert, m_Weight1Buffer, mesh->GetWeight1Buffer(), mesh, 4);
	//	//		std::fill(m_ObjIdBuffer + baseVert, m_ObjIdBuffer + baseVert  + meshVetCnt, baseInstance);

	//			std::shared_ptr<Material> material = mesh->GetMaterial();
	//			MaterialColor::Color materialColor = material->GetMaterialColor().Get();
	//			float diffuse[3] = { materialColor.diffuseColor.r,materialColor.diffuseColor.g,materialColor.diffuseColor.b };
	//			uint32_t floatsize = sizeof(float);

	//			for (size_t i = 0; i < meshVetCnt; i++)
	//			{
	//				memcpy(m_ColorBuffer + baseVert * 3 + i * 3, diffuse, sizeof(float) * 3);
	//			}
	//			
	//			uint32_t* indexBuffer = mesh->GetIndicesBuffer().first;
	//			for (size_t i = 0; i < meshIndexCnt; i++)
	//			{
	//				m_IndexBuffer[baseIndex + i] = indexBuffer[i]  + baseVert;
	//				GE_CORE_INFO("i = {0}, m_IndexBuffer[{1}] = {2} ",i, baseIndex + i, m_IndexBuffer[baseVert + i]);
	//			}

	//			/*baseMeshVert += mesh->GetVertexCount();
	//			baseMeshIndex += mesh->GetIndicesConut();*/

	//			AddObjTransformToBuffer(baseInstance, singleNode->GetObj()->GetComponent<Transform>()->GetTransformMatrix());
	//			baseVert += mesh->GetVertexCount();
	//			baseIndex += mesh->GetIndicesConut();
	//			baseInstance += 1;

	//		}
	//	
	//		
	//	}
	//}
}


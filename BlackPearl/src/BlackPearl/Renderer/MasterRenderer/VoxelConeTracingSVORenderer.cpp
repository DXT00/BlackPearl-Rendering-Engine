#include "pch.h"
#include "VoxelConeTracingSVORenderer.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Renderer/MasterRenderer/GBufferRenderer.h"
#include "BlackPearl/Config.h"
namespace BlackPearl {
	bool  VoxelConeTracingSVORenderer::s_Shadows = true;
	bool  VoxelConeTracingSVORenderer::s_IndirectDiffuseLight = true;
	bool  VoxelConeTracingSVORenderer::s_IndirectSpecularLight = true;
	bool  VoxelConeTracingSVORenderer::s_DirectLight = true;
	float VoxelConeTracingSVORenderer::s_GICoeffs = 0.8f;
	bool  VoxelConeTracingSVORenderer::s_VoxelizeNow = true;
	bool  VoxelConeTracingSVORenderer::s_HDR = false;
	bool  VoxelConeTracingSVORenderer::s_GuassianHorizontal = false;
	bool  VoxelConeTracingSVORenderer::s_GuassianVertical = false;
	bool  VoxelConeTracingSVORenderer::s_ShowBlurArea = false;
	bool  VoxelConeTracingSVORenderer::s_MipmapBlurSpecularTracing = false;
	int   VoxelConeTracingSVORenderer::s_MaxBounce = 8;
	float VoxelConeTracingSVORenderer::s_SpecularBlurThreshold = 0.1f;
	int   VoxelConeTracingSVORenderer::s_VisualizeMipmapLevel = 0;
	bool  VoxelConeTracingSVORenderer::s_Pause = false;




	VoxelConeTracingSVORenderer::VoxelConeTracingSVORenderer()
	{

	}
	VoxelConeTracingSVORenderer::~VoxelConeTracingSVORenderer()
	{
		GE_SAVE_DELETE(m_CubeObj);
		GE_SAVE_DELETE(m_BrdfLUTQuadObj);
		GE_SAVE_DELETE(m_Sobol);
		
	}


	void VoxelConeTracingSVORenderer::Init(
		unsigned int viewportWidth, unsigned int viewportHeight,
		Object* cubeObj,
		Object* brdfLUTQuadObj,
		Object* quadFinalScreenObj,
		Object* quadPathTracing,
		std::vector<Object*> objs,
		Object* skybox)
	{
		GE_ERROR_JUDGE();
		GE_ASSERT(brdfLUTQuadObj, "m_BrdfLUTQuadObj is nullptr!");
		GE_ASSERT(cubeObj, "m_CubeObj is nullptr!");
		GE_ASSERT(quadFinalScreenObj, "m_QuadFinalScreenObj is nullptr");
		GE_ASSERT(quadPathTracing, "m_QuadPathTracing is nullptr");

		m_CubeObj = cubeObj;
		m_BrdfLUTQuadObj = brdfLUTQuadObj;
		m_QuadFinalScreenObj = quadFinalScreenObj;
		m_QuadPathTracing = quadPathTracing;
		glEnable(GL_MULTISAMPLE);
		GE_ERROR_JUDGE();

		InitVoxelization();
		GE_ERROR_JUDGE();

		InitVoxelVisualization(viewportWidth, viewportHeight);
		GE_ERROR_JUDGE();

		InitPathTracing();
	//	ErrorJudge(glGetError());

		/* Automic Count Buffer*/
		GLbitfield mapFlags = (GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
		m_AtomicCountBuffer.reset(DBG_NEW AtomicBuffer());
		GE_ERROR_JUDGE();

		/* SVO buildTree shader */
		m_NodeFlagShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxelSVO/buildTree/nodeFlag.glsl"));
		m_NodeAllocShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxelSVO/buildTree/nodeAlloc.glsl"));
		m_NodeInitShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxelSVO/buildTree/nodeInit.glsl"));
		m_LeafStoreShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxelSVO/buildTree/leafStore.glsl"));
		m_NodeRenderShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxelSVO/buildTree/renderNodeKd.glsl"));
		/* SVO tracing shader*/
		m_SVOTracingShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxelSVO/svoTracing_new.glsl"));
		BuildFragmentList(objs, skybox);
		BuildSVO();

		/* bedf LUT use for gBuffer path tracing */
		m_SpecularBRDFLutShader.reset(DBG_NEW Shader("assets/shaders/ibl/brdf.glsl"));

		RenderSpecularBRDFLUTMap();
		m_IsInitialize = true;
	}

	void VoxelConeTracingSVORenderer::InitVoxelization()
	{
		//m_VoxelizationShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxelSVO/voxelizationSVO_new.glsl"));
		m_VoxelizationShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxelSVO/voxelizationSVO.glsl"));

	}
	void VoxelConeTracingSVORenderer::InitVoxelVisualization(unsigned int viewportWidth, unsigned int viewportHeight)
	{
		//Shaders
		m_VoxelVisualizationShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxelSVO/renderVoxel.glsl"));
		m_PointCubeVAO.reset(DBG_NEW VertexArray());
		m_PointCubeVAO->UnBind();
	}

	void VoxelConeTracingSVORenderer::InitPathTracing()
	{
		/*SVO path tracer */
		GLbitfield map_flags = (GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
		m_SobolSSBO.reset(DBG_NEW ShaderStorageBuffer(sizeof(GLfloat) * 2 * s_MaxBounce,map_flags));
		GE_ERROR_JUDGE();

		m_Sobol = DBG_NEW Sobol(2 * s_MaxBounce);
		GE_ERROR_JUDGE();

		//m_SobolSSBO->Bind();
		GE_ERROR_JUDGE();

		m_SobolPtr = (GLfloat*)glMapNamedBufferRange(m_SobolSSBO->GetRenderID(), 0, sizeof(GLfloat) * 2 * s_MaxBounce, map_flags);
		GE_ERROR_JUDGE();

		m_Sobol->Next(m_SobolPtr);
		m_SobolSSBO->UnBind();
		GE_ERROR_JUDGE();
		m_PathTracingGBufferShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxelSVO/pathTracingGBuffer.glsl"));
		m_PathTracingShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxelSVO/pathTracing.glsl"));
		GE_ERROR_JUDGE();

		m_PathTracingColor.reset(DBG_NEW Texture(Texture::Type::None, m_ScreenWidth, m_ScreenHeight,false, GL_LINEAR, GL_LINEAR, GL_RGBA32F, GL_RGBA,GL_CLAMP_TO_EDGE,GL_FLOAT));
		GE_ERROR_JUDGE();

		m_PathTracingAlbedo.reset(DBG_NEW Texture(Texture::Type::None, m_ScreenWidth, m_ScreenHeight, false, GL_LINEAR, GL_LINEAR, GL_RGBA8, GL_RGBA, GL_CLAMP_TO_EDGE, GL_FLOAT));
		GE_ERROR_JUDGE();
		//
		m_PathTracingNormal.reset(DBG_NEW Texture(Texture::Type::None, m_ScreenWidth, m_ScreenHeight, false, GL_LINEAR, GL_LINEAR, GL_RGBA8_SNORM, GL_RGBA, GL_CLAMP_TO_EDGE, GL_FLOAT));
		GE_ERROR_JUDGE();
		/* init GBuffer for deffer rendering */
		m_GBuffer.reset(DBG_NEW GBuffer(m_ScreenWidth, m_ScreenHeight));
		m_GBufferShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxelSVO/gBufferVoxel.glsl"));
		GE_ERROR_JUDGE();

	}


	void VoxelConeTracingSVORenderer::Voxelize(const std::vector<Object*>& objs, Object* skybox, bool storeData)
	{
		glViewport(0, 0, m_VoxelTextureSize, m_VoxelTextureSize);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		GE_ERROR_JUDGE();

		//m_DebugOctreeBufTexture.reset(DBG_NEW BufferTexture(sizeof(GLuint)*10, GL_RGBA8UI, 0));

		m_VoxelizationShader->Bind();
		GE_ERROR_JUDGE();

		m_VoxelizationShader->SetUniform1i("u_StoreData", false);
		m_VoxelizationShader->SetUniform1i("u_VoxelSize", m_VoxelTextureSize);
		//	m_AtomicCountBuffer->Bind();
		GE_ERROR_JUDGE();
		Configuration::SyncGPU();
		m_AtomicCountBuffer->BindIndex(3);
		GE_ERROR_JUDGE();
		if (storeData) {

			glBindImageTexture(0, m_VoxelPosBufTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGB10_A2UI);
			glBindImageTexture(1, m_VoxelDiffuseBufTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
			glBindImageTexture(2, m_VoxelNormBufTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
			//m_VoxelizationShader->SetUniform1i("u_voxelPos", 0);
			m_VoxelizationShader->SetUniform1i("u_StoreData", true);
		}
	

		for (auto obj : objs) {
			//m_VoxelizationShader->Bind();
			
			
			m_VoxelizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());

			m_VoxelizationShader->SetUniform1i("u_IsSkybox", false);
			if (obj->GetComponent<MeshRenderer>()->GetIsPBRObject()) {
				m_VoxelizationShader->SetUniform1i("u_IsPBRObjects", 1);
			}
			else {
				m_VoxelizationShader->SetUniform1i("u_IsPBRObjects", 0);
			}
			//glBindImageTexture(4, m_DebugOctreeBufTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);
			GE_ERROR_JUDGE();

			DrawObject(obj, m_VoxelizationShader);
			GE_ERROR_JUDGE();

		}
		GE_ERROR_JUDGE();

		if (skybox != nullptr) {

			//m_VoxelizationShader->Bind();
			m_VoxelizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());
			skybox->GetComponent<Transform>()->SetScale(m_CubeObj->GetComponent<Transform>()->GetScale() - glm::vec3(2.0f));
			skybox->GetComponent<Transform>()->SetPosition({ 0,0,0 });
			m_VoxelizationShader->SetUniform1i("u_IsSkybox", true);
			m_VoxelizationShader->SetUniform1i("u_IsPBRObjects", 0);
			DrawObject(skybox, m_VoxelizationShader);
		}
		//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
		//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
		//ShowBufferTexture(m_DebugOctreeBufTexture, 10);
		GE_ERROR_JUDGE();

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_BLEND);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		GE_ERROR_JUDGE();

	}

	void VoxelConeTracingSVORenderer::BuildFragmentList(const std::vector<Object*>& objs, Object* skybox)
	{
		//统计fragment数量
		
		Voxelize(objs, skybox, false);
		glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
		GLenum err =  glGetError();
		//ErrorJudge(err);
		//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT| GL_SHADER_STORAGE_BARRIER_BIT);
		m_AtomicCountBuffer->Bind();
		Configuration::SyncGPU();

		//m_AtomicCountBuffer->BindIndex(3);

		//glMapBufferRange() --map all or part of a buffer object's data store into the client's address space
		//GL_MAP_READ_BIT indicates that the returned pointer may be used to read buffer object data.
		//GL_MAP_WRITE_BIT indicates that the returned pointer may be used to modify buffer object data.
		GLuint* count = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
		err = glGetError();
		//ErrorJudge(err);

		m_NumVoxelFrag =  count[0];
		GE_CORE_INFO("voxel fragment list size : " + std::to_string(m_NumVoxelFrag));
		//Create buffers for voxel fragment list
		m_VoxelPosBufTexture.reset(DBG_NEW BufferTexture(sizeof(GLuint) * m_NumVoxelFrag, GL_R32UI, 0));
		m_VoxelDiffuseBufTexture.reset(DBG_NEW BufferTexture(sizeof(GLuint) * m_NumVoxelFrag, GL_RGBA8, 0));//sizeof(GLuint) = 4
		m_VoxelNormBufTexture.reset(DBG_NEW BufferTexture(sizeof(GLuint) * 2 * m_NumVoxelFrag, GL_RGBA16F, 0));

		//reset counter memery
		memset(count, 0, sizeof(GLuint));
		//glUnmapBuffer() -- release the mapping of a buffer object's data store into the client's address space
		glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
		m_AtomicCountBuffer->UnBind();

		//存储fragmetn数据
		Voxelize(objs, skybox, true);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		

		//ShowBufferTexture(m_VoxelPosBufTexture, m_NumVoxelFrag);
		//ShowBufferTexture(m_VoxelDiffuseBufTexture, m_NumVoxelFrag);

	}

	void VoxelConeTracingSVORenderer::BuildSVO()
	{
		GLuint allocCounter = 0;
		std::vector<unsigned int> allocList;
		allocList.push_back(1);

		int nTmp = 1;
		for (int i = 0; i <= m_OctreeLevel; i++)
		{
			m_TotalTreeNode += nTmp;
			nTmp *= 8;
		}
		//store octree child index
		m_OctreeNodeTex[0].reset(DBG_NEW BufferTexture(sizeof(GLuint) * m_TotalTreeNode, GL_R32UI, 0));
		//store octree diffuse color
		m_OctreeNodeTex[1].reset(DBG_NEW BufferTexture(sizeof(GLuint) * m_TotalTreeNode, GL_R32UI, 0));

		//m_DebugOctreeBufTexture.reset(DBG_NEW BufferTexture(sizeof(GLuint) * m_TotalTreeNode, GL_RG16UI, 0));
		//m_AtomicCountBuffer.reset(DBG_NEW AtomicBuffer());
		GLbitfield mapFlags = (GL_MAP_COHERENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
		m_AtomicCountBuffer.reset(DBG_NEW AtomicBuffer());
	//	m_AtomicCountBuffer->ResetValue(0);
		int nodeOffset = 0;
		int allcOffset = 1;
		int dataWidth = 1024;
		int dataHeight = (m_NumVoxelFrag + 1023) / dataWidth;
		int groupDimX = dataWidth / 8;
		int groupDimY = (dataHeight + 7) / 8;
		for (int i = 0; i < m_OctreeLevel; i++)
		{
			//flag node
			m_NodeFlagShader->Bind();
			m_NodeFlagShader->SetUniform1i("u_level", i);
			m_NodeFlagShader->SetUniform1i("u_maxLevel", m_OctreeLevel);

			m_NodeFlagShader->SetUniform1i("u_numVoxelFrag", m_NumVoxelFrag);
			m_NodeFlagShader->SetUniform1i("u_voxelDim", m_VoxelTextureSize);
			glBindImageTexture(0, m_VoxelPosBufTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGB10_A2UI);
			glBindImageTexture(1, m_OctreeNodeTex[0]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
			//glBindImageTexture(2, m_DebugOctreeBufTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG16UI);

			glDispatchCompute(groupDimX, groupDimY, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

			//ShowBufferTexture(m_OctreeNodeTex[0], m_TotalTreeNode);
			//ShowBufferTexture(m_DebugOctreeBufTexture, m_TotalTreeNode);



			//allocation node
			m_NodeAllocShader->Bind();
			int numThread = allocList[i];
			m_NodeAllocShader->SetUniform1i("u_NodeStart", nodeOffset);
			m_NodeAllocShader->SetUniform1i("u_AllocStart", allcOffset);
			m_NodeAllocShader->SetUniform1i("u_ThreadNum", numThread);
			glBindImageTexture(0, m_OctreeNodeTex[0]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
			m_AtomicCountBuffer->BindIndex(0);
			int allocGroupDimX = (allocList[i] + 63) / 64;
			//int allocGroupDimX = (allocList[i] + 127) / 128;

			glDispatchCompute(allocGroupDimX, 1, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
			//ShowBufferTexture(m_OctreeNodeTex[0], m_TotalTreeNode);

			//initial node
			unsigned int allocNodeNum;
			unsigned int reset = 0;
			m_AtomicCountBuffer->Bind();
			glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &allocNodeNum);
			glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &reset);//reset counter to zero
			m_AtomicCountBuffer->UnBind();

			int allocNodeNumTotal = 8 * allocNodeNum;
			m_NodeInitShader->Bind();
			m_NodeInitShader->SetUniform1i("u_ThreadNum", allocNodeNumTotal);
			m_NodeInitShader->SetUniform1i("u_AllocStart", allcOffset);
			glBindImageTexture(0, m_OctreeNodeTex[0]->GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI);
			glBindImageTexture(1, m_OctreeNodeTex[1]->GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI);



			int dataWidth = 1024;
			int dataHeight = (allocNodeNumTotal + 1023) / 1024;

			int initGroupDimX = dataWidth / 8;
			int initGroupDimY = (dataHeight + 7) / 8;
			glDispatchCompute(initGroupDimX, initGroupDimY, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


			//ShowBufferTexture(m_OctreeNodeTex[0], m_TotalTreeNode);

			// update offsets for next level
			allocList.push_back(allocNodeNumTotal); //titleAllocated * 8 is the number of threads
												  //we want to launch in the next level
			nodeOffset += allocList[i]; //nodeOffset is the starting node in the next level
			allcOffset += allocNodeNumTotal; //allocOffset is the starting address of remaining free space


		}
		GE_CORE_INFO("Total nodes consumed: {0}", allcOffset);
		
		//flag nonempty leaf node
		m_NodeFlagShader->Bind();
		m_NodeFlagShader->SetUniform1i("u_level", m_OctreeLevel);
		m_NodeFlagShader->SetUniform1i("u_maxLevel", m_OctreeLevel);
		m_NodeFlagShader->SetUniform1i("u_numVoxelFrag", m_NumVoxelFrag);
		m_NodeFlagShader->SetUniform1i("u_voxelDim", m_VoxelTextureSize);
		glBindImageTexture(0, m_VoxelPosBufTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGB10_A2UI);
		glBindImageTexture(1, m_OctreeNodeTex[0]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glDispatchCompute(groupDimX, groupDimY, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		//Store surface information ( Color, normal, etc. ) into the octree leaf nodes
		m_LeafStoreShader->Bind();
		m_LeafStoreShader->SetUniform1i("u_numVoxelFrag", m_NumVoxelFrag);
		m_LeafStoreShader->SetUniform1i("u_level", m_OctreeLevel);
		m_LeafStoreShader->SetUniform1i("u_voxelDim", m_VoxelTextureSize);
		glBindImageTexture(0, m_OctreeNodeTex[0]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glBindImageTexture(1, m_OctreeNodeTex[1]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glBindImageTexture(2, m_VoxelPosBufTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGB10_A2UI);
		glBindImageTexture(3, m_VoxelDiffuseBufTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
		glDispatchCompute(groupDimX, groupDimY, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		//calculate every node's diffuse color of the tree


		for (int i = m_OctreeLevel;i>=1;i--)
		{
			m_NodeRenderShader->Bind();
			m_NodeRenderShader->SetUniform1i("u_numVoxelFrag", m_NumVoxelFrag);
			m_NodeRenderShader->SetUniform1i("u_level", i);
			m_NodeRenderShader->SetUniform1i("u_voxelDim", m_VoxelTextureSize);
			glBindImageTexture(0, m_OctreeNodeTex[0]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
			glBindImageTexture(1, m_OctreeNodeTex[1]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
			glBindImageTexture(2, m_VoxelPosBufTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGB10_A2UI);
			GE_ERROR_JUDGE();

			//glBindImageTexture(3, m_VoxelDiffuseBufTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
			glDispatchCompute(groupDimX, groupDimY, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			GE_ERROR_JUDGE();

		}
		ShowBufferTexture(m_OctreeNodeTex[0], m_TotalTreeNode);
		ShowBufferTexture(m_OctreeNodeTex[1], m_TotalTreeNode);

		


	}

	void VoxelConeTracingSVORenderer::RebuildSVO(const std::vector<Object*>& objs, Object* skybox)
	{
		//m_AtomicCountBuffer.reset(DBG_NEW AtomicBuffer());
		//SyncGPU();
	//	m_AtomicCountBuffer->ResetValue(0);
		BuildFragmentList(objs, skybox);
		BuildSVO();

	}

	void VoxelConeTracingSVORenderer::RenderSpecularBRDFLUTMap()
	{
		m_SpecularBrdfLUTTexture.reset(DBG_NEW Texture(Texture::DiffuseMap, m_VoxelTextureSize, m_VoxelTextureSize, false, GL_LINEAR, GL_LINEAR, GL_RG16F, GL_RG, GL_CLAMP_TO_EDGE, GL_FLOAT));
		//std::shared_ptr<Texture> brdfLUTTexture(new Texture(Texture::None, 512, 512, GL_LINEAR, GL_LINEAR, GL_RG16F, GL_RG, GL_CLAMP_TO_EDGE, GL_FLOAT));
		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());
		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		frameBuffer->Bind();
		frameBuffer->AttachRenderBuffer(m_VoxelTextureSize, m_VoxelTextureSize);

		//m_FrameBuffer->Bind();
		//m_FrameBuffer->BindRenderBuffer();
		frameBuffer->AttachColorTexture(m_SpecularBrdfLUTTexture, 0);
		frameBuffer->BindRenderBuffer();
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);


		glViewport(0, 0, m_VoxelTextureSize, m_VoxelTextureSize);
		m_SpecularBRDFLutShader->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawObject(m_BrdfLUTQuadObj, m_SpecularBRDFLutShader);
		frameBuffer->UnBind();
		frameBuffer->CleanUp();

	}






	
	void VoxelConeTracingSVORenderer::Render(Camera* camera, const std::vector<Object*>& objs, const LightSources* lightSources,
		unsigned int viewportWidth, unsigned int viewportHeight, Object* skybox, RenderingMode reneringMode)
	{
		GE_ASSERT(m_IsInitialize, "Please Call VoxelConeTracingSVORenderer::Init() first!");
		
		//bool voxelizeNow = m_VoxelizationQueued || (m_AutomaticallyVoxelize &&m_VoxelizationSparsity > 0 && ++m_TicksSinceLastVoxelization >= m_VoxelizationSparsity);
		//RenderGBuffer(objs, skybox);
	/*	if (s_VoxelizeNow) {
			Voxelize(objs, skybox, true);
			m_TicksSinceLastVoxelization = 0;
			m_VoxelizationQueued = false;
		}*/

		switch (reneringMode) {
		case RenderingMode::VOXELIZATION_VISUALIZATION:
			RenderVoxelVisualization(camera, objs, viewportWidth, viewportHeight);
			break;
		case RenderingMode::VOXEL_CONE_TRACING:
			RenderScene(objs, lightSources, viewportWidth, viewportHeight, skybox);
			break;
		case RenderingMode::SVO_PATH_TRACING:
			PathTracingGBuffer(objs,skybox,lightSources, viewportWidth, viewportHeight);
			//PathTracing(objs, skybox, lightSources, viewportWidth, viewportHeight);

			break;
		}
	}

	void VoxelConeTracingSVORenderer::RenderVoxelVisualization(Camera* camera, const std::vector<Object*>& objs, unsigned int viewportWidth, unsigned int viewportHeight)
	{
		/* 在本函数里头完成渲染 */
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		GE_ERROR_JUDGE();
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		GE_ERROR_JUDGE();
		ShowBufferTexture(m_OctreeNodeTex[0], m_TotalTreeNode);
		ShowBufferTexture(m_OctreeNodeTex[1], m_TotalTreeNode);

		glm::mat4 viewMatrix = Renderer::GetSceneData()->ViewMatrix;
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(viewMatrix));
		glm::mat4 cubeScale = glm::scale(glm::mat4(1.0), m_CubeObj->GetComponent<Transform>()->GetScale());
		glm::mat4 projMatrix = Renderer::GetSceneData()->ProjectionMatrix;
		m_VoxelVisualizationShader->Bind();
		m_VoxelVisualizationShader->SetUniformMat4f("u_ModelView", viewMatrix);
		m_VoxelVisualizationShader->SetUniformMat4f("u_Model", cubeScale);
		GE_ERROR_JUDGE();
		m_VoxelVisualizationShader->SetUniformMat4f("u_Proj", projMatrix);
		m_VoxelVisualizationShader->SetUniformMat4f("u_Normal", normalMatrix);
		GE_ERROR_JUDGE();
		m_VoxelVisualizationShader->SetUniform1i("u_voxelDim", m_VoxelTextureSize);
		float halfDim = 1.0f / m_VoxelTextureSize;
		m_VoxelVisualizationShader->SetUniform1f("u_halfDim", halfDim);
		m_VoxelVisualizationShader->SetUniform1i("u_octreeLevel", m_OctreeLevel);
		GE_ERROR_JUDGE();
		glBindImageTexture(0, m_OctreeNodeTex[0]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		glBindImageTexture(1, m_OctreeNodeTex[1]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		GE_ERROR_JUDGE();
		m_PointCubeVAO->Bind();
		glDrawArrays(GL_POINTS, 0, m_VoxelTextureSize * m_VoxelTextureSize * m_VoxelTextureSize);
		glBindTexture(GL_TEXTURE_3D, 0);
		GE_ERROR_JUDGE();
		glBindVertexArray(0);
		m_VoxelVisualizationShader->Unbind();
		GE_ERROR_JUDGE();
		
	}

	void VoxelConeTracingSVORenderer::RenderScene(const std::vector<Object*>& objs, const LightSources* lightSources,
		unsigned int viewportWidth, unsigned int viewportHeight, Object* skybox)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);
		//glDisable(GL_CULL_FACE);
		//m_DebugOctreeBufTexture.reset(DBG_NEW BufferTexture(sizeof(GLuint) * m_ScreenWidth*m_ScreenHeight, GL_RG16UI, 0));


		m_SVOTracingShader->Bind();
		m_SVOTracingShader->SetUniform1i("u_ViewType", 0);
		m_SVOTracingShader->SetUniform1i("u_ScreenWidth", m_ScreenWidth);
		m_SVOTracingShader->SetUniform1i("u_ScreenHeight", m_ScreenHeight);

		glBindImageTexture(0, m_OctreeNodeTex[0]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		glBindImageTexture(1, m_OctreeNodeTex[1]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		//glBindImageTexture(2, m_DebugOctreeBufTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);

		DrawObject(m_QuadFinalScreenObj, m_SVOTracingShader);

		//ShowBufferTexture(m_DebugOctreeBufTexture, m_ScreenWidth * m_ScreenHeight);

	}

	void VoxelConeTracingSVORenderer::PathTracingGBuffer(std::vector<Object*> objs,Object* skybox,const LightSources* lightSources, unsigned int viewportWidth, unsigned int viewportHeight)
	{
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderGBuffer(objs, skybox);
		//DrawGBuffer(m_QuadFinalScreenObj);
		glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);
		//m_DebugOctreeBufTexture.reset(DBG_NEW BufferTexture(sizeof(GLuint) * m_ScreenWidth*m_ScreenHeight, GL_RGBA8UI, 0));

		m_PathTracingGBufferShader->Bind();
		m_PathTracingGBufferShader->SetUniform1i("u_SPP",s_Pause ? -1 : (m_SPP++));
		m_PathTracingGBufferShader->SetUniform1i("u_Bounce", s_MaxBounce);
		m_PathTracingGBufferShader->SetUniform1i("u_ViewType",0);
		m_PathTracingGBufferShader->SetUniform1i("u_ScreenWidth", m_ScreenWidth);
		m_PathTracingGBufferShader->SetUniform1i("u_ScreenHeight", m_ScreenHeight);
		m_PathTracingGBufferShader->SetUniform1i("u_octreeLevel", m_OctreeLevel);
		m_PathTracingGBufferShader->SetUniform1i("u_VoxelSize", m_VoxelTextureSize);

		m_PathTracingGBufferShader->SetUniformVec3f("u_SunRadiance", m_SunRadiance);
		m_PathTracingGBufferShader->SetUniform1i("u_Settings.directLight", s_DirectLight);
		m_PathTracingGBufferShader->SetUniform1i("u_Settings.indirectDiffuseLight", s_IndirectDiffuseLight);

		//m_SobolSSBO->Bind();
		//m_Sobol->Next(m_SobolPtr);

		//m_SobolSSBO->BindIndex(5);
		//m_PathTracingColor->Bind();
		//glBindImageTexture(0, m_PathTracingColor->GetRendererID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//m_PathTracingAlbedo->Bind();
		//glBindImageTexture(1, m_PathTracingAlbedo->GetRendererID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
		//m_PathTracingNormal->Bind();
		//glBindImageTexture(2, m_PathTracingNormal->GetRendererID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8_SNORM);
		glBindImageTexture(3, m_OctreeNodeTex[0]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		glBindImageTexture(4, m_OctreeNodeTex[1]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		//glBindImageTexture(6, m_DebugOctreeBufTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);
		
		m_PathTracingGBufferShader->SetUniform1i("u_BrdfLUTMap", 14);
		m_PathTracingGBufferShader->SetUniform1i("gPosition", 8);
		m_PathTracingGBufferShader->SetUniform1i("gNormal", 9);
		m_PathTracingGBufferShader->SetUniform1i("gDiffuse_Roughness", 10);
		m_PathTracingGBufferShader->SetUniform1i("gSpecular_Mentallic", 11);
		m_PathTracingGBufferShader->SetUniform1i("gAmbientGI_AO", 12);
		m_PathTracingGBufferShader->SetUniform1i("gNormalMap", 13);

		m_PathTracingGBufferShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());
		
		glActiveTexture(GL_TEXTURE14);
		m_SpecularBrdfLUTTexture->Bind();
		glActiveTexture(GL_TEXTURE8);
		m_GBuffer->GetPositionTexture()->Bind();
		glActiveTexture(GL_TEXTURE9);
		m_GBuffer->GetNormalTexture()->Bind();
		glActiveTexture(GL_TEXTURE10);
		m_GBuffer->GetDiffuseRoughnessTexture()->Bind();
		glActiveTexture(GL_TEXTURE11);
		m_GBuffer->GetSpecularMentallicTexture()->Bind();
		glActiveTexture(GL_TEXTURE12);
		m_GBuffer->GetAmbientGIAOTexture()->Bind();
		glActiveTexture(GL_TEXTURE13);
		m_GBuffer->GetNormalMapTexture()->Bind();

		DrawObject(m_QuadPathTracing, m_PathTracingGBufferShader);
		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	//	ShowBufferTexture(m_DebugOctreeBufTexture, m_ScreenWidth * m_ScreenHeight);

	}
	void VoxelConeTracingSVORenderer::PathTracing(std::vector<Object*> objs, Object* skybox, const LightSources* lightSources, unsigned int viewportWidth, unsigned int viewportHeight)
	{
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//RenderGBuffer(objs, skybox);
		//	DrawGBuffer(m_QuadFinalScreenObj);
		glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);
		//m_DebugOctreeBufTexture.reset(DBG_NEW BufferTexture(sizeof(GLuint) * m_ScreenWidth*m_ScreenHeight, GL_RGBA8UI, 0));

		m_PathTracingShader->Bind();
		m_PathTracingShader->SetUniform1i("u_SPP", s_Pause ? -1 : (m_SPP++));
		m_PathTracingShader->SetUniform1i("u_Bounce", s_MaxBounce);
		m_PathTracingShader->SetUniform1i("u_ViewType", 0);
		m_PathTracingShader->SetUniform1i("u_ScreenWidth", m_ScreenWidth);
		m_PathTracingShader->SetUniform1i("u_ScreenHeight", m_ScreenHeight);
		m_PathTracingShader->SetUniform1i("u_octreeLevel", m_OctreeLevel);
		//m_PathTracingShader->SetUniform1i("u_voxelDim", m_VoxelTextureSize);

		m_PathTracingShader->SetUniformVec3f("u_SunRadiance", m_SunRadiance);
		m_PathTracingShader->SetUniform1i("u_Settings.directLight", s_DirectLight);

		m_SobolSSBO->Bind();
		m_SobolSSBO->BindIndex(5);
		m_PathTracingColor->Bind();
		glBindImageTexture(0, m_PathTracingColor->GetRendererID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		m_PathTracingAlbedo->Bind();
		glBindImageTexture(1, m_PathTracingAlbedo->GetRendererID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
		m_PathTracingNormal->Bind();
		glBindImageTexture(2, m_PathTracingNormal->GetRendererID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8_SNORM);
		glBindImageTexture(3, m_OctreeNodeTex[0]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		glBindImageTexture(4, m_OctreeNodeTex[1]->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
		//glBindImageTexture(6, m_DebugOctreeBufTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);


		/*m_PathTracingShader->SetUniform1i("gPosition", 8);
		m_PathTracingShader->SetUniform1i("gNormal", 9);
		m_PathTracingShader->SetUniform1i("gDiffuse_Roughness", 10);
		m_PathTracingShader->SetUniform1i("gSpecular_Mentallic", 11);
		m_PathTracingShader->SetUniform1i("gAmbientGI_AO", 12);
		m_PathTracingShader->SetUniform1i("gNormalMap", 13);*/

		//m_PathTracingShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());

		/*glActiveTexture(GL_TEXTURE8);
		m_GBuffer->GetPositionTexture()->Bind();
		glActiveTexture(GL_TEXTURE9);
		m_GBuffer->GetNormalTexture()->Bind();
		glActiveTexture(GL_TEXTURE10);
		m_GBuffer->GetDiffuseRoughnessTexture()->Bind();
		glActiveTexture(GL_TEXTURE11);
		m_GBuffer->GetSpecularMentallicTexture()->Bind();
		glActiveTexture(GL_TEXTURE12);
		m_GBuffer->GetAmbientGIAOTexture()->Bind();
		glActiveTexture(GL_TEXTURE13);
		m_GBuffer->GetNormalMapTexture()->Bind();*/

		DrawObject(m_QuadPathTracing, m_PathTracingShader);
		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			//ShowBufferTexture(m_OctreeNodeTex[1], m_TotalTreeNode);

	}

	void VoxelConeTracingSVORenderer::RenderGBuffer(const std::vector<Object*>& objs, Object* skybox)
	{
		glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);
		m_GBuffer->Bind();
		GE_ERROR_JUDGE();

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (Object* obj : objs) {

			m_GBufferShader->Bind();
			m_GBufferShader->SetUniform1i("u_IsSkybox", false);
			GE_ASSERT(obj->GetId().index() < 256, "GBuffer store max object number is:256");
			m_GBufferShader->SetUniform1i("u_ObjectId", obj->GetId().index());
			GE_ERROR_JUDGE();

			DrawObject(obj, m_GBufferShader);
			GE_ERROR_JUDGE();

		}

		m_GBuffer->UnBind();
	}

	void VoxelConeTracingSVORenderer::ShowBufferTexture(std::shared_ptr<BufferTexture> bufferTexture, int dataLength)
	{
		std::vector<GLuint> pos(dataLength, 0);
		bufferTexture->Bind();

		//m_VoxelPosBufTexture->BindTexture();

		GLuint* data = (GLuint*)glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);//存储顺序是 A_BGR
		for (int j = 0; j < dataLength; ++j)
			pos[j] = data[j];		
		data = nullptr;
		glUnmapBuffer(GL_TEXTURE_BUFFER);
		bufferTexture->Unbind();
	}

	void VoxelConeTracingSVORenderer::DrawGBuffer(Object* gBufferDebugQuad)
	{

		glViewport(0, 0, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetPositionTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(0, 270, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetNormalTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(320, 0, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetDiffuseRoughnessTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(320, 270, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetSpecularMentallicTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(640, 0, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetAmbientGIAOTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(640, 270, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetNormalMapTexture());
		DrawObject(gBufferDebugQuad);

	}


}
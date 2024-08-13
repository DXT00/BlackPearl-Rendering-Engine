#include "pch.h"
#include "WaterRenderer.h"
#include "BlackPearl/Renderer/Buffer/Buffer.h"
#include "BlackPearl/Math/Math.h"
#include <random>
#ifdef GE_PLATFORM_ANDRIOD
#include "GLES3/gl32.h"
#endif
namespace BlackPearl {
	WaterRenderer::WaterRenderer()
	{
	}
	WaterRenderer::~WaterRenderer()
	{
	}
	void WaterRenderer::Init(Scene* scene)
	{
		const int vertexCnt = mGridDim + 1;
		std::vector<GridVertex> vertices(vertexCnt * vertexCnt);
		//�ܹ�mGridDim * mGridDim��������Σ�ÿ��������6��index
		std::vector<uint32_t> indices(mGridDim * mGridDim * 2 * 3);

		unsigned int idx = 0;
		float tex_coord_scale = 2.f;

		for (int z = -mGridDim/2; z <= mGridDim/2; z++)
		{
			for (int x = -mGridDim/2; x <= mGridDim/2; x++)
			{
				vertices[idx].position = glm::vec3(float(x), 0, float(z));
				float u = float(x) / mGridDim + 0.5;
				float v = float(z) / mGridDim + 0.5;
				vertices[idx].texcoords = glm::vec2(u, v) * tex_coord_scale;
                idx++;
			}
		}
		idx = 0;
		for (int y = 0; y < mGridDim; y++)
		{
			for (int x = 0; x < mGridDim; x++)
			{
				indices[idx++] = (vertexCnt * y) + x;
				indices[idx++] = (vertexCnt * (y + 1)) + x;
				indices[idx++] = (vertexCnt * y) + x + 1;

				indices[idx++] = (vertexCnt * y) + x + 1;
				indices[idx++] = (vertexCnt * (y + 1)) + x;
				indices[idx++] = (vertexCnt * (y + 1)) + x + 1;

			}

		}
		VertexBufferLayout layout = {
			{ElementDataType::Float3,"aPos",false,0},
			{ElementDataType::Float2,"aTexCoords",false,1}
		};
		std::shared_ptr<VertexBuffer> vertexBuffer(DBG_NEW VertexBuffer(vertices.data(), vertices.size() * sizeof(GridVertex)));
		std::shared_ptr<IndexBuffer> indexBuffer(DBG_NEW IndexBuffer(indices.data(), indices.size() * sizeof(uint32_t)));
		m_VAO = std::make_shared<VertexArray>();
		vertexBuffer->SetBufferLayout(layout);
		m_VAO->AddVertexBuffer(vertexBuffer);
        m_VAO->SetIndexBuffer(indexBuffer);

		m_CSInitialSpectrumShader.reset(DBG_NEW Shader("assets/shaders/ocean/cs_InitialSpectrum.glsl"));
		m_CSPhaseShader.reset(DBG_NEW Shader("assets/shaders/ocean/cs_Phase.glsl"));
		m_CSSpectrumShader.reset(DBG_NEW Shader("assets/shaders/ocean/cs_Spectrum.glsl"));
		m_CSFFTHorizontalShader.reset(DBG_NEW Shader("assets/shaders/ocean/cs_FFTHorizontal.glsl"));
		m_CSFFTVerticalShader.reset(DBG_NEW Shader("assets/shaders/ocean/cs_FFTVertical.glsl"));
		m_CSNormalMapShader.reset(DBG_NEW Shader("assets/shaders/ocean/cs_NormalMap.glsl"));

        m_OceanShader.reset(DBG_NEW Shader("assets/shaders/ocean/Ocean.glsl"));

		// Initial spectrum
		m_InitialSpectrumTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, mTextureResolution, mTextureResolution, false, GL_NEAREST, GL_NEAREST, GL_R32F, GL_RED, -1, GL_FLOAT));
		
		//Phase
		std::vector<float> ping_phase_array(mTextureResolution * mTextureResolution);

		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_real_distribution<float> dist(0.f, 1.f);

		for (size_t i = 0; i < mTextureResolution * mTextureResolution; ++i)
			ping_phase_array[i] = dist(rng) * 2.f * Math::PI;

		m_PingPhaseTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, mTextureResolution, mTextureResolution, false, GL_NEAREST, GL_NEAREST, GL_R32F, GL_RED, GL_CLAMP_TO_BORDER, GL_FLOAT,false, ping_phase_array.data()));
		m_PongPhaseTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, mTextureResolution, mTextureResolution, false, GL_NEAREST, GL_NEAREST, GL_R32F, GL_RED, GL_CLAMP_TO_BORDER, GL_FLOAT));
		// Time-varying spectrum

		m_SpectrumTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, mTextureResolution, mTextureResolution, false, GL_LINEAR, GL_LINEAR, GL_RGBA32F, GL_RGBA, GL_REPEAT, GL_FLOAT));
		m_TempTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, mTextureResolution, mTextureResolution, false, GL_NEAREST, GL_NEAREST, GL_RGBA32F, GL_RGBA, GL_CLAMP_TO_BORDER, GL_FLOAT));

		// Normal map
		m_NormalMapTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, mTextureResolution, mTextureResolution, false, GL_LINEAR, GL_LINEAR, GL_RGBA32F, GL_RGBA, GL_REPEAT, GL_FLOAT));

		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glClearColor(0.674f, 0.966f, 0.988f, 1.f);

		//m_CSInitialSpectrumShader->Bind();
		//m_CSInitialSpectrumShader->SetUniform1i("u_ocean_size", mGridDim);
		//m_CSInitialSpectrumShader->SetUniform1i("u_resolution", (int)mTextureResolution);

		//float wind_angle_rad = glm::radians(m_wind_angle);
		//m_CSInitialSpectrumShader->SetUniformVec2f("u_wind",glm::vec2(m_wind_magnitude * glm::cos(wind_angle_rad), m_wind_magnitude * glm::sin(wind_angle_rad)));
		//glBindImageTexture(0, m_InitialSpectrumTexture->GetRendererID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

		////->BindImage(0, GL_WRITE_ONLY, initial_spectrum_texture->internal_format);

		//glDispatchCompute(mTextureResolution / mWorkGroupDim, mTextureResolution / mWorkGroupDim, 1);
		//glFinish();

	}
	void WaterRenderer::Render(Timestep ts, Camera* camera)
	{
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glClearColor(0.674f, 0.966f, 0.988f, 1.f);
        m_CSInitialSpectrumShader->Bind();
        m_CSInitialSpectrumShader->SetUniform1i("u_ocean_size", mGridDim);
        m_CSInitialSpectrumShader->SetUniform1i("u_resolution", (int)mTextureResolution);

        float wind_angle_rad = glm::radians(m_wind_angle);
        m_CSInitialSpectrumShader->SetUniformVec2f("u_wind", glm::vec2(m_wind_magnitude * glm::cos(wind_angle_rad), m_wind_magnitude * glm::sin(wind_angle_rad)));
        glBindImageTexture(0, m_InitialSpectrumTexture->GetRendererID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

        //->BindImage(0, GL_WRITE_ONLY, initial_spectrum_texture->internal_format);

        glDispatchCompute(mTextureResolution / mWorkGroupDim, mTextureResolution / mWorkGroupDim, 1);
        glFinish();
        m_CSPhaseShader->Bind();
        m_CSPhaseShader->SetUniform1i("u_ocean_size", mGridDim);
        m_CSPhaseShader->SetUniform1f("u_delta_time", ts);
        m_CSPhaseShader->SetUniform1i("u_resolution", mTextureResolution);

        if (m_is_ping_phase)
        {
            glBindImageTexture(0, m_PingPhaseTexture->GetRendererID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
            glBindImageTexture(1, m_PongPhaseTexture->GetRendererID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

            /*ping_phase_texture->BindImage(0, GL_READ_ONLY, ping_phase_texture->internal_format);
            pong_phase_texture->BindImage(1, GL_WRITE_ONLY, pong_phase_texture->internal_format);*/
        }
        else
        {
            glBindImageTexture(1, m_PingPhaseTexture->GetRendererID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
            glBindImageTexture(0, m_PongPhaseTexture->GetRendererID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
            //ping_phase_texture->BindImage(1, GL_WRITE_ONLY, ping_phase_texture->internal_format);
            //pong_phase_texture->BindImage(0, GL_READ_ONLY, pong_phase_texture->internal_format);
        }

        glDispatchCompute(mTextureResolution / mWorkGroupDim, mTextureResolution / mWorkGroupDim, 1);
        glFinish();

        m_CSSpectrumShader->Bind();

        m_CSSpectrumShader->SetUniform1i("u_ocean_size", mGridDim);
        m_CSSpectrumShader->SetUniform1f("u_choppiness", m_choppiness);

        //m_is_ping_phase ? pong_phase_texture->BindImage(0, GL_READ_ONLY, pong_phase_texture->internal_format)
        //    : ping_phase_texture->BindImage(0, GL_READ_ONLY, ping_phase_texture->internal_format);
        //initial_spectrum_texture->BindImage(1, GL_READ_ONLY, initial_spectrum_texture->internal_format);

        m_is_ping_phase ? glBindImageTexture(0, m_PongPhaseTexture->GetRendererID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F)
            : glBindImageTexture(0, m_PingPhaseTexture->GetRendererID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);


        glBindImageTexture(1, m_InitialSpectrumTexture->GetRendererID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);

        //initial_spectrum_texture->BindImage(1, GL_READ_ONLY, initial_spectrum_texture->internal_format);

        glBindImageTexture(2, m_SpectrumTexture->GetRendererID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        //spectrum_texture->BindImage(2, GL_WRITE_ONLY, spectrum_texture->internal_format);

        glDispatchCompute(mTextureResolution / mWorkGroupDim, mTextureResolution / mWorkGroupDim, 1);
        glFinish();

        // Rows
        m_CSFFTHorizontalShader->Bind();
        m_CSFFTHorizontalShader->SetUniform1i("u_total_count", mTextureResolution);

        bool temp_as_input = false; // Should you use temp_texture as input to the FFT shader?

        for (int p = 1; p < mTextureResolution; p <<= 1)
        {
            if (temp_as_input)
            {
                glBindImageTexture(0, m_TempTexture->GetRendererID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
                glBindImageTexture(1, m_SpectrumTexture->GetRendererID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

              /*  temp_texture->BindImage(0, GL_READ_ONLY, temp_texture->internal_format);
                spectrum_texture->BindImage(1, GL_WRITE_ONLY, spectrum_texture->internal_format);*/
            }
            else
            {
                glBindImageTexture(0, m_SpectrumTexture->GetRendererID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
                glBindImageTexture(1, m_TempTexture->GetRendererID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
                /*spectrum_texture->BindImage(0, GL_READ_ONLY, spectrum_texture->internal_format);
                temp_texture->BindImage(1, GL_WRITE_ONLY, temp_texture->internal_format);*/
            }

            m_CSFFTHorizontalShader->SetUniform1i("u_subseq_count", p);

            // One work group per row
            glDispatchCompute(mTextureResolution, 1, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

            temp_as_input = !temp_as_input;
        }

        // Cols
        m_CSFFTVerticalShader->Bind();
        m_CSFFTVerticalShader->SetUniform1i("u_total_count", mTextureResolution);

        for (int p = 1; p < mTextureResolution; p <<= 1)
        {
            if (temp_as_input)
            {
                glBindImageTexture(0, m_TempTexture->GetRendererID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
                glBindImageTexture(1, m_SpectrumTexture->GetRendererID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        /*        temp_texture->BindImage(0, GL_READ_ONLY, temp_texture->internal_format);
                spectrum_texture->BindImage(1, GL_WRITE_ONLY, spectrum_texture->internal_format);*/
            }
            else
            {
                glBindImageTexture(0, m_SpectrumTexture->GetRendererID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
                glBindImageTexture(1, m_TempTexture->GetRendererID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
                /*spectrum_texture->BindImage(0, GL_READ_ONLY, spectrum_texture->internal_format);
                temp_texture->BindImage(1, GL_WRITE_ONLY, temp_texture->internal_format);*/
            }

            m_CSFFTVerticalShader->SetUniform1i("u_subseq_count", p);

            // One work group per col
            glDispatchCompute(mTextureResolution, 1, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

            temp_as_input = !temp_as_input;
        }

        // Generate Normal Map

        m_CSNormalMapShader->Bind();
        glBindImageTexture(0, m_SpectrumTexture->GetRendererID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, m_NormalMapTexture->GetRendererID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

        //spectrum_texture->BindImage(0, GL_READ_ONLY, spectrum_texture->internal_format);
        //normal_map->BindImage(1, GL_WRITE_ONLY, normal_map->internal_format);

        m_CSNormalMapShader->SetUniform1i("u_resolution", mTextureResolution);
        m_CSNormalMapShader->SetUniform1i("u_ocean_size", mGridDim);

        glDispatchCompute(mTextureResolution / mWorkGroupDim, mTextureResolution / mWorkGroupDim, 1);
        glFinish();

        // Ocean Shading

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_OceanShader->Bind();
        m_OceanShader->SetUniformMat4f("u_pv", camera->GetProjectionViewMatrix());
        m_OceanShader->SetUniformVec3f("u_world_camera_pos", camera->GetPosition());
        m_OceanShader->SetUniform1i("u_ocean_size", mGridDim);
        m_OceanShader->SetUniform1i("u_wireframe", m_wireframe_mode ? 1 : 0);

        float sun_elevation_rad = glm::radians((float)sun_elevation);
        float sun_azimuth_rad = glm::radians((float)sun_azimuth);
        m_OceanShader->SetUniformVec3f("u_sun_direction", glm::vec3(-glm::cos(sun_elevation_rad) * glm::cos(sun_azimuth_rad), -glm::sin(sun_elevation_rad),
            -glm::cos(sun_elevation_rad) * glm::sin(sun_azimuth_rad)));

        m_OceanShader->SetUniform1i("u_displacement_map", 0);

        glActiveTexture(GL_TEXTURE0);
        temp_as_input ? m_TempTexture->Bind() : m_SpectrumTexture->Bind(); // Todo: Make it clear that here we are binding disp map

        m_OceanShader->SetUniform1i("u_normal_map", 1);
        glActiveTexture(GL_TEXTURE1);
        m_NormalMapTexture->Bind();

        m_is_ping_phase = !m_is_ping_phase;

        m_VAO->Bind();
        m_VAO->GetIndexBuffer()->Bind();
        /*grid_vao->Bind();
        grid_ibo->Bind();*/
#ifdef  GE_PLATFORM_WINDOWS
        if (m_wireframe_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
        glDrawElements(GL_TRIANGLES, mGridDim * mGridDim * 2 * 3, GL_UNSIGNED_INT, 0);
	}
}

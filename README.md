### LightProbes -- Use Spherical Harmonics Coefficients to render diffuse probes--Multiple probes Blending


#### 1.Blending Multiple probes SH coefficients

iblSHTexture.glsl:

```
uniform vec3 u_SHCoeffs[10*9];//最多10个probe

vec3 SHDiffuse(const int probeIndex,const vec3 normal){
	float x = normal.x;
	float y = normal.y;
	float z = normal.z;

	vec3 result = (
		u_SHCoeffs[0+probeIndex*9] +
		
		u_SHCoeffs[1+probeIndex*9] * x +
		u_SHCoeffs[2+probeIndex*9] * y +
		u_SHCoeffs[3+probeIndex*9] * z +
		
		u_SHCoeffs[4+probeIndex*9] * z * x +
		u_SHCoeffs[5+probeIndex*9] * y * z +
		u_SHCoeffs[6+probeIndex*9] * y * x +
		u_SHCoeffs[7+probeIndex*9] * (3.0 * z * z - 1.0) +
		u_SHCoeffs[8+probeIndex*9] * (x*x - y*y)
  );

  return max(result, vec3(0.0));




}

//....
	for(int i=0;i<u_Kprobes;i++){
		environmentIrradiance+=u_ProbeWeight[i]*SHDiffuse(i,N);// 
	}
```

#### 2.Update 1 probe per frame

为防止卡顿，不需要每帧更新所有的probe,每帧只更新m_KperFrame个probe

并把environmentMap分辨率改为128

```

void IBLProbesRenderer::Render(const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes, Object* skyBox)
	{
		GE_ASSERT(m_IsInitial, "please initial IBLProbesRenderer first! IBLProbesRenderer::init()");
		/*只渲染一次即可*/
		if (!m_IsRenderSpecularBRDFLUTMap) {
			RenderSpecularBRDFLUTMap();
			m_IsRenderSpecularBRDFLUTMap = true;
		}
		
				
		if (!m_UpdateFinished) {
			//int id = m_CurrentProbeIndex;
			for (int i = 0; i < m_KperFrame; i++)
			{
				UpdateProbesMaps(lightSources, objects, skyBox, probes[m_CurrentProbeIndex++]);
				if (m_CurrentProbeIndex >= probes.size()) {
					m_UpdateFinished = true;
					m_CurrentProbeIndex = 0;
					break;
				}

			}

		}


		
		

	}
	
```

```
		unsigned int					m_SampleCounts = 1024;
		unsigned int					m_EnvironmentCubeMapResolution = 128;
		unsigned int					m_DiffuseCubeMapResolution = 32;
		unsigned int					m_SpecularCubeMapResolution = 128;

```

![SHdiffuselightProbesBlending](/results/SHdiffuselightProbesBlending3.png)
![SHdiffuselightProbesBlending](/results/SHdiffuselightProbesBlending4.png)




#include "pch.h"
#ifdef GE_API_D3D12
#include "D3D12MeshShaderRenderer.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/Common/CommonFunc.h"
#include "BlackPearl/Debugger/D3D12Debugger/HLSLPixDebugger.h"

namespace BlackPearl {

	const wchar_t* D3D12MeshShaderRenderer::c_meshShaderFilename = L"MS.cso";
	const wchar_t* D3D12MeshShaderRenderer::c_pixelShaderFilename = L"PS.cso";


	inline HRESULT ReadDataFromFile(LPCWSTR filename, byte** data, UINT* size)
	{
		using namespace Microsoft::WRL;

#if WINVER >= _WIN32_WINNT_WIN8
		CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {};
		extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
		extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
		extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
		extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
		extendedParams.lpSecurityAttributes = nullptr;
		extendedParams.hTemplateFile = nullptr;

		Wrappers::FileHandle file(CreateFile2(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &extendedParams));
#else
		Wrappers::FileHandle file(CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS, nullptr));
#endif
		if (file.Get() == INVALID_HANDLE_VALUE)
		{
			throw std::exception();
		}

		FILE_STANDARD_INFO fileInfo = {};
		if (!GetFileInformationByHandleEx(file.Get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
		{
			throw std::exception();
		}

		if (fileInfo.EndOfFile.HighPart != 0)
		{
			throw std::exception();
		}

		*data = reinterpret_cast<byte*>(malloc(fileInfo.EndOfFile.LowPart));
		*size = fileInfo.EndOfFile.LowPart;

		if (!ReadFile(file.Get(), *data, fileInfo.EndOfFile.LowPart, nullptr, nullptr))
		{
			throw std::exception();
		}

		return S_OK;
	}

	D3D12MeshShaderRenderer::D3D12MeshShaderRenderer()
		: D3D12Renderer(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight())
		, m_Viewport(0.0f, 0.0f, static_cast<float>(m_Width), static_cast<float>(m_Height))
		, m_ScissorRect(0, 0, static_cast<LONG>(m_Width), static_cast<LONG>(m_Height))
		, m_RtvDescriptorSize(0)
		, m_ConstantBufferData{}
		, m_CbvDataBegin(nullptr)
		, m_FrameIndex(0)
		, m_FrameCounter(0)
		, m_FenceEvent{}
		, m_FenceValues{}
	{
		m_BasicShaderPath = L"assets/shaders_hlsl_cso/";

	}
	D3D12MeshShaderRenderer::~D3D12MeshShaderRenderer()
	{
	}
	void D3D12MeshShaderRenderer::OnDeviceLost()
	{
	}
	void D3D12MeshShaderRenderer::OnDeviceRestored()
	{
	}
	std::wstring D3D12MeshShaderRenderer::GetAssetFullPath(LPCWSTR assetName)
	{
		return m_BasicShaderPath + assetName;
	}

	void D3D12MeshShaderRenderer::MoveToNextFrame()
	{
		const UINT64 curFenceValue = m_FenceValues[m_FrameIndex];
		//��command queue��ĩβ����һ����fence value��ΪcurFenceValue������
		ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), curFenceValue));

		// Update the frame index.
		m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

		if (m_Fence->GetCompletedValue() < m_FenceValues[m_FrameIndex])
		{
			//���С��backbuffer frame��fence��˵����֡�ı��������û��ȫ�����
			//��ô���ǿ�������һ���ﵽ��Ĵ����¼�����fence value����
			// m_FenceValues[m_FrameIndex]��������ǲ�ֹͣCPU�˵�����
			ThrowIfFailed(m_Fence->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_FenceEvent));
			//�ȴ�һֱ��fence value�ﵽ mCurrentFence
			WaitForSingleObject(m_FenceEvent, INFINITE);
		}
		m_FenceValues[m_FrameIndex] = curFenceValue + 1;
	}

	void D3D12MeshShaderRenderer::Init(Scene* scene)
	{
		if (GetModuleHandle(L"WinPixGpuCapturer.dll") == 0)
		{
			LoadLibrary(HLSLPixDebugger::GetLatestWinPixGpuCapturerPath_Cpp17().c_str());
		}

		LoadPipeline();
		LoadAssets();
		LoadScene(scene);

		// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.
		WaitForGpu();
	}
	// Wait for pending GPU work to complete.
	void D3D12MeshShaderRenderer::WaitForGpu()
	{
		// Schedule a Signal command in the queue.
		ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), m_FenceValues[m_FrameIndex]));

		// Wait until the fence has been processed.
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_FenceEvent));
		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

		// Increment the fence value for the current frame.
		m_FenceValues[m_FrameIndex]++;
	}

	void D3D12MeshShaderRenderer::LoadPipeline()
	{
		UINT dxgiFactoryFlags = 0;

		ComPtr<IDXGIFactory4> factory;
		ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

		ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);
		D3D12CreateDevice(hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_Device)
		);
		PreCheck();
		m_CommandQueue = CreateCommandQueue(m_Device);

		m_SwapChain = CreateSwapChan(factory, m_CommandQueue, m_FrameCount, m_Width, m_Height);
		m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

		CreateRenderTargetView();
		CreateDepthStencilView();
		CreateConstantBufferView();
		CreateFence();

	}

	void D3D12MeshShaderRenderer::LoadAssets()
	{
		// Create the pipeline state, which includes compiling and loading shaders.
		{
			struct
			{
				byte* data;
				uint32_t size;
			} meshShader, pixelShader;

			ReadDataFromFile(GetAssetFullPath(c_meshShaderFilename).c_str(), &meshShader.data, &meshShader.size);
			ReadDataFromFile(GetAssetFullPath(c_pixelShaderFilename).c_str(), &pixelShader.data, &pixelShader.size);

			// Pull root signature from the precompiled mesh shader.
			ThrowIfFailed(m_Device->CreateRootSignature(0, meshShader.data, meshShader.size, IID_PPV_ARGS(&m_RootSignature)));

			D3DX12_MESH_SHADER_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = m_RootSignature.Get();
			psoDesc.MS = { meshShader.data, meshShader.size };
			psoDesc.PS = { pixelShader.data, pixelShader.size };
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = m_RenderTargets[0]->GetDesc().Format;
			psoDesc.DSVFormat = m_DepthStencil->GetDesc().Format;
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);    // CW front; cull back
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);         // Opaque
			psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // Less-equal depth test w/ writes; no stencil
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.SampleDesc = DefaultSampleDesc();

			auto psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(psoDesc);

			D3D12_PIPELINE_STATE_STREAM_DESC streamDesc;
			streamDesc.pPipelineStateSubobjectStream = &psoStream;
			streamDesc.SizeInBytes = sizeof(psoStream);

			ThrowIfFailed(m_Device->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&m_PipelineState)));
		}

		// Create the command list.
		ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocators[m_FrameIndex].Get(), m_PipelineState.Get(), IID_PPV_ARGS(&m_CommandList)));

		// Command lists are created in the recording state, but there is nothing
		// to record yet. The main loop expects it to be closed, so close it now.
		ThrowIfFailed(m_CommandList->Close());

	}

	void D3D12MeshShaderRenderer::UploadObj(Object* obj)
	{
		if (obj->GetComponent<BasicInfo>()->GetType() == OT_Model) {
			UploadModel(obj);
		}
	}

	void D3D12MeshShaderRenderer::UploadModel(Object* model)
	{
		std::vector<std::shared_ptr<Mesh>>& meshes = model->GetComponent<MeshRenderer>()->GetModel()->m_Meshes;
		for (uint32_t i = 0; i < meshes.size(); ++i)
		{
			auto& m = meshes[i];
			// Create committed D3D resources of proper sizes
			auto indexDesc = CD3DX12_RESOURCE_DESC::Buffer(m->Indices_ml.size());
			auto meshletDesc = CD3DX12_RESOURCE_DESC::Buffer(m->Meshlets.size() * sizeof(m->Meshlets[0]));
			auto cullDataDesc = CD3DX12_RESOURCE_DESC::Buffer(m->CullingData.size() * sizeof(m->CullingData[0]));
			auto vertexIndexDesc = CD3DX12_RESOURCE_DESC::Buffer(DivRoundUp(m->UniqueVertexIndices.size(), 4) * 4);

			auto primitiveDesc = CD3DX12_RESOURCE_DESC::Buffer(m->PrimitiveIndices.size() * sizeof(m->PrimitiveIndices[0]));
			auto meshInfoDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(MeshInfo));

			auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			ThrowIfFailed(m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &indexDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m->IndexResource)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &meshletDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m->MeshletResource)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &cullDataDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m->CullDataResource)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &vertexIndexDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m->UniqueVertexIndexResource)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &primitiveDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m->PrimitiveIndexResource)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &meshInfoDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m->MeshInfoResource)));

			m->VertexResources.resize(m->Vertices_ml.size());

			for (uint32_t j = 0; j < m->Vertices_ml.size(); ++j)
			{
				auto vertexDesc = CD3DX12_RESOURCE_DESC::Buffer(m->Vertices_ml[j].size());
				m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &vertexDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m->VertexResources[j]));


			}

			// Create upload resources
			std::vector<ComPtr<ID3D12Resource>> vertexUploads;
			ComPtr<ID3D12Resource>              indexUpload;
			ComPtr<ID3D12Resource>              meshletUpload;
			ComPtr<ID3D12Resource>              cullDataUpload;
			ComPtr<ID3D12Resource>              uniqueVertexIndexUpload;
			ComPtr<ID3D12Resource>              primitiveIndexUpload;
			ComPtr<ID3D12Resource>              meshInfoUpload;

			auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			ThrowIfFailed(m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &indexDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indexUpload)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &meshletDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&meshletUpload)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &cullDataDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&cullDataUpload)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &vertexIndexDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uniqueVertexIndexUpload)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &primitiveDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&primitiveIndexUpload)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &meshInfoDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&meshInfoUpload)));

			// Map & copy memory to upload heap
			vertexUploads.resize(m->Vertices_ml.size());
			for (uint32_t j = 0; j < m->Vertices_ml.size(); ++j)
			{
				auto vertexDesc = CD3DX12_RESOURCE_DESC::Buffer(m->Vertices_ml[j].size());
				ThrowIfFailed(m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &vertexDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexUploads[j])));

				uint8_t* memory = nullptr;
				vertexUploads[j]->Map(0, nullptr, reinterpret_cast<void**>(&memory));
				std::memcpy(memory, m->Vertices_ml[j].data(), m->Vertices_ml[j].size());
				vertexUploads[j]->Unmap(0, nullptr);
			}

			{
				uint8_t* memory = nullptr;
				indexUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
				std::memcpy(memory, m->Indices_ml.data(), m->Indices_ml.size());
				indexUpload->Unmap(0, nullptr);
			}

			{
				uint8_t* memory = nullptr;
				meshletUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
				std::memcpy(memory, m->Meshlets.data(), m->Meshlets.size() * sizeof(m->Meshlets[0]));
				meshletUpload->Unmap(0, nullptr);
			}

			{
				uint8_t* memory = nullptr;
				cullDataUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
				std::memcpy(memory, m->CullingData.data(), m->CullingData.size() * sizeof(m->CullingData[0]));
				cullDataUpload->Unmap(0, nullptr);
			}

			{
				uint8_t* memory = nullptr;
				uniqueVertexIndexUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
				std::memcpy(memory, m->UniqueVertexIndices.data(), m->UniqueVertexIndices.size());
				uniqueVertexIndexUpload->Unmap(0, nullptr);
			}

			{
				uint8_t* memory = nullptr;
				primitiveIndexUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
				std::memcpy(memory, m->PrimitiveIndices.data(), m->PrimitiveIndices.size() * sizeof(m->PrimitiveIndices[0]));
				primitiveIndexUpload->Unmap(0, nullptr);
			}

			{
				MeshInfo info = {};
				info.IndexSize = m->IndexSize_ml;
				info.MeshletCount = static_cast<uint32_t>(m->Meshlets.size());
				info.LastMeshletVertCount = m->Meshlets.back().VertCount;
				info.LastMeshletPrimCount = m->Meshlets.back().PrimCount;


				uint8_t* memory = nullptr;
				meshInfoUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
				std::memcpy(memory, &info, sizeof(MeshInfo));
				meshInfoUpload->Unmap(0, nullptr);
			}

			// Populate our command list
			m_CommandList->Reset(m_CommandAllocators[m_FrameIndex].Get(), nullptr);

			for (uint32_t j = 0; j < m->Vertices_ml.size(); ++j)
			{
				m_CommandList->CopyResource(m->VertexResources[j].Get(), vertexUploads[j].Get());
				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m->VertexResources[j].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			D3D12_RESOURCE_BARRIER postCopyBarriers[6];

			m_CommandList->CopyResource(m->IndexResource.Get(), indexUpload.Get());
			postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(m->IndexResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			m_CommandList->CopyResource(m->MeshletResource.Get(), meshletUpload.Get());
			postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m->MeshletResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			m_CommandList->CopyResource(m->CullDataResource.Get(), cullDataUpload.Get());
			postCopyBarriers[2] = CD3DX12_RESOURCE_BARRIER::Transition(m->CullDataResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			m_CommandList->CopyResource(m->UniqueVertexIndexResource.Get(), uniqueVertexIndexUpload.Get());
			postCopyBarriers[3] = CD3DX12_RESOURCE_BARRIER::Transition(m->UniqueVertexIndexResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			m_CommandList->CopyResource(m->PrimitiveIndexResource.Get(), primitiveIndexUpload.Get());
			postCopyBarriers[4] = CD3DX12_RESOURCE_BARRIER::Transition(m->PrimitiveIndexResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			m_CommandList->CopyResource(m->MeshInfoResource.Get(), meshInfoUpload.Get());
			postCopyBarriers[5] = CD3DX12_RESOURCE_BARRIER::Transition(m->MeshInfoResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

			m_CommandList->ResourceBarrier(ARRAYSIZE(postCopyBarriers), postCopyBarriers);

			ThrowIfFailed(m_CommandList->Close());

			ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
			m_CommandQueue->ExecuteCommandLists(1, ppCommandLists);

			// Create our sync fence
			ComPtr<ID3D12Fence> fence;
			ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

			m_CommandQueue->Signal(fence.Get(), 1);

			// Wait for GPU
			if (fence->GetCompletedValue() != 1)
			{
				HANDLE event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
				fence->SetEventOnCompletion(1, event);

				WaitForSingleObjectEx(event, INFINITE, false);
				CloseHandle(event);
			}
		}
	}

	void D3D12MeshShaderRenderer::UploadModel(Model* model)
	{
		// std::vector<Mesh>& meshes = model->GetComponent<MeshRenderer>()->GetModel()->m_Meshes;
		for (uint32_t i = 0; i < model->m_Meshes.size(); ++i)
		{
			auto& m = model->m_Meshes[i];
			// Create committed D3D resources of proper sizes
			auto indexDesc = CD3DX12_RESOURCE_DESC::Buffer(m->Indices_ml.size());
			auto meshletDesc = CD3DX12_RESOURCE_DESC::Buffer(m->Meshlets.size() * sizeof(m->Meshlets[0]));
			auto cullDataDesc = CD3DX12_RESOURCE_DESC::Buffer(m->CullingData.size() * sizeof(m->CullingData[0]));
			auto vertexIndexDesc = CD3DX12_RESOURCE_DESC::Buffer(DivRoundUp(m->UniqueVertexIndices.size(), 4) * 4);
			auto primitiveDesc = CD3DX12_RESOURCE_DESC::Buffer(m->PrimitiveIndices.size() * sizeof(m->PrimitiveIndices[0]));
			auto meshInfoDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(MeshInfo));

			auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			ThrowIfFailed(m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &indexDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m->IndexResource)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &meshletDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m->MeshletResource)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &cullDataDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m->CullDataResource)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &vertexIndexDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m->UniqueVertexIndexResource)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &primitiveDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m->PrimitiveIndexResource)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &meshInfoDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m->MeshInfoResource)));

			m->VertexResources.resize(m->Vertices_ml.size());

			for (uint32_t j = 0; j < m->Vertices_ml.size(); ++j)
			{
				auto vertexDesc = CD3DX12_RESOURCE_DESC::Buffer(m->Vertices_ml[j].size());
				m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &vertexDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m->VertexResources[j]));


			}

			// Create upload resources
			std::vector<ComPtr<ID3D12Resource>> vertexUploads;
			ComPtr<ID3D12Resource>              indexUpload;
			ComPtr<ID3D12Resource>              meshletUpload;
			ComPtr<ID3D12Resource>              cullDataUpload;
			ComPtr<ID3D12Resource>              uniqueVertexIndexUpload;
			ComPtr<ID3D12Resource>              primitiveIndexUpload;
			ComPtr<ID3D12Resource>              meshInfoUpload;

			auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			ThrowIfFailed(m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &indexDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indexUpload)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &meshletDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&meshletUpload)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &cullDataDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&cullDataUpload)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &vertexIndexDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uniqueVertexIndexUpload)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &primitiveDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&primitiveIndexUpload)));
			ThrowIfFailed(m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &meshInfoDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&meshInfoUpload)));

			// Map & copy memory to upload heap
			vertexUploads.resize(m->Vertices_ml.size());
			for (uint32_t j = 0; j < m->Vertices_ml.size(); ++j)
			{
				auto vertexDesc = CD3DX12_RESOURCE_DESC::Buffer(m->Vertices_ml[j].size());
				ThrowIfFailed(m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &vertexDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexUploads[j])));

				uint8_t* memory = nullptr;
				vertexUploads[j]->Map(0, nullptr, reinterpret_cast<void**>(&memory));
				std::memcpy(memory, m->Vertices_ml[j].data(), m->Vertices_ml[j].size());
				vertexUploads[j]->Unmap(0, nullptr);
			}

			{
				uint8_t* memory = nullptr;
				indexUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
				std::memcpy(memory, m->Indices_ml.data(), m->Indices_ml.size());
				indexUpload->Unmap(0, nullptr);
			}


			{
				uint8_t* memory = nullptr;
				meshletUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
				std::memcpy(memory, m->Meshlets.data(), m->Meshlets.size() * sizeof(m->Meshlets[0]));
				meshletUpload->Unmap(0, nullptr);
			}

			{
				uint8_t* memory = nullptr;
				cullDataUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
				std::memcpy(memory, m->CullingData.data(), m->CullingData.size() * sizeof(m->CullingData[0]));
				cullDataUpload->Unmap(0, nullptr);
			}

			{
				uint8_t* memory = nullptr;
				uniqueVertexIndexUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
				std::memcpy(memory, m->UniqueVertexIndices.data(), m->UniqueVertexIndices.size());
				uniqueVertexIndexUpload->Unmap(0, nullptr);
			}

			{
				uint8_t* memory = nullptr;
				primitiveIndexUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
				std::memcpy(memory, m->PrimitiveIndices.data(), m->PrimitiveIndices.size() * sizeof(m->PrimitiveIndices[0]));
				primitiveIndexUpload->Unmap(0, nullptr);
			}

			{
				MeshInfo info = {};
				info.IndexSize = m->IndexSize_ml;
				info.MeshletCount = static_cast<uint32_t>(m->Meshlets.size());
				info.LastMeshletVertCount = m->Meshlets.back().VertCount;
				info.LastMeshletPrimCount = m->Meshlets.back().PrimCount;


				uint8_t* memory = nullptr;
				meshInfoUpload->Map(0, nullptr, reinterpret_cast<void**>(&memory));
				std::memcpy(memory, &info, sizeof(MeshInfo));
				meshInfoUpload->Unmap(0, nullptr);
			}

			// Populate our command list
			m_CommandList->Reset(m_CommandAllocators[m_FrameIndex].Get(), nullptr);

			for (uint32_t j = 0; j < m->Vertices_ml.size(); ++j)
			{
				m_CommandList->CopyResource(m->VertexResources[j].Get(), vertexUploads[j].Get());
				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m->VertexResources[j].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			D3D12_RESOURCE_BARRIER postCopyBarriers[6];

			m_CommandList->CopyResource(m->IndexResource.Get(), indexUpload.Get());
			postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(m->IndexResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			m_CommandList->CopyResource(m->MeshletResource.Get(), meshletUpload.Get());
			postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m->MeshletResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			m_CommandList->CopyResource(m->CullDataResource.Get(), cullDataUpload.Get());
			postCopyBarriers[2] = CD3DX12_RESOURCE_BARRIER::Transition(m->CullDataResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			m_CommandList->CopyResource(m->UniqueVertexIndexResource.Get(), uniqueVertexIndexUpload.Get());
			postCopyBarriers[3] = CD3DX12_RESOURCE_BARRIER::Transition(m->UniqueVertexIndexResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			m_CommandList->CopyResource(m->PrimitiveIndexResource.Get(), primitiveIndexUpload.Get());
			postCopyBarriers[4] = CD3DX12_RESOURCE_BARRIER::Transition(m->PrimitiveIndexResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			m_CommandList->CopyResource(m->MeshInfoResource.Get(), meshInfoUpload.Get());
			postCopyBarriers[5] = CD3DX12_RESOURCE_BARRIER::Transition(m->MeshInfoResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

			m_CommandList->ResourceBarrier(ARRAYSIZE(postCopyBarriers), postCopyBarriers);

			ThrowIfFailed(m_CommandList->Close());

			ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
			m_CommandQueue->ExecuteCommandLists(1, ppCommandLists);

			// Create our sync fence
			ComPtr<ID3D12Fence> fence;
			ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

			m_CommandQueue->Signal(fence.Get(), 1);

			// Wait for GPU
			if (fence->GetCompletedValue() != 1)
			{
				HANDLE event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
				fence->SetEventOnCompletion(1, event);

				WaitForSingleObjectEx(event, INFINITE, false);
				CloseHandle(event);
			}
		}
	}

	void D3D12MeshShaderRenderer::LoadScene(Scene* scene)
	{
		m_Scene = scene;
		//for (auto& obj : m_Scene->GetObjects())
		//{
		//    UploadObj(obj);
		//}
		for (auto& obj : m_Scene->GetModels())
		{
			UploadModel(obj);
		}
	}

	void D3D12MeshShaderRenderer::PreCheck()
	{
		D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_5 };
		if (FAILED(m_Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)))
			|| (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_5))
		{
			GE_CORE_ERROR("ERROR: Shader Model 6.5 is not supported\n");

		}

		D3D12_FEATURE_DATA_D3D12_OPTIONS7 features = {};
		if (FAILED(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &features, sizeof(features)))
			|| (features.MeshShaderTier == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED))
		{
			GE_CORE_ERROR("ERROR: Mesh Shaders aren't supported!\n");
		}

	}

	ComPtr<ID3D12CommandQueue> D3D12MeshShaderRenderer::CreateCommandQueue(ComPtr<ID3D12Device> device)
	{
		ComPtr<ID3D12CommandQueue> commadQueue;
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commadQueue));
		return commadQueue;
	}

	ComPtr<IDXGISwapChain3> D3D12MeshShaderRenderer::CreateSwapChan(ComPtr<IDXGIFactory4> factory, ComPtr<ID3D12CommandQueue> commandQueue, int bufferCount, int bufferWidth, int bufferHeight)
	{
		ComPtr<IDXGISwapChain3> swapChain3;
		// Describe and create the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
		swapchainDesc.BufferCount = bufferCount;
		swapchainDesc.Width = bufferWidth;
		swapchainDesc.Height = bufferHeight;
		swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swapChain;
		ThrowIfFailed(factory->CreateSwapChainForHwnd(
			commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
			(HWND)(Application::Get().GetWindow().GetNativeWindow()),
			&swapchainDesc,
			nullptr,
			nullptr,
			&swapChain
		));

		ThrowIfFailed(swapChain.As(&swapChain3));
		return swapChain3;
	}

	ComPtr<ID3D12DescriptorHeap> D3D12MeshShaderRenderer::CreateRtvHeap(ComPtr<ID3D12Device> device, int numDescriptors)
	{

		ComPtr<ID3D12DescriptorHeap> rtvHeap;
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = numDescriptors;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));
		return rtvHeap;
	}

	ComPtr<ID3D12DescriptorHeap> D3D12MeshShaderRenderer::CreateDsvHeap(ComPtr<ID3D12Device> device, int numDescriptors)
	{
		ComPtr<ID3D12DescriptorHeap> dsvHeap;

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = numDescriptors;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap)));

		return dsvHeap;
	}

	void D3D12MeshShaderRenderer::CreateFence()
	{
		// Create synchronization objects and wait until assets have been uploaded to the GPU.

		ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
		m_FenceValues[m_FrameIndex]++;

		// Create an event handle to use for frame synchronization.
		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_FenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
	}

	void D3D12MeshShaderRenderer::CreateRenderTargetView()
	{
		m_RtvHeap = CreateRtvHeap(m_Device, m_FrameCount);
		m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// Create frame resources.
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());

			// Create a RTV and a command allocator for each frame.
			for (UINT n = 0; n < m_FrameCount; n++)
			{
				ThrowIfFailed(m_SwapChain->GetBuffer(n, IID_PPV_ARGS(&m_RenderTargets[n])));
				m_Device->CreateRenderTargetView(m_RenderTargets[n].Get(), nullptr, rtvHandle);
				rtvHandle.Offset(1, m_RtvDescriptorSize);
				ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocators[n])));
			}
		}
	}

	void D3D12MeshShaderRenderer::CreateDepthStencilView()
	{
		m_DsvHeap = CreateDsvHeap(m_Device, 1);
		// Create the depth stencil view.
		{
			D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
			depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
			depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

			D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
			depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
			depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
			depthOptimizedClearValue.DepthStencil.Stencil = 0;

			const CD3DX12_HEAP_PROPERTIES depthStencilHeapProps(D3D12_HEAP_TYPE_DEFAULT);
			const CD3DX12_RESOURCE_DESC depthStencilTextureDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_Width, m_Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

			ThrowIfFailed(m_Device->CreateCommittedResource(
				&depthStencilHeapProps,
				D3D12_HEAP_FLAG_NONE,
				&depthStencilTextureDesc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&depthOptimizedClearValue,
				IID_PPV_ARGS(&m_DepthStencil)
			));

			NAME_D3D12_OBJECT(m_DepthStencil);

			m_Device->CreateDepthStencilView(m_DepthStencil.Get(), &depthStencilDesc, m_DsvHeap->GetCPUDescriptorHandleForHeapStart());
		}
	}

	void D3D12MeshShaderRenderer::CreateConstantBufferView()
	{
		const UINT64 constantBufferSize = sizeof(SceneConstantBuffer) * m_FrameCount;
		const CD3DX12_HEAP_PROPERTIES constantBufferHeapProps(D3D12_HEAP_TYPE_UPLOAD);
		const CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);

		ThrowIfFailed(m_Device->CreateCommittedResource(
			&constantBufferHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&constantBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_ConstantBuffer)));

		// Describe and create a constant buffer view.
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_ConstantBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = constantBufferSize;

		// Map and initialize the constant buffer. We don't unmap this until the
		// app closes. Keeping things mapped for the lifetime of the resource is okay.
		CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_ConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_CbvDataBegin)));

	}

	void D3D12MeshShaderRenderer::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter)
	{
		*ppAdapter = nullptr;

		ComPtr<IDXGIAdapter1> adapter;

		ComPtr<IDXGIFactory6> factory6;
		if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
		{
			for (
				UINT adapterIndex = 0;
				SUCCEEDED(factory6->EnumAdapterByGpuPreference(
					adapterIndex,
					requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
					IID_PPV_ARGS(&adapter)));
				++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}

		if (adapter.Get() == nullptr)
		{
			for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}

		*ppAdapter = adapter.Detach();
	}

	void D3D12MeshShaderRenderer::UpdateCameraMatrices(const Camera& camera)
	{
		XMVECTOR cameraPosition = { camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z, 1.0f };
		XMVECTOR front = { camera.Front().x, camera.Front().y, camera.Front().z, 0.0 };
		XMVECTOR up = { camera.Up().x, camera.Up().y, camera.Up().z, 0.0 };

		XMMATRIX world = XMMATRIX(g_XMIdentityR0, g_XMIdentityR1, g_XMIdentityR2, g_XMIdentityR3);
		XMMATRIX view = XMMATRIX(&camera.GetViewMatrix()[0][0]) ;
		XMMATRIX proj = XMMATRIX(&camera.GetProjectionMatrix()[0][0]);  
		XMMATRIX viewProj = view * proj;

		//camera.GetViewMatrix()[0][0] and camera.GetProjectionMatrix()[0][0] is col_major, need to transposr to row_major
		XMStoreFloat4x4(&m_ConstantBufferData.World, XMMatrixTranspose(world));
		XMStoreFloat4x4(&m_ConstantBufferData.WorldView, XMMatrixTranspose(world * view));
		XMStoreFloat4x4(&m_ConstantBufferData.WorldViewProj, XMMatrixTranspose(world * view * proj));
		m_ConstantBufferData.DrawMeshlets = true;

		memcpy(m_CbvDataBegin + sizeof(SceneConstantBuffer) * m_FrameIndex, &m_ConstantBufferData, sizeof(m_ConstantBufferData));
	}

	void D3D12MeshShaderRenderer::Render(const Camera& camera)
	{
		UpdateCameraMatrices(camera);
		PopulateCommandList();
		// Execute the command list.
		ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// Present the frame.
		ThrowIfFailed(m_SwapChain->Present(1, 0));

		MoveToNextFrame();
	}

	void D3D12MeshShaderRenderer::PopulateCommandList()
	{
		// Command list allocators can only be reset when the associated 
		// command lists have finished execution on the GPU; apps should use 
		// fences to determine GPU execution progress.
		ThrowIfFailed(m_CommandAllocators[m_FrameIndex]->Reset());

		// However, when ExecuteCommandList() is called on a particular command 
		// list, that command list can then be reset at any time and must be before 
		// re-recording.
		ThrowIfFailed(m_CommandList->Reset(m_CommandAllocators[m_FrameIndex].Get(), m_PipelineState.Get()));

		// Set necessary state.
		m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
		m_CommandList->RSSetViewports(1, &m_Viewport);
		m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

		// Indicate that the back buffer will be used as a render target.
		const auto toRenderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_CommandList->ResourceBarrier(1, &toRenderTargetBarrier);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_DsvHeap->GetCPUDescriptorHandleForHeapStart());
		m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

		// Record commands.
		const float clearColor[] = { 0.0f, 0.8f, 0.7f, 0.2f };
		m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		m_CommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		m_CommandList->SetGraphicsRootConstantBufferView(0, m_ConstantBuffer->GetGPUVirtualAddress() + sizeof(SceneConstantBuffer) * m_FrameIndex);

		for (auto& model : m_Scene->GetModels()) {
			for (auto& mesh : model->m_Meshes) {
				m_CommandList->SetGraphicsRoot32BitConstant(1, mesh->IndexSize_ml, 0);
				m_CommandList->SetGraphicsRootShaderResourceView(2, mesh->VertexResources[0]->GetGPUVirtualAddress());
				m_CommandList->SetGraphicsRootShaderResourceView(3, mesh->MeshletResource->GetGPUVirtualAddress());
				m_CommandList->SetGraphicsRootShaderResourceView(4, mesh->UniqueVertexIndexResource->GetGPUVirtualAddress());
				m_CommandList->SetGraphicsRootShaderResourceView(5, mesh->PrimitiveIndexResource->GetGPUVirtualAddress());
																		
				for (auto& subset : mesh->MeshletSubsets)
				{
					m_CommandList->SetGraphicsRoot32BitConstant(1, subset.Offset, 1);
					m_CommandList->DispatchMesh(subset.Count, 1, 1);
				}
			}
		}


		// Indicate that the back buffer will now be used to present.
		const auto toPresentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_CommandList->ResourceBarrier(1, &toPresentBarrier);

		ThrowIfFailed(m_CommandList->Close());
	}
}
#endif
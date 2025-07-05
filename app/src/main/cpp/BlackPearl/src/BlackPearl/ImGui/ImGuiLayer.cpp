#include "pch.h"
#ifdef USE_IMGUI
#include "ImGui/ImGuiLayer.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_win32.h"

#include "Application.h"
#include "RHI/OpenGLRHI/OpenGLWindow.h"
#include "Component/LightComponent/PointLight.h"
#include "Component/CameraComponent/PerspectiveCamera.h"
#include "Component/LightProbeComponent/LightProbeComponent.h"
#include "Component/TerrainComponent/TerrainComponent.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "Renderer/SystemTextures.h"

#ifdef GE_API_OPENGL
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLDrvPrivate.h"
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#include "RHI/OpenGLRHI/OpenGLDriver/Windows/OpenGLDrvWindows.h"
#include "RHI/OpenGLRHI/OpenGLViewport.h"
#include "RHI/OpenGLRHI/OpenGLDevice.h""
#include "RHI/OpenGLRHI/OpenGLContext.h"

#endif
//#define IMGUI_IMPL_OPENGL_LOADER_GLAD
//
//#include "examples/imgui_impl_opengl3.cpp"
//#include "examples/imgui_impl_glfw.cpp"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace BlackPearl {
    class FScopeContext;
    struct FPlatformOpenGLDevice;
    extern DeviceManager* g_deviceManager;

    HGLRC g_RenderContextRC;
    // Win32 窗口过程
    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        ImGuiLayer* imguiLayer = reinterpret_cast<ImGuiLayer*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        switch (msg) {
            if (wParam != SIZE_MINIMIZED) {
                if (hWnd == imguiLayer->GetNativeWindow()) {
                    imguiLayer->GetIO()->DisplaySize = ImVec2((float)LOWORD(lParam), (float)HIWORD(lParam));
                }
            }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }



    void ImGuiLayer::InitImGUI()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
         //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
         //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
        io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
        io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI

        // Setup Dear ImGui style
       // ImGui::StyleColorsDark();

        ImGui::StyleColorsClassic();
        //// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }


        // Setup Platform/Renderer bindings
        ImGui_ImplWin32_Init(m_hImGuiWnd);
        ImGui_ImplOpenGL3_Init("#version 130");




    }

    void ImGuiLayer::CreateImguiWindow()
    {
        m_hImGuiWnd = static_cast<HWND>(Application::Get().GetWindow().GetNativeWindow());
    }


    void ImGuiLayer::OnSetup()
    {
        m_CommandList = m_DeviceManager->GetDevice()->createCommandList();
    }

    //  imgui 在 render context 上渲染
    void ImGuiLayer::OnAttach()
    {
        printf("current context init imgui = %p\n", wglGetCurrentContext());

        Device* device = static_cast<Device*>(g_deviceManager->GetDevice());

        m_hBackupDC = wglGetCurrentDC();
        m_hBackupRC = wglGetCurrentContext();

        PlatformRenderingContextSetup(device->m_Context->PlatformDevice);
        printf("current context  init imgui = %p\n", wglGetCurrentContext());

        CreateImguiWindow();
        InitImGUI();

        wglMakeCurrent(m_hBackupDC, m_hBackupRC);


        printf("current context after init imgui = %p\n", wglGetCurrentContext());


    }

    void ImGuiLayer::OnDetach()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::OnUpdate(Timestep ts)
    {
        //OnImguiRender();
    }

    void ImGuiLayer::OnImguiRender()
    {
        printf("current context OnImguiRender imgui = %p\n", wglGetCurrentContext());

        static bool show = false;
        ImGui::ShowDemoWindow(&show);


        //ImGui::Begin("GI Settings");

        //	ImGui::Text("SVO voxel GI");
        //	ImGui::Checkbox("spp pause", &VoxelConeTracingSVORenderer::s_Pause);
        //	ImGui::Checkbox("svo direct light", &VoxelConeTracingSVORenderer::s_DirectLight);
        //	ImGui::Checkbox("svo Indirect diffuse light", &VoxelConeTracingSVORenderer::s_IndirectDiffuseLight);
        //	ImGui::Checkbox("svo Indirect specular light", &VoxelConeTracingSVORenderer::s_IndirectSpecularLight);
        //	ImGui::DragFloat("svo GICoeffs", &VoxelConeTracingSVORenderer::s_GICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");
        //	ImGui::DragFloat("specularAngle", &VoxelConeTracingSVORenderer::s_IndirestSpecularAngle, 0.2f, 0.01f, 45.0f, "%.4f ");
        //	ImGui::DragFloat("specularStep", &VoxelConeTracingSVORenderer::s_Step, 0.01f, 0.01f, 45.0f, "%.4f ");
        //	ImGui::Separator();

        //	ImGui::Text("Deferred voxel GI");
        //	ImGui::Checkbox("Voxelize", &VoxelConeTracingDeferredRenderer::s_VoxelizeNow);

        //	ImGui::Checkbox("Indirect diffuse", &VoxelConeTracingDeferredRenderer::s_IndirectDiffuseLight);
        //	ImGui::Checkbox("Indirect specular", &VoxelConeTracingDeferredRenderer::s_IndirectSpecularLight);
        //	ImGui::Checkbox("Direct light", &VoxelConeTracingDeferredRenderer::s_DirectLight);
        //	ImGui::Checkbox("Shadows", &VoxelConeTracingDeferredRenderer::s_Shadows);

        //	ImGui::Checkbox("HDR", &VoxelConeTracingDeferredRenderer::s_HDR);
        //	/*ImGui::Checkbox("voxel blur horizontal", &VoxelConeTracingDeferredRenderer::s_GuassianHorizontal);
        //	ImGui::Checkbox("voxel blur vertical", &VoxelConeTracingDeferredRenderer::s_GuassianVertical);
        //	ImGui::Checkbox("voxel blur showBlurArea", &VoxelConeTracingDeferredRenderer::s_ShowBlurArea);*/
        //	ImGui::Checkbox("Blur mipmap", &VoxelConeTracingDeferredRenderer::s_MipmapBlurSpecularTracing);
        //	ImGui::DragFloat("Specular\nBlur\nThreshold", &VoxelConeTracingDeferredRenderer::s_SpecularBlurThreshold, 0.2f, 0.0f, 1.0f, "%.4f ");
        //	ImGui::DragFloat("Indirect\nSpecular\nAngle", &VoxelConeTracingDeferredRenderer::s_IndirectSpecularAngle, 0.2f, 1.0f, 45.0f, "%.4f ");
        //	ImGui::DragFloat("GICoeffs", &VoxelConeTracingDeferredRenderer::s_GICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");
        //	ImGui::DragInt("Visualization \nmipmap\nlevel", &VoxelConeTracingDeferredRenderer::s_VisualizeMipmapLevel, 1.0f, 0, 5);



        //	ImGui::Separator();
        //	ImGui::Text("forward voxel GI");
        //	ImGui::Checkbox("voxel Indirect diffuse", &VoxelConeTracingRenderer::s_IndirectDiffuseLight);
        //	ImGui::Checkbox("voxel Indirect specular", &VoxelConeTracingRenderer::s_IndirectSpecularLight);
        //	ImGui::Checkbox("voxel direct light", &VoxelConeTracingRenderer::s_DirectLight);
        //	ImGui::DragFloat("voxel GICoeffs", &VoxelConeTracingRenderer::s_GICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");
        //	ImGui::Checkbox("voxel HDR", &VoxelConeTracingRenderer::s_HDR);
        //	ImGui::Separator();
        //	ImGui::Text("light probe GI");
        //	ImGui::DragFloat("GICoeffs", &GBufferRenderer::s_GICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");
        //	ImGui::Checkbox("HDR", &GBufferRenderer::s_HDR);

        //	ImGui::Separator();



        //	ImGui::Text("image based lighting  GI");
        //	ImGui::DragFloat("IBL GICoeffs", &IBLRenderer::s_GICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");
        //	ImGui::Checkbox("IBL HDR", &GBufferRenderer::s_HDR);

        //	ImGui::Text("SSR GI");
        //	ImGui::DragFloat("SSRGICoeffs", &GBufferRenderer::s_SSRGICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");


        //	ImGui::End();

        //	ImGui::Begin("Cloud Settings");
        //	//ImGui::Text("FPS = %.3lf", Application::s_AppFPS);
        //	//ImGui::Text("AvgFPS = %.3lf", Application::s_AppAverageFPS);
        ////	ImGui::Separator();
        //
        //	ImGui::DragFloat("rayStep", &CloudRenderer::s_rayStep, 0.2f, 0.0f, 1.0f, "%.4f ");
        //	ImGui::DragFloat("step", &CloudRenderer::s_step, 0.2f, 1.0f, 45.0f, "%.4f ");
        //	ImGui::DragFloat("colorOffset1", &CloudRenderer::s_colorOffset1, 0.2f, 0.0f, 1.0f, "%.3f ");
        //	ImGui::DragFloat("colorOffset2", &CloudRenderer::s_colorOffset2, 0.0f, 0, 1.0f, "%.3f ");
        //	ImGui::DragFloat("s_densityOffset", &CloudRenderer::s_densityOffset, 0.0f, -1.0f, 1.0f, "%.3f ");
        //	ImGui::DragFloat("s_lightAbsorptionTowardSun", &CloudRenderer::s_lightAbsorptionTowardSun, 1.0f, 0.0f, 100.0f, "%.3f ");
        //	ImGui::DragFloat("s_densityMultiplier", &CloudRenderer::s_densityMultiplier, 1.0f, 0.0f, 100.0f, "%.3f ");

        //	float boxMax[] = { CloudRenderer::s_boundsMax.x, CloudRenderer::s_boundsMax.y, CloudRenderer::s_boundsMax.z };
        //	float boxMin[] = { CloudRenderer::s_boundsMin.x, CloudRenderer::s_boundsMin.y, CloudRenderer::s_boundsMin.z };

        //	ImGui::DragFloat3("s_boundsMax", boxMax, 0.05f, 0.001f, 100.0f, "%.3f ");
        //	CloudRenderer::s_boundsMax = math::float3(boxMax[0], boxMax[1], boxMax[2]);
        //	CloudRenderer::s_boundsMin = math::float3(boxMin[0], boxMin[1], boxMin[2]);



        //	ImGui::End();



        ShowPerformance();

        ShowHierarchy();

        ShowConfiguration();

        m_fileDialog.Display();

    }

    void ImGuiLayer::ShowCamera(PerspectiveCamera* perspectiveCamera)
    {
        ImGui::Text("Yaw = %f,Pitch= %f", perspectiveCamera->Yaw(), perspectiveCamera->Pitch());
        ImGui::Text("ProjectionViewMatrix[0].x = %f,ProjectionViewMatrix[1].x = %f", perspectiveCamera->GetProjectionViewMatrix()[0].x
            , perspectiveCamera->GetProjectionViewMatrix()[1].x);
        ImGui::Text("ProjectionViewMatrix[2].x = %f,ProjectionViewMatrix[3].x = %f", perspectiveCamera->GetProjectionViewMatrix()[2].x, perspectiveCamera->GetProjectionViewMatrix()[3].x);
        ImGui::Text("Position.x = %f,Position.y = %f,Position.z = %f", perspectiveCamera->GetPosition().x, perspectiveCamera->GetPosition().y, perspectiveCamera->GetPosition().z);
        ImGui::Text("Front.x = %f,Front.y = %f,Front.z = %f", perspectiveCamera->Front().x, perspectiveCamera->Front().y, perspectiveCamera->Front().z);
        ImGui::Text("Up.x = %f,Up.y = %f,Up.z = %f", perspectiveCamera->Up().x, perspectiveCamera->Up().y, perspectiveCamera->Up().z);
        ImGui::Text("Znear = %f,Zfar = %f,Fov = %f", perspectiveCamera->GetZnear(), perspectiveCamera->GetZfar(), perspectiveCamera->GetFov());

        float moveSpeed = perspectiveCamera->GetMoveSpeed(), rotSpeed = perspectiveCamera->GetRotateSpeed();
        ImGui::DragFloat("CameraMoveSpeed", &moveSpeed, perspectiveCamera->GetMoveSpeed(), 0.1, 500, "%.3f ");
        perspectiveCamera->SetMoveSpeed(moveSpeed);
        ImGui::DragFloat("CameraRotateSpeed", &rotSpeed, perspectiveCamera->GetRotateSpeed(), 0.1, 500, "%.3f ");
        perspectiveCamera->SetRotateSpeed(rotSpeed);

    }

    void ImGuiLayer::ShowCamera(MainCamera* mainCamera)
    {
        ImGui::Text("Yaw = %f,Pitch= %f", mainCamera->Yaw(), mainCamera->Pitch());
        ImGui::Text("Position.x = %f,Position.y = %f,Position.z = %f", mainCamera->GetPosition().x, mainCamera->GetPosition().y, mainCamera->GetPosition().z);
        ImGui::Text("Front.x = %f,Front.y = %f,Front.z = %f", mainCamera->Front().x, mainCamera->Front().y, mainCamera->Front().z);
        ImGui::Text("Up.x = %f,Up.y = %f,Up.z = %f", mainCamera->Up().x, mainCamera->Up().y, mainCamera->Up().z);
        ImGui::Text("Znear = %f,Zfar = %f,Fov = %f", mainCamera->ZNear(), mainCamera->ZFar(), mainCamera->Fov());

        float moveSpeed = mainCamera->GetMoveSpeed(), rotSpeed = mainCamera->GetRotateSpeed();
        ImGui::DragFloat("CameraMoveSpeed", &moveSpeed, mainCamera->GetMoveSpeed(), 0.1, 500, "%.3f ");
        mainCamera->SetMoveSpeed(moveSpeed);
        ImGui::DragFloat("CameraRotateSpeed", &rotSpeed, mainCamera->GetRotateSpeed(), 0.1, 500, "%.3f ");
        mainCamera->SetRotateSpeed(rotSpeed);

    }

    void ImGuiLayer::ShowTerrian(Object* obj)
    {

        ImGui::Text("Terrian");
        bool dynamicTessLevel = obj->GetComponent<TerrainComponent>()->GetDynamicTess();
        ImGui::Checkbox("dynamicTessLevel", &dynamicTessLevel);
        obj->GetComponent<TerrainComponent>()->SetDynamicTess(dynamicTessLevel);


        float staticTessLevel = obj->GetComponent<TerrainComponent>()->GetStaticTessLevel();
        ImGui::DragFloat("staticTessLevel", &staticTessLevel, obj->GetComponent<TerrainComponent>()->GetStaticTessLevel(), 8.0f, 200.0f, "%.3f ");
        obj->GetComponent<TerrainComponent>()->SetTessLevel(staticTessLevel);

    }

    void ImGuiLayer::ShowShader(std::string imguiShaders, int meshIndex, int& itemIndex, int offset)
    {
        std::string buttonName = "select file##" + std::to_string(meshIndex + offset);
        std::string inputTextName = "mesh" + std::to_string(meshIndex + offset);

        //imguiShaders = mesh->GetMaterial()->GetShader()->getDesc().path;
    //	ImGui::PushID(meshIndex);

        char* shader = const_cast<char*>(imguiShaders.c_str());
        ImGui::InputText(inputTextName.c_str(), shader, IM_ARRAYSIZE(shader));
        ImGui::SameLine();
        if (ImGui::Button(buttonName.c_str())) {
            itemIndex = meshIndex;
            m_fileDialog.Open();
        }
        //	ImGui::PopID();

    }

    void ImGuiLayer::ShowTextures(std::string imguiShaders, int meshIndex, int& itemIndex, TextureType textureType, TextureType& type, int offset)
    {

        std::string buttonName = ""; //+std::to_string(meshIndex + offset);
        std::string inputTextName = "";// +std::to_string(meshIndex + offset);

        switch (textureType) {
        case TextureType::DiffuseMap:
            buttonName += "diffuseMap";
            inputTextName += "diffuseMap";
            break;
        case TextureType::SpecularMap:
            buttonName += "specularMap";
            inputTextName += "specularMap";
            break;
        case TextureType::EmissionMap:
            buttonName += "emissionMap";
            inputTextName += "emissionMap";
            break;
        case TextureType::HeightMap:
            buttonName += "heightMap";
            inputTextName += "heightMap";
            break;
        case TextureType::NormalMap:
            buttonName += "normalMap";
            inputTextName += "normalMap";
            break;
        case TextureType::CubeMap:
            buttonName += "cubeMap";
            inputTextName += "cubeMap";
            break;
        case TextureType::DepthMap:
            buttonName += "depthMap";
            inputTextName += "depthMap";
            break;
        case TextureType::AoMap:
            buttonName += "aoMap";
            inputTextName += "aoMap";
            break;
        case TextureType::RoughnessMap:
            buttonName += "roughnessMap";
            inputTextName += "roughnessMap";
            break;
        case TextureType::MentallicMap:
            buttonName += "mentallicMap";
            inputTextName += "mentallicMap";
            break;

        }
        inputTextName += "##" + std::to_string(meshIndex);
        buttonName += "##" + std::to_string(meshIndex);

        if (imguiShaders.size() == 0)
            ImGui::Text("                    ");
        else
            ImGui::Text("%s", imguiShaders.c_str());

        ImGui::SameLine();

        if (ImGui::Button(buttonName.c_str())) {
            itemIndex = meshIndex;
            type = textureType;
            m_fileDialog.Open();
        }

        //ImGui::PopID();

    }

    void ImGuiLayer::ShowMaterialProps(Props& imGuiProps)
    {
    }

    void ImGuiLayer::ShowMeshRenderer(MeshRenderer* comp)
    {

        ImGui::Text("MeshRenderer");
        const std::vector<std::shared_ptr<Mesh>>& imGuiMeshes = comp->GetMeshes();
        if (imGuiMeshes.empty()) return;

        int offset = 0;

        ImGui::TextColored({ 1.0,0.64,0.0,1.0 }, "Shader");
        std::vector<std::string> imguiShaders;
        static int itemIndex = -1;
        //std::cout << "itemIndex" << itemIndex <<std::endl;
        imguiShaders.resize(imGuiMeshes.size());
        for (int i = 0; i < imGuiMeshes.size(); i++)
        {
            if (imGuiMeshes[i]->GetMaterial()->GetShader() == nullptr)
                continue;
            imguiShaders[i] = imGuiMeshes[i]->GetMaterial()->GetShader()->GetPath();
            ShowShader(imguiShaders[i], i, itemIndex, offset);
        }
        if (itemIndex != -1) {
            if (m_fileDialog.HasSelected()) {

                imGuiMeshes[itemIndex]->GetMaterial()->SetShader(m_fileDialog.GetSelected().string());
                m_fileDialog.ClearSelected();
                itemIndex = -1;
            }
        }

        if (imGuiMeshes[0]->GetMaterial()->GetTextureMaps() != nullptr) {

            ImGui::TextColored({ 1.0,0.64,0.0,1.0 }, "TextureMaps");
            std::vector<std::string> imguiDiffuseTextures(imGuiMeshes.size());
            std::vector<std::string> imguiSpecularTextures(imGuiMeshes.size());
            std::vector<std::string> imguiEmissionTextures(imGuiMeshes.size());
            std::vector<std::string> imguiNormalTextures(imGuiMeshes.size());
            std::vector<std::string> imguiCubeTextures(imGuiMeshes.size());
            std::vector<std::string> imguiHeightTextures(imGuiMeshes.size());
            std::vector<std::string> imguiDepthTextures(imGuiMeshes.size());


            static  int itemIndexTexture = -1;
            static TextureType type;
            //std::cout << "itemIndexTexture" << itemIndexTexture << std::endl;
            //GE_CORE_TRACE("itemIndexTexture:"+std::to_string(itemIndexTexture) );
            for (int i = 0; i < imGuiMeshes.size(); i++)
            {
                std::string text = "Mesh" + std::to_string(i);
                ImGui::TextColored(ImVec4(1.0, 0.64, 0.0, 1.0), "%s", text.c_str());
                if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->diffuseTextureMap != nullptr) {
                    imguiDiffuseTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->diffuseTextureMap->getDesc().path;
                }
                ShowTextures(imguiDiffuseTextures[i], i, itemIndexTexture, TextureType::DiffuseMap, type, imGuiMeshes.size() * 2);


                if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->specularTextureMap != nullptr)
                    imguiSpecularTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->specularTextureMap->getDesc().path;
                ShowTextures(imguiSpecularTextures[i], i, itemIndexTexture, TextureType::SpecularMap, type, imGuiMeshes.size() * 2);

                if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->emissionTextureMap != nullptr)
                    imguiEmissionTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->emissionTextureMap->getDesc().path;
                ShowTextures(imguiEmissionTextures[i], i, itemIndexTexture, TextureType::EmissionMap, type, imGuiMeshes.size() * 2);

                if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->normalTextureMap != nullptr)
                    imguiNormalTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->normalTextureMap->getDesc().path;
                ShowTextures(imguiNormalTextures[i], i, itemIndexTexture, TextureType::NormalMap, type, imGuiMeshes.size() * 2);

                if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->cubeTextureMap != nullptr)
                    imguiCubeTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->cubeTextureMap->getDesc().path;
                ShowTextures(imguiCubeTextures[i], i, itemIndexTexture, TextureType::CubeMap, type, imGuiMeshes.size() * 2);

                if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->heightTextureMap != nullptr)
                    imguiHeightTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->heightTextureMap->getDesc().path;
                ShowTextures(imguiHeightTextures[i], i, itemIndexTexture, TextureType::HeightMap, type, imGuiMeshes.size() * 2);

                if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->depthTextureMap != nullptr)
                    imguiDepthTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->depthTextureMap->getDesc().path;
                ShowTextures(imguiDepthTextures[i], i, itemIndexTexture, TextureType::DepthMap, type, imGuiMeshes.size() * 2);

                if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->aoMap != nullptr)
                    imguiDepthTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->aoMap->getDesc().path;
                ShowTextures(imguiDepthTextures[i], i, itemIndexTexture, TextureType::AoMap, type, imGuiMeshes.size() * 2);


                if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->roughnessMap != nullptr)
                    imguiDepthTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->roughnessMap->getDesc().path;
                ShowTextures(imguiDepthTextures[i], i, itemIndexTexture, TextureType::RoughnessMap, type, imGuiMeshes.size() * 2);


                if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->mentallicMap != nullptr)
                    imguiDepthTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->mentallicMap->getDesc().path;
                ShowTextures(imguiDepthTextures[i], i, itemIndexTexture, TextureType::MentallicMap, type, imGuiMeshes.size() * 2);
            }
            if (itemIndexTexture != -1) {
                if (m_fileDialog.HasSelected()) {
                    TextureDesc desc;
                    desc.type = type;
                    desc.path = m_fileDialog.GetSelected().string();
                    TextureHandle texture = m_DeviceManager->GetDevice()->createTexture(desc);

                    imGuiMeshes[itemIndexTexture]->GetMaterial()->SetTexture(texture);



                    m_fileDialog.ClearSelected();
                    itemIndexTexture = -1;
                }

            }
        }

        ImGui::TextColored({ 1.0,0.64,0.0,1.0 }, "Material Properties");
        Props imGuiProps = imGuiMeshes[0]->GetMaterial()->GetProps();//TODO::Ĭ������mesh ��Material::Props ��һ����
        float imGuiShininess = imGuiProps.shininess;
        bool  imGUiBlinnLight = imGuiProps.isBinnLight;
        bool  imGUiIsPBRTextureSample = (bool)imGuiProps.isPBRTextureSample;
        bool  imGUiIsDifussTextureSample = (bool)imGuiProps.isDiffuseTextureSample;
        bool  imGUiIsSpecularTextureSample = (bool)imGuiProps.isSpecularTextureSample;
        //bool  imGUiIsMetallicrTextureSample = (bool)imGuiProps.isMetallicTextureSample;

        ImGui::Checkbox("blinnlight", &imGUiBlinnLight);
        for (auto mesh : imGuiMeshes)
            mesh->GetMaterial()->SetBinnLight(imGUiBlinnLight);

        ImGui::DragFloat("shininess", &imGuiShininess, 0.5f, 0.0f, 1024.0f, "%.3f ");
        for (auto mesh : imGuiMeshes)
            mesh->GetMaterial()->SetShininess(imGuiShininess);


        ImGui::Checkbox("usePPBRTexture", &imGUiIsPBRTextureSample);
        for (auto mesh : imGuiMeshes)
            mesh->GetMaterial()->SetPBRTextureSample((int)imGUiIsPBRTextureSample);

        ImGui::Checkbox("useDiffuseTexture", &imGUiIsDifussTextureSample);
        for (auto mesh : imGuiMeshes)
            mesh->GetMaterial()->SetTextureSampleDiffuse((int)imGUiIsDifussTextureSample);

        ImGui::Checkbox("useSpecularTexture", &imGUiIsSpecularTextureSample);
        for (auto mesh : imGuiMeshes)
            mesh->GetMaterial()->SetTextureSampleSpecular((int)imGUiIsSpecularTextureSample);

        /*ImGui::Checkbox("useMetallicTexture", &imGUiIsMetallicrTextureSample);
        for (auto mesh : imGuiMeshes)
            mesh->GetMaterial()->SetTextureSampleMetallic((int)imGUiIsMetallicrTextureSample);*/

        for (int i = 0; i < imGuiMeshes.size(); i++) {
            MaterialColor color = imGuiMeshes[i]->GetMaterial()->GetMaterialColor();
            ImGui::ColorEdit3("diffuseColor", color.diffuseColor);
            //mesh->GetMaterial()->SetMaterialColorDiffuseColor(color.diffuseColor);

            ImGui::ColorEdit3("specularColor", color.specularColor);
            //mesh->GetMaterial()->SetMaterialColorSpecularColor(color.specularColor);

            ImGui::ColorEdit3("emissiveColor", color.emissiveColor);
            //mesh->GetMaterial()->SetMaterialColorEmissionColor(color.emissionColor);
            imGuiMeshes[i]->SetMaterialColor(color);

        }


    }

    void ImGuiLayer::ShowTransform(Transform* comp, Object* obj)
    {
        ImGui::Text("Transform");

        float pos[] = { comp->GetPosition().x,comp->GetPosition().y,comp->GetPosition().z };
        ImGui::DragFloat3("position", pos, 0.2f, -100.0f, 100.0f, "%.3f ");
        obj->SetPosition({ pos[0],pos[1],pos[2] });

        float scale[] = { comp->GetScale().x,comp->GetScale().y,comp->GetScale().z };
        ImGui::DragFloat3("scale", scale, 0.05f, 0.001f, 100.0f, "%.3f ");
        obj->SetScale({ scale[0],scale[1],scale[2] });

        float rotate[] = { comp->GetRotation().x,comp->GetRotation().y,comp->GetRotation().z };
        ImGui::DragFloat3("rotation", rotate, 1.0f, -360.0f, 360.0f, "%.3f ");
        obj->SetRotation({ rotate[0],rotate[1],rotate[2] });

    }

    void ImGuiLayer::ShowLightProbe(LightProbe* probe, Object* obj)
    {
        ImGui::Text("LightProbe");

        float zFar = probe->GetZfar();
        ImGui::DragFloat("zFar", &zFar, 0.5f, 1.0, 100.0);
        probe->SetZfar(zFar);

        bool dynamicSpecularMap = probe->GetDynamicSpecularMap();
        ImGui::Checkbox("dynamicSpecularLight", &dynamicSpecularMap);
        probe->SetDynamicSpecularMap(dynamicSpecularMap);


    }

    void ImGuiLayer::ShowPointLight(PointLight* pointLight)
    {
        //if (comp->GetType() == LightType::PointLight) {
            //auto pointLight = std::dynamic_pointer_cast<PointLight>(comp);
        //auto color = pointLight->GetMeshes()->GetMaterial()->GetMaterialColor().Get();
        auto props = pointLight->GetLightProps();
        static  int attenuation = (int)pointLight->GetAttenuation().maxDistance;
        float intensity = pointLight->GetLightProps().intensity;
        float area = pointLight->GetLightProps().area;
        float bias = pointLight->GetLightProps().shadowBias;

        ImGui::ColorEdit3("ambient Color", props.ambient);
        ImGui::ColorEdit3("diffuse Color", props.diffuse);
        ImGui::ColorEdit3("specular Color", props.specular);
        ImGui::ColorEdit3("emission Color", props.emission);
        ImGui::DragInt("attenuation", &attenuation, 0.5f, 7, 3250);
        ImGui::DragFloat("intensity", &intensity, 0.1f, 0.1, 100);
        ImGui::DragFloat("lightSize", &area, 0.1f, 0.1, 100);
        ImGui::DragFloat("shadowBias", &bias, 0.001f, 0.001, 100);
        //ImGui::DragInt("pcfSamplesCnt", &ShadowMapPointLightRenderer::s_PCFSamplesCnt, 1, 2, 60);

        pointLight->SetAttenuation(attenuation);
        Light::Props pros = { props.ambient ,props.diffuse,props.specular,props.emission,intensity };
        pros.area = area;
        pros.shadowBias = bias;

        pointLight->UpdateMesh(pros);

    }

    void ImGuiLayer::ShowParallelLight(DirectionLight* parallelLight)
    {
        auto props = parallelLight->GetLightProps();
        math::float3 direction = parallelLight->GetDirection();

        float dir[] = { direction.x,direction.y,direction.z };
        ImGui::DragFloat3("direction", dir, 0.05f, -1.0f, 1.0f, "%.3f ");
        parallelLight->SetDirection({ dir[0],dir[1],dir[2] });

        float intensity = parallelLight->GetLightProps().intensity;
        //ImGui::DragFloat3("direction", (props.emission));

        ImGui::ColorEdit3("ambient Color", (props.ambient));
        ImGui::ColorEdit3("diffuse Color", (props.diffuse));
        ImGui::ColorEdit3("specular Color", (props.specular));
        ImGui::ColorEdit3("emission Color", (props.emission));
        ImGui::DragFloat("intensity", &intensity, 1.0f, 1, 100);


        parallelLight->UpdateMesh({ props.ambient ,props.diffuse,props.specular,props.emission,intensity });
    }


    //std::vector<std::string> Layer::GetObjectsName()
    //{
    //	return g_objectManager->GetObjectsName();
    //}
    void ImGuiLayer::Begin()
    {

        printf("current context begin imgui = %p\n", wglGetCurrentContext());

        m_CommandList->open();
        FRHIRenderPassInfo RPInfo(SystemTexture::Get().GetBackBuffer(), ERenderTargetActions::Load_Store,
            SystemTexture::Get().SceneDepth,
            EDepthStencilTargetActions::LoadDepthStencil_StoreDepthStencil);
        m_CommandList->beginRenderPass(RPInfo, "UIPass");

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        bool show_demo_window = false;
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
        GE_ERROR_JUDGE();

    }

    void ImGuiLayer::End()
    {
        // Rendering
        glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
        ImGui::Render();
        GE_ERROR_JUDGE();
        ImGuiIO& io = ImGui::GetIO(); //(void)io;


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        m_CommandList->endRenderPass();

        m_CommandList->close();

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {

            HDC         dc = wglGetCurrentDC();
            HGLRC       rc = wglGetCurrentContext();

            auto context = wglGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            // 恢复 OpenGL 上下文

            wglMakeCurrent(dc, rc);
        }

        GE_ERROR_JUDGE();
        printf("current context end imgui = %p\n", wglGetCurrentContext());


    }
    void ImGuiLayer::ShowPerformance()
    {
        ImGui::Begin("Performance");
        ImGui::Text("FPS = %.3lf", Application::s_AppFPS);
        ImGui::Text("AvgFPS = %.3lf", Application::s_AppAverageFPS);
        ImGui::Separator();
        ImGui::Text("s_TotalFrameNum = %d", Application::s_TotalFrameNum);
        //	ImGui::Text("DrawCalls per frame = %.3lf", (double)BasicRenderer::s_DrawCallCnt);

         //ImGui::Text("DrawCalls per frame = %.3lf", (double)Application::s_TotalFrameNum/BasicRenderer::s_DrawCallCnt);
        ImGui::Text("Objs num = %d", (int)m_ObjectsList.size());
        ImGui::Text("BackGround Objs num = %d", (int)m_BackGroundObjsList.size());

        ImGui::End();
    }

    void ImGuiLayer::ShowHierarchy()
    {

        static Object* currentObj = nullptr;
        ImGui::Begin("Hierarchy");
        if (ImGui::CollapsingHeader("Create")) {

            const char* const entityItems[] = { "Empty","DirectionLight","PointLight","SpotLight","IronMan","Deer","OldHouse","Bunny","Cube","Plane" };
            static int entityIdx = -1;
            if (ImGui::Combo("CreateEntity", &entityIdx, entityItems, 10))
            {
                switch (entityIdx)
                {
                case 0:
                    GE_CORE_INFO("Creating Empty...");
                    Layer::CreateEmpty();
                    break;
                case 1:
                    GE_CORE_INFO("Creating PointLight...");
                    Layer::CreateLight(LightType::DirectionLight);
                    break;
                case 2:
                    GE_CORE_INFO("Creating PointLight...");
                    Layer::CreateLight(LightType::PointLight);
                    break;
                case 3:
                    GE_CORE_INFO("Creating SpotLight ...");
                    Layer::CreateLight(LightType::SpotLight);
                    break;
                case 4:
                    GE_CORE_INFO("Creating IronMan ...");
                    Layer::CreateModel("assets/models/IronMan/IronMan.obj", "assets/shaders/glsl/IronMan.glsl", false, "IronMan");
                    break;
                case 5:
                    GE_CORE_INFO("Creating Deer ...");
                    //Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
                    //Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
                    //Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
                    LoadStaticBackGroundObject("Deer");

                    break;
                case 6:
                    GE_CORE_INFO("Creating OldHouse ...");
                    //Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
                    //Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
                    //Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
                    Layer::CreateModel("assets/models/OldHouse/Gost House/3D models/Gost House (5).obj", "assets/shaders/glsl/IronMan.glsl", false, "OldHouse");

                    break;
                case 7:
                    GE_CORE_INFO("Creating Bunny ...");
                    LoadStaticBackGroundObject("Bunny");
                    //Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
                    //Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
                    //Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
                    //Layer::CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/IronMan.glsl", false, "Bunny");
                case 8:
                    GE_CORE_INFO("Creating Cube ...");
                    Layer::CreateCube();
                    break;
                case 9:
                    GE_CORE_INFO("Creating Plane ...");
                    Layer::CreatePlane();
                    break;
                }
            }
        }
        if (ImGui::BeginTabBar("TabBar 0", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Scene")) {
                std::vector<Object*> objsList = GetObjects();		//TODO::
                ImGui::ListBoxHeader("CurrentEntities", (int)objsList.size(), 10);

                for (int n = 0; n < objsList.size(); n++) {
                    //ImGui::Text("%s", objsList[n].c_str());
                    bool is_selected = (currentObj != nullptr && currentObj->GetName() == objsList[n]->GetName());
                    if (ImGui::Selectable(objsList[n]->GetName().c_str(), is_selected)) {
                        currentObj = objsList[n];
                        GE_CORE_INFO(objsList[n]->GetName() + "is selected")
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
        ImGui::End();
        //////////////////Inspector/////////////////////////
        ImGui::Begin("Inspector");


        /*float pos[] = { m_Sun->GetComponent<DirectionLight>()->GetDirection().x, m_Sun->GetComponent<DirectionLight>()->GetDirection().y, m_Sun->GetComponent<DirectionLight>()->GetDirection().z };
        ImGui::DragFloat3("m_LightPos", pos, 0.1f, -100.0f, 100.0f, "%.3f ");
        m_Sun->GetComponent<DirectionLight>()->SetDirection({ pos[0],pos[1],pos[2] });*/

        /*
                ImGui::DragFloat("near_plane", &ShadowMapRenderer::s_NearPlane, 0.5f, -50.0f, 100.0f, "%.3f ");
                ImGui::DragFloat("far_plane", &ShadowMapRenderer::s_FarPlane, 0.5f, -50.0f, 100.0f, "%.3f ");*/

        if (currentObj != nullptr) {

            if (currentObj->HasComponent< Transform>()) {
                if (currentObj->GetComponent<BasicInfo>()->GetType() == OT_BatchNode) {

                }
                else {
                    ShowTransform(currentObj->GetComponent<Transform>(), currentObj);

                }

            }
            if (currentObj->HasComponent< LightProbe>()) {
                ShowLightProbe(currentObj->GetComponent<LightProbe>(), currentObj);

            }
            if (currentObj->HasComponent< MeshRenderer>()) {
                ShowMeshRenderer(currentObj->GetComponent<MeshRenderer>());
                /*backGroundObj list*/

                bool isBackGroundObj = currentObj->GetComponent<MeshRenderer>()->GetIsBackGroundObjects();
                ImGui::Checkbox("isBackGroundObj", &isBackGroundObj);
                //TODO:: ���Բ��� bitset
                if (isBackGroundObj) {
                    std::vector<Object*>::const_iterator it = std::find(m_BackGroundObjsList.begin(), m_BackGroundObjsList.end(), currentObj);
                    if (it == m_BackGroundObjsList.end()) {
                        m_BackGroundObjsList.push_back(currentObj);
                        currentObj->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
                    }

                }
                else {
                    std::vector<Object*>::const_iterator it;// = m_BackGroundObjsList.begin();
                    for (it = m_BackGroundObjsList.begin(); it != m_BackGroundObjsList.end(); it++) {
                        if ((*it)->GetId() == currentObj->GetId()) {
                            m_BackGroundObjsList.erase(it);
                            currentObj->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(false);

                            break;
                        }
                    }
                }

                /*shadowObj list*/
                bool isShadowObj = currentObj->GetComponent<MeshRenderer>()->GetIsShadowObjects();
                ImGui::Checkbox("isShadowObj", &isShadowObj);
                //TODO:: ���Բ��� bitset
                if (isShadowObj) {
                    std::vector<Object*>::const_iterator it = std::find(m_ShadowObjsList.begin(), m_ShadowObjsList.end(), currentObj);
                    if (it == m_ShadowObjsList.end()) {
                        m_ShadowObjsList.push_back(currentObj);
                        currentObj->GetComponent<MeshRenderer>()->SetIsShadowObjects(true);
                    }

                }
                else {
                    std::vector<Object*>::const_iterator it;// = m_BackGroundObjsList.begin();
                    for (it = m_ShadowObjsList.begin(); it != m_ShadowObjsList.end(); it++) {
                        if ((*it)->GetId() == currentObj->GetId()) {
                            m_ShadowObjsList.erase(it);
                            currentObj->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);

                            break;
                        }
                    }
                }


            }
            if (currentObj->HasComponent < PointLight>()) {
                ShowPointLight(currentObj->GetComponent<PointLight>());
            }
            if (currentObj->HasComponent < DirectionLight>()) {
                ShowParallelLight(currentObj->GetComponent<DirectionLight>());
            }
            if (currentObj->HasComponent<PerspectiveCamera>()) {
                if (currentObj == m_MainCamera->GetObj()) {
                    ShowCamera(m_MainCamera);
                }
                else {
                    ShowCamera(currentObj->GetComponent<PerspectiveCamera>());

                }

            }
            if (currentObj->HasComponent<TerrainComponent>()) {
                ShowTerrian(currentObj);

            }

        }

        ImGui::End();
    }


    void ImGuiLayer::ShowConfiguration()
    {
        ImGui::Begin("Config");
        ImGui::Text("Vsync: %s ", Configuration::Vsync ? "true" : "false");


        ImGui::Separator();
        ImGui::Text("Pipeline Settings");
        ImGui::Checkbox("bDeferredShading", &Configuration::bDeferredShading);
        ImGui::Checkbox("bUseSinglePass", &Configuration::bUseSinglePass);

       // ImGui::Checkbox("bUseIBL", &Configuration::bUseIBL);
        ImGui::Checkbox("bUseDirectLight", &Configuration::bUseDirectLight);
        //ImGui::Checkbox("bUseVoxel", &Configuration::bUseVoxel);

        ImGui::Separator();
        ImGui::Text("Probes Settings");
        ImGui::Checkbox("bUpdateProbePerFrame", &Configuration::bUpdateProbePerFrame);
        ImGui::Checkbox("bShowProbes", &Configuration::bShowProbes);

        ImGui::Separator();
        ImGui::Text("DebugView");
        static DebugView::Type db_type = DebugView::Type::DV_Lit;
        for (int n = 0; n < (int)DebugView::Type::DV_Cnt; n++) {
            //ImGui::Text("%s", objsList[n].c_str());
            bool is_selected = ((int)db_type == n);
            if (ImGui::Selectable(DebugView::ToString((DebugView::Type)n).c_str(), is_selected)) {
                db_type = (DebugView::Type)n;
            }
            Configuration::DebugView = db_type;
            
        }
        ImGui::End();
    }
}
#endif
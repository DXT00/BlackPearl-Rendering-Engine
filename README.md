

1.使ImGUI 显示MeshRenderer 

可以直接从ImGuig更改 Shader

2.导入文件管理模块 ：https://github.com/AirGuanZ/imgui-filebrowser

3.主要修改代码

Layer.cpp
```

	void Layer::ShowShader(static std::string &imguiShaders, static char* shader, Mesh & mesh, int meshIndex, static  int &itemIndex)
	{

		std::string buttonName = "select file##" + std::to_string(meshIndex);
		std::string inputTextName = "shader##" + std::to_string(meshIndex);

		imguiShaders = mesh.GetMaterial()->GetShader()->GetPath();
		shader = const_cast<char*>(imguiShaders.c_str());
		ImGui::InputText(inputTextName.c_str(), shader, IM_ARRAYSIZE(shader));
		ImGui::SameLine();
		if (ImGui::Button(buttonName.c_str())) {
			itemIndex = meshIndex;
			m_fileDialog.Open();
		}


	}

	void Layer::ShowMeshRenderer(std::shared_ptr<BlackPearl::MeshRenderer> comp)
	{
		ImGui::Text("MeshRenderer");


		if (!comp->GetMeshes().empty()) {
			ImGui::TextColored({ 1.0,0.64,0.0,1.0 }, "Material");
			static std::vector<std::string> imguiShaders(comp->GetMeshes().size());
			static std::vector<char*> shader(comp->GetMeshes().size());
			static  int itemIndex = -1;

			for (int i = 0; i < comp->GetMeshes().size(); i++)
			{
				ShowShader(imguiShaders[i], shader[i], comp->GetMeshes()[i], i, itemIndex);
			}
			if (itemIndex != -1) {
			

				if (m_fileDialog.HasSelected()) {
					imguiShaders[itemIndex] = m_fileDialog.GetSelected().string();
					std::cout << "Selected filename" << m_fileDialog.GetSelected().string() << std::endl;
					shader[itemIndex] = const_cast<char*>(imguiShaders[itemIndex].c_str());// (m_fileDialog.GetSelected().string().c_str());
					comp->GetMeshes()[itemIndex].GetMaterial()->SetShader("assets/shaders/" + imguiShaders[itemIndex]);

					m_fileDialog.ClearSelected();
					itemIndex = -1;
				}
				



			}

		}

		if (comp->GetModel() != nullptr) {
			ImGui::TextColored({ 1.0,0.64,0.0,1.0 }, "Model");
			static std::vector< std::string> imguiModelShaders(comp->GetModel()->GetMeshes().size());
			static std::vector<char*> Modelshader(comp->GetModel()->GetMeshes().size());
			static  int itemIndex = -1;
			

			for (int i = 0; i < comp->GetModel()->GetMeshes().size(); i++) {

				ShowShader(imguiModelShaders[i], Modelshader[i], comp->GetModel()->GetMeshes()[i], i, itemIndex);

			}
			std::cout << "itemIndex:" << itemIndex << std::endl;
			if (itemIndex != -1) {


				if (m_fileDialog.HasSelected()) {
					imguiModelShaders[itemIndex] = m_fileDialog.GetSelected().string();
					std::cout << "Selected filename" << m_fileDialog.GetSelected().string() << std::endl;
					Modelshader[itemIndex] = const_cast<char*>(imguiModelShaders[itemIndex].c_str());// (m_fileDialog.GetSelected().string().c_str());
					comp->GetModel()->GetMeshes()[itemIndex].GetMaterial()->SetShader("assets/shaders/" + imguiModelShaders[itemIndex]);

					m_fileDialog.ClearSelected();
					itemIndex = -1;
				}
				



			}

		}










	}

```
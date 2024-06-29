//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>
#include <filesystem>
#include <vector>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_scenehierarchy.hpp"
#include "gui.hpp"
#include "configFile.hpp"
#include "gameobject.hpp"
#include "core.hpp"
#include "fileUtils.hpp"
#include "stringUtils.hpp"
#include "selectobject.hpp"

using std::shared_ptr;
using std::filesystem::directory_iterator;
using std::filesystem::exists;
using std::filesystem::path;
using std::exception;
using std::vector;

using EngineFile::ConfigFileManager;
using EngineFile::ConfigFileValue;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using Core::Engine;
using Utils::File;
using Utils::String;
using Physics::Select;

namespace Graphics::GUI
{
	void GUISceneHierarchy::RenderSceneHierarchy()
	{
		ImGui::SetNextWindowSizeConstraints(EngineGUI::minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(EngineGUI::initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		static bool renderSceneHierarchy;
		if (ConfigFileManager::valuesMap.size() > 0)
		{
			renderSceneHierarchy = stoi(ConfigFileManager::valuesMap["gui_sceneHierarchy"].GetValue());
		}

		if (renderSceneHierarchy
			&& ImGui::Begin("Scene hierarchy", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				ConfigFileManager::valuesMap["gui_sceneHierarchy"].SetValue("0");
			}

			DisplayGameObjects();

			ImGui::End();
		}
	}

	void GUISceneHierarchy::DisplayGameObjects()
	{
		vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();

		for (const shared_ptr<GameObject>& obj : objects)
		{
			if (obj == nullptr) continue;

			string name = obj->GetName();

			if (ImGui::Selectable(name.c_str()))
			{
				Select::selectedObj = obj;
				Select::isObjectSelected = true;
			}

			if (ImGui::BeginPopupContextItem())
			{
				//delete selected gameobject
				if (ImGui::MenuItem("Delete"))
				{
					GameObjectManager::DestroyGameObject(obj);
				}

				ImGui::EndPopup();
			}
		}
	}
}
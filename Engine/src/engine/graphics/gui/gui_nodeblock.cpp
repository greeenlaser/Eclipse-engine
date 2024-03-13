//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

#include <filesystem>
#include <iostream>

//external
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "stb_image.h"
#include "glfw3.h"
#include "glad.h"

//engine
#include "gui_nodeblock.hpp"
#include "gui.hpp"
#include "core.hpp"
#include "input.hpp"

using std::cout;
using std::max;
using std::min;
using std::to_string;
using std::filesystem::exists;

using Core::Engine;
using Core::Input;

namespace Graphics::GUI
{
	void GUINodeBlock::SetBackgroundTexture()
	{
		string backgroundPath = Engine::enginePath + "/textures/node_background.png";
		if (!exists(backgroundPath))
		{
			Engine::CreateErrorPopup("Node window setup error", "Couldn't find node window background texture!");
			return;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		//set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//load image, create texture and generate mipmaps
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(false);
		unsigned char* data = stbi_load((backgroundPath).c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			textureID = texture;
		}
		else Engine::CreateErrorPopup("Node window setup error", "Couldn't set up node window background texture!");
		stbi_image_free(data);
	}

	void GUINodeBlock::RenderNodeBlock()
	{
		static ImVec2 lastBackgroundDragPos{};
		static ImVec2 lastNodeDragPos{};
		static bool isDraggingBackground = false;
		static bool isDraggingNode = false;

		ImGui::SetNextWindowSizeConstraints(EngineGUI::minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(EngineGUI::initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		if (renderNodeBlock
			&& ImGui::Begin("Nodeblock", NULL, windowFlags))
		{
			static float zoomFactor = 1.0f;
			static ImVec2 imagePos(0.0f, 0.0f);

			if (selectedGameObject != nullptr
				&& selectedComponent != nullptr)
			{
				string text = selectedGameObject->GetName() + " | " + selectedComponent->GetName();
				ImGui::Text(text.c_str());
			}

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				renderNodeBlock = false;
			}

			ImGuiChildFlags childWindowFlags = ImGuiWindowFlags_NoScrollbar;

			ImGui::BeginChild("NodeblockScrolling", ImVec2(0, 0), true, childWindowFlags);

			if (!Input::cameraEnabled
				&& ImGui::IsWindowHovered())
			{
				//zooming with mouse scroll wheel and ctrl key
				if (ImGui::GetIO().KeyCtrl
					&& ImGui::GetIO().MouseWheel != 0)
				{
					zoomFactor += ImGui::GetIO().MouseWheel * 0.1f;
					zoomFactor = max(zoomFactor, 0.3f);
					zoomFactor = min(zoomFactor, 5.0f);
				}

				//start dragging background
				if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)
					&& !isDraggingBackground)
				{
					isDraggingBackground = true;
					lastBackgroundDragPos = ImGui::GetMousePos();
				}
				//stop dragging background
				if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
				{
					isDraggingBackground = false;
				}

				if (isDraggingBackground)
				{
					ImVec2 currentMousePos = ImGui::GetMousePos();
					ImVec2 screenDelta = ImVec2(
						currentMousePos.x - lastBackgroundDragPos.x,
						currentMousePos.y - lastBackgroundDragPos.y);

					imagePos.x += screenDelta.x;
					imagePos.y += screenDelta.y;

					if (selectedComponent != nullptr 
						&& selectedComponent->GetNodes().size() > 0)
					{
						//update each node position when dragging background
						for (auto& node : selectedComponent->GetNodes())
						{
							if (node == selectedNode)
							{
								ImVec2 initialPos = ImVec2(node->GetInitialPos().x, node->GetInitialPos().y);
								ImVec2 newNodePos = ImVec2(
									initialPos.x + screenDelta.x / zoomFactor,
									initialPos.y + screenDelta.y / zoomFactor);
								node->SetPos(vec2(newNodePos.x, newNodePos.y));
							}
							else
							{
								vec2 nodePos = node->GetPos();
								nodePos.x += screenDelta.x / zoomFactor;
								nodePos.y += screenDelta.y / zoomFactor;
								node->SetPos(nodePos);
							}
						}
					}

					lastBackgroundDragPos = currentMousePos;
				}
			}

			ImVec2 imageSize(5000 * zoomFactor, 5000 * zoomFactor);

			ImGui::SetCursorPos(imagePos);
			ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(textureID)), imageSize);

			if (selectedComponent != nullptr)
			{
				GUINodeBlock::AddNode();

				if (selectedComponent->GetNodes().size() > 0)
				{
					bool wasNodeSelected = false;
					static ImVec2 nodePos = {};
					static ImVec2 nodeSize = {};

					//render each node
					for (const auto& node : selectedComponent->GetNodes())
					{
						string nodeWindowName = node->GetName() + to_string(node->GetID());
						string nodeWindowTitle = node->GetName() + " | " + to_string(node->GetID());
						nodePos = ImVec2(
							node->GetPos().x * zoomFactor,
							node->GetPos().y * zoomFactor);
						nodeSize = ImVec2(
							250 * zoomFactor, 
							180 * zoomFactor);

						ImGui::SetCursorPos(nodePos);

						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4 * zoomFactor, 4 * zoomFactor));
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8 * zoomFactor, 8 * zoomFactor));

						ImGui::BeginChild(nodeWindowName.c_str(), nodeSize);

						//draw background
						ImVec2 padding = ImVec2(5.0f, 5.0f);
						ImVec4 backgroundColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
						ImVec2 bgMin = ImVec2(
							ImGui::GetWindowPos().x - padding.x,
							ImGui::GetWindowPos().y - padding.y);
						ImVec2 bgMax = ImVec2(
							ImGui::GetWindowPos().x + nodeSize.x + padding.x,
							ImGui::GetWindowPos().y + nodeSize.y + padding.y);
						ImGui::GetWindowDrawList()->AddRectFilled(
							bgMin,
							bgMax,
							ImColor(backgroundColor),
							5.0f);

						//draw node circles
						if (node->GetNodeCircles().size() > 0)
						{
							for (const auto& nodeCircle : node->GetNodeCircles())
							{
								ImVec2 circlePos = {};
								float posX = ImGui::GetWindowPos().x + nodeSize.x / 2;
								posX = nodeCircle->GetSide() ==
									GUINodeCircle::Side::left ? posX - 100 * zoomFactor : posX + 100 * zoomFactor;
								float posY = ImGui::GetWindowPos().y + nodeSize.y / 2;

								switch (nodeCircle->GetSlot())
								{
								case GUINodeCircle::Slot::first:
									circlePos = ImVec2(
										posX,
										posY = posY - 35 * zoomFactor);
									break;
								case GUINodeCircle::Slot::second:
									circlePos = ImVec2(
										posX,
										posY = posY - 5 * zoomFactor);
									break;
								case GUINodeCircle::Slot::third:
									circlePos = ImVec2(
										posX,
										posY = posY + 25 * zoomFactor);
									break;
								case GUINodeCircle::Slot::fourth:
									circlePos = ImVec2(
										posX,
										posY = posY + 55 * zoomFactor);
									break;
								}
								float circleRadius = 5.0f;
								ImGui::GetWindowDrawList()->AddCircleFilled(
									circlePos,
									circleRadius,
									ImColor(255, 255, 0, 255));

								nodeCircle->SetPos(vec2(posX, posY));
								if (theStartCircle != nullptr
									&& theStartCircle == nodeCircle)
								{
									theStartCircle->SetPos(vec2(posX, posY));
								}
								if (theEndCircle != nullptr
									&& theEndCircle == nodeCircle)
								{
									theEndCircle->SetPos(vec2(posX, posY));
								}

								//calculate distance between mouse cursor and circle center
								float distance = sqrt(pow(
									ImGui::GetMousePos().x - circlePos.x, 2) 
									+ pow(ImGui::GetMousePos().y - circlePos.y, 2));

								//select circle with left mouse
								if (distance <= circleRadius
									&& ImGui::IsMouseClicked(0)
									&& !wasNodeCircleSelected)
								{
									wasNodeCircleSelected = true;

									circleCenter = circlePos;
									theStartCircle = nodeCircle;
								}

								//release curve on selected circle
								if (distance <= circleRadius
									&& ImGui::IsMouseReleased(0)
									&& wasNodeCircleSelected)
								{
									theEndCircle = nodeCircle;

									shared_ptr<GUINodeConnection> nodeConnection = GUINodeConnection::InitializeNodeConnection(
										GUINodeConnection::tempName,
										GUINodeConnection::tempID,
										theStartCircle,
										theEndCircle);
									selectedComponent->GetNodeConnections().push_back(nodeConnection);

									string startCircleName = theStartCircle->GetName() + " | " + to_string(theStartCircle->GetID());
									string endCircleName = theEndCircle->GetName() + " | " + to_string(theEndCircle->GetID());

									theStartCircle = nullptr;
									theEndCircle = nullptr;

									cout << "created new curve from " 
										<< startCircleName << " to "
										<< endCircleName << "\n";
								}

								//first bezier curve draw on top of nodes
								DrawBezierCurve();
							}
						}

						ImGui::Text(nodeWindowTitle.c_str());
						ImGui::Separator();

						ImGui::EndChild();

						ImGui::PopStyleVar(2);

						//select node
						if (ImGui::IsItemHovered()
							&& ImGui::IsItemClicked(0)
							&& !wasNodeCircleSelected)
						{
							selectedNode = node;

							wasNodeSelected = true;
						}
					}

					if (wasNodeCircleSelected
						&& ImGui::IsMouseReleased(0))
					{
						wasNodeCircleSelected = false;
						theStartCircle = nullptr;
						theEndCircle = nullptr;
					}

					//deselect node
					if (ImGui::IsMouseClicked(0)
						&& !wasNodeSelected
						&& selectedNode != nullptr)
					{
						selectedNode = nullptr;
					}

					//delete selected node with delete key
					if (selectedNode != nullptr
						&& ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
					{
						DestroyNode(selectedNode);
					}

					//start dragging node
					if (selectedNode != nullptr
						&& ImGui::IsMouseDragging(ImGuiMouseButton_Left)
						&& !isDraggingNode)
					{
						isDraggingNode = true;
						lastNodeDragPos = ImGui::GetMousePos();
					}
					//stop dragging node
					if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
					{
						isDraggingNode = false;
					}

					//drag selected node
					if (isDraggingNode)
					{
						selectedNode->SetInitialPos(selectedNode->GetPos());

						ImVec2 currentMousePos = ImGui::GetMousePos();
						ImVec2 screenDelta = ImVec2(
							currentMousePos.x - lastNodeDragPos.x,
							currentMousePos.y - lastNodeDragPos.y);

						vec2 nodePos = selectedNode->GetPos();
						nodePos.x += screenDelta.x / zoomFactor;
						nodePos.y += screenDelta.y / zoomFactor;
						selectedNode->SetPos(nodePos);

						lastNodeDragPos = currentMousePos;
					}

					//second bezier curve draw underneath nodes
					DrawBezierCurve();
				}
			}

			ImGui::EndChild();

			ImGui::End();
		}
	}

	void GUINodeBlock::DrawBezierCurve()
	{
		if (wasNodeCircleSelected)
		{
			ImVec2 mousePos = ImGui::GetMousePos();

			curveStart = ImVec2(circleCenter.x + (mousePos.x - circleCenter.x) * 0.5f, circleCenter.y);
			curveEnd = ImVec2(mousePos.x, mousePos.y);

			const int segments = 20;
			ImVec2 prevPoint = circleCenter;
			for (int i = 1; i <= segments; ++i)
			{
				float t = static_cast<float>(i) / segments;
				ImVec2 bezierPoint = ImVec2(
					(1 - t) * (1 - t) * circleCenter.x + 2 * (1 - t) * t * curveStart.x + t * t * curveEnd.x,
					(1 - t) * (1 - t) * circleCenter.y + 2 * (1 - t) * t * curveStart.y + t * t * curveEnd.y);

				ImGui::GetWindowDrawList()->AddLine(prevPoint, bezierPoint, ImColor(255, 255, 12, 255));
				prevPoint = bezierPoint;
			}
		}
		if (selectedComponent != nullptr
			&& selectedComponent->GetNodeConnections().size() > 0)
		{
			for (auto& nodeConnection : selectedComponent->GetNodeConnections())
			{
				ImVec2 start = ImVec2(
					nodeConnection->GetCurveStart()->GetPos().x,
					nodeConnection->GetCurveStart()->GetPos().y);
				ImVec2 end = ImVec2(
					nodeConnection->GetCurveEnd()->GetPos().x,
					nodeConnection->GetCurveEnd()->GetPos().y);

				const int segments = 20;
				ImVec2 prevPoint = start;
				for (int i = 1; i <= segments; ++i)
				{
					float t = static_cast<float>(i) / segments;
					ImVec2 bezierPoint = ImVec2(
						(1 - t) * (1 - t) * start.x + 2 * (1 - t) * t * start.x + t * t * end.x,
						(1 - t) * (1 - t) * start.y + 2 * (1 - t) * t * start.y + t * t * end.y);

					ImGui::GetWindowDrawList()->AddLine(prevPoint, bezierPoint, ImColor(255, 255, 12, 255));
					prevPoint = bezierPoint;
				}
			}
		}
	}

	void GUINodeBlock::AddNode()
	{
		if (!Input::cameraEnabled
			&& ImGui::IsWindowHovered()
			&& ImGui::IsMouseClicked(1))
		{
			ImGui::OpenPopup("rightclickpopup");
		}

		//right click popup for node background
		if (ImGui::BeginPopupContextItem("rightclickpopup"))
		{
			vec2 pos = vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);

			if (ImGui::MenuItem("Left 1 Right 1"))
			{
				vector<shared_ptr<GUINodeCircle>> nodeCircles;
				shared_ptr<GUINodeCircle> nodeCircleLeft1 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::left,
					GUINodeCircle::Slot::first);
				nodeCircles.push_back(nodeCircleLeft1);
				shared_ptr<GUINodeCircle> nodeCircleRight1 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::right,
					GUINodeCircle::Slot::first);
				nodeCircles.push_back(nodeCircleRight1);

				shared_ptr<GUINode> newNode = GUINode::InitializeNode(
					vec2(0),
					vec2(0),
					GUINode::tempName,
					GUINode::tempID,
					nodeCircles);

				selectedComponent->AddNode(newNode);
				newNode->SetPos(pos);
			}
			if (ImGui::MenuItem("Left 1 Right 4"))
			{
				vector<shared_ptr<GUINodeCircle>> nodeCircles;

				shared_ptr<GUINodeCircle> nodeCircleLeft1 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::left,
					GUINodeCircle::Slot::first);
				nodeCircles.push_back(nodeCircleLeft1);
				shared_ptr<GUINodeCircle> nodeCircleRight1 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::right,
					GUINodeCircle::Slot::first);
				nodeCircles.push_back(nodeCircleRight1);
				shared_ptr<GUINodeCircle> nodeCircleRight2 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::right,
					GUINodeCircle::Slot::second);
				nodeCircles.push_back(nodeCircleRight2);
				shared_ptr<GUINodeCircle> nodeCircleRight3 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::right,
					GUINodeCircle::Slot::third);
				nodeCircles.push_back(nodeCircleRight3);
				shared_ptr<GUINodeCircle> nodeCircleRight4 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::right,
					GUINodeCircle::Slot::fourth);
				nodeCircles.push_back(nodeCircleRight4);

				shared_ptr<GUINode> newNode = GUINode::InitializeNode(
					vec2(0),
					vec2(0),
					GUINode::tempName,
					GUINode::tempID,
					nodeCircles);

				selectedComponent->AddNode(newNode);
				newNode->SetPos(pos);
			}
			if (ImGui::MenuItem("Left 2 Right 2"))
			{
				vector<shared_ptr<GUINodeCircle>> nodeCircles;

				shared_ptr<GUINodeCircle> nodeCircleLeft1 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::left,
					GUINodeCircle::Slot::first);
				nodeCircles.push_back(nodeCircleLeft1);
				shared_ptr<GUINodeCircle> nodeCircleLeft2 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::left,
					GUINodeCircle::Slot::second);
				nodeCircles.push_back(nodeCircleLeft2);
				shared_ptr<GUINodeCircle> nodeCircleRight1 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::right,
					GUINodeCircle::Slot::first);
				nodeCircles.push_back(nodeCircleRight1);
				shared_ptr<GUINodeCircle> nodeCircleRight2 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::right,
					GUINodeCircle::Slot::second);
				nodeCircles.push_back(nodeCircleRight2);

				shared_ptr<GUINode> newNode = GUINode::InitializeNode(
					vec2(0),
					vec2(0),
					GUINode::tempName,
					GUINode::tempID,
					nodeCircles);

				selectedComponent->AddNode(newNode);
				newNode->SetPos(pos);
			}
			if (ImGui::MenuItem("Left 4 Right 1"))
			{
				vector<shared_ptr<GUINodeCircle>> nodeCircles;

				shared_ptr<GUINodeCircle> nodeCircleLeft1 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::left,
					GUINodeCircle::Slot::first);
				nodeCircles.push_back(nodeCircleLeft1);
				shared_ptr<GUINodeCircle> nodeCircleLeft2 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::left,
					GUINodeCircle::Slot::second);
				nodeCircles.push_back(nodeCircleLeft2);
				shared_ptr<GUINodeCircle> nodeCircleLeft3 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::left,
					GUINodeCircle::Slot::third);
				nodeCircles.push_back(nodeCircleLeft3);
				shared_ptr<GUINodeCircle> nodeCircleLeft4 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::left,
					GUINodeCircle::Slot::fourth);
				nodeCircles.push_back(nodeCircleLeft4);
				shared_ptr<GUINodeCircle> nodeCircleRight1 = GUINodeCircle::InitializeNodeCircle(
					vec2(0),
					GUINodeCircle::tempName,
					GUINodeCircle::tempID,
					GUINodeCircle::Side::right,
					GUINodeCircle::Slot::first);
				nodeCircles.push_back(nodeCircleRight1);

				shared_ptr<GUINode> newNode = GUINode::InitializeNode(
					vec2(0),
					vec2(0),
					GUINode::tempName,
					GUINode::tempID,
					nodeCircles);

				selectedComponent->AddNode(newNode);
				newNode->SetPos(pos);
			}

			ImGui::EndPopup();
		}
	}

	void GUINodeBlock::DestroyNode(const shared_ptr<GUINode>& node)
	{
		string nodeName = node->GetName() + " | " + to_string(node->GetID());

		if (selectedComponent->GetNodes().size() > 0)
		{
			vector<shared_ptr<GUINode>>& nodes = selectedComponent->GetNodes();

			//erase each connection if it is attached to a node circle on this node
			for (int i = 0; i < selectedComponent->GetNodeConnections().size();)
			{
				shared_ptr<GUINodeConnection> nodeConnection = selectedComponent->GetNodeConnections().at(i);
				shared_ptr<GUINodeCircle> start = nodeConnection->GetCurveStart();
				shared_ptr<GUINodeCircle> end = nodeConnection->GetCurveEnd();

				bool eraseConnection = false;

				for (auto& nodeCircle : node->GetNodeCircles())
				{
					if (nodeCircle == start
						|| nodeCircle == end)
					{
						eraseConnection = true;
						break;
					}
				}

				if (eraseConnection)
				{
					selectedComponent->GetNodeConnections().erase(selectedComponent->GetNodeConnections().begin() + i);
				}
				else i++;
			}

			//remove current node circles
			node->GetNodeCircles().clear();

			//remove current node from selected component nodes vector
			auto it = find(nodes.begin(), nodes.end(), node);
			if (it != nodes.end()) nodes.erase(it);
		}

		if (selectedNode == node) selectedNode = nullptr;

		cout << "deleted node " << nodeName << ", new node count is " << selectedComponent->GetNodes().size() << "\n";
	}
}
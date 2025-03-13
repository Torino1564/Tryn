#include "TrynPCH.h"
#include "ConditionalNode.h"
#include <imgui.h>
#include <imgui_stdlib.h>

namespace tryn::scr
{
	unsigned long long ConditionalNode::Execute()
	{
		if (const auto& value = pGraph->variables[varIndex].var; checkFunc(value))
		{
			const auto& info = pGraph->pinIdToInfo[pinIds[truePin]];
			return info.linked ? info.linkedId : uniqueId;
		}
		else
		{
			const auto& info = pGraph->pinIdToInfo[pinIds[falsePin]];
			return info.linked ? info.linkedId : uniqueId;
		}
	}

	void ConditionalNode::ImGuiCreate(ScriptGraph* graph, spa::Vec2I screenPos, std::function<void()>& finalBehaviour)
	{
		if(ImGui::Begin("ConditionalNodeCreate"))
		{
			static std::string name;
			if (ImGui::InputText("Node Name", &name)) {}

			static std::string varname;
			if (ImGui::InputText("Variable Name", &varname, ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsNoBlank)) {}
			static bool addNew = false;
			static bool checked = false;
			static auto s_it = graph->variables.end();
			if (ImGui::Button("Check"))
			{
				checked = true;
				const auto it = std::ranges::find_if(graph->variables, [](const auto& var)
				{
					return var.name == varname;
				} );

				if (it == graph->variables.end())
				{
					// Add new variable
					addNew = true;
					s_it = graph->variables.end();
				}
				else
				{
					addNew = false;
					s_it = it;
				}
			}
			if (checked)
			{
				if(addNew)
				{
					ImGui::Text("Variable not found in the script. Adding new variable.");
					static int currentElement = 0;
				}
				else
				{
					ImGui::Text(std::format("Found existing variable: {} - Type: {}", s_it->name, s_it->typeName).c_str());
				}
			}

			ImGui::Text("Value to compare:");
			    
			static std::variant<int, float, double, bool, std::string> inputBuffer = inputBuffer.emplace<int>();
			static const std::array items = {"int", "float", "double", "bool", "string"};
			static int currentElement = 0;
			if (ImGui::Combo("Input Type", &currentElement, items.data(), items.size()))
			{
				switch(currentElement)
				{
				case 0:
					{
						inputBuffer = {};
						inputBuffer.emplace<int>();
						break;
					}
				case 1:
					{
						inputBuffer = {};
						inputBuffer.emplace<float>();
						break;
					}
				case 2:
					{
						inputBuffer = {};
						inputBuffer.emplace<double>();
						break;
					}
				case 3:
					{
						inputBuffer = {};
						inputBuffer.emplace<bool>();
						break;
					}
				case 4:
					{
						inputBuffer = {};
						inputBuffer.emplace<std::string>();
						break;
					}
				}

			}

			switch(currentElement)
			{
			case 0:
				{
					ImGui::InputInt("Int", &std::get<int>(inputBuffer));
					break;
				}
			case 1:
				{
					ImGui::InputFloat("Float", &std::get<float>(inputBuffer));
					break;
				}
			case 2:
				{
					ImGui::InputDouble("Double", &std::get<double>(inputBuffer));
					break;
				}
			case 3:
				{
					ImGui::Checkbox("True", &std::get<bool>(inputBuffer));
					break;
				}
			case 4:
				{
					ImGui::InputText("True", &std::get<std::string>(inputBuffer));
					break;
				}
			}

			if (ImGui::Button("Create"))
			{
				switch(currentElement)
				{
				case 0:
					{
						graph->CreateNewNode(std::make_unique<ConditionalNode>(std::move(Make<int>(graph, {name}, {varname}, std::move(std::get<int>(inputBuffer))))), screenPos);
						break;
					}
				case 1:
					{
						graph->CreateNewNode(std::make_unique<ConditionalNode>(std::move(Make<float>(graph, {name}, {varname}, std::move(std::get<float>(inputBuffer))))), screenPos);
						break;
					}
				case 2:
					{
						graph->CreateNewNode(std::make_unique<ConditionalNode>(std::move(Make<double>(graph, {name}, {varname}, std::move(std::get<double>(inputBuffer))))), screenPos);
						break;
					}
				case 3:
					{
						graph->CreateNewNode(std::make_unique<ConditionalNode>(std::move(Make<bool>(graph, {name}, {varname}, std::move(std::get<bool>(inputBuffer))))), screenPos);
						break;
					}
				case 4:
					{
						graph->CreateNewNode(std::make_unique<ConditionalNode>(std::move(Make<std::string>(graph, {name}, {varname}, std::move(std::get<std::string>(inputBuffer))))), screenPos);
						break;
					}
				}

				name = {};
				varname = {};
				finalBehaviour();
				checked = false;
				addNew = false;
			}
		}
		ImGui::End();
	}

	ConditionalNode::ConditionalNode(ScriptGraph* pGraph, const std::string& name, const std::string& varName): ScriptNode(pGraph, name), varName(varName)
	{
		// Input Pin
		{
			PinInfo info{ .parentId = this->uniqueId, .name = "In", .kind = PinKind::Input, .id = pGraph->uniqueId++ };
			pGraph->pinIdToInfo.insert({info.id, info});
			pinIds.push_back(info.id);
		}

		// Output Pins
		{
			PinInfo info{ .parentId = this->uniqueId, .name = "True", .kind = PinKind::Output, .id = pGraph->uniqueId++ };
			pGraph->pinIdToInfo.insert({info.id, info});
			pinIds.push_back(info.id);
			truePin = pinIds.size() - 1;
		}
		{
			PinInfo info{ .parentId = this->uniqueId, .name = "False", .kind = PinKind::Output, .id = pGraph->uniqueId++ };
			pGraph->pinIdToInfo.insert({info.id, info});
			pinIds.push_back(info.id);
			falsePin = pinIds.size() - 1;
		}
	}
}

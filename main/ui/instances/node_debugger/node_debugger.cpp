#include "node_debugger.hpp"
#include "../../../src/module.hpp"

#include <filesystem>
#include <iostream>
#include <string>

namespace ModuleUI {

NodeEditorDebugger::NodeEditorDebugger(
    const std::string &name,
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph) {
  m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);

  m_AppWindow->m_CloseCallback = [=]() {
    Cherry::DeleteAppWindow(m_AppWindow);
    m_AppWindow->SetVisibility(false);
  };

  graph_ = graph;

  std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;

  this->ctx = vxe::get_current_context();
} // namespace ModuleUI

std::shared_ptr<Cherry::AppWindow> &NodeEditorDebugger::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<NodeEditorDebugger> NodeEditorDebugger::Create(
    const std::string &name,
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph) {
  auto instance =
      std::shared_ptr<NodeEditorDebugger>(new NodeEditorDebugger(name, graph));
  instance->SetupRenderCallback();
  return instance;
}

void NodeEditorDebugger::SetupRenderCallback() {
  auto self = shared_from_this();
  m_AppWindow->SetRenderCallback([self]() {
    if (self) {
      self->Render();
    }
  });
}

void NodeEditorDebugger::Render() {
  CherryApp.PushComponentPool(&m_ComponentPool);

  const float PANEL_W = 260.0f;
  ImVec2 avail = ImGui::GetContentRegionAvail();

  ImGui::BeginChild("##debug_left", ImVec2(PANEL_W, avail.y), true);

  ImGui::TextDisabled("Entry point");
  ImGui::SetNextItemWidth(-1);

  CherryKit::InputString("Entry", &m_EntryInput);
  if (CherryKit::ButtonText("Go").GetDataAs<bool>("isClicked")) {
    if (!m_EntryInput.empty()) {
      m_History.clear();
      m_CurrentNode = m_EntryInput;
    }
  }

  ImGui::SameLine();
  if (ImGui::SmallButton("Go")) {
    if (!m_EntryInput.empty()) {
      m_History.clear();
      m_CurrentNode = m_EntryInput;
    }
  }

  ImGui::Separator();
  ImGui::TextDisabled("History");

  for (int i = 0; i < (int)m_History.size(); ++i) {
    std::string label = std::to_string(i) + "  " + m_History[i].substr(0, 18);
    if (ImGui::Selectable(label.c_str(), false)) {
      m_CurrentNode = m_History[i];
      m_History.resize(i);
    }
  }

  ImGui::EndChild();

  ImGui::SameLine();

  ImGui::BeginChild("##debug_right", ImVec2(avail.x - PANEL_W - 8.0f, avail.y),
                    false);

  if (m_CurrentNode.empty()) {
    ImGui::TextDisabled("Enter an instance ID on the left to start.");
    ImGui::EndChild();
    CherryApp.PopComponentPool();
    return;
  }

  const NodeEdit::NodeEditInstance *inst = nullptr;
  if (graph_) {
    for (const auto &n : graph_->graph.instances)
      if (n.instance_id == m_CurrentNode) {
        inst = &n;
        break;
      }
  }

  if (!inst) {
    ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Node not found: %s",
                       m_CurrentNode.c_str());
    ImGui::EndChild();
    CherryApp.PopComponentPool();
    return;
  }

  ImGui::TextColored(ImVec4(0.4f, 0.9f, 0.6f, 1.0f), "[%s]",
                     inst->type_id.c_str());
  ImGui::SameLine();
  ImGui::TextDisabled("%s", inst->instance_id.c_str());

  ImGui::Separator();

  ImGui::TextDisabled("pos  ");
  ImGui::SameLine();
  ImGui::Text("%.1f, %.1f", inst->pos_x, inst->pos_y);
  ImGui::TextDisabled("size ");
  ImGui::SameLine();
  ImGui::Text("%.1f x %.1f", inst->size_x, inst->size_y);

  ImGui::Spacing();

  auto inputPins = NodeEdit::GetAllNodeInputPins(graph_, m_CurrentNode);
  auto outputPins = NodeEdit::GetAllNodeOutputPins(graph_, m_CurrentNode);

  auto navigateTo = [&](const std::string &next_id) {
    if (!next_id.empty()) {
      m_History.push_back(m_CurrentNode);
      m_CurrentNode = next_id;
    }
  };

  ImGui::BeginGroup();
  ImGui::TextDisabled("  Inputs (%zu)", inputPins.size());
  for (const auto &[type, id] : inputPins) {
    std::string prev = NodeEdit::GetPreviousNode(graph_, m_CurrentNode, id);
    bool connected = !prev.empty();

    ImGui::PushStyleColor(ImGuiCol_Text, connected
                                             ? ImVec4(0.9f, 0.75f, 0.3f, 1)
                                             : ImVec4(0.45f, 0.45f, 0.45f, 1));

    std::string btn = "< " + type + "##in_" + id;
    if (ImGui::SmallButton(btn.c_str()) && connected)
      navigateTo(prev);

    ImGui::PopStyleColor();

    if (connected && ImGui::IsItemHovered()) {
      ImGui::SetTooltip("-> %s", prev.c_str());
    } else if (!connected && ImGui::IsItemHovered()) {
      ImGui::SetTooltip("(not connected)");
    }
  }
  ImGui::EndGroup();

  ImGui::SameLine(0, 40.0f);

  ImGui::BeginGroup();
  ImGui::TextDisabled("  Outputs (%zu)", outputPins.size());
  for (const auto &[type, id] : outputPins) {
    std::string next = NodeEdit::GetNextNode(graph_, m_CurrentNode, id);
    bool connected = !next.empty();

    ImGui::PushStyleColor(ImGuiCol_Text, connected
                                             ? ImVec4(0.4f, 0.75f, 1.0f, 1)
                                             : ImVec4(0.45f, 0.45f, 0.45f, 1));

    std::string btn = type + "> ##out_" + id;
    if (ImGui::SmallButton(btn.c_str()) && connected)
      navigateTo(next);

    ImGui::PopStyleColor();

    if (connected && ImGui::IsItemHovered()) {
      ImGui::SetTooltip("-> %s", next.c_str());
    } else if (!connected && ImGui::IsItemHovered()) {
      ImGui::SetTooltip("(not connected)");
    }
  }
  ImGui::EndGroup();

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::TextDisabled("datas");
  std::string json_str = inst->datas.dump(2);
  ImGui::InputTextMultiline("##json", const_cast<char *>(json_str.c_str()),
                            json_str.size() + 1,
                            ImVec2(-1, avail.y - ImGui::GetCursorPosY() - 8.0f),
                            ImGuiInputTextFlags_ReadOnly);

  ImGui::EndChild();
  CherryApp.PopComponentPool();
}

}; // namespace ModuleUI

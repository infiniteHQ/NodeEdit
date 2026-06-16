#include "node_debugger.hpp"

#include <filesystem>
#include <iostream>
#include <string>

#include "../../../src/module.hpp"

namespace ModuleUI {

  NodeEditorDebugger::NodeEditorDebugger(const std::string &name, const std::shared_ptr<NodeEdit::GraphSession> &graph) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);

    app_window_->m_CloseCallback = [=]() {
      Cherry::DeleteAppWindow(app_window_);
      app_window_->SetVisibility(false);
    };

    graph_ = graph;

    std::shared_ptr<Cherry::AppWindow> win = app_window_;

    this->ctx = vxe::get_current_context();
  }  // namespace ModuleUI

  std::shared_ptr<Cherry::AppWindow> &NodeEditorDebugger::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<NodeEditorDebugger> NodeEditorDebugger::create(
      const std::string &name,
      const std::shared_ptr<NodeEdit::GraphSession> &graph) {
    auto instance = std::shared_ptr<NodeEditorDebugger>(new NodeEditorDebugger(name, graph));
    instance->setup_render_callback();
    return instance;
  }

  void NodeEditorDebugger::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  void NodeEditorDebugger::render() {
    CherryApp.PushComponentPool(&component_pool_);

    const float PANEL_W = 260.0f;
    ImVec2 avail = ImGui::GetContentRegionAvail();

    ImGui::BeginChild("##debug_left", ImVec2(PANEL_W, avail.y), true);

    ImGui::TextDisabled("Entry point");
    ImGui::SetNextItemWidth(-1);
    CherryKit::InputString("Entry", &m_EntryInput);
    ImGui::SameLine();
    if (CherryKit::ButtonText("Go").GetDataAs<bool>("isClicked")) {
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
    ImGui::BeginChild("##debug_right", ImVec2(avail.x - PANEL_W - 8.0f, avail.y), false);

    if (m_CurrentNode.empty()) {
      ImGui::TextDisabled("Enter an instance ID on the left to start.");
      ImGui::EndChild();
      CherryApp.PopComponentPool();
      return;
    }

    std::vector<std::pair<std::string, std::string>> inputPins;
    std::vector<std::pair<std::string, std::string>> outputPins;

    {
      nlohmann::json j;
      j["session_id"] = graph_->session_id;
      j["node_id"] = m_CurrentNode;
      auto args = ArgumentValues(j.dump());

      auto retIn = ReturnValues();
      vxe::call_input_event("infinitehq.nodeedit", "get_all_node_input_pins", args, retIn);
      auto rjIn = retIn.get_json();
      if (rjIn.contains("input_pins") && rjIn["input_pins"].is_array())
        for (const auto &pin : rjIn["input_pins"])
          inputPins.emplace_back(pin.value("pin_type", ""), pin.value("pin_id", ""));

      auto retOut = ReturnValues();
      vxe::call_input_event("infinitehq.nodeedit", "get_all_node_outputs_pins", args, retOut);
      auto rjOut = retOut.get_json();
      if (rjOut.contains("output_pins") && rjOut["output_pins"].is_array())
        for (const auto &pin : rjOut["output_pins"])
          outputPins.emplace_back(pin.value("pin_type", ""), pin.value("pin_id", ""));
    }

    if (inputPins.empty() && outputPins.empty()) {
      ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Node not found: %s", m_CurrentNode.c_str());
      ImGui::EndChild();
      CherryApp.PopComponentPool();
      return;
    }

    std::vector<std::vector<std::string>> prevNodes(inputPins.size());
    for (size_t i = 0; i < inputPins.size(); ++i) {
      nlohmann::json j;
      j["session_id"] = graph_->session_id;
      j["node_id"] = m_CurrentNode;
      j["input_id"] = inputPins[i].second;
      auto args = ArgumentValues(j.dump());
      auto ret = ReturnValues();
      vxe::call_input_event("infinitehq.nodeedit", "get_previous_nodes", args, ret);
      auto rj = ret.get_json();
      std::cout << rj << std::endl;
      if (rj.contains("node_ids") && rj["node_ids"].is_array())
        prevNodes[i] = rj["node_ids"].get<std::vector<std::string>>();
    }

    std::vector<std::vector<std::string>> nextNodes(outputPins.size());
    for (size_t i = 0; i < outputPins.size(); ++i) {
      nlohmann::json j;
      j["session_id"] = graph_->session_id;
      j["node_id"] = m_CurrentNode;
      j["output_id"] = outputPins[i].second;
      auto args = ArgumentValues(j.dump());
      auto ret = ReturnValues();
      vxe::call_input_event("infinitehq.nodeedit", "get_next_nodes", args, ret);
      auto rj = ret.get_json();
      std::cout << rj << std::endl;
      if (rj.contains("node_ids") && rj["node_ids"].is_array())
        nextNodes[i] = rj["node_ids"].get<std::vector<std::string>>();
    }

    ImGui::TextDisabled("%s", m_CurrentNode.c_str());
    ImGui::Separator();
    ImGui::Spacing();

    auto navigateTo = [&](const std::string &next_id) {
      if (!next_id.empty()) {
        m_History.push_back(m_CurrentNode);
        m_CurrentNode = next_id;
      }
    };

    ImGui::BeginGroup();
    ImGui::TextDisabled("  Inputs (%zu)", inputPins.size());
    for (size_t i = 0; i < inputPins.size(); ++i) {
      const auto &[type, id] = inputPins[i];
      const auto &prevList = prevNodes[i];
      bool connected = !prevList.empty();

      ImGui::PushStyleColor(ImGuiCol_Text, connected ? ImVec4(0.9f, 0.75f, 0.3f, 1) : ImVec4(0.45f, 0.45f, 0.45f, 1));
      std::string btn = "< " + type + "##in_" + id;
      if (ImGui::SmallButton(btn.c_str()) && connected)
        navigateTo(prevList[0]);
      ImGui::PopStyleColor();

      if (connected) {
        for (size_t k = 0; k < prevList.size(); ++k) {
          const std::string &prev = prevList[k];
          ImGui::SameLine(0, k == 0 ? -1 : 4);
          std::string label = prev.size() > 22 ? prev.substr(0, 20) + ".." : prev;
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.75f, 0.3f, 0.75f));
          std::string nodeBtn = label + "##innav_" + id + "_" + std::to_string(k);
          if (ImGui::SmallButton(nodeBtn.c_str()))
            navigateTo(prev);
          ImGui::PopStyleColor();
          if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", prev.c_str());
        }
      } else {
        ImGui::SameLine();
        ImGui::TextDisabled("-");
      }
    }
    ImGui::EndGroup();

    ImGui::SameLine(0, 40.0f);

    ImGui::BeginGroup();
    ImGui::TextDisabled("  Outputs (%zu)", outputPins.size());
    for (size_t i = 0; i < outputPins.size(); ++i) {
      const auto &[type, id] = outputPins[i];
      const auto &nextList = nextNodes[i];
      bool connected = !nextList.empty();

      ImGui::PushStyleColor(ImGuiCol_Text, connected ? ImVec4(0.4f, 0.75f, 1.0f, 1) : ImVec4(0.45f, 0.45f, 0.45f, 1));
      std::string btn = type + "> ##out_" + id;
      if (ImGui::SmallButton(btn.c_str()) && connected)
        navigateTo(nextList[0]);
      ImGui::PopStyleColor();

      if (connected) {
        for (size_t k = 0; k < nextList.size(); ++k) {
          const std::string &next = nextList[k];
          ImGui::SameLine(0, k == 0 ? -1 : 4);
          std::string label = next.size() > 22 ? next.substr(0, 20) + ".." : next;
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.75f, 1.0f, 0.75f));
          std::string nodeBtn = label + "##outnav_" + id + "_" + std::to_string(k);
          if (ImGui::SmallButton(nodeBtn.c_str()))
            navigateTo(next);
          ImGui::PopStyleColor();
          if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", next.c_str());
        }
      } else {
        ImGui::SameLine();
        ImGui::TextDisabled("-");
      }
    }
    ImGui::EndGroup();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::EndChild();
    CherryApp.PopComponentPool();
  }
};  // namespace ModuleUI

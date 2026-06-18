//
//  node_editor_rendering.cpp
//  Source file for main rendering functions of node editor window.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "node_editor.hpp"

#include <filesystem>
#include <iostream>
#include <string>

#include "../../../src/module.hpp"

namespace ModuleUI {

  static const std::unordered_map<std::string, Cherry::NodeSystem::PinShape> shape_map = {
    { "flow", Cherry::NodeSystem::PinShape::Flow },
    { "circle", Cherry::NodeSystem::PinShape::Circle },
    { "square", Cherry::NodeSystem::PinShape::Square },
    { "grid", Cherry::NodeSystem::PinShape::Grid },
    { "roundsquare", Cherry::NodeSystem::PinShape::RoundSquare },
    { "diamond", Cherry::NodeSystem::PinShape::Diamond }
  };

  static const std::unordered_map<std::string, Cherry::NodeSystem::NodeType> type_map = {
    { "blueprint", Cherry::NodeSystem::NodeType::Blueprint },
    { "comment", Cherry::NodeSystem::NodeType::Comment },
    { "houdini", Cherry::NodeSystem::NodeType::Houdini },
    { "simple", Cherry::NodeSystem::NodeType::Simple },
    { "tree", Cherry::NodeSystem::NodeType::Tree }
  };

  static const std::unordered_map<std::string, Cherry::NodeSystem::NodeSchemaStatus> status_map = {
    { "active", Cherry::NodeSystem::NodeSchemaStatus::Active },
    { "depreciated", Cherry::NodeSystem::NodeSchemaStatus::Depreciated },
    { "disabled", Cherry::NodeSystem::NodeSchemaStatus::Disabled },
    { "obsolete", Cherry::NodeSystem::NodeSchemaStatus::Obsolete }
  };

  NodeEditorAppWindow::NodeEditorAppWindow(
      const std::string &name,
      const std::shared_ptr<NodeEdit::NodeContext> &ctx,
      const std::shared_ptr<NodeEdit::GraphSession> &graph) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);

    if (graph) {
      if (!graph->disable_native_save_system) {
        app_window_->SetLeftMenubarCallback([this]() { render_menubar(); });
        app_window_->SetRightMenubarCallback([this]() { render_right_menubar(); });
      }
    }

    app_window_->SetSaveMode(true);

    app_window_->m_CloseCallback = [=]() {
      Cherry::DeleteAppWindow(app_window_);
      app_window_->SetVisibility(false);
    };

    backend_node_ctx_ = ctx;
    load_node_context_from_backend();
    backend_node_graph_session_ = graph;

    if (backend_node_graph_session_->logo_path.empty()) {
      if (backend_node_graph_session_->parent_appwindow.empty()) {
        app_window_->SetIcon(NodeEdit::get_path("resources/icons/edit.png"));
      } else {
        app_window_->SetIcon(NodeEdit::get_path("resources/icons/edit_gray.png"));
      }
    } else {
      app_window_->SetIcon(backend_node_graph_session_->logo_path);
    }

    ui_node_graph_.m_NodeSpawnCallback = [this](const std::string &sch_id, float x, float y, const std::string &connID) {
      spawn_node_instance(sch_id, x, y, connID);
    };

    refresh();

    std::shared_ptr<Cherry::AppWindow> win = app_window_;
  }

  void NodeEditorAppWindow::refresh_context_from_backend() {
    load_node_context_from_backend();
  }

  void NodeEditorAppWindow::load_node_context_from_backend() {
    if (!backend_node_ctx_) {
      return;
    }
    auto &pin_formats = backend_node_ctx_->pin_formats;
    auto &schemas = backend_node_ctx_->schemas;

    for (auto &pf : pin_formats) {
      Cherry::NodeSystem::PinFormat pin_format;
      pin_format.m_Color = pf.color;
      pin_format.m_Delegate = pf.delegate;

      auto it = shape_map.find(pf.shape);
      if (it != shape_map.end()) {
        pin_format.m_Shape = it->second;
      }

      pin_format.m_TypeDescription = pf.description;
      pin_format.m_TypeID = pf.type;
      pin_format.m_TypeName = pf.name;
      ui_node_ctx_.SetupPinFormat(pin_format);
    }

    for (auto &s : schemas) {
      auto sch = ui_node_ctx_.CreateSchema(s.id);
      sch->SetLabel(s.label);
      sch->SetLabelHexColor(s.label_color);
      sch->SetSecondLabel(s.second_label);
      sch->SetSecondLabelHexColor(s.second_label_color);
      sch->SetDescriptionHexColor(s.description_color);
      sch->SetHexBackgroundColor(s.background_color);
      sch->SetHexBorderColor(s.border_color);
      sch->SetHexHeaderColor(s.header_color);

      {
        auto it = status_map.find(s.status);
        if (it != status_map.end()) {
          sch->m_NodeStatus = it->second;
        }
      }

      {
        auto it = type_map.find(s.type);
        if (it != type_map.end()) {
          sch->SetType(it->second);
        }
      }

      if (!s.header_logo_path.empty()) {
        sch->SetLogoPath(s.header_logo_path);
      }

      for (auto &sip : s.input_pins) {
        sch->AddInputPin(sip.id, sip.type, sip.name);
      }

      for (auto &sop : s.output_pins) {
        sch->AddOutputPin(sop.id, sop.type, sop.name);
      }

      if (!s.header_pin.id.empty()) {
        sch->AddHeaderPin(s.header_pin.id, s.header_pin.type, s.header_pin.name);
      }

      if (s.spawnable) {
        Cherry::NodeSystem::NodeSpawnPossibility p;
        p.category = s.spawn_possibility.category;
        p.schema_id = s.spawn_possibility.schema_id;
        p.proper_logo = s.spawn_possibility.proper_logo;
        p.proper_name = s.spawn_possibility.proper_name;
        p.proper_description = s.spawn_possibility.proper_description;
        ui_node_graph_.AddPossibility(p);
      }
    }

    // Load graph context extensions
    if (!backend_node_graph_session_) {
      return;
    }
    auto &ext_pin_formats = backend_node_graph_session_->graph.ext.pin_formats;
    auto &ext_schemas = backend_node_graph_session_->graph.ext.schemas;

    for (auto &pf : ext_pin_formats) {
      Cherry::NodeSystem::PinFormat pin_format;
      pin_format.m_Color = pf.color;
      pin_format.m_Delegate = pf.delegate;

      auto it = shape_map.find(pf.shape);
      if (it != shape_map.end()) {
        pin_format.m_Shape = it->second;
      }

      pin_format.m_TypeDescription = pf.description;
      pin_format.m_TypeID = pf.type;
      pin_format.m_TypeName = pf.name;
      ui_node_ctx_.SetupPinFormat(pin_format);
    }

    for (auto &s : ext_schemas) {
      auto sch = ui_node_ctx_.CreateSchema(s.id);
      sch->SetLabel(s.label);
      sch->SetLabelHexColor(s.label_color);
      sch->SetSecondLabel(s.second_label);
      sch->SetSecondLabelHexColor(s.second_label_color);
      sch->SetDescriptionHexColor(s.description_color);
      sch->SetHexBackgroundColor(s.background_color);
      sch->SetHexBorderColor(s.border_color);
      sch->SetHexHeaderColor(s.header_color);

      {
        auto it = status_map.find(s.status);
        if (it != status_map.end()) {
          sch->m_NodeStatus = it->second;
        }
      }

      {
        auto it = type_map.find(s.type);
        if (it != type_map.end()) {
          sch->SetType(it->second);
        }
      }

      if (!s.header_logo_path.empty()) {
        sch->SetLogoPath(s.header_logo_path);
      }

      for (auto &sip : s.input_pins) {
        sch->AddInputPin(sip.id, sip.type, sip.name);
      }

      for (auto &sop : s.output_pins) {
        sch->AddOutputPin(sop.id, sop.type, sop.name);
      }

      if (!s.header_pin.id.empty()) {
        sch->AddHeaderPin(s.header_pin.id, s.header_pin.type, s.header_pin.name);
      }

      if (s.spawnable) {
        Cherry::NodeSystem::NodeSpawnPossibility p;
        p.category = s.spawn_possibility.category;
        p.schema_id = s.spawn_possibility.schema_id;
        p.proper_logo = s.spawn_possibility.proper_logo;
        p.proper_name = s.spawn_possibility.proper_name;
        p.proper_description = s.spawn_possibility.proper_description;
        ui_node_graph_.AddPossibility(p);
      }
    }
  }

  void NodeEditorAppWindow::refresh() {
    if (!backend_node_graph_session_) {
      return;
    }

    NodeEdit::refresh_graph_session(backend_node_graph_session_);

    for (const auto &extpf : backend_node_graph_session_->graph.ext.pin_formats) {
      NodeEdit::PinFormat pf;
      pf.type = extpf.type;
      pf.name = extpf.name;
      pf.color = extpf.color;
      pf.shape = extpf.shape;
      pf.description = extpf.description;
      NodeEdit::add_pin_format_to_graph_ctx_ext(backend_node_graph_session_, pf);
    }

    for (const auto &exts : backend_node_graph_session_->graph.ext.schemas) {
      NodeEdit::Schema s;
      s.id = exts.id;
      s.type = exts.type;
      s.status = exts.status;
      s.second_label_color = exts.second_label_color;
      s.second_label = exts.second_label;
      s.label = exts.label;
      s.label_color = exts.label_color;
      s.background_color = exts.background_color;

      s.header_color = exts.header_color;
      s.header_logo_path = exts.header_logo_path;
      s.header_pin.id = exts.header_pin.id;
      s.header_pin.name = exts.header_pin.name;
      s.header_pin.type = exts.header_pin.type;

      for (const auto &extpi : exts.input_pins) {
        NodeEdit::Pin pi;
        pi.id = extpi.id;
        pi.name = extpi.name;
        pi.type = extpi.type;
        s.input_pins.push_back(pi);
      }

      for (const auto &extpo : exts.output_pins) {
        NodeEdit::Pin po;
        po.id = extpo.id;
        po.name = extpo.name;
        po.type = extpo.type;
        s.output_pins.push_back(po);
      }

      if (exts.spawnable) {
        s.spawnable = exts.spawnable;
        s.spawn_possibility.category = exts.spawn_possibility.category;
        s.spawn_possibility.proper_description = exts.spawn_possibility.proper_description;
        s.spawn_possibility.proper_logo = exts.spawn_possibility.proper_logo;
        s.spawn_possibility.proper_name = exts.spawn_possibility.proper_name;
        s.spawn_possibility.schema_id = exts.spawn_possibility.schema_id;
      }

      NodeEdit::add_schema_to_graph_ctx_ext(backend_node_graph_session_, s);
    }

    auto &instances = backend_node_graph_session_->graph.instances;
    auto &connections = backend_node_graph_session_->graph.connections;

    ui_node_graph_.m_InstanciatedNodes.clear();
    for (auto &i : instances) {
      Cherry::NodeSystem::NodeInstance n;
      n.InstanceID = i.instance_id;
      n.Position.x = i.pos_x;
      n.Position.y = i.pos_y;
      n.Size.x = i.size_x;
      n.Size.y = i.size_y;
      n.TypeID = i.type_id;
      // TODO custom data of nodes
      ui_node_graph_.m_InstanciatedNodes.push_back(n);
      node_instances_counter_++;
    }

    ui_node_graph_.m_Connections.clear();
    for (auto &conn : connections) {
      Cherry::NodeSystem::NodeConnection c;
      c.NodeInstanceIDA = conn.node_instance_id_A;
      c.NodeInstanceIDB = conn.node_instance_id_B;
      c.PinIDA = conn.pin_id_A;
      c.PinIDB = conn.pin_id_B;
      ui_node_graph_.m_Connections.push_back(c);
    }

    refreshed_ = true;
  }

  void NodeEditorAppWindow::save() {
    if (!backend_node_graph_session_) {
      return;
    }

    auto &instances = backend_node_graph_session_->graph.instances;
    auto &connections = backend_node_graph_session_->graph.connections;

    connections.clear();
    instances.clear();

    for (auto n : ui_node_graph_.m_InstanciatedNodes) {
      NodeEdit::NodeInstance i;

      i.type_id = n.TypeID;
      i.instance_id = n.InstanceID;
      i.pos_x = n.Position.x;
      i.pos_y = n.Position.y;
      i.size_x = n.Size.x;
      i.size_y = n.Size.y;
      // TODO custom data of nodes

      instances.push_back(i);
    }

    for (auto c : ui_node_graph_.m_Connections) {
      NodeEdit::Connection conn;

      conn.node_instance_id_A = c.NodeInstanceIDA;
      conn.node_instance_id_B = c.NodeInstanceIDB;
      conn.pin_id_A = c.PinIDA;
      conn.pin_id_B = c.PinIDB;

      connections.push_back(conn);
    }

    NodeEdit::save_graph_session(backend_node_graph_session_);
  }

  // TODO : Optimize that with a local unordered_set ?
  std::string NodeEditorAppWindow::generate_unique_node_instance_id() {
    static thread_local std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 999999);

    while (true) {
      const int randomPart = dist(rng);

      std::ostringstream oss;
      oss << std::setw(6) << std::setfill('0') << randomPart << std::setw(6) << std::setfill('0')
          << ++node_instances_counter_;

      const std::string candidate = oss.str();

      const bool exists = std::any_of(
          ui_node_graph_.m_InstanciatedNodes.begin(), ui_node_graph_.m_InstanciatedNodes.end(), [&](const auto &ni) {
            return ni.InstanceID == candidate;
          });

      if (!exists)
        return candidate;
    }
  }

  void NodeEditorAppWindow::focus_at(const float &x, const float &y, const float &zoom) {
    ui_node_graph_.FocusAt(x, y, zoom);
  }

  void NodeEditorAppWindow::spawn_node_instance(
      const std::string &sch_id,
      const float &x,
      const float &y,
      const std::string &connID) {
    // save local UI state from the edit graph pos
    Cherry::NodeSystem::NodeInstance inst;
    inst.TypeID = sch_id;
    inst.InstanceID = generate_unique_node_instance_id();
    inst.Position = Cherry::NodeSystem::Vec2(50, 50);
    inst.Size = Cherry::NodeSystem::Vec2(50, 50);

    if (!connID.empty()) {
      // TODO: Auto connect
    }

    ui_node_graph_.AddNodeInstance(inst);

    refreshed_ = true;
  }
};  // namespace ModuleUI

//
//  node_editor_rendering.cpp
//  Source file for main rendering functions of node editor window.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <filesystem>
#include <iostream>
#include <string>

#include "../../../src/module.hpp"
#include "node_editor.hpp"

namespace ModuleUI {
  std::shared_ptr<Cherry::AppWindow> &NodeEditorAppWindow::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<NodeEditorAppWindow> NodeEditorAppWindow::create(
      const std::string &name,
      const std::shared_ptr<NodeEdit::NodeContext> &ctx,
      const std::shared_ptr<NodeEdit::GraphSession> &graph) {
    auto instance = std::shared_ptr<NodeEditorAppWindow>(new NodeEditorAppWindow(name, ctx, graph));
    instance->setup_render_callback();
    return instance;
  }

  void NodeEditorAppWindow::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  void NodeEditorAppWindow::render_menubar() {
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 3.0f);

    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    if (CherryKit::ButtonImageText("Save", NodeEdit::get_path("/resources/icons/icon_save.png"))
            .GetDataAs<bool>("isClicked")) {
      save_incoming_ = true;
    }

    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    if (CherryKit::ButtonImageText("Refresh", NodeEdit::get_path("/resources/icons/icon_refresh.png"))
            .GetDataAs<bool>("isClicked")) {
      refresh();
    }
  }

  void NodeEditorAppWindow::render() {
    if (!app_window_render_initialized_) {
      if (!backend_node_graph_session_->parent_appwindow.empty()) {
        auto parent = Cherry::GetAppWindowByName(backend_node_graph_session_->parent_appwindow);
        if (parent) {
          app_window_->SetParent(parent);
          app_window_->m_WindowRebuilded = false;
        }
      }

      app_window_render_initialized_ = true;
    }

    if (backend_node_graph_session_) {
      if (backend_node_graph_session_->graph.zoom_request.pending) {
        focus_at(
            backend_node_graph_session_->graph.zoom_request.x,
            backend_node_graph_session_->graph.zoom_request.y,
            backend_node_graph_session_->graph.zoom_request.zoom);
        backend_node_graph_session_->graph.zoom_request.pending = false;
      }

      if (backend_node_graph_session_->graph.spawn_request.pending) {
        spawn_node_instance(
            backend_node_graph_session_->graph.spawn_request.sch_id,
            backend_node_graph_session_->graph.spawn_request.x,
            backend_node_graph_session_->graph.spawn_request.y,
            backend_node_graph_session_->graph.spawn_request.conn_id);
        backend_node_graph_session_->graph.spawn_request.pending = false;
      }

      if (!backend_node_graph_session_->graph.graph_title.empty()) {
        ui_node_graph_.SetGraphTitle(backend_node_graph_session_->graph.graph_title);
      }

      if (backend_node_graph_session_->disable_native_save_system) {
        if (backend_node_graph_session_->ask_for_refresh) {
          refresh();
          backend_node_graph_session_->ask_for_refresh = false;
        }

        if (backend_node_graph_session_->ask_for_save) {
          save_incoming_ = true;
          backend_node_graph_session_->ask_for_save = false;
        }
      }
    }

    CherryStyle::AddMarginX(0.0f);
    CherryStyle::AddMarginY(3.0f);

    CherryApp.PushComponentPool(&component_pool_);
    auto &cmp = CherryKit::NodeAreaOpen("", 0, 0, &ui_node_ctx_, &ui_node_graph_);
    if (refreshed_) {
      cmp.SetProperty("refresh", "true");
      refreshed_ = false;
    }
    if (save_incoming_) {
      cmp.SetProperty("save", "true");
      save_incoming_ = false;
    }
    if (cmp.GetDataAs<bool>("graph_saved")) {
      save();
      cmp.SetData("graph_saved", "false;");
    }

    if (backend_node_graph_session_) {
      if (backend_node_graph_session_->graph.refresh_ctx) {
        refresh_context_from_backend();
        backend_node_graph_session_->graph.refresh_ctx = false;
      }

      if (backend_node_graph_session_->graph.refresh_effects) {
        ui_node_graph_.ClearAllEffects();

        for (const auto &e : backend_node_graph_session_->graph.node_effects) {
          Cherry::NodeSystem::NodeEffect ne;
          ne.InstanceID = e.instance_id;

          if (e.type == "message") {
            ne.params.type = Cherry::NodeSystem::EffectType::Node::Message;
          }

          ne.params.messageText = e.text;
          ne.params.messageColor = e.bg_color;
          ne.params.textColor = e.text_color;
          ui_node_graph_.AddNodeEffect(ne);
        }

        for (const auto &e : backend_node_graph_session_->graph.connection_effects) {
          Cherry::NodeSystem::ConnectionEffect ce;
          ce.NodeInstanceIDA = e.node_instance_id_A;
          ce.PinIDA = e.pin_id_A;
          ce.NodeInstanceIDB = e.node_instance_id_B;
          ce.PinIDB = e.pin_id_B;
          if (e.type == "flow") {
            ce.params.type = Cherry::NodeSystem::EffectType::Connection::Flow;
            ce.params.flowColor = e.flow_color;
            ce.params.flowSpeed = e.flow_speed;
            ce.params.flowIntensity = e.flow_intensity;
            ce.params.flowReverse = e.flow_reverse;
          } else if (e.type == "pulse") {
            ce.params.type = Cherry::NodeSystem::EffectType::Connection::Pulsating;
            ce.params.pulsatingColor = e.pulsating_color;
            ce.params.pulsatingRate = e.pulsating_rate;
            ce.params.pulsatingIntensity = e.pulsating_intensity;
          }
          ui_node_graph_.AddConnectionEffect(ce);
        }

        backend_node_graph_session_->graph.refresh_effects = false;
      }
    }

    CherryApp.PopComponentPool();
  }

  void NodeEditorAppWindow::render_right_menubar() {
    //
  }

  void NodeEditorAppWindow::render_bottombar() {
    //

  };
}  // namespace ModuleUI
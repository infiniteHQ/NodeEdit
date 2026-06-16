//
//  node_editor.hpp
//  Header file for main node editor window, with backend and fronted support of nodegraphs
//  and node contexts.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <vxcore/include/vortex.h>
#include <vxcore/include/vortex_internals.h>

#include "../../../src/helpers.hpp"

#ifndef NODE_EDITOR_HPP
#define NODE_EDITOR_HPP

namespace ModuleUI {
  class NodeEditorAppWindow : public std::enable_shared_from_this<NodeEditorAppWindow> {
   public:
    NodeEditorAppWindow(
        const std::string &name,
        const std::shared_ptr<NodeEdit::NodeContext> &ctx,
        const std::shared_ptr<NodeEdit::GraphSession> &graph);

    // UI
    static std::shared_ptr<NodeEditorAppWindow> create(
        const std::string &name,
        const std::shared_ptr<NodeEdit::NodeContext> &ctx,
        const std::shared_ptr<NodeEdit::GraphSession> &graph);
    void menubar();
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    void setup_render_callback();
    void render();
    void render_menubar();
    void render_right_menubar();
    void render_bottombar();
    void focus_at(const float &x, const float &y, const float &zoom = -1);

    // Main utilities
    void load_graph(const std::string &json_path);
    void load_node_context_from_backend();
    void refresh_context_from_backend();
    void refresh();
    void save();
    void spawn_node_instance(const std::string &sch_id, const float &x, const float &y, const std::string &connID);

    // Misc utils
    std::string generate_unique_node_instance_id();

   private:
    int node_instances_counter_ = 1;

    bool refreshed_ = false;
    bool save_incoming_ = false;
    bool app_window_render_initialized_ = false;

    // Backend
    std::shared_ptr<NodeEdit::GraphSession> backend_node_graph_session_;
    std::shared_ptr<NodeEdit::NodeContext> backend_node_ctx_;

    // UI  NodeSystem
    Cherry::NodeSystem::NodeContext ui_node_ctx_;
    Cherry::NodeSystem::NodeGraph ui_node_graph_;

    // Cherry
    std::shared_ptr<Cherry::AppWindow> app_window_;
    ComponentsPool component_pool_;
  };
};  // namespace ModuleUI

#endif  // LOGUTILITY_H

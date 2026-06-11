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
        const std::shared_ptr<NodeEdit::NodeEditContext> &ctx,
        const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph);

    void menubar();
    std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
    static std::shared_ptr<NodeEditorAppWindow> Create(
        const std::string &name,
        const std::shared_ptr<NodeEdit::NodeEditContext> &ctx,
        const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph);
    void SetupRenderCallback();
    void Render();
    void RenderMenubar();
    void RenderRightMenubar();
    void RenderBottombar();

    void LoadGraph(const std::string &json_path);
    // void LoadGraph(const json& v);

    void Refresh();
    void Save();

    void load_node_contextFromBackend();
    void RefreshContextFromBackend();
    void SpawnNodeInstance(const std::string &sch_id, const float &x, const float &y, const std::string &connID);

    std::string GenerateUniqueNodeInstanceID();

   private:
    int node_instances_counter_ = 1;
    std::shared_ptr<VxContext> ctx;
    bool opened;
    // Cherry
    std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    ComponentsPool m_ComponentPool;
    bool m_FileEdited;
    bool m_RefreshReady;
    bool m_SavePending;

    bool refreshed = false;
    bool save_incoming = false;

    // Backend
    std::shared_ptr<NodeEdit::NodeEditGraphSession> backend_node_graph_session;
    std::shared_ptr<NodeEdit::NodeEditContext> backend_node_ctx;

    // UI  NodeSystem
    Cherry::NodeSystem::NodeContext ui_node_ctx;
    Cherry::NodeSystem::NodeGraph ui_node_graph;
  };
};  // namespace ModuleUI

#endif  // LOGUTILITY_H

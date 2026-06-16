#pragma once
#include <vxcore/include/vortex.h>
#include <vxcore/include/vortex_internals.h>

#include "../../../src/helpers.hpp"

#ifndef NODE_DEBUGGER_HPP
#define NODE_DEBUGGER_HPP

namespace ModuleUI {
  class NodeEditorDebugger : public std::enable_shared_from_this<NodeEditorDebugger> {
   public:
    NodeEditorDebugger(const std::string &name, const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph);

    void menubar();
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    static std::shared_ptr<NodeEditorDebugger> create(
        const std::string &name,
        const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph);
    void setup_render_callback();
    void render();

   private:
    std::shared_ptr<VxContext> ctx;
    std::shared_ptr<Cherry::AppWindow> app_window_;
    std::shared_ptr<NodeEdit::NodeEditGraphSession> graph_;
    ComponentsPool component_pool_;
    std::string m_CurrentNode;
    std::string m_EntryInput;
    std::vector<std::string> m_History;
  };
};  // namespace ModuleUI

#endif  // LOGUTILITY_H

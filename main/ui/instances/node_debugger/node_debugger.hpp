#pragma once
#include "../../../src/helpers.hpp"
#include <vxcore/include/vortex.h>
#include <vxcore/include/vortex_internals.h>

#ifndef NODE_DEBUGGER_HPP
#define NODE_DEBUGGER_HPP

namespace ModuleUI {
class NodeEditorDebugger
    : public std::enable_shared_from_this<NodeEditorDebugger> {
public:
  NodeEditorDebugger(
      const std::string &name,
      const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph);

  void menubar();
  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<NodeEditorDebugger>
  Create(const std::string &name,
         const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph);
  void SetupRenderCallback();
  void Render();

private:
  std::shared_ptr<VxContext> ctx;
  std::shared_ptr<Cherry::AppWindow> m_AppWindow;
  std::shared_ptr<NodeEdit::NodeEditGraphSession> graph_;
  ComponentsPool m_ComponentPool;
  std::string m_CurrentNode;
  std::string m_EntryInput;
  std::vector<std::string> m_History;
};
}; // namespace ModuleUI

#endif // LOGUTILITY_H

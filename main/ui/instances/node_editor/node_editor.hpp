#pragma once
#include <vxcore/include/vortex.h>
#include <vxcore/include/vortex_internals.h>

#ifndef NODE_EDITOR_HPP
#define NODE_EDITOR_HPP

namespace ModuleUI {
class NodeEditorAppWindow
    : public std::enable_shared_from_this<NodeEditorAppWindow> {
public:
  NodeEditorAppWindow(const std::string &name);

  void menubar();
  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<NodeEditorAppWindow> Create(const std::string &name);
  void SetupRenderCallback();
  void Render();
  void RenderMenubar();
  void RenderRightMenubar();
  void RenderBottombar();

  void LoadContext(const std::string &context_id);
  void LoadGraph(const std::string &json_path);
  // void LoadGraph(const json& v);

private:
  std::shared_ptr<VxContext> ctx;
  bool opened;
  // Cherry
  std::shared_ptr<Cherry::AppWindow> m_AppWindow;
  ComponentsPool m_ComponentPool;
  bool m_FileEdited;
  bool m_RefreshReady;
  bool m_SavePending;

  // Node system
  Cherry::NodeSystem::NodeContext node_ctx;
  Cherry::NodeSystem::NodeGraph node_graph;
};
}; // namespace ModuleUI

#endif // LOGUTILITY_H

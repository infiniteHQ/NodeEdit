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

private:
  std::shared_ptr<VxContext> ctx;
  bool opened;
  // Cherry
  std::shared_ptr<Cherry::AppWindow> m_AppWindow;
  ComponentsPool m_ComponentPool;
  bool m_FileEdited;
  bool m_RefreshReady;
  bool m_SavePending;
};
}; // namespace ModuleUI

#endif // LOGUTILITY_H

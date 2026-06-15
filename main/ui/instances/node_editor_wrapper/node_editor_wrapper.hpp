#pragma once
#include <vxcore/include/vortex.h>
#include <vxcore/include/vortex_internals.h>

#include "../../../src/helpers.hpp"

#ifndef NODE_EDITOR_WRAPPER_HPP
#define NODE_EDITOR_WRAPPER_HPP

namespace ModuleUI {
  class NodeEditorWrapperAppWindow : public std::enable_shared_from_this<NodeEditorWrapperAppWindow> {
   public:
    NodeEditorWrapperAppWindow();

    void menubar();
    std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
    static std::shared_ptr<NodeEditorWrapperAppWindow> Create();

   private:
    std::shared_ptr<VxContext> ctx;
    std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    bool opened;
  };
};  // namespace ModuleUI

#endif  // LOGUTILITY_H

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
    std::shared_ptr<Cherry::AppWindow>& get_app_window();
    static std::shared_ptr<NodeEditorWrapperAppWindow> create();

    void set_instance_id(const std::string& gsid) {
      graph_session_id_ = gsid;
    }

   private:
    std::shared_ptr<VxContext> ctx;
    std::shared_ptr<Cherry::AppWindow> app_window_;
    bool opened;
    std::string graph_session_id_ = "undefined";
  };

  class NodeEditorOtherRandomWindow : public std::enable_shared_from_this<NodeEditorOtherRandomWindow> {
   public:
    NodeEditorOtherRandomWindow();

    void menubar();
    std::shared_ptr<Cherry::AppWindow>& get_app_window();
    static std::shared_ptr<NodeEditorOtherRandomWindow> create();
    void render();

    void setup_render_callback();
    bool i = false;

   private:
    std::shared_ptr<VxContext> ctx;
    std::shared_ptr<Cherry::AppWindow> app_window_;
    bool opened;
  };
};  // namespace ModuleUI

#endif  // LOGUTILITY_H

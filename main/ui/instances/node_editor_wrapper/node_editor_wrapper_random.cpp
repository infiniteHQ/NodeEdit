#include <filesystem>
#include <iostream>
#include <string>

#include "../../../src/module.hpp"
#include "node_editor_wrapper.hpp"

namespace ModuleUI {
  NodeEditorOtherRandomWindow::NodeEditorOtherRandomWindow() {
    app_window_ = std::make_shared<Cherry::AppWindow>("ASDASD", "ASDASD");

    app_window_->SetLeftMenubarCallback([this]() { ImGui::Text("GHE;llofsoig"); });
    app_window_->SetInternalPaddingY(0.0f);
    app_window_->SetInternalPaddingX(0.0f);

    app_window_->m_CloseCallback = [=]() {
      Cherry::DeleteAppWindow(app_window_);
      app_window_->SetVisibility(false);
    };
    this->ctx = vxe::get_current_context();
  }  // namespace ModuleUI

  std::shared_ptr<Cherry::AppWindow> &NodeEditorOtherRandomWindow::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<NodeEditorOtherRandomWindow> NodeEditorOtherRandomWindow::create() {
    auto instance = std::shared_ptr<NodeEditorOtherRandomWindow>(new NodeEditorOtherRandomWindow());
    instance->setup_render_callback();
    return instance;
  }

  void NodeEditorOtherRandomWindow::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  void NodeEditorOtherRandomWindow::render() {
    static bool i = false;

    if (!i) {
      auto parent = Cherry::GetAppWindowByName("TEST");
      if (parent) {
        app_window_->SetParent(parent);
      }
      i = true;
    }

    ImGui::Text("Hello window !");
  }

};  // namespace ModuleUI

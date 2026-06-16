#include "node_editor_wrapper.hpp"

#include <filesystem>
#include <iostream>
#include <string>

#include "../../../src/module.hpp"

namespace ModuleUI {
  NodeEditorWrapperAppWindow::NodeEditorWrapperAppWindow() {
    app_window_ = std::make_shared<Cherry::AppWindow>("TEST", "TEST");
    app_window_->SetDockingMode(true);

    app_window_->SetLeftMenubarCallback([this]() { ImGui::Text("GHE;llofsoig"); });
    app_window_->SetInternalPaddingY(0.0f);
    app_window_->SetInternalPaddingX(0.0f);

    app_window_->m_CloseCallback = [=]() {
      Cherry::DeleteAppWindow(app_window_);
      app_window_->SetVisibility(false);
    };
    this->ctx = vxe::get_current_context();
  }  // namespace ModuleUI

  std::shared_ptr<Cherry::AppWindow> &NodeEditorWrapperAppWindow::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<NodeEditorWrapperAppWindow> NodeEditorWrapperAppWindow::create() {
    auto instance = std::shared_ptr<NodeEditorWrapperAppWindow>(new NodeEditorWrapperAppWindow());
    return instance;
  }

};  // namespace ModuleUI

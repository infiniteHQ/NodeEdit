#include "node_editor_wrapper.hpp"

#include <filesystem>
#include <iostream>
#include <string>

#include "../../../src/module.hpp"

namespace ModuleUI {
  NodeEditorWrapperAppWindow::NodeEditorWrapperAppWindow() {
    m_AppWindow = std::make_shared<Cherry::AppWindow>("TEST", "TEST");
    m_AppWindow->SetDockingMode(true);

    m_AppWindow->SetLeftMenubarCallback([this]() { ImGui::Text("GHE;llofsoig"); });
    m_AppWindow->SetInternalPaddingY(0.0f);
    m_AppWindow->SetInternalPaddingX(0.0f);

    m_AppWindow->m_CloseCallback = [=]() {
      Cherry::DeleteAppWindow(m_AppWindow);
      m_AppWindow->SetVisibility(false);
    };
    this->ctx = vxe::get_current_context();
  }  // namespace ModuleUI

  std::shared_ptr<Cherry::AppWindow> &NodeEditorWrapperAppWindow::GetAppWindow() {
    return m_AppWindow;
  }

  std::shared_ptr<NodeEditorWrapperAppWindow> NodeEditorWrapperAppWindow::Create() {
    auto instance = std::shared_ptr<NodeEditorWrapperAppWindow>(new NodeEditorWrapperAppWindow());
    return instance;
  }

};  // namespace ModuleUI

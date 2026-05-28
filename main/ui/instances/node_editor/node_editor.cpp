#include "node_editor.hpp"
#include "../../../src/module.hpp"

#include <filesystem>
#include <iostream>
#include <string>

namespace ModuleUI {

NodeEditorAppWindow::NodeEditorAppWindow(const std::string &name) {
  m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
  m_AppWindow->SetLeftMenubarCallback([this]() { RenderMenubar(); });
  m_AppWindow->SetRightMenubarCallback([this]() { RenderRightMenubar(); });
  m_AppWindow->SetLeftBottombarCallback([this]() { RenderBottombar(); });
  m_AppWindow->SetSaveMode(true);

  m_AppWindow->m_CloseCallback = [=]() {
    Cherry::DeleteAppWindow(m_AppWindow);
    m_AppWindow->SetVisibility(false);
  };

  std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;

  this->ctx = vxe::get_current_context();
}

std::shared_ptr<Cherry::AppWindow> &NodeEditorAppWindow::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<NodeEditorAppWindow>
NodeEditorAppWindow::Create(const std::string &name) {
  auto instance =
      std::shared_ptr<NodeEditorAppWindow>(new NodeEditorAppWindow(name));
  instance->SetupRenderCallback();
  return instance;
}

void NodeEditorAppWindow::SetupRenderCallback() {
  auto self = shared_from_this();
  m_AppWindow->SetRenderCallback([self]() {
    if (self) {
      self->Render();
    }
  });
}

void NodeEditorAppWindow::RenderMenubar() {
  CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 3.0f);

  if (!m_FileEdited) {
    CherryGUI::BeginDisabled();
  }

  CherryNextComponent.SetProperty("padding_y", "5.5f");
  CherryNextComponent.SetProperty("padding_x", "6.0f");
  CherryNextComponent.SetProperty("size_x", "18");
  CherryNextComponent.SetProperty("size_y", "18");
  if (CherryKit::ButtonImage(
          NodeEdit::GetPath("/resources/icons/icon_save.png"))
          .GetDataAs<bool>("isClicked")) {
    m_SavePending = true;
  }

  if (!m_FileEdited) {
    CherryGUI::EndDisabled();
  }

  CherryNextComponent.SetProperty("padding_y", "5.5f");
  CherryNextComponent.SetProperty("padding_x", "6.0f");
  CherryNextComponent.SetProperty("size_x", "18");
  CherryNextComponent.SetProperty("size_y", "18");
  if (CherryKit::ButtonImage(
          NodeEdit::GetPath("/resources/icons/icon_refresh.png"))
          .GetDataAs<bool>("isClicked")) {
    m_RefreshReady = true;
  }
}
void NodeEditorAppWindow::Render() {
  CherryApp.PushComponentPool(&m_ComponentPool);

  CherryKit::TitleFive("Test");

  CherryApp.PopComponentPool();
}

void NodeEditorAppWindow::RenderRightMenubar() {
  //
}

void NodeEditorAppWindow::RenderBottombar() {
  //
}

}; // namespace ModuleUI

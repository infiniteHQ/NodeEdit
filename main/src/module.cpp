#include "module.hpp"

// runtime pointer
#ifndef CNodeEdit
std::weak_ptr<NodeEdit::Context> CNodeEdit;
#endif

std::shared_ptr<NodeEdit::Context> NodeEdit::create_context() {
  auto ctx = std::make_shared<NodeEdit::Context>();

  set_current_context(ctx);

  return ctx;
}

void NodeEdit::DestroyContext(std::shared_ptr<NodeEdit::Context> ctx) {
  set_current_context(nullptr);
}

void NodeEdit::set_current_context(std::shared_ptr<NodeEdit::Context> ctx) {
  CNodeEdit = ctx;
}

std::shared_ptr<NodeEdit::Context> NodeEdit::get_current_context() {
  return CNodeEdit.lock();
}

std::string NodeEdit::GetPath(const std::string &path) {
  return get_current_context()->interface->cook_path(path);
}

void NodeEdit::StartNodeEditTestInstance() {
  auto inst = ModuleUI::NodeEditorAppWindow::Create("TEST");
  Cherry::AddAppWindow(inst->GetAppWindow());
  get_current_context()->editor_instances.push_back(inst);
}
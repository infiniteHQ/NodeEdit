#include "module.hpp"

void NodeEdit::CreateContext() {
  NodeEdit::Context *ctx = new (NodeEdit::Context);
  CNodeEdit = ctx;
}

void NodeEdit::DestroyContext() { free(CNodeEdit); }

bool NodeEdit::IsValidFile(const std::string &path) {
  namespace fs = std::filesystem;

  if (!fs::is_directory(path)) {
    return false;
  }

  for (const auto &entry : fs::directory_iterator(path)) {
    if (entry.is_regular_file() &&
        entry.path().filename() == "SampleConfig.txt") {
      return true;
    }
  }

  return false;
}

void NodeEdit::StartNodeEditTestInstance() {
  auto inst = ModuleUI::NodeEditorAppWindow::Create("TEST");
  Cherry::AddAppWindow(inst->GetAppWindow());
  CNodeEdit->editor_instances.push_back(inst);
}

std::string NodeEdit::GetPath(const std::string &path) {
  return CNodeEdit->interface->cook_path(path);
}

void NodeEdit::Hello() { vxe::log_info("Tt", "cc"); }

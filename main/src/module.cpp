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

void NodeEdit::StartTextEditorInstance(const std::string &path) {
  std::string filename = fs::path(path).filename().string();

  const size_t maxLen = 24;
  if (filename.size() > maxLen) {
    filename = filename.substr(0, maxLen - 3) + "...";
  }

  std::string window_name =
      filename + "####" +
      std::to_string(CNodeEdit->m_text_editor_instances.size());

  auto inst = ModuleUI::TextEditorAppWindow::Create(path, window_name);
  Cherry::AddAppWindow(inst->GetAppWindow());
  CNodeEdit->m_text_editor_instances.push_back(inst);
}

std::string NodeEdit::GetPath(const std::string &path) {
  return CNodeEdit->m_interface->cook_path(path);
}

void NodeEdit::Hello() { vxe::log_info("Tt", "cc"); }

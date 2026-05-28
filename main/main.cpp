#include "./src/module.hpp"

#ifndef CNodeEdit
NodeEdit::Context *CNodeEdit = NULL;
#endif

class Module : public ModuleInterface {
public:
  void execute() override {
    // Create the context pointer of this module
    NodeEdit::CreateContext();

    // Get the interface pointer
    CNodeEdit->m_interface =
        ModuleInterface::get_editor_module_by_name(this->name());

    this->add_content_browser_item_handler(ItemHandlerInterface(
        "file_cpp", NodeEdit::StartTextEditorInstance, "Edit",
        "Edit this C++ file", NodeEdit::GetPath("resources/icons/edit.png")));
    this->add_content_browser_item_handler(ItemHandlerInterface(
        "file_lua", NodeEdit::StartTextEditorInstance, "Edit",
        "Edit this Lua file", NodeEdit::GetPath("resources/icons/edit.png")));
    this->add_content_browser_item_handler(ItemHandlerInterface(
        "file_json", NodeEdit::StartTextEditorInstance, "Edit",
        "Edit this JSON file", NodeEdit::GetPath("resources/icons/edit.png")));
    this->add_content_browser_item_handler(
        ItemHandlerInterface("file_hpp", NodeEdit::StartTextEditorInstance,
                             "Edit", "Edit this C++ header file",
                             NodeEdit::GetPath("resources/icons/edit.png")));
    this->add_content_browser_item_handler(ItemHandlerInterface(
        "file_c", NodeEdit::StartTextEditorInstance, "Edit", "Edit this C file",
        NodeEdit::GetPath("resources/icons/edit.png")));
    this->add_content_browser_item_handler(
        ItemHandlerInterface("file_h", NodeEdit::StartTextEditorInstance,
                             "Edit", "Edit this C header file",
                             NodeEdit::GetPath("resources/icons/edit.png")));
    this->add_content_browser_item_handler(
        ItemHandlerInterface("file_python", NodeEdit::StartTextEditorInstance,
                             "Edit", "Edit this Python file",
                             NodeEdit::GetPath("resources/icons/edit.png")));

    this->add_content_browser_item_identifier(ItemIdentifierInterface(
        NodeEdit::IsValidFile, "text_edit:superfile", "Super file", "#553333"));

    this->set_credits_file(NodeEdit::GetPath("CREDITS"));
    this->add_documentation("Take the editor", "Edit a txt file",
                            NodeEdit::GetPath("docs/main.md"));
    this->add_documentation("Take the editor", "Find specific text",
                            NodeEdit::GetPath("docs/main.md"));
  }

  void init_ui() override {
    // CherryApp.AddFont(
    //     "JetBrainsMono",
    //     NodeEdit::GetPath("resources/fonts/JetBrainsMono-Regular.ttf"), 40.0f);
  }

  void destroy() override {
    // Reset module
    this->reset_module();

    // Clear windows
    for (auto i : CNodeEdit->m_text_editor_instances) {
      CherryApp.DeleteAppWindow(i->GetAppWindow());
    }

    // Clear context
    // DestroyContext();
  }
};

#ifdef _WIN32
extern "C" __declspec(dllexport) ModuleInterface *create_em() {
  return new Module();
}
#else
extern "C" ModuleInterface *create_em() { return new Module(); }
#endif

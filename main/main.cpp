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

    this->add_toolbar_handler(ToolbarHandlerInterface(
        NodeEdit::StartNodeEditTestInstance, "NodeEdit", "",
        NodeEdit::GetPath("resources/icons/edit.png")));
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

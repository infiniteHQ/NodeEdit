#include "./src/module.hpp"

// main.cpp
class Module : public ModuleInterface {
public:
  std::shared_ptr<NodeEdit::Context> ctx;

  void execute() override {
    ctx = NodeEdit::create_context();

    auto m = ModuleInterface::get_editor_module_by_name(this->name());
    NodeEdit::get_current_context()->interface = m;

    this->add_toolbar_handler(ToolbarHandlerInterface(
        NodeEdit::StartNodeEditTestInstance, "NodeEdit", ""));

    this->add_content_browser_item_identifier(ItemIdentifierInterface(
        NodeEdit::IsGraphFile, "infinitehq_nodeedit:graph", "Node Graph",
        "#B1FF31", NodeEdit::GetPath("resources/icons/edit.png"),
        NodeEdit::GetPath("resources/icons/edit.png")));

    this->add_content_browser_item_handler(ItemHandlerInterface(
        "infinitehq_nodeedit:graph", NodeEdit::OpenGraph, "Edit",
        "Edit this graph", NodeEdit::GetPath("resources/icons/edit.png")));
  }

  void destroy() override {
    this->reset_module();

    for (auto i : NodeEdit::get_current_context()->editor_instances) {
      CherryApp.DeleteAppWindow(i->GetAppWindow());
    }

    NodeEdit::DestroyContext(ctx);
    ctx.reset();
  }
};

#ifdef _WIN32
extern "C" __declspec(dllexport) ModuleInterface *create_em() {
  return new Module();
}
#else
extern "C" ModuleInterface *create_em() { return new Module(); }
#endif

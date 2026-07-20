#include "./src/module.hpp"

// main.cpp
class infinitehq_nodeedit : public ModuleInterface {
 public:
  std::shared_ptr<NodeEdit::Context> ctx;

  void execute() override {
    ctx = NodeEdit::create_context();

    auto m = ModuleInterface::get_editor_module_by_name(this->name());
    NodeEdit::get_current_context()->m_interface = m;

    this->add_content_browser_item_identifier(ItemIdentifierInterface(
        NodeEdit::is_graph_file,
        "infinitehq_nodeedit:graph",
        "Node Graph",
        "#0DFFEB",
        NodeEdit::get_path("resources/icons/cb_i.png")));

    this->add_content_browser_item_handler(ItemHandlerInterface(
        "infinitehq_nodeedit:graph",
        NodeEdit::open_graph,
        "Edit",
        "Edit this graph",
        NodeEdit::get_path("resources/icons/edit.png")));

    this->add_input_event(ModuleInputEvent(NodeEdit::ie_create_node_context, "create_node_context"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_setup_pin_format, "setup_pin_format"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_setup_schema, "setup_schema"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_setup_schema_to_graph_ext, "setup_schema_to_graph_ext"));

    this->add_input_event(ModuleInputEvent(NodeEdit::ie_clear_all_graph_ext_schemas, "clear_all_graph_ext_schemas"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_delete_graph_ext_schema, "delete_graph_ext_schema"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_clear_all_graph_ext_pin_formats, "clear_all_graph_ext_pin_formats"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_setup_schema_to_graph_ext, "delete_graph_ext_pin_format"));

    this->add_input_event(ModuleInputEvent(NodeEdit::ie_open_graph, "open_graph"));

    this->add_input_event(ModuleInputEvent(NodeEdit::ie_add_effect_to_node, "add_effect_to_node"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_add_effect_to_connection, "add_effect_to_connection"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_remove_node_effects_from_node, "remove_node_effects_from_node"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_set_graph_title, "set_graph_title"));
    this->add_input_event(
        ModuleInputEvent(NodeEdit::ie_remove_connection_effects_from_node, "remove_connection_effects_from_node"));
    this->add_input_event(
        ModuleInputEvent(NodeEdit::ie_remove_connection_effects_from_pin, "remove_connection_effects_from_pin"));

    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_next_nodes, "get_next_nodes"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_previous_nodes, "get_previous_nodes"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_search_node_output_pin_by_type, "search_node_output_pin_by_type"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_search_node_input_pin_by_type, "search_node_input_pin_by_type"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_search_node_type, "search_node_type"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_all_node_input_pins, "get_all_node_input_pins"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_all_node_output_pins, "get_all_node_outputs_pins"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_all_node_input_pins, "get_all_node_input_pins"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_refresh_nodegraph, "refresh_nodegraph"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_save_nodegraph, "save_nodegraph"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_focus_at, "focus_at"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_zoom_at, "zoom_at"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_spawn_at, "spawn_at"));

    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_pin_format, "get_pin_format"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_all_pin_formats, "get_all_pin_formats"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_schema, "get_schema"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_all_schemas, "get_all_schemas"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_all_ext_schemas, "get_all_ext_schemas"));

    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_all_graph_ext_pin_formats, "get_all_graph_ext_pin_formats"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_ext_pin_format, "get_ext_pin_format"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_ext_schema, "get_ext_schema"));

    this->add_input_event(ModuleInputEvent(NodeEdit::ie_find_node_by_schema_id, "find_node_by_schema_id"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_node_type_id, "get_node_type_id"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_node_data, "get_node_data"));

    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_next_nodes_silently, "get_next_nodes_silently"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_previous_nodes_silently, "get_previous_nodes_silently"));
    this->add_input_event(
        ModuleInputEvent(NodeEdit::ie_search_node_output_pin_by_type_silently, "search_node_output_pin_by_type_silently"));
    this->add_input_event(
        ModuleInputEvent(NodeEdit::ie_search_node_input_pin_by_type_silently, "search_node_input_pin_by_type_silently"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_search_node_type_silently, "search_node_type_silently"));
    this->add_input_event(
        ModuleInputEvent(NodeEdit::ie_get_all_node_input_pins_silently, "get_all_node_input_pins_silently"));
    this->add_input_event(
        ModuleInputEvent(NodeEdit::ie_get_all_node_output_pins_silently, "get_all_node_output_pins_silently"));

    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_ext_pin_format_silently, "get_ext_pin_format_silently"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_ext_schema_silently, "get_ext_schema_silently"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_all_ext_schemas_silently, "get_all_ext_schemas_silently"));
    this->add_input_event(
        ModuleInputEvent(NodeEdit::ie_get_all_graph_ext_pin_formats_silently, "get_all_graph_ext_pin_formats_silently"));

    this->add_input_event(ModuleInputEvent(NodeEdit::ie_find_node_by_schema_id_silently, "find_node_by_schema_id_silently"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_node_type_id_silently, "get_node_type_id_silently"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_node_data_silently, "get_node_data_silently"));

    this->add_input_event(ModuleInputEvent(NodeEdit::ie_clear_silent_session_cache, "clear_silent_session_cache"));
    this->add_input_event(ModuleInputEvent(NodeEdit::ie_get_connection_source_pin, "get_connection_source_pin"));
    this->add_input_event(
        ModuleInputEvent(NodeEdit::ie_get_connection_source_pin_silently, "get_connection_source_pin_silently"));
    NodeEdit::setup_example_context();
  }

  void destroy() override {
    this->reset_module();

    for (auto i : NodeEdit::get_current_context()->editor_instances) {
      CherryApp.DeleteAppWindow(i->get_app_window());
    }

    NodeEdit::destroy_context(ctx);
    ctx.reset();
  }
};

#ifdef _WIN32
extern "C" __declspec(dllexport) ModuleInterface *create_em() {
  return new infinitehq_nodeedit();
}
#else
extern "C" ModuleInterface *create_em() {
  return new infinitehq_nodeedit();
}
#endif

#include <vxcore/include/vortex.h>
#include <vxcore/include/vortex_internals.h>

#include <vxgui/editor/main/editor.hpp>

#include "../ui/instances/node_debugger/node_debugger.hpp"
#include "../ui/instances/node_editor/node_editor.hpp"
#include "./helpers.hpp"

#ifndef SAMPLE_MODULE_HPP
#define SAMPLE_MODULE_HPP

#ifndef NODEEDIT_API
#define NODEEDIT_API
#endif

// TODO :
// Graph context extensions
// Vortex Events APi

namespace NodeEdit {
  struct Context {
    std::shared_ptr<ModuleInterface> interface;
    std::vector<std::shared_ptr<ModuleUI::NodeEditorAppWindow>> editor_instances;
    std::vector<std::shared_ptr<ModuleUI::NodeEditorDebugger>> editor_debuggers;
    std::vector<std::shared_ptr<NodeEditContext>> contexts;
    std::vector<std::shared_ptr<NodeEditGraphSession>> graph_sessions;
  };
}  // namespace NodeEdit
// context pointer
#ifndef CNodeEdit
extern NODEEDIT_API std::weak_ptr<NodeEdit::Context> CNodeEdit;
#endif

namespace NodeEdit {
  // Context
  NODEEDIT_API std::shared_ptr<NodeEdit::Context> create_context();
  NODEEDIT_API void destroy_context(std::shared_ptr<NodeEdit::Context> ctx);
  NODEEDIT_API void set_current_context(std::shared_ptr<NodeEdit::Context> ctx);
  NODEEDIT_API std::shared_ptr<NodeEdit::Context> get_current_context();

  // Node context API
  NODEEDIT_API std::shared_ptr<NodeEditContext> create_node_context(const std::string &name);
  NODEEDIT_API std::shared_ptr<NodeEditContext> get_node_context(const std::string &name);
  NODEEDIT_API void destroy_node_context(const std::string &name);
  NODEEDIT_API void load_context_from_file(const std::string &path);
  NODEEDIT_API void load_node_context(const nlohmann::json &path);
  NODEEDIT_API void add_schema_to_context(const std::string &ctx_id, const NodeEditSchema &schema);
  NODEEDIT_API void add_pin_format_to_context(const std::string &ctx_id, const NodeEditPinFormat &pin_format);

  // utils
  NODEEDIT_API std::string get_path(const std::string &path);
  NODEEDIT_API nlohmann::json dump_graph_to_json(const NodeEdit::NodeEditGraph &graph);
  NODEEDIT_API nlohmann::json dump_context_to_json(const std::string ctx);
  NODEEDIT_API NodeEditGraph populate_graph(const nlohmann::json &j);
  NODEEDIT_API std::string get_graph_context_name(const nlohmann::json &j);
  NODEEDIT_API nlohmann::json get_graph_in_json(const nlohmann::json &j);
  NODEEDIT_API std::shared_ptr<NodeEdit::NodeEditGraphSession> open_graph_and_get_session(const std::string &path);
  NODEEDIT_API void open_graph(const std::string &path);
  NODEEDIT_API void create_graph_session_from_file(const std::string &path);
  NODEEDIT_API bool is_graph_file(const std::string &path);
  NODEEDIT_API bool is_context_exist(const std::string &ctx_name);
  NODEEDIT_API std::shared_ptr<NodeEdit::NodeEditGraphSession> get_graph_session_by_id(const std::string &session_id);

  // debug
  NODEEDIT_API void setup_example_context();

  // save system
  NODEEDIT_API void save_graph_session(const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph);
  NODEEDIT_API void refresh_graph_session(const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph);

  // node graph api
  NODEEDIT_API std::string get_next_node(
      const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
      const std::string &nodeid,
      const std::string &outputid);  // instance_id

  NODEEDIT_API std::string get_previous_node(
      const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
      const std::string &nodeid,
      const std::string &inputid);  // instance_id

  NODEEDIT_API std::string search_node_output_pin_by_type(
      const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
      const std::string &nodeid,
      const std::string &type);  //  id (from NodeEditPin)

  NODEEDIT_API std::string search_node_input_pin_by_type(
      const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
      const std::string &nodeid,
      const std::string &type);  // id (from NodeEditPin)

  NODEEDIT_API std::string search_node_type(
      const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
      const std::string &type);  // return instance_id

  // helpers
  NODEEDIT_API const NodeEdit::NodeEditSchema *find_schema(const NodeEdit::NodeEditContext &ctx, const std::string &type_id);
  NODEEDIT_API const NodeEdit::NodeEditContext *find_context(const std::string &context_id);
  NODEEDIT_API const NodeEdit::NodeEditInstance *find_instance(
      const NodeEdit::NodeEditGraph &graph,
      const std::string &instance_id);

  NODEEDIT_API std::vector<std::pair<std::string, std::string>> get_all_node_input_pins(
      const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
      const std::string &nodeid);

  NODEEDIT_API std::vector<std::pair<std::string, std::string>> get_all_node_output_pins(
      const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
      const std::string &nodeid);

  NODEEDIT_API void add_schema_to_graph_ctx_ext(
      const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
      const NodeEditSchema &schema);

  NODEEDIT_API void add_pin_format_to_graph_ctx_ext(
      const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
      const NodeEditPinFormat &pin_format);

  // Effect API
  NODEEDIT_API void add_effect_to_node(
      const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
      const NodeEditNodeEffect &e);

  NODEEDIT_API void add_effect_to_connection(
      const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
      const NodeEditConnectionEffect &e);

  NODEEDIT_API void remove_node_effects_from_node(
      const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
      const std::string &nodeid);

  NODEEDIT_API void remove_connection_effects_from_node(
      const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
      const std::string &nodeid);

  NODEEDIT_API void remove_connection_effects_from_pin(
      const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
      const std::string &nodeid,
      const std::string &pin_id);

  NODEEDIT_API void set_graph_title(const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph, const std::string &title);

  // events
  NODEEDIT_API void ie_create_node_context(ArgumentValues &args, ReturnValues &ret);
  NODEEDIT_API void ie_setup_pin_format(ArgumentValues &args, ReturnValues &ret);
  NODEEDIT_API void ie_setup_schema(ArgumentValues &args, ReturnValues &ret);
  NODEEDIT_API void ie_open_graph(ArgumentValues &args, ReturnValues &ret);

  NODEEDIT_API void ie_add_effect_to_node(ArgumentValues &args, ReturnValues &ret);
  NODEEDIT_API void ie_add_effect_to_connection(ArgumentValues &args, ReturnValues &ret);
  NODEEDIT_API void ie_remove_node_effects_from_node(ArgumentValues &args, ReturnValues &ret);
  NODEEDIT_API void ie_remove_connection_effects_from_node(ArgumentValues &args, ReturnValues &ret);
  NODEEDIT_API void ie_remove_connection_effects_from_pin(ArgumentValues &args, ReturnValues &ret);
  NODEEDIT_API void ie_set_graph_title(ArgumentValues &args, ReturnValues &ret);

}  // namespace NodeEdit

#endif  // SAMPLE_MODULE_HPP
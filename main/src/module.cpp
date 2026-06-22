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

void NodeEdit::destroy_context(std::shared_ptr<NodeEdit::Context> ctx) {
  set_current_context(nullptr);
}

void NodeEdit::set_current_context(std::shared_ptr<NodeEdit::Context> ctx) {
  CNodeEdit = ctx;
}

std::shared_ptr<NodeEdit::Context> NodeEdit::get_current_context() {
  return CNodeEdit.lock();
}

std::string NodeEdit::get_path(const std::string &path) {
  return get_current_context()->interface->cook_path(path);
}

bool NodeEdit::is_graph_file(const std::string &path) {
  std::string fullpath = path + "/graph.nodegraph";
  return fs::exists(fullpath);
}

void NodeEdit::open_graph(const std::string &path) {
  NodeEdit::open_graph_and_get_session(path);
}

static std::vector<std::shared_ptr<ModuleUI::NodeEditorWrapperAppWindow>> s_instances;
void NodeEdit::open_graphDEBUG(const std::string &path) {
  std::string id;
  auto inst = ModuleUI::NodeEditorWrapperAppWindow::create();
  Cherry::AddAppWindow(inst->get_app_window());
  s_instances.push_back(inst);

  {
    auto i = ModuleUI::NodeEditorOtherRandomWindow::create();
    Cherry::AddAppWindow(i->get_app_window());
  }

  {
    nlohmann::json j;
    j["path"] = path;
    j["disable_native_saving_system"] = true;
    j["graph_title"] = "ADGSDFH";
    j["parent_appwindow"] = "TEST";
    j["logo_path"] = NodeEdit::get_path("resources/icons/icon_magnifying_glass.png");

    auto ret = ReturnValues();
    auto args = ArgumentValues(j.dump());
    vxe::call_input_event("infinitehq.nodeedit", "open_graph", args, ret);

    id = ret.get_json()["session_id"];
    inst->set_instance_id(id);
  }

  {
    nlohmann::json j;
    j["session_id"] = id;
    auto args = ArgumentValues(j.dump());
    auto ret = ReturnValues();
    vxe::call_input_event("infinitehq.nodeedit", "refresh_nodegraph", args, ret);
  }
}

std::shared_ptr<NodeEdit::GraphSession> NodeEdit::open_graph_and_get_session(
    const std::string &path,
    const std::string &parent,
    const bool &disable_native_save,
    const std::string &logo_path) {
  if (!is_graph_file(path)) {
    get_current_context()->interface->log_error("No graph file in selected file ! (" + path + ")");
  }

  std::string fullpath = path + "/graph.nodegraph";

  std::ifstream file(fullpath);
  if (!file.is_open()) {
    get_current_context()->interface->log_error("Failed to open file : (" + fullpath + ")");
    return nullptr;
  }

  nlohmann::json j;
  try {
    file >> j;
  } catch (const nlohmann::json::parse_error &e) {
    get_current_context()->interface->log_error("Failed to parse graph file : (" + fullpath + ") -> " + e.what());
    return nullptr;
  }

  std::string ctx_name = get_graph_context_name(j);

  if (!is_context_exist(ctx_name)) {
    get_current_context()->interface->log_error(
        "This graph contain an unknown graph context : (" + fullpath + "). Did you have a missing module ?");
  }

  auto ctx = NodeEdit::get_node_context(ctx_name);

  auto graph = populate_graph(get_graph_in_json(j));
  graph.graph_title = "Node graph";
  graph.refresh_effects = true;

  auto gs = std::make_shared<NodeEdit::GraphSession>();
  gs->session_id = generate_session_id();
  gs->path = fullpath;
  gs->graph = graph;
  gs->disable_native_close_system = true;
  gs->disable_native_save_system = disable_native_save;
  gs->parent_appwindow = parent;
  gs->context_id = ctx_name;
  gs->logo_path = logo_path;

  {
    auto inst = ModuleUI::NodeEditorAppWindow::create("Node graph", ctx, gs);
    Cherry::AddAppWindow(inst->get_app_window());
    get_current_context()->editor_instances.push_back(inst);
  }
  {
    auto inst = ModuleUI::NodeEditorDebugger::create("Node debug", gs);
    Cherry::AddAppWindow(inst->get_app_window());
    get_current_context()->editor_debuggers.push_back(inst);
  }

  get_current_context()->graph_sessions.push_back(gs);

  return gs;
}

std::string NodeEdit::generate_session_id() {
  static std::atomic<uint32_t> counter{ 0 };
  auto id = counter.fetch_add(1, std::memory_order_relaxed);
  return std::format("{:010d}", id);
}

/*
Input events :
add_pin_format (params : ctx and pin format object)
add_schema (params : ctx and pin format object)
get_connection (id/name of I/O pin )
get_connection (only type and if Input or Ouput) (for sequences pins for
example)

*/

void NodeEdit::setup_example_context() {
  // create node context
  {
    auto args = ArgumentValues(R"({
    "name": "efusion_blueprint"
  })");
    auto ret = ReturnValues();
    vxe::call_input_event("infinitehq.nodeedit", "create_node_context", args, ret);
  }

  // Setup pin formats
  {
    auto args = ArgumentValues(R"({
    "context_name": "efusion_blueprint",
    "type":         "bool",
    "name":         "Boolean",
    "color":        "#00FF00",
    "shape":        "circle",
    "description":  "Simple boolean"
  })");
    auto ret = ReturnValues();
    vxe::call_input_event("infinitehq.nodeedit", "setup_pin_format", args, ret);
  }
  {
    auto args = ArgumentValues(R"({
    "context_name": "efusion_blueprint",
    "type":         "eee",
    "name":         "Boolean",
    "color":        "#FF0000",
    "delegate":         true,
    "shape":        "circle",
    "description":  "Simple eee"
  })");
    auto ret = ReturnValues();
    vxe::call_input_event("infinitehq.nodeedit", "setup_pin_format", args, ret);
  }

  // Setup schemas
  {
    auto args = ArgumentValues(R"({
    "context_name": "efusion_blueprint",
    "id": "is_cool",
    "type": "blueprint",
    "status": "active",
    "description_color": "#B1FF31",
    "second_label": "This is cool",
    "label": "Is cool",
    "label_color": "Is cool",
    "header_color": "#B1FF31",
    "input_pins": [
      { "id": "bool1", "name": "Bool One", "type": "bool" },
      { "id": "bool2", "type": "bool" }
    ],
    "output_pins": [
      { "id": "bool5", "name": "Bool One", "type": "eee" },
      { "id": "bool3", "type": "bool" },
      { "id": "bool4", "name": "Bool Four", "type": "bool" }
    ],
    "spawnable": true,
    "spawn_possibility": {
      "category": "base",
      "proper_description": "Spwan a is cool node",
      "proper_logo": "resources/icons/edit.png",
      "proper_name": "Is Cool",
      "schema_id": "is_cool"
    }
  })");
    auto ret = ReturnValues();
    vxe::call_input_event("infinitehq.nodeedit", "setup_schema", args, ret);
  }
}

std::shared_ptr<NodeEdit::NodeContext> NodeEdit::create_node_context(const std::string &name) {
  auto &contexts = get_current_context()->contexts;
  auto existing =
      std::find_if(contexts.begin(), contexts.end(), [&name](const auto &ptr) { return ptr && ptr->id == name; });
  if (existing != contexts.end()) {
    get_current_context()->interface->log_error(name + " node graph context already exist !");
  }

  auto ctx = std::make_shared<NodeContext>();
  ctx->id = name;

  contexts.push_back(ctx);

  return ctx;
}

std::shared_ptr<NodeEdit::NodeContext> NodeEdit::get_node_context(const std::string &name) {
  auto &contexts = get_current_context()->contexts;
  auto existing =
      std::find_if(contexts.begin(), contexts.end(), [&name](const auto &ptr) { return ptr && ptr->id == name; });

  return *existing;
}

void NodeEdit::destroy_node_context(const std::string &name) {
  auto &contexts = get_current_context()->contexts;

  contexts.erase(
      std::remove_if(
          contexts.begin(),
          contexts.end(),
          [&name](const std::shared_ptr<NodeContext> &ptr) { return ptr && ptr->id == name; }),
      contexts.end());
}

void NodeEdit::add_schema_to_context(const std::string &ctx_id, const NodeEdit::Schema &schema) {
  auto &contexts = get_current_context()->contexts;

  auto c = std::find_if(contexts.begin(), contexts.end(), [&ctx_id](const std::shared_ptr<NodeContext> &ptr) {
    return ptr && ptr->id == ctx_id;
  });

  if (c == contexts.end()) {
    get_current_context()->interface->log_error(ctx_id + " does not exist! Cannot add schema to this context.");
    return;
  }

  (*c)->schemas.push_back(schema);
}

void NodeEdit::add_pin_format_to_context(const std::string &ctx_id, const NodeEdit::PinFormat &pin_format) {
  auto &contexts = get_current_context()->contexts;

  auto c = std::find_if(contexts.begin(), contexts.end(), [&ctx_id](const std::shared_ptr<NodeContext> &ptr) {
    return ptr && ptr->id == ctx_id;
  });

  if (c == contexts.end()) {
    get_current_context()->interface->log_error(ctx_id + " does not exist! Cannot add schema to this context.");
    return;
  }

  (*c)->pin_formats.push_back(pin_format);
}

void NodeEdit::save_graph_session(const std::shared_ptr<NodeEdit::GraphSession> &gs) {
  if (!gs) {
    return;
  }

  auto file_path = gs->path;

  auto json = dump_graph_to_json(gs->graph);
  std::string graph_raw = json.dump();
  std::string context_id = gs->context_id;

  nlohmann::json output;
  output["context_id"] = context_id;
  output["graph"] = json;

  std::ofstream file(file_path);
  if (!file.is_open()) {
    return;
  }
  file << output.dump(4);
  file.close();
}

void NodeEdit::refresh_graph_session(const std::shared_ptr<NodeEdit::GraphSession> &gs) {
  if (!gs) {
    return;
  }

  auto file_path = gs->path;

  std::ifstream file(file_path);
  if (!file.is_open()) {
    get_current_context()->interface->log_error("Failed to open file : (" + file_path + ")");
    return;
  }

  nlohmann::json j;
  try {
    file >> j;
  } catch (const nlohmann::json::parse_error &e) {
    get_current_context()->interface->log_error("Failed to parse graph file : (" + file_path + ") -> " + e.what());
    return;
  }

  std::string ctx_name = get_graph_context_name(j);

  if (!is_context_exist(ctx_name)) {
    get_current_context()->interface->log_error(
        "This graph contain an unknown graph context : (" + file_path + "). Did you have a missing module ?");
    return;
  }

  auto graph = populate_graph(get_graph_in_json(j));

  gs->graph = graph;
}

nlohmann::json NodeEdit::dump_graph_to_json(const NodeEdit::Graph &graph) {
  nlohmann::json j;

  j["instances"] = nlohmann::json::array();
  for (const auto &inst : graph.instances) {
    nlohmann::json node;
    node["type_id"] = inst.type_id;
    node["instance_id"] = inst.instance_id;
    node["pos_x"] = inst.pos_x;
    node["pos_y"] = inst.pos_y;
    node["size_x"] = inst.size_x;
    node["size_y"] = inst.size_y;
    node["datas"] = inst.datas;
    j["instances"].push_back(node);
  }

  j["connections"] = nlohmann::json::array();
  for (const auto &conn : graph.connections) {
    nlohmann::json c;
    c["node_instance_id_A"] = conn.node_instance_id_A;
    c["pin_id_A"] = conn.pin_id_A;
    c["node_instance_id_B"] = conn.node_instance_id_B;
    c["pin_id_B"] = conn.pin_id_B;
    j["connections"].push_back(c);
  }

  j["ctx_ext"] = nlohmann::json::object();

  j["ctx_ext"]["pin_formats"] = nlohmann::json::array();
  for (const auto &pf : graph.ext.pin_formats) {
    nlohmann::json c;
    c["type"] = pf.type;
    c["delegate"] = pf.delegate;
    c["description"] = pf.description;
    c["name"] = pf.name;
    c["color"] = pf.color;
    c["shape"] = pf.shape;
    j["ctx_ext"]["pin_formats"].push_back(c);
  }

  j["ctx_ext"]["schemas"] = nlohmann::json::array();
  for (const auto &s : graph.ext.schemas) {
    nlohmann::json schema;

    schema["input_pins"] = nlohmann::json::array();
    for (const auto &pin : s.input_pins) {
      nlohmann::json p;
      p["type"] = pin.type;
      p["name"] = pin.name;
      p["id"] = pin.id;
      schema["input_pins"].push_back(p);
    }

    schema["output_pins"] = nlohmann::json::array();
    for (const auto &pin : s.output_pins) {
      nlohmann::json p;
      p["type"] = pin.type;
      p["name"] = pin.name;
      p["id"] = pin.id;
      schema["output_pins"].push_back(p);
    }

    schema["header_pin"]["type"] = s.header_pin.type;
    schema["header_pin"]["name"] = s.header_pin.name;
    schema["header_pin"]["id"] = s.header_pin.id;

    schema["spawnable"] = s.spawnable;
    if (s.spawnable) {
      nlohmann::json sp;
      sp["proper_name"] = s.spawn_possibility.proper_name;
      sp["proper_description"] = s.spawn_possibility.proper_description;
      sp["proper_logo"] = s.spawn_possibility.proper_logo;
      sp["category"] = s.spawn_possibility.category;
      sp["schema_id"] = s.spawn_possibility.schema_id;
      schema["spawn_possibility"] = sp;
    }

    schema["id"] = s.id;
    schema["header_color"] = s.header_color;
    schema["border_color"] = s.border_color;
    schema["background_color"] = s.background_color;
    schema["label"] = s.label;
    schema["label_color"] = s.label_color;
    schema["second_label"] = s.second_label;
    schema["second_label_color"] = s.second_label_color;
    schema["description_color"] = s.description_color;
    schema["header_logo_path"] = s.header_logo_path;
    schema["status"] = s.status;
    schema["type"] = s.type;

    j["ctx_ext"]["schemas"].push_back(schema);
  }

  return j;
}

NodeEdit::Graph NodeEdit::populate_graph(const nlohmann::json &j) {
  NodeEdit::Graph graph;

  if (j.contains("instances") && j["instances"].is_array()) {
    for (const auto &node : j["instances"]) {
      NodeEdit::NodeInstance inst;
      inst.type_id = node.value("type_id", "");
      inst.instance_id = node.value("instance_id", "");
      inst.pos_x = node.value("pos_x", 0.0f);
      inst.pos_y = node.value("pos_y", 0.0f);
      inst.size_x = node.value("size_x", 0.0f);
      inst.size_y = node.value("size_y", 0.0f);
      inst.datas = node.contains("datas") ? node["datas"] : nlohmann::json::object();
      graph.instances.push_back(std::move(inst));
    }
  }

  if (j.contains("connections") && j["connections"].is_array()) {
    for (const auto &c : j["connections"]) {
      NodeEdit::Connection conn;
      conn.node_instance_id_A = c.value("node_instance_id_A", "");
      conn.pin_id_A = c.value("pin_id_A", "");
      conn.node_instance_id_B = c.value("node_instance_id_B", "");
      conn.pin_id_B = c.value("pin_id_B", "");
      graph.connections.push_back(std::move(conn));
    }
  }

  if (j.contains("ctx_ext") && j["ctx_ext"].is_object()) {
    const auto &ext = j["ctx_ext"];

    if (ext.contains("pin_formats") && ext["pin_formats"].is_array()) {
      for (const auto &c : ext["pin_formats"]) {
        NodeEdit::PinFormat pf;
        pf.type = c.value("type", "");
        pf.delegate = c.value("delegate", false);
        pf.description = c.value("description", "");
        pf.name = c.value("name", "");
        pf.color = c.value("color", "");
        pf.shape = c.value("shape", "circle");
        graph.ext.pin_formats.push_back(std::move(pf));
      }
    }

    if (ext.contains("schemas") && ext["schemas"].is_array()) {
      for (const auto &s : ext["schemas"]) {
        NodeEdit::Schema schema;

        if (s.contains("input_pins") && s["input_pins"].is_array()) {
          for (const auto &p : s["input_pins"]) {
            NodeEdit::Pin pin;
            pin.type = p.value("type", "");
            pin.name = p.value("name", "");
            pin.id = p.value("id", "");
            schema.input_pins.push_back(std::move(pin));
          }
        }

        if (s.contains("output_pins") && s["output_pins"].is_array()) {
          for (const auto &p : s["output_pins"]) {
            NodeEdit::Pin pin;
            pin.type = p.value("type", "");
            pin.name = p.value("name", "");
            pin.id = p.value("id", "");
            schema.output_pins.push_back(std::move(pin));
          }
        }

        if (s.contains("header_pin") && s["header_pin"].is_object()) {
          schema.header_pin.type = s["header_pin"].value("type", "");
          schema.header_pin.name = s["header_pin"].value("name", "");
          schema.header_pin.id = s["header_pin"].value("id", "");
        }

        schema.spawnable = s.value("spawnable", false);
        if (schema.spawnable && s.contains("spawn_possibility") && s["spawn_possibility"].is_object()) {
          const auto &sp = s["spawn_possibility"];
          schema.spawn_possibility.proper_name = sp.value("proper_name", "");
          schema.spawn_possibility.proper_description = sp.value("proper_description", "");
          schema.spawn_possibility.proper_logo = sp.value("proper_logo", "");
          schema.spawn_possibility.category = sp.value("category", "");
          schema.spawn_possibility.schema_id = sp.value("schema_id", "");
        }

        schema.id = s.value("id", "");
        schema.header_color = s.value("header_color", "");
        schema.border_color = s.value("border_color", "");
        schema.background_color = s.value("background_color", "");
        schema.label = s.value("label", "");
        schema.label_color = s.value("label_color", "");
        schema.second_label = s.value("second_label", "");
        schema.second_label_color = s.value("second_label_color", "");
        schema.description_color = s.value("description_color", "");
        schema.header_logo_path = s.value("header_logo_path", "");
        schema.status = s.value("status", "");
        schema.type = s.value("type", "");

        graph.ext.schemas.push_back(std::move(schema));
      }
    }
  }

  return graph;
}

bool NodeEdit::is_context_exist(const std::string &ctx_name) {
  for (auto &c : get_current_context()->contexts) {
    if (!c) {
      return false;
    }
    if (c->id == ctx_name) {
      return true;
    }
  }
  return false;
}

std::string NodeEdit::get_graph_context_name(const nlohmann::json &j) {
  std::string context_name = "";

  if (j.contains("context_id") && j["context_id"].is_string()) {
    context_name = j.value("context_id", "");
  }

  return context_name;
}

nlohmann::json NodeEdit::get_graph_in_json(const nlohmann::json &j) {
  if (j.contains("graph") && j["graph"].is_object()) {
    return j["graph"];
  }

  return nlohmann::json::object();
}

const NodeEdit::NodeInstance *NodeEdit::find_instance(const NodeEdit::Graph &graph, const std::string &instance_id) {
  for (const auto &inst : graph.instances)
    if (inst.instance_id == instance_id)
      return &inst;
  return nullptr;
}

const NodeEdit::Schema *NodeEdit::find_schema(const NodeEdit::NodeContext &ctx, const std::string &type_id) {
  for (const auto &schema : ctx.schemas)
    if (schema.id == type_id)
      return &schema;
  return nullptr;
}

const NodeEdit::NodeContext *NodeEdit::find_context(const std::string &context_id) {
  for (const auto &ctx : get_current_context()->contexts)
    if (ctx->id == context_id)
      return ctx.get();
  return nullptr;
}

std::vector<std::string> NodeEdit::get_next_nodes(
    const std::shared_ptr<NodeEdit::GraphSession> &graph,
    const std::string &nodeid,
    const std::string &outputid) {
  if (!graph)
    return {};
  std::vector<std::string> result;
  for (const auto &conn : graph->graph.connections) {
    if (conn.node_instance_id_A == nodeid && conn.pin_id_A == outputid)
      result.push_back(conn.node_instance_id_B);
  }
  return result;
}

std::vector<std::string> NodeEdit::get_previous_nodes(
    const std::shared_ptr<NodeEdit::GraphSession> &graph,
    const std::string &nodeid,
    const std::string &inputid) {
  if (!graph)
    return {};
  std::vector<std::string> result;
  for (const auto &conn : graph->graph.connections) {
    if (conn.node_instance_id_B == nodeid && conn.pin_id_B == inputid)
      result.push_back(conn.node_instance_id_A);
  }
  return result;
}

std::string NodeEdit::search_node_output_pin_by_type(
    const std::shared_ptr<NodeEdit::GraphSession> &graph,
    const std::string &nodeid,
    const std::string &type) {
  if (!graph)
    return {};

  const auto *inst = find_instance(graph->graph, nodeid);
  if (!inst)
    return {};

  const auto *ctx = find_context(graph->context_id);
  if (!ctx)
    return {};

  const auto *schema = find_schema(*ctx, inst->type_id);
  if (!schema)
    return {};

  for (const auto &pin : schema->output_pins)
    if (pin.type == type)
      return pin.id;

  return {};
}

std::string NodeEdit::search_node_input_pin_by_type(
    const std::shared_ptr<NodeEdit::GraphSession> &graph,
    const std::string &nodeid,
    const std::string &type) {
  if (!graph)
    return {};

  const auto *inst = find_instance(graph->graph, nodeid);
  if (!inst)
    return {};

  const auto *ctx = find_context(graph->context_id);
  if (!ctx)
    return {};

  const auto *schema = find_schema(*ctx, inst->type_id);
  if (!schema)
    return {};

  for (const auto &pin : schema->input_pins)
    if (pin.type == type)
      return pin.id;

  return {};
}

std::string NodeEdit::search_node_type(const std::shared_ptr<NodeEdit::GraphSession> &graph, const std::string &type) {
  if (!graph)
    return {};

  const auto *ctx = find_context(graph->context_id);
  if (!ctx)
    return {};

  for (const auto &inst : graph->graph.instances) {
    const auto *schema = find_schema(*ctx, inst.type_id);
    if (schema && schema->type == type)
      return inst.instance_id;
  }
  return {};
}

std::vector<std::pair<std::string, std::string>> NodeEdit::get_all_node_input_pins(
    const std::shared_ptr<NodeEdit::GraphSession> &graph,
    const std::string &nodeid) {
  if (!graph)
    return {};

  const auto *inst = find_instance(graph->graph, nodeid);
  if (!inst)
    return {};

  const auto *ctx = find_context(graph->context_id);
  if (!ctx)
    return {};

  const auto *schema = find_schema(*ctx, inst->type_id);
  if (!schema)
    return {};

  std::vector<std::pair<std::string, std::string>> result;
  result.reserve(schema->input_pins.size());

  for (const auto &pin : schema->input_pins)
    result.emplace_back(pin.type, pin.id);

  return result;
}

std::vector<std::pair<std::string, std::string>> NodeEdit::get_all_node_output_pins(
    const std::shared_ptr<NodeEdit::GraphSession> &graph,
    const std::string &nodeid) {
  if (!graph)
    return {};

  const auto *inst = find_instance(graph->graph, nodeid);
  if (!inst)
    return {};

  const auto *ctx = find_context(graph->context_id);
  if (!ctx)
    return {};

  const auto *schema = find_schema(*ctx, inst->type_id);
  if (!schema)
    return {};

  std::vector<std::pair<std::string, std::string>> result;
  result.reserve(schema->output_pins.size());

  for (const auto &pin : schema->output_pins)
    result.emplace_back(pin.type, pin.id);

  return result;
}

// Effect API
void NodeEdit::add_effect_to_node(const std::shared_ptr<NodeEdit::GraphSession> &graph, const NodeEdit::NodeEffect &e) {
  if (!graph) {
    return;
  }
  graph->graph.node_effects.push_back(e);
  graph->graph.refresh_effects = true;
}

void NodeEdit::add_effect_to_connection(
    const std::shared_ptr<NodeEdit::GraphSession> &graph,
    const NodeEdit::ConnectionEffect &e) {
  if (!graph) {
    return;
  }
  graph->graph.connection_effects.push_back(e);
  graph->graph.refresh_effects = true;
}

void NodeEdit::remove_node_effects_from_node(
    const std::shared_ptr<NodeEdit::GraphSession> &graph,
    const std::string &nodeid) {
  if (!graph) {
    return;
  }
  auto &effects = graph->graph.node_effects;
  effects.erase(
      std::remove_if(effects.begin(), effects.end(), [&nodeid](const NodeEffect &e) { return e.instance_id == nodeid; }),
      effects.end());
  graph->graph.refresh_effects = true;
}

void NodeEdit::remove_connection_effects_from_node(
    const std::shared_ptr<NodeEdit::GraphSession> &graph,
    const std::string &nodeid) {
  if (!graph) {
    return;
  }
  auto &effects = graph->graph.connection_effects;
  effects.erase(
      std::remove_if(
          effects.begin(),
          effects.end(),
          [&nodeid](const ConnectionEffect &e) { return e.node_instance_id_A == nodeid || e.node_instance_id_B == nodeid; }),
      effects.end());
  graph->graph.refresh_effects = true;
}

void NodeEdit::remove_connection_effects_from_pin(
    const std::shared_ptr<NodeEdit::GraphSession> &graph,
    const std::string &nodeid,
    const std::string &pin_id) {
  if (!graph) {
    return;
  }
  auto &effects = graph->graph.connection_effects;
  effects.erase(
      std::remove_if(
          effects.begin(),
          effects.end(),
          [&nodeid, &pin_id](const ConnectionEffect &e) {
            return (e.node_instance_id_A == nodeid || e.node_instance_id_B == nodeid) &&
                   (e.pin_id_A == pin_id || e.pin_id_B == pin_id);
          }),
      effects.end());
  graph->graph.refresh_effects = true;
}

void NodeEdit::set_graph_title(const std::shared_ptr<NodeEdit::GraphSession> &graph, const std::string &title) {
  if (!graph) {
    return;
  }

  graph->graph.graph_title = title;
  graph->graph.refresh_effects = true;
}

void NodeEdit::add_schema_to_graph_ctx_ext(
    const std::shared_ptr<NodeEdit::GraphSession> &graph,
    const NodeEdit::Schema &schema) {
  if (!graph) {
    return;
  }

  graph->graph.ext.schemas.push_back(schema);
  graph->graph.refresh_ctx = true;
}

void NodeEdit::add_pin_format_to_graph_ctx_ext(
    const std::shared_ptr<NodeEdit::GraphSession> &graph,
    const NodeEdit::PinFormat &pin_format) {
  if (!graph) {
    return;
  }
  graph->graph.ext.pin_formats.push_back(pin_format);
  graph->graph.refresh_ctx = true;
}

std::shared_ptr<NodeEdit::GraphSession> NodeEdit::get_graph_session_by_id(const std::string &session_id) {
  for (const auto &gs : get_current_context()->graph_sessions) {
    if (gs->session_id == session_id) {
      return gs;
    }
  }
  return nullptr;
}
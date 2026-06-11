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
  std::cout << "sdf" << std::endl;
}

bool NodeEdit::IsGraphFile(const std::string &path) {
  std::string fullpath = path + "/graph.nodegraph";
  return fs::exists(fullpath);
}

void NodeEdit::OpenGraph(const std::string &path) {
  if (!IsGraphFile(path)) {
    get_current_context()->interface->log_error("No graph file in selected file ! (" + path + ")");
  }

  std::string fullpath = path + "/graph.nodegraph";

  std::ifstream file(fullpath);
  if (!file.is_open()) {
    get_current_context()->interface->log_error("Failed to open file : (" + fullpath + ")");
    return;
  }

  nlohmann::json j;
  try {
    file >> j;
  } catch (const nlohmann::json::parse_error &e) {
    get_current_context()->interface->log_error("Failed to parse graph file : (" + fullpath + ") -> " + e.what());
    return;
  }

  std::string ctx_name = GetGraphContextName(j);

  if (!IsContextExist(ctx_name)) {
    get_current_context()->interface->log_error(
        "This graph contain an unknown graph context : (" + fullpath + "). Did you have a missing module ?");
  }

  auto ctx = NodeEdit::get_node_context(ctx_name);

  auto graph = PopulateGraph(GetGraph(j));
  graph.graph_title = "Node graph";
  graph.refresh_effects = true;

  auto gs = std::make_shared<NodeEdit::NodeEditGraphSession>();
  gs->name = path;
  gs->path = fullpath;
  gs->graph = graph;
  gs->context_id = ctx_name;
  {
    auto inst = ModuleUI::NodeEditorAppWindow::Create("Node graph", ctx, gs);
    Cherry::AddAppWindow(inst->GetAppWindow());
    get_current_context()->editor_instances.push_back(inst);
  }
  {
    auto inst = ModuleUI::NodeEditorDebugger::Create("Node debug", gs);
    Cherry::AddAppWindow(inst->GetAppWindow());
    get_current_context()->editor_debuggers.push_back(inst);
  }
}

/*
Input events :
add_pin_format (params : ctx and pin format object)
add_schema (params : ctx and pin format object)
get_connection (id/name of I/O pin )
get_connection (only type and if Input or Ouput) (for sequences pins for
example)

*/

void NodeEdit::SetupExampleContext() {
  // Create node context
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
    "header_pin": {
      "id": "hbool",
      "name": "test",
      "type": "bool"
    },
    "input_pins": [
      { "id": "bool1", "name": "Bool One", "type": "bool" },
      { "id": "bool2", "type": "bool" }
    ],
    "output_pins": [
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

std::shared_ptr<NodeEdit::NodeEditContext> NodeEdit::create_node_context(const std::string &name) {
  auto &contexts = get_current_context()->contexts;
  auto existing =
      std::find_if(contexts.begin(), contexts.end(), [&name](const auto &ptr) { return ptr && ptr->id == name; });
  if (existing != contexts.end()) {
    get_current_context()->interface->log_error(name + " node graph context already exist !");
  }

  auto ctx = std::make_shared<NodeEditContext>();
  ctx->id = name;

  contexts.push_back(ctx);

  return ctx;
}

std::shared_ptr<NodeEdit::NodeEditContext> NodeEdit::get_node_context(const std::string &name) {
  auto &contexts = get_current_context()->contexts;
  auto existing =
      std::find_if(contexts.begin(), contexts.end(), [&name](const auto &ptr) { return ptr && ptr->id == name; });

  return *existing;
}

void NodeEdit::DestroyContext(const std::string &name) {
  auto &contexts = get_current_context()->contexts;

  contexts.erase(
      std::remove_if(
          contexts.begin(),
          contexts.end(),
          [&name](const std::shared_ptr<NodeEditContext> &ptr) { return ptr && ptr->id == name; }),
      contexts.end());
}

void NodeEdit::AddSchemaToContext(const std::string &ctx_id, const NodeEdit::NodeEditSchema &schema) {
  auto &contexts = get_current_context()->contexts;

  auto c = std::find_if(contexts.begin(), contexts.end(), [&ctx_id](const std::shared_ptr<NodeEditContext> &ptr) {
    return ptr && ptr->id == ctx_id;
  });

  if (c == contexts.end()) {
    get_current_context()->interface->log_error(ctx_id + " does not exist! Cannot add schema to this context.");
    return;
  }

  (*c)->schemas.push_back(schema);
}

void NodeEdit::AddPinFormatToContext(const std::string &ctx_id, const NodeEdit::NodeEditPinFormat &pin_format) {
  auto &contexts = get_current_context()->contexts;

  auto c = std::find_if(contexts.begin(), contexts.end(), [&ctx_id](const std::shared_ptr<NodeEditContext> &ptr) {
    return ptr && ptr->id == ctx_id;
  });

  if (c == contexts.end()) {
    get_current_context()->interface->log_error(ctx_id + " does not exist! Cannot add schema to this context.");
    return;
  }

  (*c)->pin_formats.push_back(pin_format);
}

void NodeEdit::SaveGraphSession(const std::shared_ptr<NodeEdit::NodeEditGraphSession> &gs) {
  if (!gs) {
    return;
  }

  auto file_path = gs->path;

  auto json = DumpGraph(gs->graph);
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

void NodeEdit::RefreshGraphSession(const std::shared_ptr<NodeEdit::NodeEditGraphSession> &gs) {
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

  std::string ctx_name = GetGraphContextName(j);

  if (!IsContextExist(ctx_name)) {
    get_current_context()->interface->log_error(
        "This graph contain an unknown graph context : (" + file_path + "). Did you have a missing module ?");
    return;
  }

  auto graph = PopulateGraph(GetGraph(j));

  gs->graph = graph;
}

nlohmann::json NodeEdit::DumpGraph(const NodeEdit::NodeEditGraph &graph) {
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

NodeEdit::NodeEditGraph NodeEdit::PopulateGraph(const nlohmann::json &j) {
  NodeEdit::NodeEditGraph graph;

  if (j.contains("instances") && j["instances"].is_array()) {
    for (const auto &node : j["instances"]) {
      NodeEdit::NodeEditInstance inst;
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
      NodeEdit::NodeEditConnection conn;
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
        NodeEdit::NodeEditPinFormat pf;
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
        NodeEdit::NodeEditSchema schema;

        if (s.contains("input_pins") && s["input_pins"].is_array()) {
          for (const auto &p : s["input_pins"]) {
            NodeEdit::NodeEditPin pin;
            pin.type = p.value("type", "");
            pin.name = p.value("name", "");
            pin.id = p.value("id", "");
            schema.input_pins.push_back(std::move(pin));
          }
        }

        if (s.contains("output_pins") && s["output_pins"].is_array()) {
          for (const auto &p : s["output_pins"]) {
            NodeEdit::NodeEditPin pin;
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

bool NodeEdit::IsContextExist(const std::string &ctx_name) {
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

std::string NodeEdit::GetGraphContextName(const nlohmann::json &j) {
  std::string context_name = "";

  if (j.contains("context_id") && j["context_id"].is_string()) {
    context_name = j.value("context_id", "");
  }

  return context_name;
}

nlohmann::json NodeEdit::GetGraph(const nlohmann::json &j) {
  if (j.contains("graph") && j["graph"].is_object()) {
    return j["graph"];
  }

  return nlohmann::json::object();
}

const NodeEdit::NodeEditInstance *NodeEdit::FindInstance(
    const NodeEdit::NodeEditGraph &graph,
    const std::string &instance_id) {
  for (const auto &inst : graph.instances)
    if (inst.instance_id == instance_id)
      return &inst;
  return nullptr;
}

const NodeEdit::NodeEditSchema *NodeEdit::FindSchema(const NodeEdit::NodeEditContext &ctx, const std::string &type_id) {
  for (const auto &schema : ctx.schemas)
    if (schema.id == type_id)
      return &schema;
  return nullptr;
}

const NodeEdit::NodeEditContext *NodeEdit::FindContext(const std::string &context_id) {
  for (const auto &ctx : get_current_context()->contexts)
    if (ctx->id == context_id)
      return ctx.get();
  return nullptr;
}

std::string NodeEdit::GetNextNode(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const std::string &nodeid,
    const std::string &outputid) {
  if (!graph)
    return {};

  for (const auto &conn : graph->graph.connections) {
    if (conn.node_instance_id_A == nodeid && conn.pin_id_A == outputid)
      return conn.node_instance_id_B;
  }
  return {};
}

std::string NodeEdit::GetPreviousNode(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const std::string &nodeid,
    const std::string &inputid) {
  if (!graph)
    return {};

  for (const auto &conn : graph->graph.connections) {
    if (conn.node_instance_id_B == nodeid && conn.pin_id_B == inputid)
      return conn.node_instance_id_A;
  }
  return {};
}

std::string NodeEdit::SearchNodeOutputPinByType(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const std::string &nodeid,
    const std::string &type) {
  if (!graph)
    return {};

  const auto *inst = FindInstance(graph->graph, nodeid);
  if (!inst)
    return {};

  const auto *ctx = FindContext(graph->context_id);
  if (!ctx)
    return {};

  const auto *schema = FindSchema(*ctx, inst->type_id);
  if (!schema)
    return {};

  for (const auto &pin : schema->output_pins)
    if (pin.type == type)
      return pin.id;

  return {};
}

std::string NodeEdit::SearchNodeInputPinByType(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const std::string &nodeid,
    const std::string &type) {
  if (!graph)
    return {};

  const auto *inst = FindInstance(graph->graph, nodeid);
  if (!inst)
    return {};

  const auto *ctx = FindContext(graph->context_id);
  if (!ctx)
    return {};

  const auto *schema = FindSchema(*ctx, inst->type_id);
  if (!schema)
    return {};

  for (const auto &pin : schema->input_pins)
    if (pin.type == type)
      return pin.id;

  return {};
}

std::string NodeEdit::SearchNodeType(const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph, const std::string &type) {
  if (!graph)
    return {};

  const auto *ctx = FindContext(graph->context_id);
  if (!ctx)
    return {};

  for (const auto &inst : graph->graph.instances) {
    const auto *schema = FindSchema(*ctx, inst.type_id);
    if (schema && schema->type == type)
      return inst.instance_id;
  }
  return {};
}

std::vector<std::pair<std::string, std::string>> NodeEdit::GetAllNodeInputPins(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const std::string &nodeid) {
  if (!graph)
    return {};

  const auto *inst = FindInstance(graph->graph, nodeid);
  if (!inst)
    return {};

  const auto *ctx = FindContext(graph->context_id);
  if (!ctx)
    return {};

  const auto *schema = FindSchema(*ctx, inst->type_id);
  if (!schema)
    return {};

  std::vector<std::pair<std::string, std::string>> result;
  result.reserve(schema->input_pins.size());

  for (const auto &pin : schema->input_pins)
    result.emplace_back(pin.type, pin.id);

  return result;
}

std::vector<std::pair<std::string, std::string>> NodeEdit::GetAllNodeOutputPins(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const std::string &nodeid) {
  if (!graph)
    return {};

  const auto *inst = FindInstance(graph->graph, nodeid);
  if (!inst)
    return {};

  const auto *ctx = FindContext(graph->context_id);
  if (!ctx)
    return {};

  const auto *schema = FindSchema(*ctx, inst->type_id);
  if (!schema)
    return {};

  std::vector<std::pair<std::string, std::string>> result;
  result.reserve(schema->output_pins.size());

  for (const auto &pin : schema->output_pins)
    result.emplace_back(pin.type, pin.id);

  return result;
}

// Effect API
void NodeEdit::AddEffectToNode(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const NodeEdit::NodeEditNodeEffect &e) {
  if (!graph) {
    return;
  }
  graph->graph.node_effects.push_back(e);
  graph->graph.refresh_effects = true;
}

void NodeEdit::AddEffectToConnection(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const NodeEdit::NodeEditConnectionEffect &e) {
  if (!graph) {
    return;
  }
  graph->graph.connection_effects.push_back(e);
  graph->graph.refresh_effects = true;
}

void NodeEdit::RemoveNodeEffectsFromNode(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const std::string &nodeid) {
  if (!graph) {
    return;
  }
  auto &effects = graph->graph.node_effects;
  effects.erase(
      std::remove_if(
          effects.begin(), effects.end(), [&nodeid](const NodeEditNodeEffect &e) { return e.instance_id == nodeid; }),
      effects.end());
  graph->graph.refresh_effects = true;
}

void NodeEdit::RemoveConnectionEffectsFromNode(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const std::string &nodeid) {
  if (!graph) {
    return;
  }
  auto &effects = graph->graph.connection_effects;
  effects.erase(
      std::remove_if(
          effects.begin(),
          effects.end(),
          [&nodeid](const NodeEditConnectionEffect &e) {
            return e.node_instance_id_A == nodeid || e.node_instance_id_B == nodeid;
          }),
      effects.end());
  graph->graph.refresh_effects = true;
}

void NodeEdit::RemoveConnectionEffectsFromPin(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
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
          [&nodeid, &pin_id](const NodeEditConnectionEffect &e) {
            return (e.node_instance_id_A == nodeid || e.node_instance_id_B == nodeid) &&
                   (e.pin_id_A == pin_id || e.pin_id_B == pin_id);
          }),
      effects.end());
  graph->graph.refresh_effects = true;
}

void NodeEdit::SetGraphTitle(const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph, const std::string &title) {
  if (!graph) {
    return;
  }

  graph->graph.graph_title = title;
  graph->graph.refresh_effects = true;
}

void NodeEdit::AddSchemaToGraphCtxExt(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const NodeEdit::NodeEditSchema &schema) {
  if (!graph) {
    return;
  }

  graph->graph.ext.schemas.push_back(schema);
  graph->graph.refresh_ctx = true;
}

void NodeEdit::AddPinFormatToGraphCtxExt(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const NodeEdit::NodeEditPinFormat &pin_format) {
  if (!graph) {
    return;
  }
  graph->graph.ext.pin_formats.push_back(pin_format);
  graph->graph.refresh_ctx = true;
}
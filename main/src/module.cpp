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
  auto ctx = SetupExampleContext();
  auto gs = SetupExampleGraphSession();
  auto inst = ModuleUI::NodeEditorAppWindow::Create("TEST", ctx, gs);
  Cherry::AddAppWindow(inst->GetAppWindow());
  get_current_context()->editor_instances.push_back(inst);
}

bool NodeEdit::IsGraphFile(const std::string &path) {
  std::string fullpath = path + "/graph.nodegraph";
  return fs::exists(fullpath);
}

void NodeEdit::OpenGraph(const std::string &path) {
  auto ctx = SetupExampleContext();

  if (!IsGraphFile(path)) {
    get_current_context()->interface->log_error(
        "No graph file in selected file ! (" + path + ")");
  }

  std::string fullpath = path + "/graph.nodegraph";

  std::ifstream file(fullpath);
  if (!file.is_open()) {
    get_current_context()->interface->log_error("Failed to open file : (" +
                                                fullpath + ")");
    return;
  }

  nlohmann::json j;
  try {
    file >> j;
  } catch (const nlohmann::json::parse_error &e) {
    get_current_context()->interface->log_error(
        "Failed to parse graph file : (" + fullpath + ") -> " + e.what());
    return;
  }

  std::string ctx_name = GetGraphContextName(j);

  if (!IsContextExist(ctx_name)) {
    get_current_context()->interface->log_error(
        "This graph contain an unknown graph context : (" + fullpath +
        "). Did you have a missing module ?");
  }

  auto graph = PopulateGraph(GetGraph(j));

  auto gs = std::make_shared<NodeEdit::NodeEditGraphSession>();
  gs->name = path;
  gs->path = fullpath;
  gs->graph = graph;
  gs->context_id = ctx_name;

  auto inst = ModuleUI::NodeEditorAppWindow::Create("TEST", ctx, gs);
  Cherry::AddAppWindow(inst->GetAppWindow());
  get_current_context()->editor_instances.push_back(inst);
}

/*
Input events :
add_pin_format (params : ctx and pin format object)
add_schema (params : ctx and pin format object)
get_connection (id/name of I/O pin )
get_connection (only type and if Input or Ouput) (for sequences pins for
example)

*/

std::shared_ptr<NodeEdit::NodeEditContext> NodeEdit::SetupExampleContext() {
  auto c = NodeEdit::CreateContext("efusion_blueprint");

  // Setup pins formats
  {
    NodeEditPinFormat pf;
    pf.type = "bool";
    pf.name = "Boolean";
    pf.color = "#AF2525";
    pf.shape = "circle";
    pf.description = "Simple boolean";
    NodeEdit::AddPinFormatToContext(c->id, pf);
  }

  // Setup schemas
  {
    NodeEditSchema s;
    s.id = "is_cool";
    s.type = "blueprint";
    s.status = "active";
    s.second_label = "This is cool";
    s.label = "Is cool";
    s.header_color = "#B1FF31";

    NodeEditPin pi_a;
    pi_a.id = "bool1";
    pi_a.type = "bool";
    s.input_pins.push_back(pi_a);
    NodeEditPin pi_b;
    pi_b.id = "bool2";
    pi_b.type = "bool";
    s.input_pins.push_back(pi_b);
    NodeEditPin po_a;
    po_a.id = "bool3";
    po_a.type = "bool";
    s.output_pins.push_back(po_a);
    NodeEditPin po_b;
    po_b.id = "bool4";
    po_b.type = "bool";
    s.output_pins.push_back(po_b);

    s.spawnable = true;
    s.spawn_possibility.category = "base";
    s.spawn_possibility.proper_description = "Spwan a is cool node";
    s.spawn_possibility.proper_logo =
        NodeEdit::GetPath("resources/icons/edit.png");
    s.spawn_possibility.proper_name = "Is Cool";
    s.spawn_possibility.schema_id = "is_cool";

    NodeEdit::AddSchemaToContext(c->id, s);
  }

  return c;
}

std::shared_ptr<NodeEdit::NodeEditGraphSession>
NodeEdit::SetupExampleGraphSession() {
  auto gs = std::make_shared<NodeEdit::NodeEditGraphSession>();

  {
    NodeEdit::NodeEditInstance inst;
    inst.type_id = "is_cool";
    inst.instance_id = "test1";
    inst.pos_x = 40;
    inst.pos_y = 40;
    gs->graph.instances.push_back(inst);
  }

  {
    NodeEdit::NodeEditInstance inst;
    inst.type_id = "is_cool";
    inst.instance_id = "test2";
    inst.pos_x = 100;
    inst.pos_y = 100;
    gs->graph.instances.push_back(inst);
  }

  return gs;
}

std::shared_ptr<NodeEdit::NodeEditContext>
NodeEdit::CreateContext(const std::string &name) {
  auto &contexts = get_current_context()->contexts;
  auto existing =
      std::find_if(contexts.begin(), contexts.end(),
                   [&name](const auto &ptr) { return ptr && ptr->id == name; });
  if (existing != contexts.end()) {
    get_current_context()->interface->log_error(
        name + " node graph context already exist !");
  }

  auto ctx = std::make_shared<NodeEditContext>();
  ctx->id = name;

  contexts.push_back(ctx);

  return ctx;
}

void NodeEdit::DestroyContext(const std::string &name) {
  auto &contexts = get_current_context()->contexts;

  contexts.erase(
      std::remove_if(contexts.begin(), contexts.end(),
                     [&name](const std::shared_ptr<NodeEditContext> &ptr) {
                       return ptr && ptr->id == name;
                     }),
      contexts.end());
}

void NodeEdit::AddSchemaToContext(const std::string &ctx_id,
                                  const NodeEditSchema &schema) {
  auto &contexts = get_current_context()->contexts;

  auto c = std::find_if(contexts.begin(), contexts.end(),
                        [&ctx_id](const std::shared_ptr<NodeEditContext> &ptr) {
                          return ptr && ptr->id == ctx_id;
                        });

  if (c == contexts.end()) {
    get_current_context()->interface->log_error(
        ctx_id + " does not exist! Cannot add schema to this context.");
    return;
  }

  (*c)->schemas.push_back(schema);
}

void NodeEdit::AddPinFormatToContext(const std::string &ctx_id,
                                     const NodeEditPinFormat &pin_format) {
  auto &contexts = get_current_context()->contexts;

  auto c = std::find_if(contexts.begin(), contexts.end(),
                        [&ctx_id](const std::shared_ptr<NodeEditContext> &ptr) {
                          return ptr && ptr->id == ctx_id;
                        });

  if (c == contexts.end()) {
    get_current_context()->interface->log_error(
        ctx_id + " does not exist! Cannot add schema to this context.");
    return;
  }

  (*c)->pin_formats.push_back(pin_format);
}

void NodeEdit::SaveGraphSession(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &gs) {
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

void NodeEdit::RefreshGraphSession(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &gs) {
  if (!gs) {
    return;
  }

  auto file_path = gs->path;

  std::ifstream file(file_path);
  if (!file.is_open()) {
    get_current_context()->interface->log_error("Failed to open file : (" +
                                                file_path + ")");
    return;
  }

  nlohmann::json j;
  try {
    file >> j;
  } catch (const nlohmann::json::parse_error &e) {
    get_current_context()->interface->log_error(
        "Failed to parse graph file : (" + file_path + ") -> " + e.what());
    return;
  }

  std::string ctx_name = GetGraphContextName(j);

  if (!IsContextExist(ctx_name)) {
    get_current_context()->interface->log_error(
        "This graph contain an unknown graph context : (" + file_path +
        "). Did you have a missing module ?");
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
      inst.datas =
          node.contains("datas") ? node["datas"] : nlohmann::json::object();
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
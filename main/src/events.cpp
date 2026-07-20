#include "module.hpp"

void NodeEdit::ie_create_node_context(ArgumentValues &args, ReturnValues &ret) {
  if (args.get_json().contains("name")) {
    const std::string ctx_name = args.get_json()["name"];
    NodeEdit::create_node_context(ctx_name);
    // TODO: ret true
  } else {
    // TODO: ret false
  }
}

void NodeEdit::ie_setup_pin_format(ArgumentValues &args, ReturnValues &ret) {
  if (args.get_json().contains("context_name")) {
    const std::string ctx_name = args.get_json()["context_name"];
    auto c = NodeEdit::get_node_context(ctx_name);
    // TODO: Use is_context_exist;

    if (!c) {
      // TODO error
      return;
    }

    PinFormat pf;
    if (args.get_json().contains("type")) {
      const std::string type = args.get_json()["type"];
      pf.type = type;
    } else {
      // TODO error
      return;
    }
    if (args.get_json().contains("name")) {
      const std::string name = args.get_json()["name"];
      pf.name = name;
    } else {
      // TODO error
      return;
    }
    if (args.get_json().contains("color")) {
      const std::string color = args.get_json()["color"];
      pf.color = color;
    } else {
      // TODO error
      return;
    }
    if (args.get_json().contains("shape")) {
      const std::string shape = args.get_json()["shape"];
      pf.shape = shape;
    } else {
      // TODO error
      return;
    }
    if (args.get_json().contains("description")) {
      const std::string description = args.get_json()["description"];
      pf.description = description;
    } else {
      // TODO error
      return;
    }

    // optional
    if (args.get_json().contains("delegate")) {
      const bool delegate = args.get_json()["delegate"];
      pf.delegate = delegate;
    }

    NodeEdit::add_pin_format_to_context(c->id, pf);

  } else {
  }
}

void NodeEdit::ie_setup_schema(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();

  if (!j.contains("context_name"))
    return;
  const std::string ctx_name = j["context_name"];

  auto c = NodeEdit::get_node_context(ctx_name);
  if (!c) {
    get_current_context()->m_interface->log_error("Cannot create schema for ctx: (" + ctx_name + "). Context not found.");
    // TODO err in return values
    return;
  }

  Schema s;

  if (!j.contains("id"))
    return;
  s.id = j["id"];
  if (!j.contains("type"))
    return;
  s.type = j["type"];

  if (j.contains("header_logo_path"))
    s.header_logo_path = j["header_logo_path"];
  if (j.contains("status"))
    s.status = j["status"];
  if (j.contains("description_color"))
    s.description_color = j["description_color"];
  if (j.contains("second_label"))
    s.second_label = j["second_label"];
  if (j.contains("second_label_color"))
    s.second_label_color = j["second_label_color"];
  if (j.contains("label"))
    s.label = j["label"];
  if (j.contains("label_color"))
    s.label_color = j["label_color"];
  if (j.contains("header_color"))
    s.header_color = j["header_color"];
  if (j.contains("border_color"))
    s.border_color = j["border_color"];
  if (j.contains("background_color"))
    s.background_color = j["background_color"];
  if (j.contains("spawnable"))
    s.spawnable = j["spawnable"];

  if (j.contains("header_pin")) {
    const auto &hp = j["header_pin"];
    if (hp.contains("id"))
      s.header_pin.id = hp["id"];
    if (hp.contains("name"))
      s.header_pin.name = hp["name"];
    if (hp.contains("type"))
      s.header_pin.type = hp["type"];
  }

  if (j.contains("input_pins")) {
    for (const auto &pin_json : j["input_pins"]) {
      Pin pin;
      if (pin_json.contains("id"))
        pin.id = pin_json["id"];
      if (pin_json.contains("name"))
        pin.name = pin_json["name"];
      if (pin_json.contains("type"))
        pin.type = pin_json["type"];
      s.input_pins.push_back(pin);
    }
  }

  if (j.contains("output_pins")) {
    for (const auto &pin_json : j["output_pins"]) {
      Pin pin;
      if (pin_json.contains("id"))
        pin.id = pin_json["id"];
      if (pin_json.contains("name"))
        pin.name = pin_json["name"];
      if (pin_json.contains("type"))
        pin.type = pin_json["type"];
      s.output_pins.push_back(pin);
    }
  }

  if (j.contains("spawn_possibility")) {
    const auto &sp = j["spawn_possibility"];
    if (sp.contains("category"))
      s.spawn_possibility.category = sp["category"];
    if (sp.contains("proper_description"))
      s.spawn_possibility.proper_description = sp["proper_description"];
    if (sp.contains("proper_logo"))
      s.spawn_possibility.proper_logo = sp["proper_logo"];
    if (sp.contains("proper_name"))
      s.spawn_possibility.proper_name = sp["proper_name"];
    if (sp.contains("schema_id"))
      s.spawn_possibility.schema_id = sp["schema_id"];
  }

  NodeEdit::add_schema_to_context(c->id, s);
}

void NodeEdit::ie_setup_schema_to_graph_ext(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();

  if (!j.contains("session_id"))
    return;
  const std::string session_id = j["session_id"];

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    get_current_context()->m_interface->log_error(
        "Cannot find graph session for id: (" + gs->session_id + "). Context not found.");
    // TODO err in return values
    return;
  }

  Schema s;

  if (!j.contains("id"))
    return;
  s.id = j["id"];
  if (!j.contains("type"))
    return;
  s.type = j["type"];

  if (j.contains("header_logo_path"))
    s.header_logo_path = j["header_logo_path"];
  if (j.contains("status"))
    s.status = j["status"];
  if (j.contains("description_color"))
    s.description_color = j["description_color"];
  if (j.contains("second_label"))
    s.second_label = j["second_label"];
  if (j.contains("second_label_color"))
    s.second_label_color = j["second_label_color"];
  if (j.contains("label"))
    s.label = j["label"];
  if (j.contains("label_color"))
    s.label_color = j["label_color"];
  if (j.contains("header_color"))
    s.header_color = j["header_color"];
  if (j.contains("border_color"))
    s.border_color = j["border_color"];
  if (j.contains("background_color"))
    s.background_color = j["background_color"];
  if (j.contains("spawnable"))
    s.spawnable = j["spawnable"];

  if (j.contains("header_pin")) {
    const auto &hp = j["header_pin"];
    if (hp.contains("id"))
      s.header_pin.id = hp["id"];
    if (hp.contains("name"))
      s.header_pin.name = hp["name"];
    if (hp.contains("type"))
      s.header_pin.type = hp["type"];
  }

  if (j.contains("input_pins")) {
    for (const auto &pin_json : j["input_pins"]) {
      Pin pin;
      if (pin_json.contains("id"))
        pin.id = pin_json["id"];
      if (pin_json.contains("name"))
        pin.name = pin_json["name"];
      if (pin_json.contains("type"))
        pin.type = pin_json["type"];
      s.input_pins.push_back(pin);
    }
  }

  if (j.contains("output_pins")) {
    for (const auto &pin_json : j["output_pins"]) {
      Pin pin;
      if (pin_json.contains("id"))
        pin.id = pin_json["id"];
      if (pin_json.contains("name"))
        pin.name = pin_json["name"];
      if (pin_json.contains("type"))
        pin.type = pin_json["type"];
      s.output_pins.push_back(pin);
    }
  }

  if (j.contains("spawn_possibility")) {
    const auto &sp = j["spawn_possibility"];
    if (sp.contains("category"))
      s.spawn_possibility.category = sp["category"];
    if (sp.contains("proper_description"))
      s.spawn_possibility.proper_description = sp["proper_description"];
    if (sp.contains("proper_logo"))
      s.spawn_possibility.proper_logo = sp["proper_logo"];
    if (sp.contains("proper_name"))
      s.spawn_possibility.proper_name = sp["proper_name"];
    if (sp.contains("schema_id"))
      s.spawn_possibility.schema_id = sp["schema_id"];
  }
  NodeEdit::add_schema_to_graph_ctx_ext(gs, s);
}

void NodeEdit::ie_open_graph(ArgumentValues &args, ReturnValues &ret) {
  if (!args.get_json().contains("path"))
    return;
  const std::string path = args.get_json()["path"];

  bool disable_native_saving_system = false;
  std::string parent_appwindow = "";  // nothing by default
  std::string logo_path = "";         // nothing by default
  std::string custom_name = "";       // nothing by default

  if (args.get_json().contains("disable_native_saving_system") &&
      args.get_json()["disable_native_saving_system"].is_boolean()) {
    disable_native_saving_system = args.get_json()["disable_native_saving_system"];
  }

  if (args.get_json().contains("parent_appwindow") && args.get_json()["parent_appwindow"].is_string()) {
    parent_appwindow = args.get_json()["parent_appwindow"];
  }

  if (args.get_json().contains("logo_path") && args.get_json()["logo_path"].is_string()) {
    logo_path = args.get_json()["logo_path"];
  }

  if (args.get_json().contains("name") && args.get_json()["name"].is_string()) {
    custom_name = args.get_json()["name"];
  }

  auto gs =
      NodeEdit::open_graph_and_get_session(path, parent_appwindow, disable_native_saving_system, logo_path, custom_name);
  std::string id = gs->session_id;

  if (args.get_json().contains("graph_title") && args.get_json()["graph_title"].is_string()) {
    gs->graph.graph_title = args.get_json()["graph_title"];
  }

  nlohmann::json result;
  result["session_id"] = id;
  ret = ReturnValues(result.dump());
}

void NodeEdit::ie_add_effect_to_node(ArgumentValues &args, ReturnValues &ret) {
  if (!args.get_json().contains("session_id"))
    return;
  const std::string session_id = args.get_json()["session_id"];

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    // TODO: error
    return;
  }

  NodeEdit::NodeEffect e;

  if (args.get_json().contains("bg_color"))
    e.bg_color = args.get_json()["bg_color"];

  if (args.get_json().contains("instance_id"))
    e.instance_id = args.get_json()["instance_id"];

  if (args.get_json().contains("text"))
    e.text = args.get_json()["text"];

  if (args.get_json().contains("text_color"))
    e.text_color = args.get_json()["text_color"];

  if (args.get_json().contains("type"))
    e.type = args.get_json()["type"];

  NodeEdit::add_effect_to_node(gs, e);
}

void NodeEdit::ie_add_effect_to_connection(ArgumentValues &args, ReturnValues &ret) {
  if (!args.get_json().contains("session_id"))
    return;
  const std::string session_id = args.get_json()["session_id"];

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    get_current_context()->m_interface->log_error("Graph session not found! (" + session_id + ")");
    return;
  }

  NodeEdit::ConnectionEffect e;

  if (args.get_json().contains("flow_color"))
    e.flow_color = args.get_json()["flow_color"];

  if (args.get_json().contains("flow_intensity"))
    e.flow_intensity = args.get_json()["flow_intensity"];

  if (args.get_json().contains("flow_reverse"))
    e.flow_reverse = args.get_json()["flow_reverse"];

  if (args.get_json().contains("flow_speed"))
    e.flow_speed = args.get_json()["flow_speed"];

  if (args.get_json().contains("node_instance_id_A"))
    e.node_instance_id_A = args.get_json()["node_instance_id_A"];

  if (args.get_json().contains("node_instance_id_B"))
    e.node_instance_id_B = args.get_json()["node_instance_id_B"];

  if (args.get_json().contains("pin_id_A"))
    e.pin_id_A = args.get_json()["pin_id_A"];

  if (args.get_json().contains("pin_id_B"))
    e.pin_id_B = args.get_json()["pin_id_B"];

  if (args.get_json().contains("pulsating_color"))
    e.pulsating_color = args.get_json()["pulsating_color"];

  if (args.get_json().contains("pulsating_intensity"))
    e.pulsating_intensity = args.get_json()["pulsating_intensity"];

  if (args.get_json().contains("pulsating_rate"))
    e.pulsating_rate = args.get_json()["pulsating_rate"];

  if (args.get_json().contains("type"))
    e.type = args.get_json()["type"];

  NodeEdit::add_effect_to_connection(gs, e);
}

void NodeEdit::ie_remove_node_effects_from_node(ArgumentValues &args, ReturnValues &ret) {
  if (!args.get_json().contains("session_id"))
    return;
  const std::string session_id = args.get_json()["session_id"];

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    // TODO: error
    return;
  }

  if (args.get_json().contains("node_id")) {
    const std::string node_id = args.get_json()["node_id"];
    NodeEdit::remove_node_effects_from_node(gs, node_id);
  } else {
    // TODO: error
  }
}

void NodeEdit::ie_remove_connection_effects_from_node(ArgumentValues &args, ReturnValues &ret) {
  if (!args.get_json().contains("session_id"))
    return;
  const std::string session_id = args.get_json()["session_id"];

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    // TODO: error
    return;
  }

  if (args.get_json().contains("node_id")) {
    const std::string node_id = args.get_json()["node_id"];
    NodeEdit::remove_connection_effects_from_node(gs, node_id);
  } else {
    // TODO: error
  }
}

void NodeEdit::ie_remove_connection_effects_from_pin(ArgumentValues &args, ReturnValues &ret) {
  if (!args.get_json().contains("session_id"))
    return;
  const std::string session_id = args.get_json()["session_id"];

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    // TODO: error
    return;
  }

  if (args.get_json().contains("node_id") && args.get_json().contains("pin_id")) {
    const std::string node_id = args.get_json()["node_id"];
    const std::string pin_id = args.get_json()["pin_id"];

    NodeEdit::remove_connection_effects_from_pin(gs, node_id, pin_id);
  } else {
    // TODO: error
  }
}

void NodeEdit::ie_set_graph_title(ArgumentValues &args, ReturnValues &ret) {
  if (!args.get_json().contains("session_id"))
    return;
  const std::string session_id = args.get_json()["session_id"];

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    // TODO: error
    return;
  }

  if (args.get_json().contains("title")) {
    const std::string title = args.get_json()["title"];
    NodeEdit::set_graph_title(gs, title);
  } else {
    // TODO: error
  }
}

void NodeEdit::ie_get_next_nodes(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();
  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    get_current_context()->m_interface->log_error("Graph session not found! (" + session_id + ")");
    return;
  }
  if (j.contains("node_id") && j.contains("output_id") && j["node_id"].is_string() && j["output_id"].is_string()) {
    const std::string node_id = j["node_id"].get<std::string>();
    const std::string output_id = j["output_id"].get<std::string>();
    const std::vector<std::string> result = NodeEdit::get_next_nodes(gs, node_id, output_id);
    ret.set_json({ { "node_ids", result } });
  } else {
    get_current_context()->m_interface->log_error("Cannot fetch next nodes, cannot find output_id or node_id");
  }
}

void NodeEdit::ie_get_previous_nodes(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();
  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    get_current_context()->m_interface->log_error("Graph session not found! (" + session_id + ")");
    return;
  }
  if (j.contains("node_id") && j.contains("input_id") && j["node_id"].is_string() && j["input_id"].is_string()) {
    const std::string node_id = j["node_id"].get<std::string>();
    const std::string input_id = j["input_id"].get<std::string>();
    const std::vector<std::string> result = NodeEdit::get_previous_nodes(gs, node_id, input_id);
    ret.set_json({ { "node_ids", result } });
  } else {
    get_current_context()->m_interface->log_error("Cannot fetch previous nodes, cannot find input_id or node_id");
  }
}

void NodeEdit::ie_search_node_output_pin_by_type(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();

  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    // TODO: error
    return;
  }

  if (j.contains("node_id") && j.contains("type") && j["node_id"].is_string() && j["type"].is_string()) {
    const std::string node_id = j["node_id"].get<std::string>();
    const std::string type = j["type"].get<std::string>();
    const std::string result = NodeEdit::search_node_output_pin_by_type(gs, node_id, type);
    ret.set_json({ { "pin_id", result } });
  }
}

void NodeEdit::ie_search_node_input_pin_by_type(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();

  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    // TODO: error
    return;
  }

  if (j.contains("node_id") && j.contains("type") && j["node_id"].is_string() && j["type"].is_string()) {
    const std::string node_id = j["node_id"].get<std::string>();
    const std::string type = j["type"].get<std::string>();
    const std::string result = NodeEdit::search_node_input_pin_by_type(gs, node_id, type);
    ret.set_json({ { "pin_id", result } });
  }
}

void NodeEdit::ie_search_node_type(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();

  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    // TODO: error
    return;
  }

  if (j.contains("type") && j["type"].is_string()) {
    const std::string type = j["type"].get<std::string>();
    const std::string result = NodeEdit::search_node_type(gs, type);
    ret.set_json({ { "node_id", result } });
  }
}

void NodeEdit::ie_get_all_node_input_pins(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();

  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    // TODO: error
    return;
  }

  if (j.contains("node_id") && j["node_id"].is_string()) {
    const std::string node_id = j["node_id"].get<std::string>();
    const std::vector<std::pair<std::string, std::string>> result = NodeEdit::get_all_node_input_pins(gs, node_id);

    nlohmann::json pins = nlohmann::json::array();
    for (const auto &[pin_type, pin_id] : result) {
      pins.push_back({ { "pin_type", pin_type }, { "pin_id", pin_id } });
    }

    ret.set_json({ { "input_pins", pins } });
  }
}

void NodeEdit::ie_get_all_node_output_pins(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();

  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    // TODO: error
    return;
  }

  if (j.contains("node_id") && j["node_id"].is_string()) {
    const std::string node_id = j["node_id"].get<std::string>();
    const std::vector<std::pair<std::string, std::string>> result = NodeEdit::get_all_node_output_pins(gs, node_id);

    nlohmann::json pins = nlohmann::json::array();
    for (const auto &[pin_type, pin_id] : result) {
      pins.push_back({ { "pin_type", pin_type }, { "pin_id", pin_id } });
    }

    ret.set_json({ { "output_pins", pins } });
  }
}

void NodeEdit::ie_save_nodegraph(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();

  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    return;
  }

  gs->ask_for_save = true;
}

void NodeEdit::ie_refresh_nodegraph(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();

  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    return;
  }

  gs->ask_for_refresh = true;
}

NODEEDIT_API void NodeEdit::ie_focus_at(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();

  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    return;
  }

  if (!j.contains("x") || !j["x"].is_number_integer())
    return;

  if (!j.contains("y") || !j["y"].is_number_integer())
    return;

  const int x = j["x"].get<int>();
  const int y = j["y"].get<int>();

  gs->graph.zoom_request.x = x;
  gs->graph.zoom_request.y = y;
  gs->graph.zoom_request.zoom = -1;
  gs->graph.zoom_request.pending = true;
}

NODEEDIT_API void NodeEdit::ie_zoom_at(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();

  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    return;
  }

  if (!j.contains("x") || !j["x"].is_number_integer())
    return;

  if (!j.contains("y") || !j["y"].is_number_integer())
    return;

  const int x = j["x"].get<int>();
  const int y = j["y"].get<int>();

  gs->graph.zoom_request.x = x;
  gs->graph.zoom_request.y = y;
  gs->graph.zoom_request.zoom = 1;
  gs->graph.zoom_request.pending = true;
}

NODEEDIT_API void NodeEdit::ie_spawn_at(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();

  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();

  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    return;
  }

  if (!j.contains("schema_id") || !j["schema_id"].is_string())
    return;

  if (!j.contains("x") || !j["x"].is_number_integer())
    return;

  if (!j.contains("y") || !j["y"].is_number_integer())
    return;

  const std::string schema_id = j["schema_id"].get<std::string>();
  const int x = j["x"].get<int>();
  const int y = j["y"].get<int>();

  gs->graph.spawn_request.x = x;
  gs->graph.spawn_request.y = y;
  gs->graph.spawn_request.sch_id = schema_id;

  if (j.contains("connection_id") && j["connection_id"].is_string()) {
    const std::string connection_id = j["connection_id"].get<std::string>();
    gs->graph.spawn_request.conn_id = connection_id;
  }

  gs->graph.spawn_request.pending = true;
}

void NodeEdit::ie_clear_all_graph_ext_schemas(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("session_id"))
    return;
  const std::string session_id = j["session_id"];
  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    get_current_context()->m_interface->log_error(
        "Cannot find graph session for id: (" + session_id + "). Context not found.");
    // TODO err in return values
    return;
  }
  NodeEdit::clear_all_graph_ext_schemas(gs);
}

void NodeEdit::ie_delete_graph_ext_schema(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("session_id"))
    return;
  const std::string session_id = j["session_id"];
  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    get_current_context()->m_interface->log_error(
        "Cannot find graph session for id: (" + session_id + "). Context not found.");
    // TODO err in return values
    return;
  }
  if (!j.contains("schema_id"))
    return;
  const std::string schema_id = j["schema_id"];
  NodeEdit::delete_graph_ext_schema(gs, schema_id);
}

void NodeEdit::ie_delete_graph_ext_pin_format(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("session_id"))
    return;
  const std::string session_id = j["session_id"];
  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    get_current_context()->m_interface->log_error(
        "Cannot find graph session for id: (" + session_id + "). Context not found.");
    // TODO err in return values
    return;
  }
  if (!j.contains("pin_format_type"))
    return;
  const std::string pin_format_type = j["pin_format_type"];
  NodeEdit::delete_graph_ext_pin_format(gs, pin_format_type);
}

void NodeEdit::ie_clear_all_graph_ext_pin_formats(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("session_id"))
    return;
  const std::string session_id = j["session_id"];
  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    get_current_context()->m_interface->log_error(
        "Cannot find graph session for id: (" + session_id + "). Context not found.");
    // TODO err in return values
    return;
  }
  NodeEdit::clear_all_graph_ext_pin_formats(gs);
}

void NodeEdit::ie_get_pin_format(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("context_id"))
    return;
  const std::string context_id = j["context_id"];
  if (!j.contains("type"))
    return;
  const std::string type = j["type"];

  const auto *pf = NodeEdit::get_pin_format(context_id, type);
  if (!pf) {
    get_current_context()->m_interface->log_error(
        "Cannot find pin format for type: (" + type + ") in context: (" + context_id + ").");
    // TODO err in return values
    return;
  }
  ret.set_json(NodeEdit::pin_format_to_json(*pf));
}

void NodeEdit::ie_get_schema(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("context_id"))
    return;
  const std::string context_id = j["context_id"];
  if (!j.contains("schema_id"))
    return;
  const std::string schema_id = j["schema_id"];

  const auto *s = NodeEdit::get_schema(context_id, schema_id);
  if (!s) {
    get_current_context()->m_interface->log_error(
        "Cannot find schema for id: (" + schema_id + ") in context: (" + context_id + ").");
    // TODO err in return values
    return;
  }
  ret.set_json(NodeEdit::schema_to_json(*s));
}

void NodeEdit::ie_get_ext_pin_format(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("session_id"))
    return;
  const std::string session_id = j["session_id"];
  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    get_current_context()->m_interface->log_error(
        "Cannot find graph session for id: (" + session_id + "). Context not found.");
    // TODO err in return values
    return;
  }
  if (!j.contains("type"))
    return;
  const std::string type = j["type"];

  const auto *pf = NodeEdit::get_ext_pin_format(gs, type);
  if (!pf) {
    get_current_context()->m_interface->log_error(
        "Cannot find ext pin format for type: (" + type + ") in session: (" + session_id + ").");
    // TODO err in return values
    return;
  }
  ret.set_json(NodeEdit::pin_format_to_json(*pf));
}

void NodeEdit::ie_get_ext_schema(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("session_id"))
    return;
  const std::string session_id = j["session_id"];
  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    get_current_context()->m_interface->log_error(
        "Cannot find graph session for id: (" + session_id + "). Context not found.");
    // TODO err in return values
    return;
  }
  if (!j.contains("schema_id"))
    return;
  const std::string schema_id = j["schema_id"];

  const auto *s = NodeEdit::get_ext_schema(gs, schema_id);
  if (!s) {
    get_current_context()->m_interface->log_error(
        "Cannot find ext schema for id: (" + schema_id + ") in session: (" + session_id + ").");
    // TODO err in return values
    return;
  }
  ret.set_json(NodeEdit::schema_to_json(*s));
}

void NodeEdit::ie_get_all_ext_schemas(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("session_id"))
    return;
  const std::string session_id = j["session_id"];
  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    get_current_context()->m_interface->log_error(
        "Cannot find graph session for id: (" + session_id + "). Context not found.");
    // TODO err in return values
    return;
  }
  ret.set_json({ { "schemas", NodeEdit::get_all_ext_schemas(gs) } });
}

void NodeEdit::ie_get_all_pin_formats(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("context_id"))
    return;
  const std::string context_id = j["context_id"];

  ret.set_json({ { "pin_formats", NodeEdit::get_all_pin_formats(context_id) } });
}

void NodeEdit::ie_get_all_schemas(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("context_id"))
    return;
  const std::string context_id = j["context_id"];

  ret.set_json({ { "schemas", NodeEdit::get_all_schemas(context_id) } });
}

void NodeEdit::ie_get_all_graph_ext_pin_formats(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("session_id"))
    return;
  const std::string session_id = j["session_id"];
  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs) {
    get_current_context()->m_interface->log_error(
        "Cannot find graph session for id: (" + session_id + "). Context not found.");
    // TODO err in return values
    return;
  }
  ret.set_json({ { "pin_formats", NodeEdit::get_all_graph_ext_pin_formats(gs) } });
}

void NodeEdit::ie_find_node_by_schema_id(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();
  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs)
    return;
  if (!j.contains("schema_id") || !j["schema_id"].is_string())
    return;
  const std::string schema_id = j["schema_id"].get<std::string>();
  ret.set_json({ { "node_id", NodeEdit::find_node_by_schema_id(gs, schema_id) } });
}

void NodeEdit::ie_get_node_type_id(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();
  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs)
    return;
  if (!j.contains("node_id") || !j["node_id"].is_string())
    return;
  const std::string node_id = j["node_id"].get<std::string>();
  ret.set_json({ { "type_id", NodeEdit::get_node_type_id(gs, node_id) } });
}

void NodeEdit::ie_get_node_data(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();
  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs)
    return;
  if (!j.contains("node_id") || !j["node_id"].is_string())
    return;
  const std::string node_id = j["node_id"].get<std::string>();
  ret.set_json({ { "datas", NodeEdit::get_node_data(gs, node_id) } });
}

void NodeEdit::ie_get_next_nodes_silently(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("path") || !j["path"].is_string())
    return;
  const std::string path = j["path"].get<std::string>();

  auto gs = NodeEdit::get_or_create_silent_session(path);
  if (!gs) {
    get_current_context()->m_interface->log_error("Cannot open graph silently for path: (" + path + ")");
    return;
  }

  if (j.contains("node_id") && j.contains("output_id") && j["node_id"].is_string() && j["output_id"].is_string()) {
    const std::string node_id = j["node_id"].get<std::string>();
    const std::string output_id = j["output_id"].get<std::string>();
    const std::vector<std::string> result = NodeEdit::get_next_nodes(gs, node_id, output_id);
    ret.set_json({ { "node_ids", result } });
  } else {
    get_current_context()->m_interface->log_error("Cannot fetch next nodes, cannot find output_id or node_id");
  }
}

void NodeEdit::ie_get_previous_nodes_silently(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("path") || !j["path"].is_string())
    return;
  const std::string path = j["path"].get<std::string>();

  auto gs = NodeEdit::get_or_create_silent_session(path);
  if (!gs) {
    get_current_context()->m_interface->log_error("Cannot open graph silently for path: (" + path + ")");
    return;
  }

  if (j.contains("node_id") && j.contains("input_id") && j["node_id"].is_string() && j["input_id"].is_string()) {
    const std::string node_id = j["node_id"].get<std::string>();
    const std::string input_id = j["input_id"].get<std::string>();
    const std::vector<std::string> result = NodeEdit::get_previous_nodes(gs, node_id, input_id);
    ret.set_json({ { "node_ids", result } });
  } else {
    get_current_context()->m_interface->log_error("Cannot fetch previous nodes, cannot find input_id or node_id");
  }
}

void NodeEdit::ie_search_node_output_pin_by_type_silently(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("path") || !j["path"].is_string())
    return;
  const std::string path = j["path"].get<std::string>();

  auto gs = NodeEdit::get_or_create_silent_session(path);
  if (!gs) {
    get_current_context()->m_interface->log_error("Cannot open graph silently for path: (" + path + ")");
    return;
  }

  if (j.contains("node_id") && j.contains("type") && j["node_id"].is_string() && j["type"].is_string()) {
    const std::string node_id = j["node_id"].get<std::string>();
    const std::string type = j["type"].get<std::string>();
    const std::string result = NodeEdit::search_node_output_pin_by_type(gs, node_id, type);
    ret.set_json({ { "pin_id", result } });
  }
}

void NodeEdit::ie_search_node_input_pin_by_type_silently(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("path") || !j["path"].is_string())
    return;
  const std::string path = j["path"].get<std::string>();

  auto gs = NodeEdit::get_or_create_silent_session(path);
  if (!gs) {
    get_current_context()->m_interface->log_error("Cannot open graph silently for path: (" + path + ")");
    return;
  }

  if (j.contains("node_id") && j.contains("type") && j["node_id"].is_string() && j["type"].is_string()) {
    const std::string node_id = j["node_id"].get<std::string>();
    const std::string type = j["type"].get<std::string>();
    const std::string result = NodeEdit::search_node_input_pin_by_type(gs, node_id, type);
    ret.set_json({ { "pin_id", result } });
  }
}

void NodeEdit::ie_search_node_type_silently(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("path") || !j["path"].is_string())
    return;
  const std::string path = j["path"].get<std::string>();

  auto gs = NodeEdit::get_or_create_silent_session(path);
  if (!gs) {
    get_current_context()->m_interface->log_error("Cannot open graph silently for path: (" + path + ")");
    return;
  }

  if (j.contains("type") && j["type"].is_string()) {
    const std::string type = j["type"].get<std::string>();
    const std::string result = NodeEdit::search_node_type(gs, type);
    ret.set_json({ { "node_id", result } });
  }
}

void NodeEdit::ie_get_all_node_input_pins_silently(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("path") || !j["path"].is_string())
    return;
  const std::string path = j["path"].get<std::string>();

  auto gs = NodeEdit::get_or_create_silent_session(path);
  if (!gs) {
    get_current_context()->m_interface->log_error("Cannot open graph silently for path: (" + path + ")");
    return;
  }

  if (j.contains("node_id") && j["node_id"].is_string()) {
    const std::string node_id = j["node_id"].get<std::string>();
    const std::vector<std::pair<std::string, std::string>> result = NodeEdit::get_all_node_input_pins(gs, node_id);

    nlohmann::json pins = nlohmann::json::array();
    for (const auto &[pin_type, pin_id] : result) {
      pins.push_back({ { "pin_type", pin_type }, { "pin_id", pin_id } });
    }

    ret.set_json({ { "input_pins", pins } });
  }
}

void NodeEdit::ie_get_all_node_output_pins_silently(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("path") || !j["path"].is_string())
    return;
  const std::string path = j["path"].get<std::string>();

  auto gs = NodeEdit::get_or_create_silent_session(path);
  if (!gs) {
    get_current_context()->m_interface->log_error("Cannot open graph silently for path: (" + path + ")");
    return;
  }

  if (j.contains("node_id") && j["node_id"].is_string()) {
    const std::string node_id = j["node_id"].get<std::string>();
    const std::vector<std::pair<std::string, std::string>> result = NodeEdit::get_all_node_output_pins(gs, node_id);

    nlohmann::json pins = nlohmann::json::array();
    for (const auto &[pin_type, pin_id] : result) {
      pins.push_back({ { "pin_type", pin_type }, { "pin_id", pin_id } });
    }

    ret.set_json({ { "output_pins", pins } });
  }
}

void NodeEdit::ie_get_ext_pin_format_silently(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("path") || !j["path"].is_string())
    return;
  const std::string path = j["path"].get<std::string>();

  auto gs = NodeEdit::get_or_create_silent_session(path);
  if (!gs) {
    get_current_context()->m_interface->log_error("Cannot open graph silently for path: (" + path + ")");
    return;
  }

  if (!j.contains("type"))
    return;
  const std::string type = j["type"];

  const auto *pf = NodeEdit::get_ext_pin_format(gs, type);
  if (!pf) {
    get_current_context()->m_interface->log_error(
        "Cannot find ext pin format for type: (" + type + ") in path: (" + path + ").");
    return;
  }
  ret.set_json(NodeEdit::pin_format_to_json(*pf));
}

void NodeEdit::ie_get_ext_schema_silently(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("path") || !j["path"].is_string())
    return;
  const std::string path = j["path"].get<std::string>();

  auto gs = NodeEdit::get_or_create_silent_session(path);
  if (!gs) {
    get_current_context()->m_interface->log_error("Cannot open graph silently for path: (" + path + ")");
    return;
  }

  if (!j.contains("schema_id"))
    return;
  const std::string schema_id = j["schema_id"];

  const auto *s = NodeEdit::get_ext_schema(gs, schema_id);
  if (!s) {
    get_current_context()->m_interface->log_error(
        "Cannot find ext schema for id: (" + schema_id + ") in path: (" + path + ").");
    return;
  }
  ret.set_json(NodeEdit::schema_to_json(*s));
}

void NodeEdit::ie_get_all_ext_schemas_silently(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("path") || !j["path"].is_string())
    return;
  const std::string path = j["path"].get<std::string>();

  auto gs = NodeEdit::get_or_create_silent_session(path);
  if (!gs) {
    get_current_context()->m_interface->log_error("Cannot open graph silently for path: (" + path + ")");
    return;
  }

  ret.set_json({ { "schemas", NodeEdit::get_all_ext_schemas(gs) } });
}

void NodeEdit::ie_get_all_graph_ext_pin_formats_silently(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("path") || !j["path"].is_string())
    return;
  const std::string path = j["path"].get<std::string>();

  auto gs = NodeEdit::get_or_create_silent_session(path);
  if (!gs) {
    get_current_context()->m_interface->log_error("Cannot open graph silently for path: (" + path + ")");
    return;
  }

  ret.set_json({ { "pin_formats", NodeEdit::get_all_graph_ext_pin_formats(gs) } });
}

void NodeEdit::ie_find_node_by_schema_id_silently(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("path") || !j["path"].is_string())
    return;
  const std::string path = j["path"].get<std::string>();

  auto gs = NodeEdit::get_or_create_silent_session(path);
  if (!gs)
    return;

  if (!j.contains("schema_id") || !j["schema_id"].is_string())
    return;
  const std::string schema_id = j["schema_id"].get<std::string>();
  ret.set_json({ { "node_id", NodeEdit::find_node_by_schema_id(gs, schema_id) } });
}

void NodeEdit::ie_get_node_type_id_silently(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("path") || !j["path"].is_string())
    return;
  const std::string path = j["path"].get<std::string>();

  auto gs = NodeEdit::get_or_create_silent_session(path);
  if (!gs)
    return;

  if (!j.contains("node_id") || !j["node_id"].is_string())
    return;
  const std::string node_id = j["node_id"].get<std::string>();
  ret.set_json({ { "type_id", NodeEdit::get_node_type_id(gs, node_id) } });
}

void NodeEdit::ie_get_node_data_silently(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("path") || !j["path"].is_string())
    return;
  const std::string path = j["path"].get<std::string>();

  auto gs = NodeEdit::get_or_create_silent_session(path);
  if (!gs)
    return;

  if (!j.contains("node_id") || !j["node_id"].is_string())
    return;
  const std::string node_id = j["node_id"].get<std::string>();
  ret.set_json({ { "datas", NodeEdit::get_node_data(gs, node_id) } });
}

void NodeEdit::ie_clear_silent_session_cache(ArgumentValues &args, ReturnValues &ret) {
  NodeEdit::clear_silent_session_cache();
}

void NodeEdit::ie_get_connection_source_pin(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("session_id") || !j["session_id"].is_string())
    return;
  const std::string session_id = j["session_id"].get<std::string>();
  auto gs = NodeEdit::get_graph_session_by_id(session_id);
  if (!gs)
    return;
  if (!j.contains("node_id") || !j.contains("input_id"))
    return;
  const std::string node_id = j["node_id"].get<std::string>();
  const std::string input_id = j["input_id"].get<std::string>();
  ret.set_json({ { "pin_id", NodeEdit::get_connection_source_pin(gs, node_id, input_id) } });
}

void NodeEdit::ie_get_connection_source_pin_silently(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();
  if (!j.contains("path") || !j["path"].is_string())
    return;
  const std::string path = j["path"].get<std::string>();

  auto gs = NodeEdit::get_or_create_silent_session(path);
  if (!gs) {
    get_current_context()->m_interface->log_error("Cannot open graph silently for path: (" + path + ")");
    return;
  }
  if (!j.contains("node_id") || !j.contains("input_id"))
    return;
  const std::string node_id = j["node_id"].get<std::string>();
  const std::string input_id = j["input_id"].get<std::string>();
  ret.set_json({ { "pin_id", NodeEdit::get_connection_source_pin(gs, node_id, input_id) } });
}
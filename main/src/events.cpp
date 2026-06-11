#include "module.hpp"

NODEEDIT_API void NodeEdit::ie_create_node_context(ArgumentValues &args, ReturnValues &ret) {
  if (args.get_json().contains("name")) {
    const std::string ctx_name = args.get_json()["name"];
    NodeEdit::create_node_context(ctx_name);
    // TODO: ret true
  } else {
    // TODO: ret false
  }
}

NODEEDIT_API void NodeEdit::ie_setup_pin_format(ArgumentValues &args, ReturnValues &ret) {
  if (args.get_json().contains("context_name")) {
    const std::string ctx_name = args.get_json()["context_name"];
    auto c = NodeEdit::get_node_context(ctx_name);
    // TODO: Use IsContextExist;

    if (!c) {
      // TODO error
      return;
    }

    NodeEditPinFormat pf;
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

    NodeEdit::AddPinFormatToContext(c->id, pf);

  } else {
  }
}

NODEEDIT_API void NodeEdit::ie_setup_schema(ArgumentValues &args, ReturnValues &ret) {
  const auto &j = args.get_json();

  if (!j.contains("context_name"))
    return;
  const std::string ctx_name = j["context_name"];

  auto c = NodeEdit::get_node_context(ctx_name);
  if (!c) {
    get_current_context()->interface->log_error("Cannot create schema for ctx: (" + ctx_name + "). Context not found.");
    return;
  }

  NodeEditSchema s;

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
      NodeEditPin pin;
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
      NodeEditPin pin;
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

  NodeEdit::AddSchemaToContext(c->id, s);
}
NODEEDIT_API void NodeEdit::ie_open_graph(ArgumentValues &args, ReturnValues &ret) {
  if (!args.get_json().contains("path"))
    return;

  const std::string path = args.get_json()["path"];

  NodeEdit::OpenGraph(path);
}
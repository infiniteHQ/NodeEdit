#pragma once

#include <vxcore/include/vortex.h>
#include <vxcore/include/vortex_internals.h>
#include <vxgui/editor/main/editor.hpp>

#ifndef NODEEDIT_HELPERS
#define NODEEDIT_HELPERS

namespace NodeEdit {

// node ctx
struct NodeEditPinFormat {
  std::string type; // Must be unique on the NodeContext
  bool delegate = false;
  std::string description;
  std::string name;

  // hex
  std::string color;

  // flow, circle, square, grid, round-square, diamond (def: circle)
  std::string shape;
};

struct NodeEditPin {
  std::string type;
  // std::string color_variant;
  std::string name; // optionnal: custom name
  std::string id;
};

struct NodeEditNodeEffect {
  std::string instance_id;

  std::string type; // message
  std::string text;
  std::string text_color;
  std::string bg_color;
};

struct NodeEditConnectionEffect {
  std::string node_instance_id_A;
  std::string pin_id_A;
  std::string node_instance_id_B;
  std::string pin_id_B;

  std::string type; // pulse, flow

  float pulsating_intensity = 1.0f;
  float pulsating_rate = 2.0f;
  std::string pulsating_color = "#FF4444FF";

  float flow_intensity = 1.0f;
  float flow_speed = 120.0f;
  bool flow_reverse = false;
  std::string flow_color = "#44AAFFFF";
};

struct NodeEditSpawnPossibility {
  std::string proper_name;
  std::string proper_description;
  std::string proper_logo;
  std::string category;
  std::string schema_id;
};

struct NodeEditSchema {
  std::vector<NodeEditPin> input_pins;
  std::vector<NodeEditPin> output_pins;
  NodeEditPin header_pin;

  bool spawnable = false;
  NodeEditSpawnPossibility spawn_possibility;

  // props
  std::string id;
  std::string header_color;
  std::string border_color;
  std::string background_color;
  std::string label;
  std::string label_color;
  std::string second_label;
  std::string second_label_color;
  std::string description_color;
  std::string header_logo_path;

  // state
  std::string status; // active, disabled, depreciated, obsolete
  std::string type;   // blueprint, simple, three, comment, houdini
};

struct NodeEditContext {
  std::vector<NodeEditSchema> schemas;
  std::vector<NodeEditPinFormat> pin_formats;
  bool allow_comments;
  bool allow_sequences;
  std::string id;
};

// node graph
struct NodeEditInstance {
  std::string type_id;
  std::string instance_id;
  float pos_x;
  float pos_y;
  float size_x;
  float size_y;
  nlohmann::json datas;
};

struct NodeEditConnection {
  std::string node_instance_id_A;
  std::string pin_id_A;
  std::string node_instance_id_B;
  std::string pin_id_B;
};

struct NodeEditGraph {
  std::vector<NodeEditInstance> instances;
  std::vector<NodeEditConnection> connections;

  // effects
  std::vector<NodeEditConnectionEffect> connection_effects;
  std::vector<NodeEditNodeEffect> node_effects;
  bool refresh_effects;
};

struct NodeEditGraphSession {
  NodeEditGraph graph;
  std::string name;
  std::string context_id;
  std::string path; // json file
};
} // namespace NodeEdit

#endif // NODEEDIT_HELPERS
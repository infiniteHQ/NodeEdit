#pragma once

#include <vxcore/include/vortex.h>
#include <vxcore/include/vortex_internals.h>

#include <vxgui/editor/main/editor.hpp>

#ifndef NODEEDIT_HELPERS
#define NODEEDIT_HELPERS

namespace NodeEdit {

  // node ctx
  struct PinFormat {
    std::string type;  // Must be unique on the NodeContext
    bool delegate = false;
    std::string description;
    std::string name;

    // hex
    std::string color;

    // flow, circle, square, grid, round-square, diamond (def: circle)
    std::string shape;
  };

  struct Pin {
    std::string type;
    // std::string color_variant;
    std::string name;  // optionnal: custom name
    std::string id;
  };

  struct NodeEffect {
    std::string instance_id;

    std::string type;  // message
    std::string text;
    std::string text_color;
    std::string bg_color;
  };

  struct ConnectionEffect {
    std::string node_instance_id_A;
    std::string pin_id_A;
    std::string node_instance_id_B;
    std::string pin_id_B;

    std::string type;  // pulse, flow

    float pulsating_intensity = 1.0f;
    float pulsating_rate = 2.0f;
    std::string pulsating_color = "#FF4444FF";

    float flow_intensity = 1.0f;
    float flow_speed = 120.0f;
    bool flow_reverse = false;
    std::string flow_color = "#44AAFFFF";
  };

  struct SpawnPossibility {
    std::string proper_name;
    std::string proper_description;
    std::string proper_logo;
    std::string category;
    std::string schema_id;
  };

  struct Schema {
    std::vector<Pin> input_pins;
    std::vector<Pin> output_pins;
    Pin header_pin;

    bool spawnable = false;
    SpawnPossibility spawn_possibility;

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
    std::string status;  // active, disabled, depreciated, obsolete
    std::string type;    // blueprint, simple, three, comment, houdini
  };

  struct NodeContext {
    std::vector<Schema> schemas;
    std::vector<PinFormat> pin_formats;
    bool allow_comments;
    bool allow_sequences;
    std::string id;
  };

  struct GraphContextExtensions {
    std::vector<Schema> schemas;
    std::vector<PinFormat> pin_formats;
  };

  // node graph
  struct NodeInstance {
    std::string type_id;
    std::string instance_id;
    float pos_x;
    float pos_y;
    float size_x;
    float size_y;
    nlohmann::json datas;
  };

  struct Connection {
    std::string node_instance_id_A;
    std::string pin_id_A;
    std::string node_instance_id_B;
    std::string pin_id_B;
  };

  struct ZoomRequest {
    bool pending = false;
    float x;
    float y;
    float zoom;
  };

  struct SpawnRequest {
    bool pending = false;
    float x;
    float y;
    std::string sch_id;
    std::string conn_id;
  };

  struct Graph {
    std::vector<NodeInstance> instances;
    std::vector<Connection> connections;

    // effects
    std::vector<ConnectionEffect> connection_effects;
    std::vector<NodeEffect> node_effects;
    ZoomRequest zoom_request;
    SpawnRequest spawn_request;

    // ctx modifications/extension
    GraphContextExtensions ext;

    std::string graph_title;

    bool refresh_effects;
    bool refresh_ctx;
  };

  struct GraphSession {
    Graph graph;
    std::string session_id;
    std::string context_id;
    std::string parent_appwindow;
    std::string path;  // json file
    bool disable_native_save_system = false;
    bool ask_for_save = false;
    bool ask_for_refresh = false;
  };
}  // namespace NodeEdit

#endif  // NODEEDIT_HELPERS
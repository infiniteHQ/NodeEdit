#include "../ui/instances/node_editor/node_editor.hpp"
#include <vxcore/include/vortex.h>
#include <vxcore/include/vortex_internals.h>
#include <vxgui/editor/main/editor.hpp>

#ifndef SAMPLE_MODULE_HPP
#define SAMPLE_MODULE_HPP

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
  std::string color_variant;
  std::string name;
  std::string id;
};

struct NodeEditSchema {
  std::vector<NodeEditPin> input_pins;
  std::vector<NodeEditPin> output_pins;
  NodeEditPin header_pin;

  // props
  std::string id;
  std::string hex_header_color;
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
struct NodeEditInstance {};
struct NodeEditConnection {};
struct NodeEditGraph {};
struct NodeEditGraphSession {
  NodeEditGraph graph;
  std::string id;
  std::string path; // json file
};

struct Context {
  std::shared_ptr<ModuleInterface> interface;
  std::vector<std::shared_ptr<ModuleUI::NodeEditorAppWindow>> editor_instances;
  std::vector<std::shared_ptr<NodeEditContext>> contexts;
  std::vector<std::shared_ptr<NodeEditGraphSession>> graph_sessions;
};
} // namespace NodeEdit

#ifndef NODEEDIT_API
#define NODEEDIT_API
#endif

// context pointer
#ifndef CNodeEdit
extern NODEEDIT_API std::weak_ptr<NodeEdit::Context> CNodeEdit;
#endif

namespace NodeEdit {
// Context
NODEEDIT_API std::shared_ptr<NodeEdit::Context> create_context();
NODEEDIT_API void DestroyContext(std::shared_ptr<NodeEdit::Context> ctx);
NODEEDIT_API void set_current_context(std::shared_ptr<NodeEdit::Context> ctx);
NODEEDIT_API std::shared_ptr<NodeEdit::Context> get_current_context();

// Content browser utils
NODEEDIT_API bool IsContextFile(const std::string &path);
NODEEDIT_API bool IsGraphFile(const std::string &path);

// Node context setup
NODEEDIT_API void AddSchemaToContext();
NODEEDIT_API void AddPinFormatToContext();
NODEEDIT_API void CreateContext(const std::string &name);
NODEEDIT_API void LoadContextFromFile(const std::string &path);
// TODO: Override existing schema/pinformat or simply add to existing context
NODEEDIT_API void EmplaceContextFromFile(const std::string &path);

// utils
NODEEDIT_API std::string GetPath(const std::string &path);

// Main API
NODEEDIT_API void GetConnected(const std::string &ctx_id,
                               const std::string &node_id,
                               const std::string &pin_id); // Get the nodeID

// UI API
NODEEDIT_API void StartNodeEditTestInstance();

} // namespace NodeEdit

#endif // SAMPLE_MODULE_HPP
#include "../ui/instances/node_editor/node_editor.hpp"
#include <vxcore/include/vortex.h>
#include <vxcore/include/vortex_internals.h>
#include <vxgui/editor/main/editor.hpp>

#ifndef SAMPLE_MODULE_HPP
#define SAMPLE_MODULE_HPP

namespace NodeEdit {

// node ctx
struct NodeEditPinFormat {};
struct NodeEditSchema {};
struct NodeEditContext {};

// node graph
struct NodeEditInstances {};
struct NodeEditConnection {};
struct NodeEditGraph {};

struct Context {
  std::shared_ptr<ModuleInterface> interface;
  std::vector<std::shared_ptr<ModuleUI::NodeEditorAppWindow>> editor_instances;
  std::vector<std::shared_ptr<NodeEditContext>> contexts;
  // TODO : Loaded graphs (vector of shared_ptr<NodeEditGraph>)
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
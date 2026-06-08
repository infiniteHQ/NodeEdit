#include "../ui/instances/node_debugger/node_debugger.hpp"
#include "../ui/instances/node_editor/node_editor.hpp"
#include "./helpers.hpp"
#include <vxcore/include/vortex.h>
#include <vxcore/include/vortex_internals.h>
#include <vxgui/editor/main/editor.hpp>

#ifndef SAMPLE_MODULE_HPP
#define SAMPLE_MODULE_HPP

#ifndef NODEEDIT_API
#define NODEEDIT_API
#endif

namespace NodeEdit {
struct Context {
  std::shared_ptr<ModuleInterface> interface;
  std::vector<std::shared_ptr<ModuleUI::NodeEditorAppWindow>> editor_instances;
  std::vector<std::shared_ptr<ModuleUI::NodeEditorDebugger>> editor_debuggers;
  std::vector<std::shared_ptr<NodeEditContext>> contexts;
  std::vector<std::shared_ptr<NodeEditGraphSession>> graph_sessions;
};
} // namespace NodeEdit
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
NODEEDIT_API bool IsContextExist(const std::string &ctx_name);

// Node context API
NODEEDIT_API std::shared_ptr<NodeEditContext>
CreateContext(const std::string &name);
NODEEDIT_API void DestroyContext(const std::string &name);
NODEEDIT_API void LoadContextFromFile(const std::string &path);
// TODO: Override existing schema/pinformat or simply add to existing context
NODEEDIT_API void EmplaceContextFromFile(const std::string &path);
NODEEDIT_API void LoadContext(const nlohmann::json &path);
NODEEDIT_API void EmplaceContext(const nlohmann::json &path);
// TODO: Emplace schema, pin format etc...
NODEEDIT_API void AddSchemaToContext(const std::string &ctx_id,
                                     const NodeEditSchema &schema);
NODEEDIT_API void AddPinFormatToContext(const std::string &ctx_id,
                                        const NodeEditPinFormat &pin_format);

NODEEDIT_API json DumpContextTojson(const std::string ctx);

// Graphs API
NODEEDIT_API void OpenGraph(const std::string &path); // master (ui+backend)
NODEEDIT_API void CreateGraphSessionFromFile(const std::string &path);

// utils
NODEEDIT_API std::string GetPath(const std::string &path);

// Main API

NODEEDIT_API void
SaveGraphSession(const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph);
NODEEDIT_API void RefreshGraphSession(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph);

NODEEDIT_API void GetConnected(const std::string &ctx_id,
                               const std::string &node_id,
                               const std::string &pin_id); // Get the nodeID

// UI API
NODEEDIT_API void StartNodeEditTestInstance();

NODEEDIT_API std::shared_ptr<NodeEditContext> SetupExampleContext();
NODEEDIT_API std::shared_ptr<NodeEditGraphSession> SetupExampleGraphSession();

NODEEDIT_API nlohmann::json DumpGraph(const NodeEdit::NodeEditGraph &graph);
NODEEDIT_API NodeEditGraph PopulateGraph(const nlohmann::json &j);
NODEEDIT_API std::string GetGraphContextName(const nlohmann::json &j);
NODEEDIT_API nlohmann::json GetGraph(const nlohmann::json &j);

// Nodegraph API

// Get the nodeID
NODEEDIT_API std::string
GetNextNode(const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
            const std::string &nodeid,
            const std::string &outputid); // instance_id

NODEEDIT_API std::string
GetPreviousNode(const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
                const std::string &nodeid,
                const std::string &inputid); // instance_id

NODEEDIT_API std::string SearchNodeOutputPinByType(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const std::string &nodeid,
    const std::string &type); //  id (from NodeEditPin)

NODEEDIT_API std::string SearchNodeInputPinByType(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const std::string &nodeid,
    const std::string &type); // id (from NodeEditPin)
NODEEDIT_API std::string
SearchNodeType(const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
               const std::string &type); // return instance_id

// helpers
NODEEDIT_API const NodeEdit::NodeEditInstance *
FindInstance(const NodeEdit::NodeEditGraph &graph,
             const std::string &instance_id);

NODEEDIT_API const NodeEdit::NodeEditSchema *
FindSchema(const NodeEdit::NodeEditContext &ctx, const std::string &type_id);

NODEEDIT_API const NodeEdit::NodeEditContext *
FindContext(const std::string &context_id);

NODEEDIT_API std::vector<std::pair<std::string, std::string>>
GetAllNodeInputPins(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const std::string &nodeid);

NODEEDIT_API std::vector<std::pair<std::string, std::string>>
GetAllNodeOutputPins(
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph,
    const std::string &nodeid);

} // namespace NodeEdit

#endif // SAMPLE_MODULE_HPP
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
  auto inst = ModuleUI::NodeEditorAppWindow::Create("TEST");
  Cherry::AddAppWindow(inst->GetAppWindow());
  get_current_context()->editor_instances.push_back(inst);
}

bool NodeEdit::IsGraphFile(const std::string &path) {
  std::string filename = path + "/graph.nodegraph";
  return fs::exists(filename);
}

void NodeEdit::OpenGraph(const std::string &path) {
  if (!IsGraphFile(path)) {
    get_current_context()->interface->log_error(
        "No graph file in selected file ! (" + path + ")");
  }

  // TODO: Verify graph ctx
  // TODO: Find context
  // TODO: Load graph
  // TODO: Start session
  // TODO: Start UI
}

void NodeEdit::SetupExampleContext() {
  auto c = NodeEdit::CreateContext("efusion_blueprint");

  // Setup pins formats
  {
    NodeEditPinFormat pf;
    NodeEdit::AddPinFormatToContext(c, pf);
  }

  // Setup schemas
  {
    NodeEditSchema s;
    NodeEditPin pi_a;
    NodeEditPin pi_b;
    NodeEditPin po_a;
    NodeEditPin po_b;
    NodeEdit::AddSchemaToContext(c, s);
  }
}

std::string NodeEdit::CreateContext(const std::string &name) {
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

  return name;
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

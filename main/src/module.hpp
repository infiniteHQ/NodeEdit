#include "../ui/instances/node_editor/node_editor.hpp"
#include <vxcore/include/vortex.h>
#include <vxcore/include/vortex_internals.h>
#include <vxgui/editor/main/editor.hpp>

#ifndef SAMPLE_MODULE_HPP
#define SAMPLE_MODULE_HPP

namespace NodeEdit {
struct Context {
  std::shared_ptr<ModuleInterface> interface;
  std::vector<std::shared_ptr<ModuleUI::NodeEditorAppWindow>> editor_instances;
};
} // namespace NodeEdit

#ifndef NODEEDIT_API
#define NODEEDIT_API
#endif

#ifndef CNodeEdit
extern NODEEDIT_API NodeEdit::Context *CNodeEdit;
#endif

namespace NodeEdit {
NODEEDIT_API void CreateContext();
NODEEDIT_API void DestroyContext();

NODEEDIT_API std::string GetPath(const std::string &path);
NODEEDIT_API void Hello();

NODEEDIT_API void StartNodeEditTestInstance();
NODEEDIT_API bool IsValidFile(const std::string &path);

NODEEDIT_API void AddSchemaToContext();
NODEEDIT_API void AddPinFormatToContext();

} // namespace NodeEdit

#endif // SAMPLE_MODULE_HPP
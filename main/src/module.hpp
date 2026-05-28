#include "../ui/instances/node_editor/node_editor.hpp"
#include <vxcore/include/vortex.h>
#include <vxcore/include/vortex_internals.h>
#include <vxgui/editor/main/editor.hpp>

#ifndef SAMPLE_MODULE_HPP
#define SAMPLE_MODULE_HPP

namespace NodeEdit {
struct Context {
  std::shared_ptr<ModuleInterface> m_interface;
  std::vector<std::shared_ptr<ModuleUI::NodeEditorAppWindow>>
      m_text_editor_instances;
};
} // namespace NodeEdit

#ifndef TEXTEDIT_API
#define TEXTEDIT_API
#endif

#ifndef CNodeEdit
extern TEXTEDIT_API NodeEdit::Context *CNodeEdit;
#endif

namespace NodeEdit {
TEXTEDIT_API void CreateContext();
TEXTEDIT_API void DestroyContext();

TEXTEDIT_API std::string GetPath(const std::string &path);
TEXTEDIT_API void Hello();

TEXTEDIT_API void StartNodeEditTestInstance();
TEXTEDIT_API bool IsValidFile(const std::string &path);
} // namespace NodeEdit

#endif // SAMPLE_MODULE_HPP
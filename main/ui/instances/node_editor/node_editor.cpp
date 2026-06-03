#include "node_editor.hpp"
#include "../../../src/module.hpp"

#include <filesystem>
#include <iostream>
#include <string>

namespace ModuleUI {

static const std::unordered_map<std::string, Cherry::NodeSystem::PinShape>
    shape_map = {{"flow", Cherry::NodeSystem::PinShape::Flow},
                 {"circle", Cherry::NodeSystem::PinShape::Circle},
                 {"square", Cherry::NodeSystem::PinShape::Square},
                 {"grid", Cherry::NodeSystem::PinShape::Grid},
                 {"roundsquare", Cherry::NodeSystem::PinShape::RoundSquare},
                 {"diamond", Cherry::NodeSystem::PinShape::Diamond}};
static const std::unordered_map<std::string, Cherry::NodeSystem::NodeType>
    type_map = {{"blueprint", Cherry::NodeSystem::NodeType::Blueprint},
                {"comment", Cherry::NodeSystem::NodeType::Comment},
                {"houdini", Cherry::NodeSystem::NodeType::Houdini},
                {"simple", Cherry::NodeSystem::NodeType::Simple},
                {"tree", Cherry::NodeSystem::NodeType::Tree}};

static const std::unordered_map<std::string,
                                Cherry::NodeSystem::NodeSchemaStatus>
    status_map = {
        {"active", Cherry::NodeSystem::NodeSchemaStatus::Active},
        {"depreciated", Cherry::NodeSystem::NodeSchemaStatus::Depreciated},
        {"disabled", Cherry::NodeSystem::NodeSchemaStatus::Disabled},
        {"obsolete", Cherry::NodeSystem::NodeSchemaStatus::Obsolete}};

NodeEditorAppWindow::NodeEditorAppWindow(
    const std::string &name,
    const std::shared_ptr<NodeEdit::NodeEditContext> &ctx) {
  m_AppWindow = std::make_shared<Cherry::AppWindow>("NODEGRAPH", "NODEGRAPH");
  m_AppWindow->SetLeftMenubarCallback([this]() { RenderMenubar(); });
  m_AppWindow->SetRightMenubarCallback([this]() { RenderRightMenubar(); });
  m_AppWindow->SetLeftBottombarCallback([this]() { RenderBottombar(); });
  m_AppWindow->SetSaveMode(true);
  m_AppWindow->SetInternalPaddingY(0.0f);
  m_AppWindow->SetInternalPaddingX(0.0f);

  m_AppWindow->m_CloseCallback = [=]() {
    Cherry::DeleteAppWindow(m_AppWindow);
    m_AppWindow->SetVisibility(false);
  };

  backend_node_ctx = ctx;
  LoadContextFromBackend();

  {
    Cherry::NodeSystem::NodeInstance inst;
    inst.TypeID = "is_cool";
    inst.InstanceID = "test1";
    inst.Position = Cherry::NodeSystem::Vec2(40, 40);
    inst.Size = Cherry::NodeSystem::Vec2(40, 40);
    ui_node_graph.AddNodeInstance(inst);
  }

  {
    Cherry::NodeSystem::NodeInstance inst;
    inst.TypeID = "is_cool";
    inst.InstanceID = "test2";
    inst.Position = Cherry::NodeSystem::Vec2(80, 80);
    inst.Size = Cherry::NodeSystem::Vec2(40, 40);
    ui_node_graph.AddNodeInstance(inst);
  }

  std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;

  this->ctx = vxe::get_current_context();
}

std::shared_ptr<Cherry::AppWindow> &NodeEditorAppWindow::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<NodeEditorAppWindow> NodeEditorAppWindow::Create(
    const std::string &name,
    const std::shared_ptr<NodeEdit::NodeEditContext> &ctx) {
  auto instance =
      std::shared_ptr<NodeEditorAppWindow>(new NodeEditorAppWindow(name, ctx));
  instance->SetupRenderCallback();
  return instance;
}

void NodeEditorAppWindow::SetupRenderCallback() {
  auto self = shared_from_this();
  m_AppWindow->SetRenderCallback([self]() {
    if (self) {
      self->Render();
    }
  });
}

void NodeEditorAppWindow::RenderMenubar() {
  CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 3.0f);

  if (!m_FileEdited) {
    CherryGUI::BeginDisabled();
  }

  CherryNextComponent.SetProperty("padding_y", "5.5f");
  CherryNextComponent.SetProperty("padding_x", "6.0f");
  CherryNextComponent.SetProperty("size_x", "18");
  CherryNextComponent.SetProperty("size_y", "18");
  if (CherryKit::ButtonImage(
          NodeEdit::GetPath("/resources/icons/icon_save.png"))
          .GetDataAs<bool>("isClicked")) {
    m_SavePending = true;
  }

  if (!m_FileEdited) {
    CherryGUI::EndDisabled();
  }

  CherryNextComponent.SetProperty("padding_y", "5.5f");
  CherryNextComponent.SetProperty("padding_x", "6.0f");
  CherryNextComponent.SetProperty("size_x", "18");
  CherryNextComponent.SetProperty("size_y", "18");
  if (CherryKit::ButtonImage(
          NodeEdit::GetPath("/resources/icons/icon_refresh.png"))
          .GetDataAs<bool>("isClicked")) {
    m_RefreshReady = true;
  }
}
void NodeEditorAppWindow::Render() {
  CherryApp.PushComponentPool(&m_ComponentPool);

  if (!refreshed) {
    CherryKit::NodeAreaOpen("", 0, 0, &ui_node_ctx, &ui_node_graph);
  } else {
    refreshed = false;
  }

  CherryApp.PopComponentPool();
}

void NodeEditorAppWindow::RenderRightMenubar() {
  //
}

void NodeEditorAppWindow::RenderBottombar() {
  //
}

void NodeEditorAppWindow::Refresh() {
  // TODO: Refresh Backend
  // TODO: Refresh UI
}

void NodeEditorAppWindow::Save() {
  // TODO: Patch backend
  // TODO: Call SaveBackend functin from the api
}

void NodeEditorAppWindow::LoadContextFromBackend() {
  if (!backend_node_ctx) {
    return;
  }
  auto &pin_formats = backend_node_ctx->pin_formats;
  auto &schemas = backend_node_ctx->schemas;

  for (auto &pf : pin_formats) {
    Cherry::NodeSystem::PinFormat pin_format;
    pin_format.m_Color = pf.color;
    pin_format.m_Delegate = pf.delegate;

    auto it = shape_map.find(pf.shape);
    if (it != shape_map.end()) {
      pin_format.m_Shape = it->second;
    }

    pin_format.m_TypeDescription = pf.description;
    pin_format.m_TypeID = pf.type;
    pin_format.m_TypeName = pf.name;
    ui_node_ctx.SetupPinFormat(pin_format);
  }

  for (auto &s : schemas) {
    auto sch = ui_node_ctx.CreateSchema(s.id);
    sch->SetLabel(s.label);
    sch->SetLabelHexColor(s.label_color);
    sch->SetSecondLabel(s.second_label);
    sch->SetSecondLabelHexColor(s.second_label_color);
    sch->SetDescriptionHexColor(s.description_color);
    sch->SetHexBackgroundColor(s.background_color);
    sch->SetHexBorderColor(s.border_color);
    sch->SetHexHeaderColor(s.header_color);

    {
      auto it = status_map.find(s.status);
      if (it != status_map.end()) {
        sch->m_NodeStatus = it->second;
      }
    }

    {
      auto it = type_map.find(s.type);
      if (it != type_map.end()) {
        sch->SetType(it->second);
      }
    }

    if (!s.header_logo_path.empty()) {
      sch->SetLogoPath(s.header_logo_path);
    }

    for (auto &sip : s.input_pins) {
      sch->AddInputPin(sip.id, sip.type);
    }

    for (auto &sop : s.output_pins) {
      sch->AddOutputPin(sop.id, sop.type);
    }

    if (!s.header_pin.id.empty()) {
      sch->AddHeaderPin(s.header_pin.id, s.header_pin.type);
    }
  }
}

}; // namespace ModuleUI

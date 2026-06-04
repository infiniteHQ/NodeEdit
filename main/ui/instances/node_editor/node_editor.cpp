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
    const std::shared_ptr<NodeEdit::NodeEditContext> &ctx,
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph) {
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
  backend_node_graph_session = graph;

  ui_node_graph.m_NodeSpawnCallback = [this](const std::string &sch_id, float x,
                                             float y,
                                             const std::string &connID) {
    SpawnNodeInstance(sch_id, x, y, connID);
  };

  std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;

  this->ctx = vxe::get_current_context();
} // namespace ModuleUI

std::shared_ptr<Cherry::AppWindow> &NodeEditorAppWindow::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<NodeEditorAppWindow> NodeEditorAppWindow::Create(
    const std::string &name,
    const std::shared_ptr<NodeEdit::NodeEditContext> &ctx,
    const std::shared_ptr<NodeEdit::NodeEditGraphSession> &graph) {
  auto instance = std::shared_ptr<NodeEditorAppWindow>(
      new NodeEditorAppWindow(name, ctx, graph));
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

  CherryNextComponent.SetProperty("padding_y", "5.5f");
  CherryNextComponent.SetProperty("padding_x", "6.0f");
  CherryNextComponent.SetProperty("size_x", "18");
  CherryNextComponent.SetProperty("size_y", "18");
  if (CherryKit::ButtonImage(
          NodeEdit::GetPath("/resources/icons/icon_save.png"))
          .GetDataAs<bool>("isClicked")) {
    save_incoming = true;
  }

  CherryNextComponent.SetProperty("padding_y", "5.5f");
  CherryNextComponent.SetProperty("padding_x", "6.0f");
  CherryNextComponent.SetProperty("size_x", "18");
  CherryNextComponent.SetProperty("size_y", "18");
  if (CherryKit::ButtonImage(
          NodeEdit::GetPath("/resources/icons/icon_refresh.png"))
          .GetDataAs<bool>("isClicked")) {
    Refresh();
  }
}
void NodeEditorAppWindow::Render() {
  CherryApp.PushComponentPool(&m_ComponentPool);

  auto &cmp = CherryKit::NodeAreaOpen("", 0, 0, &ui_node_ctx, &ui_node_graph);

  if (refreshed) {
    cmp.SetProperty("refresh", "true");
    refreshed = false;
  }

  if (save_incoming) {
    cmp.SetProperty("save", "true");
    save_incoming = false;
  }

  if (cmp.GetDataAs<bool>("graph_saved")) {
    Save();
    cmp.SetData("graph_saved", "false;");
  }

  CherryApp.PopComponentPool();
}

void NodeEditorAppWindow::RenderRightMenubar() {
  //
}

void NodeEditorAppWindow::RenderBottombar() {
  //
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

    if (s.spawnable) {
      Cherry::NodeSystem::NodeSpawnPossibility p;
      p.category = s.spawn_possibility.category;
      p.schema_id = s.spawn_possibility.schema_id;
      p.proper_logo = s.spawn_possibility.proper_logo;
      p.proper_name = s.spawn_possibility.proper_name;
      p.proper_description = s.spawn_possibility.proper_description;
      ui_node_graph.AddPossibility(p);
    }
  }
}

void NodeEditorAppWindow::Refresh() {
  if (!backend_node_graph_session) {
    return;
  }

  NodeEdit::RefreshGraphSession(backend_node_graph_session);

  auto &instances = backend_node_graph_session->graph.instances;
  auto &connections = backend_node_graph_session->graph.connections;

  ui_node_graph.m_InstanciatedNodes.clear();
  for (auto &i : instances) {
    Cherry::NodeSystem::NodeInstance n;
    n.InstanceID = i.instance_id;
    n.Position.x = i.pos_x;
    n.Position.y = i.pos_y;
    n.TypeID = i.type_id;
    // TODO custom data of nodes
    ui_node_graph.m_InstanciatedNodes.push_back(n);
  }

  ui_node_graph.m_Connections.clear();
  for (auto &conn : connections) {
    Cherry::NodeSystem::NodeConnection c;
    c.NodeInstanceIDA = conn.node_instance_id_A;
    c.NodeInstanceIDB = conn.node_instance_id_B;
    c.PinIDA = conn.pin_id_A;
    c.PinIDB = conn.pin_id_B;
    ui_node_graph.m_Connections.push_back(c);
  }
  refreshed = true;
}

void NodeEditorAppWindow::Save() {
  if (!backend_node_graph_session) {
    return;
  }

  auto &instances = backend_node_graph_session->graph.instances;
  auto &connections = backend_node_graph_session->graph.connections;

  connections.clear();
  instances.clear();

  for (auto n : ui_node_graph.m_InstanciatedNodes) {
    NodeEdit::NodeEditInstance i;

    i.type_id = n.TypeID;
    i.instance_id = n.InstanceID;
    i.pos_x = n.Position.x;
    i.pos_y = n.Position.y;
    // TODO custom data of nodes

    instances.push_back(i);
  }

  for (auto c : ui_node_graph.m_Connections) {
    NodeEdit::NodeEditConnection conn;

    conn.node_instance_id_A = c.NodeInstanceIDA;
    conn.node_instance_id_B = c.NodeInstanceIDB;
    conn.pin_id_A = c.PinIDA;
    conn.pin_id_B = c.PinIDB;

    connections.push_back(conn);
  }

  NodeEdit::SaveGraphSession(backend_node_graph_session);
}

static int id_counter = 1;
void NodeEditorAppWindow::SpawnNodeInstance(const std::string &sch_id,
                                            const float &x, const float &y,
                                            const std::string &connID) {
  // Save local UI state from the edit graph pos
  Cherry::NodeSystem::NodeInstance inst;
  inst.TypeID = sch_id;
  inst.InstanceID = std::to_string(id_counter++);
  inst.Position = Cherry::NodeSystem::Vec2(50, 50);
  inst.Size = Cherry::NodeSystem::Vec2(50, 50);

  if (!connID.empty()) {
    // TODO: Auto connect
  }

  ui_node_graph.AddNodeInstance(inst);

  refreshed = true;
}
}; // namespace ModuleUI

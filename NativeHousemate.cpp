#include "pch.h"

#include "NativeHousemate.h"

NativeHousemate::NativeHousemate::NativeHousemate(Dalamud::Plugin::DalamudPluginInterface^ pluginInterface,
												Dalamud::Game::Gui::GameGui^ gameGui,
												Dalamud::Game::ClientState::ClientState^ clientState,
												Dalamud::Game::Command::CommandManager^ commandManager,
												Dalamud::Game::Framework^ framework,
												Dalamud::Game::ClientState::Objects::ObjectTable^ objectTable,
												Dalamud::Game::SigScanner^ sigScanner,
												Dalamud::Data::DataManager^ dataManager)
{
	Framework = framework;
	GameGui = gameGui;
	ClientState = clientState;
	CommandManager = commandManager;
	PluginInterface = pluginInterface;
	ObjectTable = objectTable;

	auto pluginConfiguration = safe_cast<Configuration^>(PluginInterface->GetPluginConfig());

	if (pluginConfiguration == nullptr)
	{
		pluginConfiguration = gcnew Configuration();
	}

	_ui = gcnew HousemateUI(pluginConfiguration);

	const auto handler = gcnew Dalamud::Game::Command::CommandInfo(gcnew Dalamud::Game::Command::CommandInfo::HandlerDelegate(this, &NativeHousemate::OnCommand));
	handler->HelpMessage = "/housemate will open the Housemate window.";

	CommandManager->AddHandler(this->CommandName, handler);

	HousingData::Init(dataManager);
	HousingMemory::Init(sigScanner);

	PluginInterface->UiBuilder->Draw += gcnew ImGuiScene::RawDX11Scene::BuildUIDelegate(this, &NativeHousemate::DrawUI);
	PluginInterface->UiBuilder->OpenConfigUi += gcnew EventHandler(this, &NativeHousemate::DrawConfigUI);
}

NativeHousemate::NativeHousemate::~NativeHousemate()
{
	delete _ui;

	CommandManager->RemoveHandler(this->CommandName);
	PluginInterface->UiBuilder->Draw -= gcnew ImGuiScene::RawDX11Scene::BuildUIDelegate(this, &NativeHousemate::DrawUI);
	PluginInterface->UiBuilder->OpenConfigUi -= gcnew EventHandler(this, &NativeHousemate::DrawConfigUI);
}

void NativeHousemate::NativeHousemate::OnCommand(String^ command, String^ args)
{
	_ui->Visible = true;
}

void NativeHousemate::NativeHousemate::DrawConfigUI(Object^ sender, EventArgs^ e)
{
	_ui->Visible = true;
}

void NativeHousemate::NativeHousemate::DrawUI()
{
	_ui->Draw();
}

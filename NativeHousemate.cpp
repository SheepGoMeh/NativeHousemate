#include "pch.h"

#include "NativeHousemate.h"

void NativeHousemate::NativeHousemate::Initialize(DalamudPluginInterface^ pluginInterface)
{
	_pi = pluginInterface;

	_configuration = safe_cast<Configuration^>(_pi->GetPluginConfig());

	if (_configuration == nullptr)
	{
		_configuration = gcnew Configuration();
	}

	_ui = gcnew HousemateUI(_configuration, _pi);

	const auto handler = gcnew CommandInfo(gcnew CommandInfo::HandlerDelegate(this, &NativeHousemate::OnCommand));
	handler->HelpMessage = "/housemate will open the Housemate window.";

	_pi->CommandManager->AddHandler(this->CommandName, handler);

	HousingData::Init(_pi);
	HousingMemory::Init(_pi);
	
	_pi->UiBuilder->OnBuildUi += gcnew ImGuiScene::RawDX11Scene::BuildUIDelegate(this, &NativeHousemate::DrawUI);
	_pi->UiBuilder->OnOpenConfigUi += gcnew EventHandler(this, &NativeHousemate::DrawConfigUI);
}

NativeHousemate::NativeHousemate::~NativeHousemate()
{
	delete _ui;
	
	_pi->CommandManager->RemoveHandler(this->CommandName);
	delete _pi;
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

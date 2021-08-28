#pragma once
#include "HousemateUI.h"

using namespace System;

namespace NativeHousemate
{
	public ref class NativeHousemate : Dalamud::Plugin::IDalamudPlugin
	{
		HousemateUI^ _ui;
		literal String^ CommandName = "/housemate";

	public:
		static Dalamud::Game::Framework^ Framework;
		static Dalamud::Game::Gui::GameGui^ GameGui;
		static Dalamud::Game::ClientState::ClientState^ ClientState;
		static Dalamud::Game::Command::CommandManager^ CommandManager;
		static Dalamud::Plugin::DalamudPluginInterface^ PluginInterface;
		static Dalamud::Game::ClientState::Objects::ObjectTable^ ObjectTable;
		
		property String^ Name { virtual String^ get() { return "Housemate"; } }
		NativeHousemate(Dalamud::Plugin::DalamudPluginInterface^ pluginInterface,
						Dalamud::Game::Gui::GameGui^ gameGui,
						Dalamud::Game::ClientState::ClientState^ clientState,
						Dalamud::Game::Command::CommandManager^ commandManager,
						Dalamud::Game::Framework^ framework,
						Dalamud::Game::ClientState::Objects::ObjectTable^ objectTable,
						Dalamud::Game::SigScanner^ sigScanner,
						Dalamud::Data::DataManager^ dataManager);
		~NativeHousemate();

	private:
		void OnCommand(String^ command, String^ args);
		void DrawConfigUI(Object^ sender, EventArgs^ e);
		void DrawUI();
	};
}

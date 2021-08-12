#pragma once
#include "HousemateUI.h"

using namespace System;
using namespace Dalamud::Game::Command;
using namespace Dalamud::Plugin;

namespace NativeHousemate
{
	public ref class NativeHousemate : IDalamudPlugin
	{
		literal String^ CommandName = "/housemate";
		DalamudPluginInterface^ _pi;
		HousemateUI^ _ui;
		Configuration^ _configuration;
		
	public:
		virtual void Initialize(DalamudPluginInterface^ pluginInterface);
		property String^ Name { virtual String^ get() { return "Housemate"; } }
		~NativeHousemate();

	private:
		void OnCommand(String^ command, String^ args);
		void DrawConfigUI(Object^ sender, EventArgs^ e);
		void DrawUI();
	};
}

#pragma once

#include "pch.h"
#include "HousingData.h"
#include "HousingMemory.h"
#include "Configuration.h"

using namespace ImGuiNET;
using namespace System::Numerics;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace NativeHousemate
{
	public ref class HousemateUI : IDisposable
	{
		static initonly Vector4 Gray = ImGui::ColorConvertU32ToFloat4(0xBBBBBBFF);
		static initonly Vector4 White = ImGui::ColorConvertU32ToFloat4(0xFFFFFFFF);

		static HousingData^ Data = HousingData::Instance;
		static HousingMemory^ Mem = HousingMemory::Instance;

		ImGuiListClipper* _clipper;
		Configuration^ _config;
		bool _outLast;
		bool _objectsOpen;

		void PlacardRender(float renderDistance);
		void Render(float renderDistance);
		void Render();
		void DrawPlotPlate(Dalamud::Game::ClientState::Objects::Types::GameObject^ placard, uint32_t placardId, CommonLandSet^ land);
		void DrawMainWindow();
		void OutdoorTab();
		void RenderHousingObjectList(bool collapsible);
		void IndoorTab();
		void SettingsTab();

	public:
		HousemateUI(Configuration^ pluginConfiguration);
		~HousemateUI();

		void Draw();


		bool Visible;
	};
}

#pragma once

#include "pch.h"

#include "Structs.h"

using namespace System::Runtime;
using namespace Dalamud::Plugin;
using namespace Dalamud::Configuration;

namespace NativeHousemate
{
	[Serializable]
	public ref class Configuration : IPluginConfiguration
	{
		[NonSerialized] DalamudPluginInterface^ _pluginInterface;
		[NonSerialized] int _version = 1;
	public:
		bool Render;
		float RenderDistance = 10.0f;
		bool SortObjectLists;
		SortType SortType = SortType::Distance;

		property int Version
		{
			virtual int get()
			{
				return _version;
			}

			virtual void set(int version)
			{
				_version = version;
			}
		}

		void Save();
		void Initialize(DalamudPluginInterface^ pluginInterface);
	};
}

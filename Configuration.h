#pragma once

#include "pch.h"

#include "Structs.h"

using namespace System::Runtime;

namespace NativeHousemate
{
	[Serializable]
	public ref class Configuration : Dalamud::Configuration::IPluginConfiguration
	{
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
	};
}

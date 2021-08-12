#include "pch.h"
#include "Configuration.h"

void NativeHousemate::Configuration::Save()
{
	_pluginInterface->SavePluginConfig(this);
}

void NativeHousemate::Configuration::Initialize(DalamudPluginInterface^ pluginInterface)
{
	_pluginInterface = pluginInterface;
}

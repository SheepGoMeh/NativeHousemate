#include "pch.h"
#include "Configuration.h"
#include "NativeHousemate.h"

void NativeHousemate::Configuration::Save()
{
	NativeHousemate::PluginInterface->SavePluginConfig(this);
}
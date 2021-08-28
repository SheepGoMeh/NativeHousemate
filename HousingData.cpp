#include "pch.h"
#include "HousingData.h"

NativeHousemate::HousingData::HousingData(Dalamud::Data::DataManager^ dataManager)
{
	auto sheet = dataManager->GetExcelSheet<HousingLandSet^>();
	auto terriKeys = gcnew array<uint32_t>{339, 340, 341, 641};

	_territoryToLandSetDict = gcnew Dictionary<uint32_t, Dictionary<uint32_t, CommonLandSet^>^>();

	for (uint32_t i = 0; i != sheet->RowCount; ++i)
	{
		if (terriKeys->Length < 1)
		{
			continue;
		}

		auto row = sheet->GetRow(i);
		auto rowDict = gcnew Dictionary<uint32_t, CommonLandSet^>();

		for (auto j = 0; j != row->LandSets->Length; ++j)
		{
			auto cset = CommonLandSet::FromExd(row->LandSets[j], j);
			rowDict[cset->PlacardId] = cset;
		}

		_territoryToLandSetDict[terriKeys[i]] = rowDict;
	}

	auto unitedExteriorSheet = dataManager->GetExcelSheet<HousingUnitedExterior^>();
	_unitedDict = gcnew Dictionary<uint32_t, uint32_t>();

	for each (auto row in unitedExteriorSheet)
	{
		for each (auto item in row->Item)
		{
			_unitedDict[item->Row] = row->RowId;
		}
	}

	auto itemSheet = dataManager->GetExcelSheet<Item^>();
	_itemDict = gcnew Dictionary<uint32_t, Item^>();

	for each (auto item in itemSheet)
	{
		if (item->AdditionalData != 0 && (item->ItemSearchCategory->Row == 65 || item->ItemSearchCategory->Row == 66))
		{
			_itemDict[item->AdditionalData] = item;
		}
	}

	auto stainSheet = dataManager->GetExcelSheet<Stain^>();
	_stainDict = gcnew Dictionary<uint32_t, Stain^>();

	for each (auto row in stainSheet)
	{
		_stainDict[row->RowId] = row;
	}

	auto furnitureSheet = dataManager->GetExcelSheet<HousingFurniture^>();
	_furnitureDict = gcnew Dictionary<uint32_t, HousingFurniture^>();

	for each (auto row in furnitureSheet)
	{
		_furnitureDict[row->RowId] = row;
	}

	auto yardObjectSheet = dataManager->GetExcelSheet<HousingYardObject^>();
	_yardObjectDict = gcnew Dictionary<uint32_t, HousingYardObject^>();

	for each (auto row in yardObjectSheet)
	{
		_yardObjectDict[row->RowId] = row;
	}

	Dalamud::Logging::PluginLog::Log(String::Format("Loaded {0} landset rows", _territoryToLandSetDict->Keys->Count));
	Dalamud::Logging::PluginLog::Log(String::Format("Loaded {0} furniture", _furnitureDict->Keys->Count));
	Dalamud::Logging::PluginLog::Log(String::Format("Loaded {0} landset yard objects", _yardObjectDict->Keys->Count));
	Dalamud::Logging::PluginLog::Log(String::Format("Loaded {0} landset united parts", _unitedDict->Keys->Count));
	Dalamud::Logging::PluginLog::Log(String::Format("Loaded {0} landset stain infos", _stainDict->Keys->Count));
	Dalamud::Logging::PluginLog::Log(String::Format("Loaded {0} landset items with AdditionalData", _itemDict->Keys->Count));
}

void NativeHousemate::HousingData::Init(Dalamud::Data::DataManager^ data)
{
	Instance = gcnew HousingData(data);
}

bool NativeHousemate::HousingData::TryGetYardObject(uint32_t id, HousingYardObject^% yardObject)
{
	return _yardObjectDict->TryGetValue(id, yardObject);
}

bool NativeHousemate::HousingData::TryGetFurniture(uint32_t id, HousingFurniture^% furniture)
{
	return _furnitureDict->TryGetValue(id, furniture);
}

bool NativeHousemate::HousingData::TryGetLandSetDict(uint32_t id, Dictionary<uint32_t, CommonLandSet^>^% dict)
{
	return _territoryToLandSetDict->TryGetValue(id, dict);
}

bool NativeHousemate::HousingData::TryGetItem(uint32_t id, Item^% item)
{
	return _itemDict->TryGetValue(id, item);
}

bool NativeHousemate::HousingData::TryGetStain(uint32_t id, Stain^% stain)
{
	return _stainDict->TryGetValue(id, stain);
}

bool NativeHousemate::HousingData::IsUnitedExteriorPart(uint32_t id, Item^% item)
{
	item = nullptr;
	uint32_t unitedId;

	if (!_unitedDict->TryGetValue(id, unitedId))
	{
		return false;
	}

	return _itemDict->TryGetValue(unitedId, item);
}

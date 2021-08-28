#include "pch.h"
#include "HousingMemory.h"

#include <algorithm>
#include <vector>

#include "HousingData.h"
#include "Utils.h"

NativeHousemate::HousingMemory::HousingMemory(Dalamud::Game::SigScanner^ scanner)
{
	try
	{
		auto HousingModulePtr = scanner->GetStaticAddressFromSig(
			"40 53 48 83 EC 20 33 DB 48 39 1D ?? ?? ?? ?? 75 2C 45 33 C0 33 D2 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 85 C0 74 11 48 8B C8 E8 ?? ?? ?? ?? 48 89 05 ?? ?? ?? ?? EB 07",
			0xA);
		auto LayoutWorldPtr = scanner->GetStaticAddressFromSig(
			"48 8B 05 ?? ?? ?? ?? 48 8B 48 20 48 85 C9 74 31 83 B9 ?? ?? ?? ?? ?? 74 28 80 B9 ?? ?? ?? ?? ?? 75 1F 80 B9 ?? ?? ?? ?? ?? 74 03 B0 01 C3",
			0x2);

		Dalamud::Logging::PluginLog::Log("Pre-HousingModule at {0:X}", HousingModulePtr.ToInt64().ToString());
		Dalamud::Logging::PluginLog::Log("Pre-LayoutWorld at {0:X}", LayoutWorldPtr.ToInt64().ToString());

		HousingModule = *static_cast<NativeHousemate::HousingModule**>(HousingModulePtr.ToPointer());
		LayoutWorld = *static_cast<NativeHousemate::LayoutWorld**>(LayoutWorldPtr.ToPointer());

		Dalamud::Logging::PluginLog::Log("HousingModule at {0:X}", reinterpret_cast<intptr_t>(HousingModule));
		Dalamud::Logging::PluginLog::Log("LayoutWorld at {0:X}", reinterpret_cast<intptr_t>(HousingModule));
	}
	catch (Exception^ e)
	{
		Dalamud::Logging::PluginLog::Log(e, "Could not load Housemate!!");
	}
}

void NativeHousemate::HousingMemory::Init(Dalamud::Game::SigScanner^ scanner)
{
	Instance = gcnew HousingMemory(scanner);
}

NativeHousemate::InteriorFloor NativeHousemate::HousingMemory::CurrentFloor()
{
	return HousingModule->currentTerritory == nullptr || HousingModule->IsOutdoors()
				? InteriorFloor::None
				: HousingModule->CurrentFloor();
}

uint32_t NativeHousemate::HousingMemory::GetTerritoryTypeId()
{
	return LayoutWorld == nullptr || LayoutWorld->ActiveLayout == nullptr
				? 0
				: LayoutWorld->ActiveLayout->TerritoryTypeId;
}

float NativeHousemate::HousingMemory::GetInteriorLightLevel()
{
	return IsOutdoors()
				? 0.0f
				: LayoutWorld == nullptr || LayoutWorld->ActiveLayout == nullptr
				? 0.0f
				: LayoutWorld->ActiveLayout->IndoorAreaData == nullptr
				? 0.0f
				: LayoutWorld->ActiveLayout->IndoorAreaData->LightLevel;
}

array<NativeHousemate::CommonFixture^>^ NativeHousemate::HousingMemory::GetInteriorCommonFixtures(int32_t floorId)
{
	if (IsOutdoors())
	{
		return gcnew array<CommonFixture^>(0);
	}

	if (LayoutWorld == nullptr || LayoutWorld->ActiveLayout == nullptr)
	{
		return gcnew array<CommonFixture^>(0);
	}

	if (LayoutWorld->ActiveLayout->IndoorAreaData == nullptr)
	{
		return gcnew array<CommonFixture^>(0);
	}

	const auto floor = LayoutWorld->ActiveLayout->IndoorAreaData->Floors[floorId];
	auto ret = gcnew array<CommonFixture^>(sizeof IndoorAreaData::Floors / sizeof *IndoorAreaData::Floors);

	for (auto i = 0; i != ret->Length; ++i)
	{
		const auto key = floor.Parts[i];
		Item^ item;

		if (!HousingData::Instance->TryGetItem(key, item))
		{
			HousingData::Instance->IsUnitedExteriorPart(key, item);
		}

		ret[i] = gcnew CommonFixture(
			false,
			i,
			key,
			nullptr,
			item);
	}

	return ret;
}

array<NativeHousemate::CommonFixture^>^ NativeHousemate::HousingMemory::GetExteriorCommonFixtures(int32_t plotId)
{
	if (IsIndoors())
	{
		return gcnew array<CommonFixture^>(0);
	}

	if (GetHousingController() == nullptr)
	{
		return gcnew array<CommonFixture^>(0);
	}

	const auto home = LayoutWorld->ActiveLayout->HousingController->Houses[plotId];

	if (home.Size == -1)
	{
		return gcnew array<CommonFixture^>(0);
	}

	if (home.Parts[0].Category == -1)
	{
		return gcnew array<CommonFixture^>(0);
	}

	auto ret = gcnew array<CommonFixture^>(sizeof HouseCustomize::Parts / sizeof *HouseCustomize::Parts);

	for (auto i = 0; i != ret->Length; ++i)
	{
		const auto colorId = home.Parts[i].Color;

		Item^ item;
		Stain^ stain;

		HousingData::Instance->TryGetStain(colorId, stain);
		HousingData::Instance->TryGetItem(home.Parts[i].FixtureKey, item);

		ret[i] = gcnew CommonFixture(
			true,
			home.Parts[i].Category,
			home.Parts[i].FixtureKey,
			stain,
			item);
	}

	return ret;
}

bool NativeHousemate::HousingMemory::TryGetHousingGameObject(int32_t index, HousingGameObject% gameObject)
{
	if (HousingModule == nullptr ||
		HousingModule->GetCurrentManager() == nullptr ||
		HousingModule->GetCurrentManager()->objects == nullptr ||
		HousingModule->GetCurrentManager()->objects[index] == nullptr)
	{
		return false;
	}

	gameObject = *static_cast<HousingGameObject*>(HousingModule->GetCurrentManager()->objects[index]);

	return true;
}

bool sortByDistance(const std::pair<void*, float> a, const std::pair<void*, float> b)
{
	return a.second < b.second;
}

bool NativeHousemate::HousingMemory::TryGetSortedHousingGameObjectList(List<HousingGameObject>^% objects,
																		Vector3 playerPos)
{
	if (HousingModule == nullptr ||
		HousingModule->GetCurrentManager() == nullptr ||
		HousingModule->GetCurrentManager()->objects == nullptr)
	{
		return false;
	}

	objects = gcnew List<HousingGameObject>();

	std::vector<std::pair<void*, float>> tmpObjects;

	for (auto i = 0; i != sizeof HousingObjectManager::objects / sizeof *HousingObjectManager::objects; ++i)
	{
		const auto ptr = HousingModule->GetCurrentManager()->objects[i];

		if (ptr == nullptr)
		{
			continue;
		}

		tmpObjects.push_back(
			std::make_pair(ptr,
							Utils::DistanceFromPlayer(*static_cast<HousingGameObject*>(ptr), playerPos)));
	}

	std::sort(std::begin(tmpObjects), std::end(tmpObjects), sortByDistance);

	for (const auto it : tmpObjects)
	{
		objects->Add(*static_cast<HousingGameObject*>(it.first));
	}

	return true;
}

private ref class SortByName : Comparer<NativeHousemate::HousingGameObject>
{
public:
	virtual int Compare(NativeHousemate::HousingGameObject a, NativeHousemate::HousingGameObject b) override
	{
		String^ name1 = String::Empty;
		String^ name2 = String::Empty;

		HousingFurniture^ furniture;
		HousingYardObject^ yardObject;

		if (NativeHousemate::HousingData::Instance->TryGetFurniture(a.housingRowId, furniture))
		{
			name1 = furniture->Item->Value->Name->ToString();
		}
		else if (NativeHousemate::HousingData::Instance->TryGetYardObject(a.housingRowId, yardObject))
		{
			name1 = yardObject->Item->Value->Name->ToString();
		}

		if (NativeHousemate::HousingData::Instance->TryGetFurniture(b.housingRowId, furniture))
		{
			name2 = furniture->Item->Value->Name->ToString();
		}
		else if (NativeHousemate::HousingData::Instance->TryGetYardObject(b.housingRowId, yardObject))
		{
			name2 = yardObject->Item->Value->Name->ToString();
		}

		return String::Compare(name1, name2, StringComparison::Ordinal);
	}
};

bool NativeHousemate::HousingMemory::TryGetNameSortedHousingGameObjectList(List<HousingGameObject>^% objects)
{
	if (HousingModule == nullptr ||
		HousingModule->GetCurrentManager() == nullptr ||
		HousingModule->GetCurrentManager()->objects == nullptr)
	{
		return false;
	}

	objects = gcnew List<HousingGameObject>();

	for (auto i = 0; i != sizeof HousingObjectManager::objects / sizeof *HousingObjectManager::objects; ++i)
	{
		const auto ptr = HousingModule->GetCurrentManager()->objects[i];

		if (ptr == nullptr)
		{
			continue;
		}

		objects->Add(*static_cast<HousingGameObject*>(ptr));
	}

	objects->Sort(gcnew SortByName());

	return true;
}

bool NativeHousemate::HousingMemory::TryGetUnsortedHousingGameObjectList(List<HousingGameObject>^% objects)
{
	if (HousingModule == nullptr ||
		HousingModule->GetCurrentManager() == nullptr ||
		HousingModule->GetCurrentManager()->objects == nullptr)
	{
		return false;
	}

	objects = gcnew List<HousingGameObject>();

	for (auto i = 0; i != sizeof HousingObjectManager::objects / sizeof *HousingObjectManager::objects; ++i)
	{
		const auto ptr = HousingModule->GetCurrentManager()->objects[i];

		if (ptr == nullptr)
		{
			continue;
		}

		objects->Add(*static_cast<HousingGameObject*>(ptr));
	}

	return true;
}

NativeHousemate::HousingController* NativeHousemate::HousingMemory::GetHousingController()
{
	return LayoutWorld == nullptr ||
			LayoutWorld->ActiveLayout == nullptr ||
			LayoutWorld->ActiveLayout->HousingController == nullptr
				? nullptr
				: LayoutWorld->ActiveLayout->HousingController;
}

bool NativeHousemate::HousingMemory::IsOutdoors()
{
	return HousingModule == nullptr ? false : HousingModule->IsOutdoors();
}

bool NativeHousemate::HousingMemory::IsIndoors()
{
	return HousingModule == nullptr ? false : HousingModule->IsIndoors();
}

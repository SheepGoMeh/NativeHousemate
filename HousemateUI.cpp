#include "pch.h"
#include "HousemateUI.h"
#include "Utils.h"

void NativeHousemate::HousemateUI::PlacardRender(float renderDistance)
{
	auto placardIdOffset = 120;

	Dictionary<uint32_t, CommonLandSet^>^ landSets;

	if (!Data->TryGetLandSetDict(Mem->GetTerritoryTypeId(), landSets))
	{
		return;
	}

	auto actorTable = _pi->ClientState->Objects;
	if (actorTable == nullptr)
	{
		return;
	}

	for each (auto actor in actorTable)
	{
		if (actor == nullptr || _pi->ClientState->LocalPlayer == nullptr)
		{
			continue;
		}

		CommonLandSet^ landSet;
		auto placardId = *reinterpret_cast<uint32_t*>(static_cast<intptr_t*>(actor->Address.ToPointer()) +
			placardIdOffset);

		if (landSets->TryGetValue(placardId, landSet) ||
			Vector3::Distance(_pi->ClientState->LocalPlayer->Position, actor->Position) > renderDistance)
		{
			continue;
		}

		DrawPlotPlate(actor, placardId, landSet);
	}
}

void NativeHousemate::HousemateUI::Render(float renderDistance)
{
	if (Mem->CurrentManager == nullptr)
	{
		return;
	}

	for (auto i = 0; i != sizeof HousingObjectManager::objects / sizeof *HousingObjectManager::objects; ++i)
	{
		auto hObject = static_cast<HousingGameObject*>(Mem->CurrentManager->objects[i]);

		if (hObject == nullptr)
		{
			continue;
		}

		String^ objectName = "unknown";

		if (Mem->IsOutdoors())
		{
			HousingYardObject^ yardObject;

			if (Data->TryGetYardObject(hObject->housingRowId, yardObject))
			{
				objectName = yardObject->Item->Value->Name->ToString();
			}
		}
		else
		{
			HousingFurniture^ furnitureObject;

			if (Data->TryGetFurniture(hObject->housingRowId, furnitureObject))
			{
				objectName = furnitureObject->Item->Value->Name->ToString();
			}
		}

		Nullable<Vector3>^ nPos = _pi->ClientState == nullptr
									? Nullable<Vector3>()
									: _pi->ClientState->LocalPlayer == nullptr
									? Nullable<Vector3>()
									: Nullable<Vector3>(_pi->ClientState->LocalPlayer->Position);

		Vector2 screenCoords;

		if (!nPos->HasValue ||
			!_pi->Framework->Gui->WorldToScreen(hObject, screenCoords) ||
			Utils::DistanceFromPlayer(*hObject, nPos->Value) > renderDistance)
		{
			continue;
		}

		ImGui::PushID(String("HousingObjects").Concat(i));
		ImGui::SetNextWindowPos(Vector2(screenCoords.X, screenCoords.Y));
		ImGui::SetNextWindowBgAlpha(0.5f);
		ImGui::SetNextWindowViewport(ImGui::GetMainViewport().ID);

		ImGui::Begin(String("hou").Concat(i),
					ImGuiWindowFlags::NoDecoration |
					ImGuiWindowFlags::AlwaysAutoResize |
					ImGuiWindowFlags::NoSavedSettings |
					ImGuiWindowFlags::NoMove |
					ImGuiWindowFlags::NoMouseInputs |
					ImGuiWindowFlags::NoFocusOnAppearing |
					ImGuiWindowFlags::NoNav);

		if (String::IsNullOrEmpty(objectName))
		{
			ImGui::End();
			continue;
		}

		ImGui::Text(objectName);

		Stain^ color;
		if (hObject->color != 0 && Data->TryGetStain(hObject->color, color))
		{
			ImGui::SameLine();
			Utils::StainButton(objectName, color);
		}

		ImGui::End();
		ImGui::PopID();
	}
}

void NativeHousemate::HousemateUI::Render()
{
	Render(50.0f);
}

void NativeHousemate::HousemateUI::DrawPlotPlate(GameObject^ placard, uint32_t placardId, CommonLandSet^ land)
{
	if (Mem->GetHousingController() == nullptr)
	{
		return;
	}

	Vector2 screenCoords;
	auto customize = Mem->GetHousingController()->Houses[land->PlotIndex];

	if (_pi->Framework->Gui->WorldToScreen(
		Vector3(placard->Position.X, placard->Position.Z + 4, placard->Position.Z), screenCoords))
	{
		ImGui::PushID(String("Placard").Concat(placardId));
		ImGui::SetNextWindowPos(Vector2(screenCoords.X, screenCoords.Y));
		ImGui::SetNextWindowBgAlpha(0.5f);
		ImGui::SetNextWindowViewport(ImGui::GetMainViewport().ID);

		ImGui::Begin(String("Plot ").Concat(land->PlotIndex + 1),
					ImGuiWindowFlags::NoDecoration |
					ImGuiWindowFlags::AlwaysAutoResize |
					ImGuiWindowFlags::NoSavedSettings |
					ImGuiWindowFlags::NoMove |
					ImGuiWindowFlags::NoMouseInputs |
					ImGuiWindowFlags::NoFocusOnAppearing |
					ImGuiWindowFlags::NoNav);

		Item^ item;
		Stain^ color;

		auto roof = customize.Parts[static_cast<uint32_t>(ExteriorPartsType::Roof)];

		if (roof.FixtureKey != 0 && Data->IsUnitedExteriorPart(roof.FixtureKey, item))
		{
			ImGui::Text(String("Exterior: ").Concat(item->Name));

			if (roof.Color != 0 && Data->TryGetStain(roof.Color, color))
			{
				ImGui::SameLine();
				Utils::StainButton(item->Name, color);
			}
		}
		else
		{
			for (auto i = 0; i != sizeof HouseCustomize::Parts / sizeof *HouseCustomize::Parts; ++i)
			{
				if (!Data->TryGetItem(customize.Parts[i].FixtureKey, item))
				{
					continue;
				}

				ImGui::Text(String::Format(
					"{0}: {1}",
					Utils::GetExteriorPartDescriptor(static_cast<ExteriorPartsType>(i)), item->Name));

				if (customize.Parts[i].Color != 0 && Data->TryGetStain(customize.Parts[i].Color, color))
				{
					ImGui::SameLine();
					Utils::StainButton(item->Name, color);
				}
			}
		}

		ImGui::End();
		ImGui::PopID();
	}
}

void NativeHousemate::HousemateUI::DrawMainWindow()
{
	if (!Visible)
	{
		return;
	}

	ImGui::SetNextWindowSize(Vector2(375, 600), ImGuiCond::FirstUseEver);
	if (ImGui::Begin("Housemate", Visible, ImGuiWindowFlags::NoScrollbar | ImGuiWindowFlags::NoScrollWithMouse))
	{
		ImGui::BeginTabBar("MainTabs");

		auto isSwapping = false;

		if (Mem->IsOutdoors())
		{
			if (!_outLast)
			{
				isSwapping = true;
			}

			OutdoorTab();
			_outLast = true;
		}

		if (Mem->IsIndoors())
		{
			if (!_outLast)
			{
				isSwapping = true;
			}

			IndoorTab();
			_outLast = true;
		}

		if (!isSwapping)
		{
			SettingsTab();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void NativeHousemate::HousemateUI::OutdoorTab()
{
	if (!ImGui::BeginTabItem("Outdoors"))
	{
		return;
	}

	if (Mem->CurrentManager == nullptr || Mem->IsIndoors())
	{
		ImGui::Text("You aren't in an outdoor housing zone!");
		ImGui::EndTabItem();
		return;
	}

	if (!ImGui::CollapsingHeader("Homes", ImGuiTreeNodeFlags::DefaultOpen))
	{
		RenderHousingObjectList(true);
		ImGui::EndTabItem();
		return;
	}

	ImGui::BeginChild("##COLUMNAPIISDUMBIHATEYOU1", Vector2(200, ImGui::GetFontSize()), false);
	ImGui::Columns(2);
	ImGui::Text("Plot");
	ImGui::SetColumnWidth(0, 38);
	ImGui::NextColumn();
	ImGui::Text("Parts");
	ImGui::NextColumn();
	ImGui::SetColumnWidth(1, 300);
	ImGui::EndChild();

	const auto size = _objectsOpen
						? ImGui::GetFontSize() * 15.0f
						: ImGui::GetWindowHeight() - 140.0f * Utils::GlobalFontScale();

	ImGui::BeginChild("##homes", Vector2(-1, size), false);

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 38);
	ImGui::SetColumnWidth(1, 300);

	const auto basePlot = Mem->GetExteriorCommonFixtures(0);
	const auto subPlot = Mem->GetExteriorCommonFixtures(30);

	int start = 0, end = 0;

	if (basePlot->Length != 0 && subPlot->Length == 0)
	{
		start = 0;
		end = 30;
	}
	else if (basePlot->Length == 0 && subPlot->Length != 0)
	{
		start = 30;
		end = 60;
	}

	for (auto plotId = start; plotId < end; plotId++)
	{
		auto exterior = Mem->GetExteriorCommonFixtures(plotId);
		if (exterior->Length == 0 || exterior[0]->FixtureType == -1) continue;

		ImGui::Text(String::Empty->Concat(plotId + 1));
		ImGui::NextColumn();

		Item^ unitedItem;

		if (Data->IsUnitedExteriorPart(exterior[0]->FixtureKey, unitedItem))
		{
			ImGui::Text(String("Walls: ").Concat(unitedItem->Name));
			if (exterior[0]->Stain != nullptr && exterior[0]->Stain->RowId != 0)
			{
				ImGui::SameLine();
				Utils::StainButton(String::Format("{0}##{1}", exterior[0]->Stain->Name, unitedItem->Name),
									exterior[0]->Stain);
			}

			ImGui::NextColumn();
			continue;
		}

		for (auto ext = 0; ext < exterior->Length; ext++)
		{
			if (exterior[ext]->FixtureKey == 0) continue;

			const auto desc = Utils::GetExteriorPartDescriptor(static_cast<ExteriorPartsType>(ext));

			ImGui::Text(String::Format("{0}: {1}", desc, exterior[ext]->Stain->RowId));
			if (exterior[ext]->Stain != nullptr && exterior[ext]->Stain->RowId != 0)
			{
				ImGui::SameLine();
				Utils::StainButton(String("##").Concat(exterior[ext]->Item->Name), exterior[ext]->Stain);
			}
		}

		ImGui::NextColumn();
	}

	ImGui::EndChild();
	RenderHousingObjectList(true);
	ImGui::EndTabItem();
}

void NativeHousemate::HousemateUI::RenderHousingObjectList(bool collapsible)
{
	if (collapsible)
	{
		if (!ImGui::CollapsingHeader("Nearby housing objects"))
		{
			_objectsOpen = false;
			return;
		}
	}
	else
	{
		ImGui::Text("Nearby housing objects");
	}
	_objectsOpen = true;

	ImGui::BeginChild("##COLUMNAPIISDUMBIHATEYOU2", Vector2(200, ImGui::GetFontSize()), false);
	ImGui::Columns(2);
	ImGui::Text("Distance");
	ImGui::SetColumnWidth(0, 61);
	ImGui::NextColumn();
	ImGui::Text("Item");
	ImGui::NextColumn();
	ImGui::SetColumnWidth(1, 300);
	ImGui::EndChild();

	ImGui::BeginChild("##housingObjects", Vector2(-1, -1), false);

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 61);
	ImGui::SetColumnWidth(1, 300);

	Nullable<Vector3>^ nPos = _pi->ClientState == nullptr
								? Nullable<Vector3>()
								: _pi->ClientState->LocalPlayer == nullptr
								? Nullable<Vector3>()
								: Nullable<Vector3>(_pi->ClientState->LocalPlayer->Position);

	if (!nPos->HasValue)
	{
		ImGui::EndChild();
		return;
	}

	List<HousingGameObject>^ dObjects;
	bool dObjectsLoaded;
	if (_configuration->SortObjectLists)
	{
		if (_configuration->SortType == SortType::Distance)
			dObjectsLoaded = Mem->TryGetSortedHousingGameObjectList(dObjects, nPos->Value);
		else
			dObjectsLoaded = Mem->TryGetNameSortedHousingGameObjectList(dObjects);
	}
	else
	{
		dObjectsLoaded = Mem->TryGetUnsortedHousingGameObjectList(dObjects);
	}

	if (!dObjectsLoaded)
	{
		ImGui::EndChild();
		return;
	}

	ImGuiNative::ImGuiListClipper_Begin(_clipper, dObjects->Count, -1.0f); // _clipper->Begin(dObjects->Count);

	while (ImGuiNative::ImGuiListClipper_Step(_clipper)) // _clipper.Step()
		for (auto i = _clipper->DisplayStart; i < _clipper->DisplayEnd; i++)
		{
			auto gameObject = dObjects[i];

			HousingYardObject^ yardObject;
			HousingFurniture^ furnitureObject;

			auto itemName = String::Empty;
			if (Data->TryGetYardObject(gameObject.housingRowId, yardObject))
				itemName = yardObject->Item->Value->Name->ToString();
			if (Data->TryGetFurniture(gameObject.housingRowId, furnitureObject))
				itemName = furnitureObject->Item->Value->Name->ToString();

			const auto distance = Utils::DistanceFromPlayer(gameObject, nPos->Value);

			ImGui::Text(String::Format("{0:F2}", distance));
			ImGui::NextColumn();
			ImGui::Text(itemName);

			Stain^ stain;

			if (gameObject.color != 0 && Data->TryGetStain(gameObject.color, stain))
			{
				ImGui::SameLine();
				Utils::StainButton(String::Format("##{0}{1}", distance, itemName), stain);
			}

			ImGui::NextColumn();
		}


	ImGui::EndChild();
}

void NativeHousemate::HousemateUI::IndoorTab()
{
	if (!ImGui::BeginTabItem("Indoors")) return;

	if (Mem->CurrentManager == nullptr || Mem->IsOutdoors())
	{
		ImGui::Text("You aren't in an indoor housing zone!");
		ImGui::EndTabItem();
		return;
	}

	if (!ImGui::CollapsingHeader("Fixtures", ImGuiTreeNodeFlags::DefaultOpen))
	{
		RenderHousingObjectList(false);
		ImGui::EndTabItem();
		return;
	}

	// Column header outside of the child
	const auto fixtureColumnWidth = 135.0f;

	ImGui::BeginChild("##COLUMNAPIISDUMBIHATEYOU1", Vector2(200, ImGui::GetFontSize()), false);
	ImGui::Columns(2);
	ImGui::Text("Type");
	ImGui::SetColumnWidth(0, fixtureColumnWidth);
	ImGui::NextColumn();
	ImGui::Text("Part");
	ImGui::NextColumn();
	ImGui::SetColumnWidth(1, 300);
	ImGui::EndChild();
	ImGui::Separator();

	ImGui::BeginChild("##fixtures", Vector2(-1, ImGui::GetFontSize() * 17.0f), false);

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, fixtureColumnWidth);
	ImGui::SetColumnWidth(1, 300);

	try
	{
		for (auto i = 0; i != sizeof IndoorAreaData::Floors / sizeof *IndoorAreaData::Floors; i++)
		{
			auto fixtures = Mem->GetInteriorCommonFixtures(i);
			if (fixtures->Length == 0) continue;
			const auto isCurrentFloor = Mem->CurrentFloor() == static_cast<InteriorFloor>(i);

			for (auto j = 0; j < sizeof IndoorFloorData::Parts / sizeof *IndoorFloorData::Parts; j++)
			{
				if (fixtures[j]->FixtureKey == -1 || fixtures[j]->FixtureKey == 0) continue;
				const auto fixtureName = Utils::GetInteriorPartDescriptor(static_cast<InteriorPartsType>(j));

				auto color = isCurrentFloor ? White : Gray;

				ImGui::TextColored(
					color,
					String::Format("{0} {1}", Utils::GetFloorDescriptor(static_cast<InteriorFloor>(i)), fixtureName));
				ImGui::NextColumn();
				ImGui::TextColored(color, fixtures[j]->Item->Name);
				ImGui::NextColumn();
			}

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Columns(2);
		}

		ImGui::Columns(1);
		ImGui::Text("Light level: {Mem.GetInteriorLightLevel()}");
	}
	catch (Exception^ e)
	{
		PluginLog::Log(e, "hey");
	}

	ImGui::EndChild();
	ImGui::Separator();

	RenderHousingObjectList(false);

	ImGui::EndTabItem();
}

void NativeHousemate::HousemateUI::SettingsTab()
{
	if (!ImGui::BeginTabItem("Settings"))
	{
		return;
	}

	auto render = _configuration->Render;
	auto renderDistance = _configuration->RenderDistance;
	auto sortedObjects = _configuration->SortObjectLists;
	auto sortType = _configuration->SortType;

	if (ImGui::Checkbox("Display housing object overlay", render))
	{
		_configuration->Render = render;
	}

	if (render && ImGui::SliderFloat("View distance", renderDistance, 0, 100))
	{
		_configuration->RenderDistance = renderDistance;
	}

	if (ImGui::Checkbox("Sort housing object lists", sortedObjects))
	{
		_configuration->SortObjectLists = sortedObjects;
	}

	if (sortedObjects)
	{
		ImGui::Text("Sort objects by:");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##sortCombo", sortType.ToString()))
		{
			if (ImGui::Selectable(SortType::Distance.ToString()))
			{
				_configuration->SortType = SortType::Distance;
			}
			if (ImGui::Selectable(SortType::Name.ToString()))
			{
				_configuration->SortType = SortType::Name;
			}
			ImGui::EndCombo();
		}
	}

	if (ImGui::Button("Save"))
	{
		_configuration->Save();
	}

	ImGui::EndTabItem();
}

void NativeHousemate::HousemateUI::Draw()
{
	if (_configuration->Render)
	{
		Render(_configuration->RenderDistance);
		PlacardRender(_configuration->RenderDistance);
	}

	DrawMainWindow();
}

NativeHousemate::HousemateUI::HousemateUI(Configuration^ configuration, DalamudPluginInterface^ pi)
{
	_pi = pi;
	_configuration = configuration;

	_clipper = new ImGuiListClipper();
}

NativeHousemate::HousemateUI::~HousemateUI()
{
	if (_clipper)
	{
		delete _clipper;
	}
}

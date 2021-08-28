#pragma once

#include "pch.h"

#include "Structs.h"

using namespace System::Linq;
using namespace System::Numerics;
using namespace System::Collections::Generic;

namespace NativeHousemate
{
	public ref class HousingMemory
	{
		HousingMemory(Dalamud::Game::SigScanner^ scanner);

		HousingModule* HousingModule;
		LayoutWorld* LayoutWorld;

		static HousingMemory^ _instance;

	public:
		property static HousingMemory^ Instance
		{
		public:
			HousingMemory^ get()
			{
				return _instance;
			}

		private:
			void set(HousingMemory^ value)
			{
				_instance = value;
			}
		}

		property HousingObjectManager* CurrentManager
		{
			HousingObjectManager* get()
			{
				return HousingModule->currentTerritory;
			}
		}

		static void Init(Dalamud::Game::SigScanner^ scanner);
		InteriorFloor CurrentFloor();
		uint32_t GetTerritoryTypeId();
		float GetInteriorLightLevel();
		array<CommonFixture^>^ GetInteriorCommonFixtures(int32_t floorId);
		array<CommonFixture^>^ GetExteriorCommonFixtures(int32_t plotId);
		bool TryGetHousingGameObject(int32_t index, [Out] HousingGameObject% gameObject);
		bool TryGetSortedHousingGameObjectList([Out] List<HousingGameObject>^% objects, Vector3 playerPos);
		bool TryGetNameSortedHousingGameObjectList([Out] List<HousingGameObject>^% objects);
		bool TryGetUnsortedHousingGameObjectList([Out] List<HousingGameObject>^% objects);
		HousingController* GetHousingController();
		bool IsOutdoors();
		bool IsIndoors();
	};
}

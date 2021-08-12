#pragma once

#include "pch.h"
#include "HousingLandSet.h"

using namespace System::Runtime::InteropServices;
using namespace Lumina::Excel::GeneratedSheets;

namespace NativeHousemate
{
	public enum class SortType
	{
		Distance,
		Name
	};

	public enum class ExteriorPartsType
	{
		None = -1,
		Roof = 0,
		Walls,
		Windows,
		Door,
		RoofOpt,
		WallOpt,
		SignOpt,
		Fence
	};

	public enum class InteriorFloor
	{
		None = -1,
		Ground = 0,
		Upstairs,
		Basement,
		External
	};

	public enum class InteriorPartsType
	{
		None = -1,
		Walls,
		Windows,
		Door,
		Floor,
		Light
	};

	public ref struct CommonLandSet
    {
        uint32_t LandRange;
        uint32_t PlacardId;

        uint32_t UnknownRange1;

        uint32_t InitialPrice;
        uint8_t Size;
        int32_t PlotIndex;

        static CommonLandSet^ FromExd(HousingLandSet::LandSet lset, int index)
        {
			const auto ret = gcnew CommonLandSet();
            ret->LandRange = lset.LandRange;
            ret->PlacardId = lset.PlacardId;
            ret->UnknownRange1 = lset.UnknownRange1;
            ret->InitialPrice = lset.InitialPrice;
            ret->Size = lset.Size;
            ret->PlotIndex = index;
            return ret;
        }

        String^ SizeString()
        {
			switch (this->Size)
			{
				case 0:
					return gcnew String("small");
				case 1:
					return gcnew String("medium");
				case 2:
					return gcnew String("large");
				default:
					return gcnew String("unknown");
			}
        }
    };

	public ref struct CommonFixture
	{
		bool IsExterior;
		int FixtureType;
		int FixtureKey;
		Stain^ Stain;
		Item^ Item;

		CommonFixture(bool isExterior, int fixtureType, int fixtureKey, GeneratedSheets::Stain^ stain, GeneratedSheets::Item^ item)
		{
			this->IsExterior = isExterior;
			this->FixtureType = fixtureType;
			this->FixtureKey = fixtureKey;
			this->Stain = stain;
			this->Item = item;
		}
	};

	template<typename T, int size>
	[System::Runtime::CompilerServices::UnsafeValueType]
	[System::Runtime::InteropServices::StructLayout
		(
		System::Runtime::InteropServices::LayoutKind::Explicit,
		Size=(sizeof(T)*size)
		)
	]
	public value struct inline_array {
	private:
		[System::Runtime::InteropServices::FieldOffset(0)]
		T dummy_item;

	public:
		T% operator[](int index) {
			return *((&dummy_item)+index);
		}

		static operator interior_ptr<T>(inline_array<T,size>% ia) {
			return &ia.dummy_item;
		}
	};

	ref class R {
		inline_array<int, 10> arr;
	};

	[StructLayout(LayoutKind::Explicit)]
	public value struct HousingGameObject
	{
		[FieldOffset(0x30)] inline_array<uint8_t, 64> name;
		[FieldOffset(0x080)] uint32_t housingRowId;
		[FieldOffset(0x0A0)] float X;
		[FieldOffset(0x0A4)] float Y;
		[FieldOffset(0x0A8)] float Z;
		[FieldOffset(0x0B0)] float rotation;
		[FieldOffset(0x1A8)] uint32_t housingRowId2;
		[FieldOffset(0x1B0)] uint8_t color;
	};

	public struct NativeHousingGameObject
	{
		uint8_t unk1[0x30];
		uint8_t name[64];
		uint8_t unk2[0x10];
		uint32_t housingRowId;
		uint8_t unk3[0x1C];
		float X;
		float Y;
		float Z;
		uint32_t unk4;
		float rotation;
		uint8_t unk5[0xF4];
		uint32_t housingRowId2;
		uint32_t unk6;
		uint8_t color;
	};

	public struct HousingObjectManager
	{
		uint8_t unk1[0x8980];
		void* objects[400];
	};

	public struct HousingModule
	{
		HousingObjectManager* currentTerritory;
		HousingObjectManager* outdoorTerritory;
		HousingObjectManager* indoorTerritory;

		HousingObjectManager* GetCurrentManager()
		{
			return outdoorTerritory != nullptr ? outdoorTerritory : indoorTerritory;
		}

		bool IsOutdoors()
		{
			return outdoorTerritory != nullptr;
		}

		bool IsIndoors()
		{
			return indoorTerritory != nullptr;
		}

		InteriorFloor CurrentFloor()
		{
			if (indoorTerritory == nullptr)
			{
				return InteriorFloor::None;
			}

			const auto gameFloor = *reinterpret_cast<int32_t*>(reinterpret_cast<int8_t*>(indoorTerritory) + 0x9704);

			switch (gameFloor)
			{
				case 0:
					return InteriorFloor::Ground;
				case 1:
					return InteriorFloor::Upstairs;
				case 10:
					return InteriorFloor::Basement;
				default:
					return InteriorFloor::None;
			}
		}
	};

	public struct HousePart
	{
		uint32_t Category;
		int32_t Unknown1;
		uint16_t FixtureKey;
		uint8_t Color : sizeof(uint16_t);
		int32_t Unknown2;
		void* Unknown3;
		uint8_t Padding[12];
	};

	public struct HouseCustomize
	{
		uint8_t unk1[0x10];
		int32_t Size;
		uint8_t unk2[0xC];
		HousePart Parts[8];
		uint8_t unk3[0x70];
	};

	public struct HousingController
	{
		uint8_t unk1[0x8];
		uint32_t AreaType;
		uint8_t unk2[0x1E4];
		HouseCustomize Houses[60];
	};

	public struct IndoorFloorData
	{
		int32_t Parts[5];
	};

	public struct IndoorAreaData
	{
		uint8_t unk1[0x28];
		IndoorFloorData Floors[4];
		float LightLevel;
	};

	public struct LayoutManager
	{
		uint8_t unk1[0x20];
		uint32_t TerritoryTypeId;
		uint8_t unk2[0x5C];
		HousingController* HousingController;
		uint8_t unk3[0xC];
		IndoorAreaData* IndoorAreaData;
	};

	public struct LayoutWorld
	{
		uint8_t unk1[0x20];
		LayoutManager* ActiveLayout;
	};
}

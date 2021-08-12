#pragma once
#include "pch.h"

using namespace Lumina;
using namespace Lumina::Data;
using namespace Lumina::Excel;

namespace NativeHousemate
{
	[SheetAttribute("HousingLandSet")]
	public ref class HousingLandSet : ExcelRow
	{
	public:
		value struct LandSet
		{
			uint32_t LandRange;
			uint32_t PlacardId;
			uint32_t UnknownRange1;
			uint32_t InitialPrice;
			uint32_t Size;
		};

		array<LandSet>^ LandSets;

	private:
		uint32_t _unknownRange;
		uint32_t _unknownRange2;

	public:
		property uint32_t UnknownRange
		{
		public:
			uint32_t get() { return _unknownRange; };
		private:
			void set(uint32_t value) { _unknownRange = value; };
		};

		property uint32_t UnknownRange2
		{
		public:
			uint32_t get() { return _unknownRange2; };
		private:
			void set(uint32_t value) { _unknownRange2 = value; };
		};

		void PopulateData(RowParser^ parser, GameData^ data, Language language) override;
	};
}

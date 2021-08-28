#pragma once

#include "pch.h"
#include "Structs.h"

using namespace System::IO;
using namespace System::Collections::Generic;
using namespace Newtonsoft::Json;
using namespace Lumina::Excel::GeneratedSheets;
using namespace System::Runtime::InteropServices;

namespace NativeHousemate
{
	public ref class HousingData
	{
		initonly Dictionary<uint32_t, HousingFurniture^>^ _furnitureDict;
		initonly Dictionary<uint32_t, Item^>^ _itemDict;
		initonly Dictionary<uint32_t, Stain^>^ _stainDict;

		initonly Dictionary<uint32_t, Dictionary<uint32_t, CommonLandSet^>^>^ _territoryToLandSetDict;
		initonly Dictionary<uint32_t, uint32_t>^ _unitedDict;
		initonly Dictionary<uint32_t, HousingYardObject^>^ _yardObjectDict;

		HousingData(Dalamud::Data::DataManager^ dataManager);

		static HousingData^ _instance;

	public:
		property static HousingData^ Instance
		{
		public:
			HousingData^ get()
			{
				return _instance;
			}

		private:
			void set(HousingData^ value)
			{
				_instance = value;
			}
		}

		static void Init(Dalamud::Data::DataManager^ data);
		bool TryGetYardObject(uint32_t id, [Out] HousingYardObject^% yardObject);
		bool TryGetFurniture(uint32_t id, [Out] HousingFurniture^% furniture);
		bool TryGetLandSetDict(uint32_t id, [Out] Dictionary<uint32_t, CommonLandSet^>^% dict);
		bool TryGetItem(uint32_t id, [Out] Item^% item);
		bool TryGetStain(uint32_t id, [Out] Stain^% stain);
		bool IsUnitedExteriorPart(uint32_t id, [Out] Item^% item);
	};
}

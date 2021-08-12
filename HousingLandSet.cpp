#include "pch.h"
#include "HousingLandSet.h"

void NativeHousemate::HousingLandSet::PopulateData(RowParser^ parser, GameData^ data, Language language)
{
	this->RowId = parser->Row;
	this->SubRowId = parser->SubRow;

	this->LandSets = parser->ReadStructuresAsArray<LandSet>(0, 60);
	this->UnknownRange = parser->ReadColumn<uint32_t>(300);
	this->UnknownRange2 = parser->ReadColumn<uint32_t>(301);
}

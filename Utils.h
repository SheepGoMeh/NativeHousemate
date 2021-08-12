#pragma once

#include "pch.h"
#include "Structs.h"

using namespace ImGuiNET;
using namespace System::Numerics;
using namespace Lumina::Excel::GeneratedSheets;

namespace NativeHousemate
{
	public ref class Utils
	{
		static bool Collides(Vector2 origin, Vector2 bounds, Vector2 pos)
		{
			return pos.X > origin.X && pos.X < bounds.X &&
				pos.Y > origin.Y && pos.Y < bounds.Y;
		}

		static void ColorTooltip(String^ text, Vector4 color)
		{
			ImGui::BeginTooltip();
			auto size = Vector2(ImGui::GetFontSize() * 4 + ImGui::GetStyle().FramePadding.Y * 2,
				ImGui::GetFontSize() * 4 + ImGui::GetStyle().FramePadding.Y * 2);
			auto cr = static_cast<int>(color.X * 255);
			auto cg = static_cast<int>(color.Y * 255);
			auto cb = static_cast<int>(color.Z * 255);
			auto ca = static_cast<int>(color.W * 255);
			ImGui::ColorButton("##preview", color, ImGuiColorEditFlags::None, size);
			ImGui::SameLine();
			ImGui::Text(String::Format(
				"{0}\n" +
				"#{1:X2}{2:X2}{3:X2}{4:X2}\n" +
				"R: {1}, G: {2}, B: {3}, A: {4}\n" +
				"({5:F3}, {6:F3}, {7:F3}, {8:F3})",
				text, cr, cb, cg, ca, color.X, color.Y, color.Z, color.W));
			ImGui::EndTooltip();
		}
		
		static Vector4 StainToVector4(uint32_t stainColor)
		{
			const auto s = 1.0f / 255.0f;

			return Vector4(((stainColor >> 16) & 0xFF) * s, ((stainColor >> 8) & 0xFF) * s,
							((stainColor >> 0) & 0xFF) * s, ((stainColor >> 24) & 0xFF) * s);
		}

	public:
		static String^ GetExteriorPartDescriptor(ExteriorPartsType partsType)
		{
			switch (partsType)
			{
			case ExteriorPartsType::None: return "None";
			case ExteriorPartsType::Roof: return "Roof";
			case ExteriorPartsType::Walls: return "Walls";
			case ExteriorPartsType::Windows: return "Windows";
			case ExteriorPartsType::Door: return "Door";
			case ExteriorPartsType::RoofOpt: return "Roof (opt)";
			case ExteriorPartsType::WallOpt: return "Wall (opt)";
			case ExteriorPartsType::SignOpt: return "Signboard (opt)";
			case ExteriorPartsType::Fence: return "Fence";
			default: return "Unknown";
			}
		}

		static String^ GetInteriorPartDescriptor(InteriorPartsType partsType)
		{
			switch (partsType)
			{
			case InteriorPartsType::None: return "None";
			case InteriorPartsType::Walls: return "Walls";
			case InteriorPartsType::Windows: return "Windows";
			case InteriorPartsType::Door: return "Door";
			case InteriorPartsType::Floor: return "Floor";
			case InteriorPartsType::Light: return "Light";
			default: return "Unknown";
			}
		}

		static String^ GetFloorDescriptor(InteriorFloor floor)
		{
			switch (floor)
			{
			case InteriorFloor::None: return "None";
			case InteriorFloor::Ground: return "Ground Floor";
			case InteriorFloor::Upstairs: return "2nd Floor";
			case InteriorFloor::Basement: return "Basement Floor";
			case InteriorFloor::External: return "Main";
			default: return "Unknown";
			}
		}

		static float DistanceFromPlayer(HousingGameObject obj, Vector3 playerPos)
		{
			return Vector3::Distance(playerPos, Vector3(obj.X, obj.Y, obj.Z));
		}

		static void StainButton(String^ id, Stain^ color, Vector2 size)
		{
			auto mousePos = ImGui::GetIO().MousePos;
			auto floatColor = StainToVector4(color->Color);
			auto topLeft = ImGui::GetCursorScreenPos();
			ImGui::ColorButton(String("##").Concat(id), floatColor, ImGuiColorEditFlags::NoTooltip, size);
			const auto buttonVisible = ImGui::IsItemVisible();
			const auto bottom = ImGui::GetCursorScreenPos().Y;
			ImGui::SameLine();
			const auto right = ImGui::GetCursorScreenPos().X;
			ImGui::Text("");

			if (buttonVisible && Collides(topLeft, Vector2(right, bottom), mousePos))
			{
				ColorTooltip(color->Name, floatColor);
			}
		}

		static void StainButton(String^ id, Stain^ color)
		{
			auto size = Vector2(ImGui::GetFontSize());
			StainButton(id, color, size);
		}

		static float GlobalFontScale()
		{
			return ImGui::GetIO().FontGlobalScale;
		}
	};
}
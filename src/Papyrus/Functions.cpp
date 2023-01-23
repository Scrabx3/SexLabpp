#include "Papyrus/Functions.h"

#include "Papyrus/Settings.h"

std::vector<std::string> SLPP::MergeStringArrayEx(RE::StaticFunctionTag*, std::vector<std::string> a_array1, std::vector<std::string> a_array2, bool a_removedupes)
{
	if (a_array1.empty()) {
		return a_array2;
	}
	decltype(a_array1) ret{ a_array1 };
	for (auto&& str : a_array2) {
		if (a_removedupes && [&ret, &str]() -> bool {
					for (auto&& s : ret) {
						if (SexLab::IsEqualString(str, s)) {
							return true;
						}
					}
					return false;
				}()) {
			continue;
		}
		ret.push_back(str);
	}
	return ret;
}

std::vector<std::string> SLPP::RemoveStringEx(RE::StaticFunctionTag*, std::vector<std::string> a_array, std::string a_remove)
{
	decltype(a_array) ret{ a_array };
	const auto where = std::remove_if(ret.begin(), ret.end(), [&a_remove](auto& str) { return SexLab::IsEqualString(str, a_remove); });
	ret.erase(where, ret.end());
	return ret;
}

void SLPP::SetPositions(VM* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_positions, RE::TESObjectREFR* a_center)
{
	if (!a_center) {
		a_vm->TraceStack("Cannot center Actors around a none Reference", a_stackID);
		return;
	} else if (a_positions.empty()) {
		a_vm->TraceStack("Empty Array. Nothing to do", a_stackID);
		return;
	}
	const auto centerPos = a_center->GetPosition();
	const auto centerAng = a_center->GetAngle();

	for (auto&& subject : a_positions) {
		subject->data.angle = centerAng;
		subject->SetPosition(centerPos, true);
		subject->Update3DPosition(true);
	}

	// const auto setposition = [centerAng, centerPos](RE::Actor* actor) {
	// 	for (size_t i = 0; i < 6; i++) {
	// 		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	// 		actor->data.angle.z = centerAng.z;
	// 		actor->SetPosition(centerPos, false);
	// 	}
	// };
	// std::for_each(a_positions.begin(), a_positions.end(), [&setposition](RE::Actor* subject) { std::thread(setposition, subject).detach(); });
}

void SLPP::SetPositionsEx(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_refs, RE::TESObjectREFR* a_center, std::vector<float> a_offsets)
{
	if (!a_center) {
		a_vm->TraceStack("Cannot center Actors around a none Reference", a_stackID);
		return;
	} else if (a_refs.empty() || std::find(a_refs.begin(), a_refs.end(), nullptr) != a_refs.end()) {
		a_vm->TraceStack("Empty reference array. Nothing to do", a_stackID);
		return;
	} else if (a_refs.size() * 4 != a_offsets.size()) {
		a_vm->TraceStack("Offsets needs to be a mx4 matrix", a_stackID);
		return;
	}
	const auto centerPos = a_center->GetPosition();
	const auto centerAng = [&a_center]() { auto ret = a_center->GetAngle(); ret.z += static_cast<float>(std::_Pi); return ret; }();

	std::vector<RE::NiPoint3> positions{}, angles{};
	positions.reserve(a_refs.size());
	angles.reserve(a_refs.size());
	for (size_t i = 0; i < a_refs.size(); i++) {
		RE::NiPoint3 pos{ centerPos }, ang{ centerAng };
		const auto forward = a_offsets[i * 4 + 0];
		pos.x += forward * sin(ang.z);
		pos.y += forward * cos(ang.z);

		const auto side = a_offsets[i * 4 + 1];
		pos.x += side * cos(ang.z);
		pos.y += side * sin(ang.z);

		pos.z += a_offsets[i * 4 + 2];
		ang.z += a_offsets[i * 4 + 3] * static_cast<float>(std::_Pi / 180.0);
		positions.push_back(pos);
		angles.push_back(ang);
	}
	for (size_t i = 0; i < a_refs.size(); i++) {
		a_refs[i]->data.angle = angles[i];
		a_refs[i]->SetPosition(positions[i], true);
		a_refs[i]->Update3DPosition(true);
	}

	// for (size_t i = 0; i < a_refs.size(); i++) {
	// 	std::thread(
	// 			[](RE::Actor* a_ref, const RE::NiPoint3 a_position, const RE::NiPoint3 a_angle) {
	// 				for (size_t i = 0; i < 6; i++) {
	// 					std::this_thread::sleep_for(std::chrono::milliseconds(300));
	// 					a_ref->data.angle.z = a_angle.z;
	// 					a_ref->SetPosition(a_position, false);
	// 				}
	// 			},
	// 			a_refs[i], positions[i], angles[i])
	// 			.detach();
	// }
}


bool SLPP::MatchTags(RE::StaticFunctionTag*, std::vector<std::string_view> a_tags, std::vector<std::string_view> a_match)
{
	enum
	{
		Unmatched,	// has optional tags, but all are invalid
		Matched,		// has optional tags with at least of them matching
		Undefined		// No optional tags
	};
	auto match = Undefined;
	for (auto&& tag : a_match) {
		if (tag.empty())
			continue;

		switch (tag[0]) {
		case '~':
			if (match != Matched) {
				const auto& find = tag.substr(1);
				const auto where = std::find_if(a_tags.begin(), a_tags.end(),
					[&find](auto& str) { return SexLab::IsEqualString(find, str); });
				match = where != a_tags.end() ? Matched : Unmatched;
			}
			break;
		case '-':
			{
				const auto& find = tag.substr(1);
				const auto where = std::find_if(a_tags.begin(), a_tags.end(),
					[&find](auto& str) { return SexLab::IsEqualString(find, str); });
				if (where != a_tags.end())
					return false;
			}
			break;
		default:
			{
				const auto where = std::find_if(a_tags.begin(), a_tags.end(),
					[&tag](auto& str) { return SexLab::IsEqualString(tag, str); });
				if (where == a_tags.end())
					return false;
			}
			break;
		}
	}
	return match != Unmatched;
}

std::vector<RE::TESObjectREFR*> SLPP::FindBeds(VM* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::TESObjectREFR* a_center, float a_radius, float a_radiusz)
{
	if (!a_center) {
		a_vm->TraceStack("CenterRef is none", a_stackID);
		return {};
	}
	const auto center = a_center->GetPosition();
	std::vector<RE::TESObjectREFR*> ret{};
	const auto add = [&](RE::TESObjectREFR& ref) {
		if (!ref.GetBaseObject()->Is(RE::FormType::Furniture) && a_radiusz > 0.0f ? (std::fabs(center.z - ref.GetPosition().z) <= a_radiusz) : true)
			if (SexLab::GetIsBed(&ref))
				ret.push_back(&ref);
		return RE::BSContainer::ForEachResult::kContinue;
	};
	const auto TES = RE::TES::GetSingleton();
	if (const auto interior = TES->interiorCell; interior) {
		interior->ForEachReferenceInRange(center, a_radius, add);
	} else if (const auto grids = TES->gridCells; grids) {
		// Derived from: https://github.com/powerof3/PapyrusExtenderSSE
		auto gridLength = grids->length;
		if (gridLength > 0) {
			float yPlus = center.y + a_radius;
			float yMinus = center.y - a_radius;
			float xPlus = center.x + a_radius;
			float xMinus = center.x - a_radius;
			for (uint32_t x = 0, y = 0; (x < gridLength && y < gridLength); x++, y++) {
				const auto gridcell = grids->GetCell(x, y);
				if (gridcell && gridcell->IsAttached()) {
					auto cellCoords = gridcell->GetCoordinates();
					if (!cellCoords)
						continue;
					float worldX = cellCoords->worldX;
					float worldY = cellCoords->worldY;
					if (worldX < xPlus && (worldX + 4096.0) > xMinus && worldY < yPlus && (worldY + 4096.0) > yMinus) {
						gridcell->ForEachReferenceInRange(center, a_radius, add);
					}
				}
			}
		}
		if (!ret.empty()) {
			std::sort(ret.begin(), ret.end(), [&](RE::TESObjectREFR* a_refA, RE::TESObjectREFR* a_refB) {
				return center.GetDistance(a_refA->GetPosition()) < center.GetDistance(a_refB->GetPosition());
			});
		}
	}
	return ret;
}

bool SLPP::IsBed(VM* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::TESObjectREFR* a_reference)
{
	if (!a_reference) {
		a_vm->TraceStack("Reference is none", a_stackID);
		return false;
	}
	return SexLab::GetIsBed(a_reference);
}

RE::TESAmmo* SLPP::GetEquippedAmmo(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
{
	if (!a_actor) {
		a_vm->TraceStack("Cannot get equipped ammo of a none reference", a_stackID);
		return nullptr;
	}
	return a_actor->GetCurrentAmmo();
}

RE::SpellItem* SLPP::GetHDTHeelSpell(VM* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_reference)
{
	if (!a_reference) {
		a_vm->TraceStack("Cannot retrieve hdt spell from a none reference", a_stackID);
		return nullptr;
	}
	static const auto* const heeleffect = RE::TESDataHandler::GetSingleton()->LookupForm<RE::EffectSetting>(0x800, "hdtHighHeel.esm"sv);
	if (!heeleffect)
		return nullptr;

	for (const auto& spell : a_reference->GetActorRuntimeData().addedSpells) {
		if (!spell || spell->effects.empty())
			continue;

		for (const auto& effect : spell->effects) {
			if (effect && effect->baseEffect == heeleffect)
				return spell;
		}
	}
	return nullptr;
}


std::vector<RE::TESForm*> SLPP::StripActor(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_reference, uint32_t a_slotmask)
{
	if (!a_reference) {
		a_vm->TraceStack("Cannot retrieve hdt spell from a none reference", a_stackID);
		return {};
	}
	std::vector<RE::TESForm*> ret{};
	const auto& manager = RE::ActorEquipManager::GetSingleton();
	const auto cstrip = Settings::StripConfig::GetSingleton();
	const auto& inventory = a_reference->GetInventory();
	for (const auto& [form, data] : inventory) {
		if (form->Is(RE::FormType::LeveledItem) || !data.second->IsWorn() || !form->GetPlayable() || form->GetName()[0] == '\0') {
			continue;
		}
		const auto strip = [&]() {
			manager->UnequipObject(a_reference, form);
			ret.push_back(form);
		};
		switch (cstrip->CheckStrip(form)) {
		case Settings::StripConfig::Strip::NoStrip:
			continue;
		case Settings::StripConfig::Strip::Always:
			strip();
			continue;
		}
		const auto& biped = form->As<RE::BGSBipedObjectForm>();
		if (biped) {
			const auto& slots = static_cast<uint32_t>(biped->GetSlotMask());
			if (slots & a_slotmask) {
				strip();
				continue;
			}
		}
	}
	return ret;
}

void SLPP::WriteStrip(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_form, bool a_neverstrip)
{
	if (!a_form) {
		a_vm->TraceStack("Cannot write a none reference", a_stackID);
		return;
	}
	const auto strip = a_neverstrip ? Settings::StripConfig::Strip::NoStrip : Settings::StripConfig::Strip::Always;
	Settings::StripConfig::GetSingleton()->AddArmor(a_form, strip);
}

void SLPP::EraseStrip(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_form)
{
	if (!a_form) {
		a_vm->TraceStack("Cannot erase a none reference", a_stackID);
		return;
	}
	Settings::StripConfig::GetSingleton()->RemoveArmor(a_form);
}

void SLPP::EraseStripAll(RE::StaticFunctionTag*)
{
	Settings::StripConfig::GetSingleton()->RemoveArmorAll();
}

int32_t SLPP::CheckStrip(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_form)
{
	if (!a_form) {
		a_vm->TraceStack("Cannot check a none reference", a_stackID);
		return 0;
	}
	const auto s = Settings::StripConfig::GetSingleton()->CheckStrip(a_form);
	switch (s) {
	case Settings::StripConfig::Strip::NoStrip:
		return -1;
	case Settings::StripConfig::Strip::Always:
		return 1;
	default:
		return 0;
	}
}

std::vector<RE::TESForm*> SLPP::GetStrippables(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_reference, bool a_wornonly)
{
	if (!a_reference) {
		a_vm->TraceStack("Cannot retrieve hdt spell from a none reference", a_stackID);
		return {};
	}
	std::vector<RE::TESForm*> ret{};
	const auto cstrip = Settings::StripConfig::GetSingleton();
	const auto& inventory = a_reference->GetInventory();
	for (const auto& [form, data] : inventory) {
		if (!form->IsArmor() && !form->IsWeapon() || a_wornonly && !data.second->IsWorn())
			continue;
		if (cstrip->CheckKeywords(form) != Settings::StripConfig::Strip::None)
			continue;

		ret.push_back(form);
	}
	return ret;
}

std::string SLPP::GetEditorID(VM* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_form)
{
	if (!a_form) {
		a_vm->TraceStack("Cannot retrieve editor ID. Form is none", a_stackID);
		return ""s;
	}
	const auto ret = a_form->GetFormEditorID();
	return ret ? ret : ""s;
}

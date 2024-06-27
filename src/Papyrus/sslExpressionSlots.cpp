#include "sslExpressionSlots.h"

#include "Registry/Expression.h"

namespace Papyrus::ExpressionSlots
{
	namespace BaseExpression
	{
		float GetModifier(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, uint32_t a_id)
		{
			if (!a_actor) {
				a_vm->TraceStack("Actor is none", a_stackID);
				return 0.0f;
			}
			const auto data = a_actor->GetFaceGenAnimationData();
			if (!data)
				return 0.0f;
			const auto& keyframe = data->modifierKeyFrame;
			return a_id < keyframe.count ? keyframe.values[a_id] : 0.0f;
		}

		float GetPhoneme(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, uint32_t a_id)
		{
			if (!a_actor) {
				a_vm->TraceStack("Actor is none", a_stackID);
				return 0.0f;
			}
			const auto data = a_actor->GetFaceGenAnimationData();
			if (!data)
				return 0.0f;
			const auto& keyframe = data->phenomeKeyFrame;
			return a_id < keyframe.count ? keyframe.values[a_id] : 0.0f;
		}

		float GetExpression(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, bool a_getid)
		{
			if (!a_actor) {
				a_vm->TraceStack("Actor is none", a_stackID);
				return 0.0f;
			}
			const auto data = a_actor->GetFaceGenAnimationData();
			if (!data)
				return 0.0f;
			const auto& keyframe = data->expressionKeyFrame;
			for (size_t i = 0; i < keyframe.count; i++) {
				if (keyframe.values[i] > 0.0f) {
					return a_getid ? i : keyframe.values[i];
				}
			}
			return 0.0f;
		}

		int GetVersion(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
		{
			auto profile = Registry::Expression::GetSingleton()->GetProfile(a_id);
			if (!profile) {
				a_vm->TraceStack("Invalid Expression Profile ID", a_stackID);
				return {};
			}
			return profile->version;
		}

		std::vector<RE::BSFixedString> GetExpressionTags(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
		{
			auto profile = Registry::Expression::GetSingleton()->GetProfile(a_id);
			if (!profile) {
				a_vm->TraceStack("Invalid Expression Profile ID", a_stackID);
				return {};
			}
			return profile->tags.AsVector();
		}

		void SetExpressionTags(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, std::vector<RE::BSFixedString> a_newtags)
		{
			auto expr = Registry::Expression::GetSingleton();
			if (!expr->GetProfile(a_id)) {
				a_vm->TraceStack("Invalid Expression Profile ID", a_stackID);
				return;
			}
			expr->UpdateTags(a_id, { a_newtags });
		}

		bool GetEnabled(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
		{
			auto profile = Registry::Expression::GetSingleton()->GetProfile(a_id);
			if (!profile) {
				a_vm->TraceStack("Invalid Expression Profile ID", a_stackID);
				return false;
			}
			return profile->enabled;
		}

		void SetEnabled(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, bool a_enabled)
		{
			auto expr = Registry::Expression::GetSingleton();
			if (!expr->GetProfile(a_id)) {
				a_vm->TraceStack("Invalid Expression Profile ID", a_stackID);
				return;
			}
			expr->SetEnabled(a_id, a_enabled);
		}

		int GetExpressionScaleMode(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
		{
			auto profile = Registry::Expression::GetSingleton()->GetProfile(a_id);
			if (!profile) {
				a_vm->TraceStack("Invalid Expression Profile ID", a_stackID);
				return false;
			}
			return static_cast<int>(profile->scaling);
		}

		void SetExpressionScaleMode(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, int a_idx)
		{
			auto expr = Registry::Expression::GetSingleton();
			if (!expr->GetProfile(a_id)) {
				a_vm->TraceStack("Invalid Expression Profile ID", a_stackID);
				return;
			}
			if (a_idx < 0 || a_idx >= static_cast<int>(Registry::Expression::Profile::Scaling::Total)) {
				a_vm->TraceStack("Invalid Scaling Index", a_stackID);
				return;
			}
			expr->SetScaling(a_id, Registry::Expression::Profile::Scaling(a_idx));
		}

		std::vector<int32_t> GetLevelCounts(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
		{
			auto profile = Registry::Expression::GetSingleton()->GetProfile(a_id);
			if (!profile) {
				a_vm->TraceStack("Invalid Expression Profile ID", a_stackID);
				return { 0, 0 };
			}
			return { static_cast<int32_t>(profile->data[RE::SEXES::kMale].size()), static_cast<int32_t>(profile->data[RE::SEXES::kFemale].size()) };
		}

		std::vector<float> GetNthValues(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, bool a_female, int n)
		{
			auto profile = Registry::Expression::GetSingleton()->GetProfile(a_id);
			if (!profile) {
				a_vm->TraceStack("Invalid Expression Profile ID", a_stackID);
				return std::vector<float>(Registry::Expression::Profile::Total);
			}
			auto& ret = profile->data[a_female];
			if (ret.size() <= n) {
				a_vm->TraceStack("Invalid level", a_stackID);
				return std::vector<float>(Registry::Expression::Profile::Total);
			}
			return { ret[n].begin(), ret[n].end() };
		}

		std::vector<float> GetValues(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, bool a_female, float a_strength)
		{
			auto profile = Registry::Expression::GetSingleton()->GetProfile(a_id);
			if (!profile) {
				a_vm->TraceStack("Invalid Expression Profile ID", a_stackID);
				return std::vector<float>(Registry::Expression::Profile::Total);
			}
			auto ret = profile->GetData(a_female ? RE::SEXES::kFemale : RE::SEXES::kMale, a_strength);
			return { ret.begin(), ret.end() };
		}

		void SetValues(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, bool a_female, int a_level, std::vector<float> a_values)
		{
			if (a_values.size() != Registry::Expression::Profile::Total) {
				a_vm->TraceStack("Invalid Value Size", a_stackID);
				return;
			}
			auto expr = Registry::Expression::GetSingleton();
			if (!expr->GetProfile(a_id)) {
				a_vm->TraceStack("Invalid Expression Profile ID", a_stackID);
				return;
			}
			expr->UpdateValues(a_id, a_female, a_level, a_values);
		}

		bool CreateEmptyProfile(RE::StaticFunctionTag*, RE::BSFixedString a_id)
		{
			return Registry::Expression::GetSingleton()->CreateProfile(a_id);
		}

		void SaveExpression(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
		{
			auto p = Registry::Expression::GetSingleton()->GetProfile(a_id);
			if (!p) {
				a_vm->TraceStack("Invalid Profile", a_stackID);
				return;
			}
			p->Save();
		}
	}	 // namespace BaseExpression

	std::vector<RE::BSFixedString> GetAllProfileIDs(RE::StaticFunctionTag*)
  {
		std::vector<RE::BSFixedString> ret{};
		Registry::Expression::GetSingleton()->ForEachProfile([&](const Registry::Expression::Profile& profile) {
			ret.push_back(profile.id);
      return false;
		});
    return ret;
	}
  
	std::vector<RE::BSFixedString> GetExpressionsByStatus(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int a_status)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return {};
		}
		std::vector<RE::BSFixedString> ret{};
		RE::BSFixedString tag;
		switch (a_status) {
		case ExpressionStatus::Submissive:
			tag = "Victim";
			break;
		case ExpressionStatus::Dominant:
			tag = "Aggressor";
			break;
		default:
			tag = "Normal";
			break;
		}
		Registry::Expression::GetSingleton()->ForEachProfile([&](const Registry::Expression::Profile& profile) {
			if (!profile.tags.HasTag(tag)) {
				return false;
			}
			ret.push_back(profile.id);
			return false;
		});
		return ret;
	}

	std::vector<RE::BSFixedString> GetExpressionsByTags(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::string_view a_tags)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return {};
		}
		std::vector<RE::BSFixedString> ret{};
		auto list = Registry::StringSplit(a_tags);
		Registry::TagDetails search{ list };
		Registry::Expression::GetSingleton()->ForEachProfile([&](const Registry::Expression::Profile& profile) {
			if (!search.MatchTags(profile.tags)) {
				return false;
			}
			ret.push_back(profile.id);
			return false;
		});
		return ret;
	}

}	 // namespace Papyrus::ExpressionSlots

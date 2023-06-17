#include "Tags.h"

namespace Registry
{
#define MAPENTRY(value)                                   \
	{                                                       \
		AsLower(#value##s), value \
	}

	using enum Tag;
	static inline const std::map<std::string_view, Tag> lookup = {
		MAPENTRY(SixtyNine),
		MAPENTRY(Aggressive),
		MAPENTRY(Anal),
		MAPENTRY(Asphyxiation),
		MAPENTRY(Blowjob),
		MAPENTRY(Boobjob),
		MAPENTRY(Cowgirl),
		MAPENTRY(Cunnilingius),
		MAPENTRY(Deepthroat),
		MAPENTRY(Doggy),
		MAPENTRY(DoublePenetration),
		MAPENTRY(Feet),
		MAPENTRY(Femdom),
		MAPENTRY(Footjob),
		MAPENTRY(Forced),
		MAPENTRY(Gay),
		MAPENTRY(Handjob),
		MAPENTRY(LeadIn),
		MAPENTRY(Lesbian),
		MAPENTRY(Masturbation),
		MAPENTRY(Missionary),
		MAPENTRY(Oral),
		MAPENTRY(Penetration),
		MAPENTRY(ReverseCowgirl),
		MAPENTRY(Spitroast),
		MAPENTRY(TriplePenetration),
		MAPENTRY(Vaginal),
		MAPENTRY(Facesitting),
		MAPENTRY(Maledom),
		MAPENTRY(Dominant),
		MAPENTRY(Hugging),
		MAPENTRY(Kissing),
		MAPENTRY(Kneeling),
		MAPENTRY(Standing),
		MAPENTRY(Loving),
		MAPENTRY(Lying),
		MAPENTRY(Behind),
		MAPENTRY(Facing)
	};
#undef MAPENTRY

	bool TagHandler::AddTag(BaseTag& a_enumeration, const std::string_view a_stringtag)
	{
		ASSERTLOWERCASE(a_stringtag);
		const auto where = lookup.find(a_stringtag);
		if (where == lookup.end())
			return false;
		a_enumeration.set(where->second);
		return true;
	}

	bool TagHandler::RemoveTag(BaseTag& a_enumeration, const std::string_view a_stringtag)
	{
		ASSERTLOWERCASE(a_stringtag);
		const auto where = lookup.find(a_stringtag);
		if (where == lookup.end())
			return false;
		a_enumeration.reset(where->second);
		return true;
	}

	int32_t TagHandler::HasTag(const BaseTag& a_enumeration, const std::string_view a_cmp)
	{
		ASSERTLOWERCASE(a_cmp);
		const auto where = lookup.find(a_cmp);
		if (where == lookup.end())
			return -1;

		return a_enumeration.all(where->second);
	}

	bool TagData::MatchTags(const std::vector<std::string_view>& a_match) const
	{
		enum
		{
			no_option = -1,
			missing_option = 0,
			has_option = 1,
		};

		auto option = no_option;
		for (auto&& it : a_match) {
			if (it.empty()) {
				continue;
			}

			ASSERTLOWERCASE(it);
			switch (it[0]) {
			case '~':
				if (option != has_option) {
					option = TagHandler::HasTag(tag, it.substr(1)) || std::find(extra.begin(), extra.end(), it.substr(1)) != extra.end() ?
										 has_option :
										 missing_option;
				}
				break;
			case '-':
				if (TagHandler::HasTag(tag, it.substr(1)) || std::find(extra.begin(), extra.end(), it.substr(1)) != extra.end()) {
					return false;
				}
				break;
			default:
				if (!TagHandler::HasTag(tag, it.substr(1)) && std::find(extra.begin(), extra.end(), it.substr(1)) == extra.end()) {
					return false;
				}
				break;
			}
		}
		return option != missing_option;
	}

	bool TagData::MatchTags(const TagTypeData& a_data) const
	{
		if (!this->tag.all(a_data[TagType::Required].first.get()) ||
				this->tag.none(a_data[TagType::Optional].first.get()) || 
				this->tag.any(a_data[TagType::Disallow].first.get())) {
			return false;
		}
		const auto hasExtra = [this](const RE::BSFixedString& a_tag) {
			const auto where = std::find(this->extra.begin(), this->extra.end(), a_tag);
			return where != this->extra.end();
		};
		// Continue if has all tags
		for (auto&& it : a_data[TagType::Required].second) {
			if (!hasExtra(it)) {
				return false;
			}
		}
		// Continue if Has none tags
		for (auto&& it : a_data[TagType::Disallow].second) {
			if (hasExtra(it)) {
				return false;
			}
		}
		// Return true if has any tag
		for (auto&& it : a_data[TagType::Optional].second) {
			if (hasExtra(it)) {
				return true;
			}
		}
		return false;
	}

	TagData::TagTypeData TagData::ParseTagsByType(const std::string_view a_tags)
	{
		const auto list = StringSplit(a_tags);
		return ParseTagsByType(list);
	}

	TagData::TagTypeData TagData::ParseTagsByType(const std::vector<std::string_view>& a_tags)
	{
		TagTypeData ret{};
		for (auto&& tag : a_tags) {
			if (tag.empty())
				continue;

			switch (tag[0]) {
			case '~':
				{
					auto tag_ = tag.substr(1);
					auto& [base, extra] = ret[TagType::Optional];
					if (TagHandler::AddTag(base, tag_))
						continue;
					extra.push_back(tag_);
				}
				break;
			case '-':
				{
					auto tag_ = tag.substr(1);
					auto& [base, extra] = ret[TagType::Disallow];
					if (TagHandler::AddTag(base, tag_))
						continue;
					extra.push_back(tag_);
				}
				break;
			default:
				{
					auto& [base, extra] = ret[TagType::Required];
					if (TagHandler::AddTag(base, tag))
						continue;
					extra.push_back(tag);
				}
				break;
			}
		}
		return ret;
	}

}
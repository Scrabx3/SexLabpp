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
}
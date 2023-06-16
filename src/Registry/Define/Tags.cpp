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

	TagData::TagData(std::string_view a_tags)
	{
		ASSERTLOWERCASE(a_tags);
		auto splits = StringSplit(a_tags, ',');
		for (auto&& it : splits) {
			AddTag(it);
		}
	}

	bool TagData::AddTag(std::string_view a_tag)
	{
		ASSERTLOWERCASE(a_tag);
		if (TagHandler::AddTag(this->tag, a_tag)) {
			return true;
		}
		if (std::find(extra.begin(), extra.end(), a_tag) == extra.end()) {
			extra.push_back(a_tag);
			std::sort(extra.begin(), extra.end(), [](const RE::BSFixedString& a_lhs, const RE::BSFixedString& a_rhs) {
				const auto cmp = a_lhs.size() <=> a_rhs.size();
				if (cmp < 0)
					return true;
				else if (cmp > 0)
					return false;

				for (RE::detail::BSFixedString<char>::size_type i = 0; i < a_lhs.size(); i++) {
					const auto char_cmp = a_lhs[i] <=> a_rhs[i];
					if (char_cmp < 0)
						return true;
					else if (char_cmp > 0)
						return false;
				}
				return false;
			});
			return true;
		}
		return false;
	}

	bool TagData::operator==(const TagData& a_rhs) const
	{
		return this->tag == a_rhs.tag && this->extra == a_rhs.extra;
	}
}
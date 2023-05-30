#include "Tags.h"

namespace Registry
{
#define MAPENTRY(value)                                   \
	{                                                       \
		SexLab::AsLower(#value##s), value \
	}

	using enum Tag;
	static inline const std::map<std::string, Tag> lookup = {
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

#define EDITTAG(func)                                                                               \
	assert(std::find_if_not(a_stringtag.begin(), a_stringtag.end(), ::islower) == a_stringtag.end()); \
	const auto where = lookup.find(a_stringtag);                                                      \
	if (where == lookup.end())                                                                        \
		return false;                                                                                   \
	a_enumeration.func(where->second);                                                                \
	return true;

	bool TagHandler::AddTag(stl::enumeration<Tag, uint64_t>& a_enumeration, const std::string& a_stringtag)
	{
		EDITTAG(set)
	}

	bool TagHandler::RemoveTag(stl::enumeration<Tag, uint64_t>& a_enumeration, const std::string& a_stringtag)
	{
		EDITTAG(reset)
	}

#undef EDITTAG
}
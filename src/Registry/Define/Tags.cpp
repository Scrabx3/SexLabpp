#include "Tags.h"

namespace Registry
{
#define MAPENTRY(value) \
	{                     \
		#value, value       \
	}

	struct TagCmp
	{
		bool operator()(const RE::BSFixedString& lhs, const RE::BSFixedString& rhs) const
		{
			return strcmp(lhs.data(), rhs.data()) < 0;
		}
	};


	using enum Tag;
	static inline const std::map<RE::BSFixedString, Tag, TagCmp> TagTable = {
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

	TagData::TagData(const std::vector<std::string_view>& a_tags)
	{
		for (auto&& tag : a_tags) {
			AddTag(tag);
		}
	}

	void TagData::AddTag(Tag a_tag)
	{
		_basetags.set(a_tag);
	}

	void TagData::AddTag(const TagData& a_tag)
	{
		_basetags.set(a_tag._basetags.get());
		_extratags.reserve(_extratags.size() + a_tag._extratags.size());
		for (auto&& tag : a_tag._extratags) {
			AddExtraTag(tag);
		}
	}

	void TagData::AddTag(RE::BSFixedString a_tag)
	{
		const auto where = TagTable.find(a_tag);
		if (where != TagTable.end()) {
			_basetags.set(where->second);
		} else {
			AddExtraTag(a_tag);
		}
	}

	void TagData::RemoveTag(Tag a_tag)
	{
		_basetags.reset(a_tag);
	}

	void TagData::RemoveTag(const TagData& a_tag)
	{
		_basetags.reset(a_tag._basetags.get());
		for (auto&& tag : a_tag._extratags) {
			RemoveExtraTag(tag);
		}
	}

	void TagData::RemoveTag(const RE::BSFixedString& a_tag)
	{
		const auto where = TagTable.find(a_tag);
		if (where != TagTable.end()) {
			_basetags.reset(where->second);
		} else {
			RemoveExtraTag(a_tag);
		}
	}

	bool TagData::HasTag(Tag a_tag) const
	{
		return _basetags.all(a_tag);
	}

	bool TagData::HasTag(const RE::BSFixedString& a_tag) const
	{
		const auto where = TagTable.find(a_tag);
		return where == TagTable.end() ? HasExtraTag(a_tag) : _basetags.all(where->second);
	}

	bool TagData::HasTags(const TagData& a_tag, bool a_all) const
	{
		if (a_all) {
			if (!_basetags.all(a_tag._basetags.get()))
				return false;	 // Want all but missing base
		} else if (_basetags.any(a_tag._basetags.get())) {
			return true;	// Want any and has at least 1 base match
		}
		for (auto&& tag : a_tag._extratags) {
			bool has = HasTag(tag);
			if (a_all) {
				if (!has)
					return false;	 // Wants all but misses at least one
			} else if (has) {
				return true;	// wants any and has at least one
			}
		}
		// if here we either want all and havent found 1 missing
		// or want any and havent found 1 matching
		return a_all;
	}

	void TagData::ForEachExtra(std::function<bool(const std::string_view)> a_visitor) const
	{
		for (auto&& tag : _extratags) {
			if (a_visitor(tag.data()))
				return;
		}
	}

	void TagData::AddExtraTag(const RE::BSFixedString& a_tag)
	{
		if (HasExtraTag(a_tag))
			return;
		_extratags.push_back(a_tag);
	}

	void TagData::RemoveExtraTag(const RE::BSFixedString& a_tag)
	{
		const auto where = std::find(_extratags.begin(), _extratags.end(), a_tag);
		if (where == _extratags.end())
			return;
		_extratags.erase(where);
	}

	bool TagData::HasExtraTag(const RE::BSFixedString& a_tag) const
	{
		return std::find(_extratags.begin(), _extratags.end(), a_tag) != _extratags.end();
	}

	TagDetails::TagDetails(const std::string_view a_tags) :
		TagDetails(StringSplit(a_tags, ',')) {}

	TagDetails::TagDetails(const std::vector<std::string_view> a_tags)
	{
		for (auto&& tag : a_tags) {
			if (tag.empty())
				continue;

			switch (tag[0]) {
			case '!':	 // Scene Meta for Papyrus, ignore
				continue;
			case '~':
				_tags[TagType::Optional].AddTag(tag.substr(1));
				break;
			case '-':
				_tags[TagType::Disallow].AddTag(tag.substr(1));
				break;
			default:
				_tags[TagType::Disallow].AddTag(tag);
				break;
			}
		}
	}

	TagDetails::TagDetails(const std::array<TagData, TagType::Total> a_tags)
	{
		for (size_t i = 0; i < TagType::Total; i++)
			_tags[i] = a_tags[i];
	}

	bool TagDetails::MatchTags(const TagData& a_data) const
	{
		if (a_data.HasTags(_tags[TagType::Disallow], false))
			return false;
		if (!a_data.HasTags(_tags[TagType::Optional], false))
			return false;
		return a_data.HasTags(_tags[TagType::Required], true);
	}

}
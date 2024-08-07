#pragma once

namespace Registry::Node
{
	static constexpr std::string_view HEAD{ "NPC Head [Head]"sv };				 // Back of throat
	static constexpr std::string_view PELVIS{ "NPC Pelvis [Pelv]"sv };		 // bottom mid (front)
	static constexpr std::string_view SPINELOWER{ "NPC Spine [Spn0]"sv };	 // bottom mid (back)

	static constexpr std::string_view HANDLEFT{ "NPC L Finger20 [LF20]"sv };	// Base of middle finger
	static constexpr std::string_view HANDRIGHT{ "NPC R Finger20 [RF20]"sv };
	static constexpr std::string_view FOOTLEFT{ "NPC L Foot [Lft ]"sv };	// Ankle
	static constexpr std::string_view FOOTRIGHT{ "NPC R Foot [Rft ]"sv };

	static constexpr std::string_view CLITORIS{ "Clitoral1"sv };
	static constexpr std::string_view VAGINA{ "VaginaDeep1"sv };
	static constexpr std::string_view VAGINALLEFT{ "NPC L Pussy02"sv };
	static constexpr std::string_view VAGINALRIGHT{ "NPC R Pussy02"sv };
	static constexpr std::string_view ANAL{ "NPC Anus Deep2"sv };
	static constexpr std::string_view ANALLEFT{ "NPC LB Anus2"sv };
	static constexpr std::string_view ANALRIGHT{ "NPC RB Anus2"sv };
	static constexpr std::array SOSSTART{
		"NPC Genitals01 [Gen01]"sv, "AH Base"sv, "DD 2"sv, "DD2"sv, "NPC IceGenital02"sv, "BearD 3"sv, "GS 3"sv, "BoarDick01"sv, "RD 2"sv, "CDPenis 2"sv, "CO 2"sv,
		"ElkD03"sv, "DwarvenSpiderDildo01"sv, "FD 3"sv, "GD 3"sv, "Goat_Penis02"sv, "Horker_Penis04"sv, "HS 3"sv, "SCD 3"sv, "SkeeverD 03"sv, "TD 3"sv, "VLDick03"sv,
		"WWD 4"sv, "NPC Torso Rock 01"sv, "NPC Torso Rock 02"sv, "Torso Rock 2"sv, "Torso Rock 1"sv
	};
	static constexpr std::array SOSMID{
		"NPC Genitals04 [Gen04]"sv, "AH 3"sv, "DD 3"sv, "DD3"sv, "NPC IceGenital03"sv, "BearD 6"sv, "GS 4"sv, "BoarDick03"sv, "RD 3"sv, "CDPenis 5"sv, "CO 5"sv,
		"ElkD04"sv, "DwarvenSpiderDildo02"sv, "FD 4"sv, "GD 4"sv, "Goat_Penis04"sv, "Horker_Penis06"sv, "HS 6"sv, "SCD 4"sv, "SkeeverD 05"sv, "TD 5"sv, "VLDick05"sv,
		"WWD 7"sv
	};
	static constexpr std::array SOSTIP{
		"NPC Genitals06 [Gen06]"sv, "AH 6"sv, "DD 6"sv, "DD6"sv, "BearD 8"sv, "GS 7"sv, "BoarDick06"sv, "RD 5"sv, "CDPenis 7"sv, "CO 9"sv, "ElkD06"sv, "DwarvenSpiderDildo03"sv,
		"FD 7"sv, "GD 7"sv, "Goat_Penis06"sv, "Horker_Penis10"sv, "HS 7"sv, "SCD 7"sv, "SkeeverD 07"sv, "TD 7"sv, "VLDick06"sv, "WWD 9"sv
	};

	static constexpr std::array SCHLONG_ANGLES{
		25.0f, 32.0f, 39.0f, 46.0f, 53.0f, 60.0f, 67.0f, 74.0f, 81.0f, 88.0f, 95.0f, 102.0f, 109.0f, 116.0f, 123.0f, 130.0f, 137.0f, 144.0f, 151.0f
	};
	
	struct NodeData
	{
		NodeData(RE::Actor* a_actor);
		~NodeData() = default;

		RE::NiPointer<RE::NiNode> head;
		RE::NiPointer<RE::NiNode> pelvis;
		RE::NiPointer<RE::NiNode> spine_lower;

		RE::NiPointer<RE::NiNode> hand_left;
		RE::NiPointer<RE::NiNode> hand_right;
		RE::NiPointer<RE::NiNode> foot_left;
		RE::NiPointer<RE::NiNode> foot_rigt;

		RE::NiPointer<RE::NiNode> clitoris;
		std::vector<RE::NiPointer<RE::NiNode>> sos_base;
		std::vector<RE::NiPointer<RE::NiNode>> sos_mid;
		std::vector<RE::NiPointer<RE::NiNode>> sos_front;

	public:
		RE::NiPoint3 ApproximateTip() const;
		RE::NiPoint3 ApproximateMid() const;
		RE::NiPoint3 ApproximateBase() const;

	private:
		RE::NiPoint3 ApproximateNode(float a_forward, float a_upward) const;
	};
}
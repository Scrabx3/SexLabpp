#pragma once

#include "IPluginInterface.h"

namespace Registry
{
	class Scale :
		public Singleton<Scale>
	{
		static constexpr const char* namekey{ "SexLabRegistry" };
		static constexpr const char* basenode{ "NPC" };
		// NOTE: below isnt used, Im not sure if its necessary
		static constexpr const char* extranode{ "NPC Root [Root]" };

		enum ScaleModes
		{
			Multiplicative = 0,
			Averaged = 1,
			Additive = 2,
			Maximum = 3,
		};

	public:
		float GetScale(RE::TESObjectREFR* a_reference);
		void SetScale(RE::Actor* a_reference, float a_absolutescale);
		void RemoveScale(RE::Actor* a_reference);

	private:
		struct ScaleNodeVisitor : public SKEE::INiTransformInterface::NodeVisitor
		{
			virtual bool VisitPosition(const char* node, const char* key, SKEE::INiTransformInterface::Position& position) override;
			virtual bool VisitRotation(const char* node, const char* key, SKEE::INiTransformInterface::Rotation& rotation) override;
			virtual bool VisitScale(const char* node, const char* key, float scale) override;
			virtual bool VisitScaleMode(const char* node, const char* key, uint32_t scaleMode) override;
		};

		SKEE::INiTransformInterface* transformInterface = []() {
			const auto intfc = SKEE::GetInterfaceMap();
			return intfc ? SKEE::GetNiTransformInterface(intfc) : nullptr;
		}();
	};

}  // namespace Registry

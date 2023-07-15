#include "stdafx.h"
#include "SpehsEngine/ImGui/Utility/ImGuiUtility.h"

#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Input/CustomEventParametersRecorder.h"
#include "SpehsEngine/Input/EventSignaler.h"
#include "SpehsEngine/ImGui/Utility/ImGuiInput.h"
#include "SpehsEngine/ImGui/Utility/IState.h"


namespace ImGui
{
	template<> template<>
	bool Impl<se::input::CustomEventParameters>::input(const Label label, se::input::CustomEventParameters& customEventParameters, se::input::EventSignaler& eventSignaler, StateWrapper& stateWrapper)
	{
		struct State : public IState
		{
			std::unique_ptr<se::input::CustomEventParametersRecorder> customEventParametersRecorder;
		};
		State& state = stateWrapper.get<State>();

		bool changed = false;
		if (state.customEventParametersRecorder)
		{
			ImGui::Text(se::formatString("%s: press any key...", label));
			if (const std::optional<se::input::CustomEventParameters> result = state.customEventParametersRecorder->getCustomEventParameters())
			{
				customEventParameters = *result;
				state.customEventParametersRecorder.reset();
				changed = true;
			}
		}
		else if (ImGui::Button(se::formatString("%s: %s", label, customEventParameters.toString().c_str())))
		{
			state.customEventParametersRecorder.reset(new se::input::CustomEventParametersRecorder(eventSignaler));
		}

		return changed;
	}
}
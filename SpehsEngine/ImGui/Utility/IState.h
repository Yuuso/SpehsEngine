#pragma once


namespace ImGui
{
	/*
		State interface.
		Sometimes the gui requires state, but the caller doesn't need to know the details of the implementation.
		The caller isn't expected to create the state.
	*/
	struct IState
	{
		virtual ~IState() {}
	};

	/*
		For convenience, wrap the state function parameter in a struct with a handy getter:
		void ImGuiFunction(State& state) vs void ImGuiFunction(std::unique_ptr<State>& state)
	*/
	struct StateWrapper
	{
		template<typename StateType>
		StateType& get()
		{
			static_assert(std::is_base_of<IState, StateType>::value, "StateType must be derived from ImGui::IState");
			static_assert(std::is_default_constructible<StateType>::value, "StateType must be default constructible");
			if (!iState)
			{
				iState.reset(new StateType());
			}
			StateType* const stateType = dynamic_cast<StateType*>(iState.get());
			se_assert(stateType);
			return *stateType;
		}

		std::unique_ptr<IState> iState;
	};
}

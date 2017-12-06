#pragma once
#include "InputState.h"
#include "TimeState.h"

namespace spehs
{
	/*
		A collection of data that represents a state of a generic system (game)
	*/
	class SystemState
	{
	public:
		SystemState();
		~SystemState();

		InputState input;
		time::TimeState time;

	private:
	};
}
#pragma once
#include <unordered_map>
#include <crtdbg.h>//assert
#include "SpehsEngine/Input/InputEnumerations.h"
#include "SpehsEngine/Core/Vector.h"



namespace se
{
	class InputManager;
	class InputState
	{
		friend class InputManager;
	public:
		class UseState
		{
			friend class InputManager;
		public:
			UseState(const bool _used = false);
			void operator=(const UseState& other) = delete;
			bool isUsed() const;
			void use() const;
			bool tryUse() const;

		protected:
			mutable bool used;
		};

		class Key;
		template<typename T>
		class State : public UseState
		{
			friend class InputManager;
		public:
			State(const T& _value, const bool _used = false)
				: UseState(_used)
				, value(_value)
			{
			}

			void operator=(const State& other) = delete;

			T operator()() const
			{
				return value;
			}

			operator T() const
			{
				return value;
			}

		private:
			T value;
		};
		template<typename T>
		class DerivedState : public UseState, public T
		{
		public:
			DerivedState(const T& _value, const bool _used = false)
				: UseState(_used)
				, T(_value)
			{
			}

			void operator=(const DerivedState<T>& other) = delete;
			void operator=(const UseState& other) = delete;
			void operator=(const T& other) = delete;
		};

	public:

		class Key
		{
			friend class InputManager;
		public:
			Key();
			Key(const bool _down, const bool _pressed, const bool _released);
			void operator=(const Key& other) = delete;

			State<bool> down;
			State<bool> press;
			State<bool> release;
		};

		class Pointer
		{
		public:
			Pointer(const ivec2 _position, const ivec2 _movement);
			DerivedState<ivec2> position;
			DerivedState<ivec2> movement;
		};

		class TextEntered : public DerivedState<std::string>
		{
		public:
			TextEntered(const std::string& _value, const bool _used = false);
			operator bool() const;
		};

		InputState();
		~InputState();
		const Key& key(const KeyboardKey keyboardKey) const;
		const Key& key(const MouseButton keyboardKey) const;

		TextEntered textEntered;
		TextEntered fileDropped;
		Pointer pointer;

		bool ctrlModifier;
		bool shiftModifier;

	private:
		std::unordered_map<unsigned, Key> keys;
	};
}
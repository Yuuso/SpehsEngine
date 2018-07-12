#include "stdafx.h"
#include "InputState.h"

namespace se
{
	InputState::UseState::UseState(const bool _used)
		: used(_used)
	{
	}
	
	bool InputState::UseState::isUsed() const
	{
		return used;
	}

	void InputState::UseState::use() const
	{
		_ASSERT(used == false);
		used = true;
	}

	bool InputState::UseState::tryUse() const
	{
		if (used)
			return false;
		used = true;
		return true;
	}

	InputState::Key::Key()
		: down(false)
		, press(false, true)
		, release(false, true)
	{

	}

	InputState::Key::Key(const bool _down, const bool _pressed, const bool _released)
		: down(_down)
		, press(_pressed)
		, release(_released)
	{

	}
	
	InputState::Pointer::Pointer(const ivec2 _position, const ivec2 _movement)
		: position(_position)
		, movement(_movement)
	{

	}

	InputState::TextEntered::TextEntered(const std::string& _value, const bool _used)
		: DerivedState<std::string>(_value, _used)
	{

	}

	InputState::TextEntered::operator bool() const
	{
		return size() > 0;
	}

	InputState::InputState()
		: textEntered("")
		, fileDropped("")
		, pointer(ivec2::zero, ivec2::zero)
		, keys()
	{
	}

	InputState::~InputState()
	{
	}

	const InputState::Key& InputState::key(const KeyboardKey keyboardKey) const
	{
		static const Key default;
		const std::unordered_map<unsigned, Key>::const_iterator it = keys.find((unsigned)keyboardKey);
		return it == keys.end() ? default: it->second;
	}

	const InputState::Key& InputState::key(const MouseButton button) const
	{
		static const Key default;
		const std::unordered_map<unsigned, Key>::const_iterator it = keys.find((unsigned)button);
		return it == keys.end() ? default: it->second;
	}
}
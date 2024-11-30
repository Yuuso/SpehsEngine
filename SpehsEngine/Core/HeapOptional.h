#pragma once

#include "SpehsEngine/Core/HasMemberFunction.h"


namespace se
{
	/*
		Works like std::optional<T> but uses std::unique_ptr<T> as the underlying implementation.
		Implements deep copying.
		For polymorphic types the usage must be restricted to the specified type T.
		If you require polymorphism then use DeepPtr<T>.
	*/
	template<typename T, typename D = std::default_delete<T>>
	class HeapOptional
	{
		static_assert(sizeof(T) > sizeof(std::unique_ptr<T>), "Use std::optional<T> instead");
	public:

		typedef T element_type;

		HeapOptional() = default;
		HeapOptional(const T& _copy)
			: impl(copy(_copy))
		{
		}
		HeapOptional(T&& _move)
			: impl(new T(std::move(_move)))
		{
		}
		HeapOptional(HeapOptional<T, D>&& _move)
			: impl(std::move(_move.impl))
		{
		}
		HeapOptional(const HeapOptional<T, D>& _copy)
		{
			if (_copy)
			{
				impl.reset(copy(*_copy));
			}
		}

		HeapOptional<T, D>& operator=(const HeapOptional<T, D>& _copy)
		{
			if (_copy)
			{
				impl.reset(copy(*_copy));
			}
			else
			{
				impl.reset();
			}
			return *this;
		}

		HeapOptional<T, D>& operator=(HeapOptional<T, D>&& _move)
		{
			impl = std::move(_move.impl);
			return *this;
		}

		explicit operator bool() const
		{
			return bool(impl);
		}

		T* operator->() const
		{
			return impl.operator->();
		}

		T& operator*() const
		{
			return *impl;
		}

		void reset()
		{
			impl.reset();
		}

		void swap(HeapOptional<T, D>& other)
		{
			impl.swap(other.impl);
		}

		T& emplace(const T& t)
		{
			impl.reset(copy(t));
			return *impl;
		}

		T& emplace(T&& t)
		{
			impl.reset(new T(std::move(t)));
			return *impl;
		}

		T& emplace()
		{
			impl.reset(new T());
			return *impl;
		}

		bool has_value() const
		{
			return impl.get() != nullptr;
		}

		T& value()
		{
			se_assert(impl);
			return *impl;
		}

		const T& value() const
		{
			se_assert(impl);
			return *impl;
		}

	private:

		SPEHS_HAS_MEMBER_FUNCTION(clone, has_clone);
		template<typename V>
		typename std::enable_if<has_clone<V, std::unique_ptr<V>(V::*)() const>::value, V*>::type copy(const V& v) const
		{
			return v.clone().release();
		}
		template<typename V>
		typename std::enable_if<!has_clone<V, std::unique_ptr<V>(V::*)() const>::value, V*>::type copy(const V& v) const
		{
			static_assert(std::is_copy_constructible<V>::value, "Type must be copy constructible or have a clone method.");
			return new V(v); // If you get a compile error here you need to make the type either copy constructible or add a 'std::unique_ptr<T> clone() const' method to it
		}

		std::unique_ptr<T, D> impl;
	};
}

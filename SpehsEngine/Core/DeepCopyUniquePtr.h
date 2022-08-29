#pragma once

#include "SpehsEngine/Core/HasMemberFunction.h"
#include <memory>


namespace se
{
	/*
		Works like the std::unique_ptr<T, D> but implements deep copying.
	*/
	template<typename T, typename D = std::default_delete<T>>
	class DeepCopyUniquePtr
	{
	public:	

		typedef T element_type;

		DeepCopyUniquePtr() = default;

		DeepCopyUniquePtr(T* const ptr)
			: impl(ptr)
		{

		}

		DeepCopyUniquePtr(DeepCopyUniquePtr<T, D>&& move)
			: impl(std::move(move.impl))
		{
		}

		DeepCopyUniquePtr(const DeepCopyUniquePtr<T, D>& other)
		{
			if (other)
			{
				impl.reset(copy(*other));
			}
		}

		DeepCopyUniquePtr<T, D>& operator=(const DeepCopyUniquePtr<T, D>& other)
		{
			if (other)
			{
				impl.reset(copy(*other));
			}
			else
			{
				impl.reset();
			}
			return *this;
		}
		
		DeepCopyUniquePtr<T, D>& operator=(DeepCopyUniquePtr<T, D>&& move)
		{
			impl = std::move(move.impl);
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

		T* get()
		{
			return impl.get();
		}

		const T* get() const
		{
			return impl.get();
		}

		void reset(T* const ptr = nullptr)
		{
			impl.reset(ptr);
		}

		T* release()
		{
			return impl.release();
		}

		std::unique_ptr<T, D>& getImpl() { return impl; }
		const std::unique_ptr<T, D>& getImpl() const { return impl; }

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

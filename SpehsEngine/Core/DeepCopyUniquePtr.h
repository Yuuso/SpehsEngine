#pragma once

#include <memory>


namespace se
{
	/*
		Works like the std::unique_ptr<T, D> but implements deep copying.
	*/
	template<typename T, typename D = std::default_delete<T>>
	class DeepCopyUniquePtr
	{
		static_assert(std::is_copy_constructible<T>::value, "Type T must be copy constructible.");
	public:	

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
				impl.reset(new T(*other));
			}
		}

		DeepCopyUniquePtr<T, D>& operator=(const DeepCopyUniquePtr<T, D>& other)
		{
			if (other)
			{
				impl.reset(new T(*other));
			}
			else
			{
				impl.reset();
			}
			return *this;
		}
		
		DeepCopyUniquePtr<T, D>& operator=(DeepCopyUniquePtr<T, D>&& move)
		{
			impl = std::move(move);
			return *this;
		}

		operator bool() const
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

	private:
		std::unique_ptr<T, D> impl;
	};
}

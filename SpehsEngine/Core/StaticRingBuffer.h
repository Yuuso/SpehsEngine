#pragma once


namespace se
{
	template<typename T>
	struct IStaticRingBuffer
	{
		virtual ~IStaticRingBuffer() = default;
		virtual bool pushFront(const T& t) = 0;
		virtual bool pushBack(const T& t) = 0;
		virtual T& at(const size_t index) = 0;
		virtual const T& at(const size_t index) const = 0;
		virtual T& operator[](const size_t index) = 0;
		virtual const T& operator[](const size_t index) const = 0;
		virtual T& getFront() = 0;
		virtual const T& getFront() const = 0;
		virtual T& getBack() = 0;
		virtual const T& getBack() const = 0;
		virtual const T* getBuffer() const = 0;
		virtual void popFront() = 0;
		virtual void popBack() = 0;
		virtual size_t getMaxSize() const = 0;
		virtual size_t getSize() const = 0;
		virtual size_t getBeginOffset() const = 0;
		virtual size_t getEndOffset() const = 0;
		virtual bool isEmpty() const = 0;
		virtual bool isFull() const = 0;
	};

	template<typename T, uint32_t MaxSize>
	class StaticRingBuffer final : public IStaticRingBuffer<T>
	{
	public:

		static_assert(MaxSize > 0, "MaxSize cannot be zero.");
		static_assert(std::is_fundamental<T>::value, "Only fundamental types are supported currently. Consider adding support when needed.");

		bool pushFront(const T& t) final
		{
			if (size < MaxSize)
			{
				if (beginOffset > 0)
				{
					beginOffset--;
				}
				else
				{
					beginOffset = MaxSize - 1;
				}
				buffer[beginOffset] = t;
				size++;
				return true;
			}
			else
			{
				return false;
			}
		}
		
		bool pushBack(const T& t) final
		{
			if (size < MaxSize)
			{
				endOffset++;
				if (endOffset >= MaxSize)
				{
					endOffset = 0;
				}
				buffer[endOffset] = t;
				size++;
				return true;
			}
			else
			{
				return false;
			}
		}

		void popFront() final
		{
			if (size > 0)
			{
				beginOffset++;
				if (beginOffset >= MaxSize)
				{
					beginOffset = 0;
				}
				size--;
			}
		}

		void popBack() final
		{
			if (size > 0)
			{
				if (endOffset == 0)
				{
					endOffset = MaxSize - 1;
				}
				else
				{
					endOffset--;
				}
				size--;
			}
		}

		T& at(const size_t index) final
		{
			const size_t offset = beginOffset + index;
			if (offset < MaxSize)
			{
				return buffer[offset];
			}
			else
			{
				return buffer[offset - MaxSize];
			}
		}

		const T& at(const size_t index) const final
		{
			const size_t offset = beginOffset + index;
			if (offset < MaxSize)
			{
				return buffer[offset];
			}
			else
			{
				return buffer[offset - MaxSize];
			}
		}

		T& operator[](const size_t index) final { return at(index); }
		const T& operator[](const size_t index) const final { return at(index); }

		T& getFront() final { return at(beginOffset); }
		const T& getFront() const final { return at(beginOffset); }
		T& getBack() final { return at(endOffset); }
		const T& getBack() const final { return at(endOffset); }

		const T* getBuffer() const final { return buffer; }
		size_t getMaxSize() const final { return MaxSize; }
		size_t getSize() const final { return size; }
		size_t getBeginOffset() const final { return beginOffset; }
		size_t getEndOffset() const final { return endOffset; }
		bool isEmpty() const final { return size == 0; }
		bool isFull() const final { return size == MaxSize; }

	private:
		T buffer[MaxSize];
		size_t beginOffset = 0;
		size_t endOffset = MaxSize - 1;
		size_t size = 0;
	};
}

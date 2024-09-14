#pragma once


namespace se
{
	template<typename T, uint32_t MaxSize>
	class StaticRingBuffer
	{
	public:

		static_assert(MaxSize > 0, "MaxSize cannot be zero.");
		static_assert(std::is_fundamental<T>::value, "Only fundamental types are supported currently. Consider adding support when needed.");

		bool pushFront(const T& t)
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
		
		bool pushBack(const T& t)
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

		void popFront()
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

		void popBack()
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

		T& at(const size_t index)
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

		const T& at(const size_t index) const
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

		inline T& operator[](const size_t index) { return at(index); }
		inline const T& operator[](const size_t index) const { return at(index); }

		inline T& getFront() { return at(beginOffset); }
		inline const T& getFront() const { return at(beginOffset); }
		inline T& getBack() { return at(endOffset); }
		inline const T& getBack() const { return at(endOffset); }

		inline const T* getBuffer() const { return buffer; }
		inline size_t getMaxSize() const { return MaxSize; }
		inline size_t getSize() const { return size; }
		inline size_t getBeginOffset() const { return beginOffset; }
		inline size_t getEndOffset() const { return endOffset; }
		inline bool isEmpty() const { return size == 0; }
		inline bool isFull() const { return size == MaxSize; }

	private:
		T buffer[MaxSize];
		size_t beginOffset = 0;
		size_t endOffset = MaxSize - 1;
		size_t size = 0;
	};
}

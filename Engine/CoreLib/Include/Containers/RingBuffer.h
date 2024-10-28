#pragma once

#include "Containers/Array.h"

namespace Private
{
	template<typename T>
	class TRingBufferIterator
	{
	public:

		using SizeType = typename TSpan<T>::SizeType;

		/**
		 * @brief Sets default values for this iterator's properties.
		 *
		 * @param items The items to iterate.
		 * @param index The index to begin iterating at.
		 * @param loopedPastEnd True if this iterator should be marked as having looped past the end of the items, otherwise false.
		 */
		TRingBufferIterator(const TSpan<T> items, const SizeType index, const bool loopedPastEnd)
			: m_Items { items }
			, m_CurrentIndex { index }
			, m_StartIndex { index }
			, m_LoopedPastEnd { loopedPastEnd }
		{
		}

		/**
		 * @brief Checks to see if this ring buffer iterator is at the end of the items.
		 *
		 * @return True if this ring buffer iterator is at the end of the items, otherwise false.
		 */
		[[nodiscard]] bool IsAtEnd() const
		{
			return m_LoopedPastEnd && m_CurrentIndex == m_StartIndex;
		}

		/**
		 * @brief Checks to see if this ring buffer iterator equals another.
		 *
		 * @param other The other ring buffer iterator.
		 * @return True if this ring buffer iterator equals \p other, otherwise false.
		 */
		[[nodiscard]] bool Equals(const TRingBufferIterator& other) const
		{
			return m_Items.GetData() == other.m_Items.GetData() &&
			       m_CurrentIndex == other.m_CurrentIndex &&
			       m_LoopedPastEnd == other.m_LoopedPastEnd;
		}

		/**
		 * @brief Moves this iterator to the next element.
		 */
		void MoveNext()
		{
			++m_CurrentIndex;

			if (m_CurrentIndex >= m_Items.Num())
			{
				m_CurrentIndex = 0;
				m_LoopedPastEnd = true;
			}
		}

		/**
		 * @brief Checks to see if this ring buffer iterator equals another.
		 *
		 * @param other The other ring buffer iterator.
		 * @return True if this ring buffer iterator equals \p other, otherwise false.
		 */
		[[nodiscard]] bool operator==(const TRingBufferIterator& other) const
		{
			return Equals(other);
		}

		/**
		 * @brief Checks to see if this ring buffer iterator equals another.
		 *
		 * @param other The other ring buffer iterator.
		 * @return True if this ring buffer iterator equals \p other, otherwise false.
		 */
		[[nodiscard]] bool operator!=(const TRingBufferIterator& other) const
		{
			return Equals(other) == false;
		}

		// TODO Is this the post-increment operator?
		[[nodiscard]] TRingBufferIterator operator++()
		{
			TRingBufferIterator copy = *this;
			MoveNext();
			return copy;
		}

		// TODO Is this the pre-increment operator?
		[[nodiscard]] TRingBufferIterator& operator++(int)
		{
			MoveNext();
			return *this;
		}

	private:

		TSpan<T> m_Items;
		SizeType m_CurrentIndex = 0;
		SizeType m_StartIndex = 0;
		bool m_LoopedPastEnd = false;
	};
}

/**
 * @brief Defines a circular array buffer.
 *
 * @tparam T The type contained within the buffer.
 * @tparam N The number of elements within the buffer.
 */
template<typename T, int32 N>
class TRingBuffer
{
	static_assert(N >= 2, "Ring buffers must contain at least two items (otherwise what's the point?)");

public:

	UM_DEFAULT_COPY(TRingBuffer);

	using ElementType = T;
	using IteratorType = Private::TRingBufferIterator<T>;
	using ConstIteratorType = Private::TRingBufferIterator<const T>;
	using SizeType = typename TArray<T>::SizeType;

	/**
	 * @brief Sets default values for this ring buffer's properties.
	 */
	TRingBuffer()
	{
		m_Items.Reserve(N);
	}

	/**
	 * @brief Moves ownership of another ring buffer's resources to this ring buffer.
	 *
	 * @param other The other ring buffer.
	 */
	TRingBuffer(TRingBuffer&& other) noexcept
		: m_Items { MoveTemp(other.m_Items) }
		, m_HeadIndex { other.m_HeadIndex }
	{
		other.m_HeadIndex = 0;
	}

	/**
	 * @brief Adds the given value to this ring buffer.
	 *
	 * @param value The value to add.
	 */
	void Add(const T& value)
	{
		(void)Emplace(Forward<T>(value));
	}

	/**
	 * @brief Adds the given value to this ring buffer.
	 *
	 * @param value The value to add.
	 */
	void Add(T&& value)
	{
		(void)Emplace(Forward<T>(value));
	}

	/**
	 * @brief Creates a const iterator for this ring buffer.
	 *
	 * @return A const iterator for this ring buffer.
	 */
	[[nodiscard]] ConstIteratorType CreateConstIterator() const
	{
		return ConstIteratorType { m_Items.AsSpan(), m_HeadIndex, false };
	}

	/**
	 * @brief Creates an iterator for this ring buffer.
	 *
	 * @return An iterator for this ring buffer.
	 */
	[[nodiscard]] ConstIteratorType CreateIterator() const
	{
		return ConstIteratorType { m_Items.AsSpan(), m_HeadIndex, false };
	}

	/**
	 * @brief Creates an iterator for this ring buffer.
	 *
	 * @return An iterator for this ring buffer.
	 */
	[[nodiscard]] IteratorType CreateIterator()
	{
		return IteratorType { m_Items.AsSpan(), m_HeadIndex, false };
	}

	/**
	 * @brief Constructs a new value at the "end" of this ring buffer.
	 *
	 * @tparam ArgTypes The types of the arguments to pass along to the value's constructor.
	 * @param args The arguments to pass along to the value's constructor.
	 * @return The emplaced value.
	 */
	template<typename... ArgTypes>
	[[nodiscard]] T& Emplace(ArgTypes&&... args)
	{
		T& value = m_Items.EmplaceAt(m_HeadIndex, Forward<ArgTypes>(args)...);
		AdvanceHeadIndex();
		return value;
	}

	/**
	 * @brief Gets the number of items in this ring buffer.
	 *
	 * @return The number of items in this ring buffer.
	 */
	[[nodiscard]] SizeType Num() const
	{
		return m_Items.Num();
	}

	/**
	 * @brief Moves ownership of another ring buffer's resources to this ring buffer.
	 *
	 * @param other The other ring buffer.
	 * @return This ring buffer.
	 */
	TRingBuffer& operator=(TRingBuffer&& other) noexcept
	{
		if (&other == this)
		{
			return *this;
		}

		m_Items = MoveTemp(other.m_Items);
		m_HeadIndex = other.m_HeadIndex;
		other.m_HeadIndex = 0;

		return *this;
	}

	// STL compatibility BEGIN
	IteratorType      begin()        { return CreateIterator(); }
	ConstIteratorType begin()  const { return CreateIterator(); }
	ConstIteratorType cbegin() const { return CreateConstIterator(); }
	IteratorType      end()          { return IteratorType { m_Items.AsSpan(), m_HeadIndex, true }; }
	ConstIteratorType end()    const { return ConstIteratorType { m_Items.AsSpan(), m_HeadIndex, true }; }
	ConstIteratorType cend()   const { return ConstIteratorType { m_Items.AsSpan(), m_HeadIndex, true }; }
	// STL compatibility END

private:

	/**
	 * @brief Advances the head index.
	 */
	void AdvanceHeadIndex()
	{
		++m_HeadIndex;

		if (m_HeadIndex >= m_Items.GetCapacity())
		{
			m_HeadIndex = 0;
		}
	}

	TArray<T> m_Items;
	SizeType m_HeadIndex = 0;
};

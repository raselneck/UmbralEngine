#pragma once

#include "Containers/Array.h"

/**
 * @brief Defines a stack (a last-in-first-out, or LIFO, data structure).
 *
 * @tparam T The element type contained within the stack.
 */
template<typename T>
class TStack
{
	using ArrayType = TArray<T>;

public:

	using ElementType = typename ArrayType::ElementType;
	using PointerType = typename ArrayType::PointerType;
	using ConstPointerType = typename ArrayType::ConstPointerType;
	using SizeType = typename ArrayType::SizeType;

	/**
	 * @brief Gets a copy of this stack as an array where the top of the stack starts at index zero.
	 *
	 * @return A copy of this stack as an array.
	 */
	[[nodiscard]] ArrayType AsArray() const
	{
		return m_Values.Reversed();
	}

	/**
	 * @brief Checks to see if this stack can pop an item off.
	 *
	 * @return True if an item can be popped off of this stack, otherwise false.
	 */
	[[nodiscard]] bool CanPop() const
	{
		return m_Values.Num() > 0;
	}

	/**
	 * @brief Checks to see if this stack is empty.
	 *
	 * @return True if this stack is empty, otherwise false.
	 */
	[[nodiscard]] bool IsEmpty() const
	{
		return m_Values.IsEmpty();
	}

	/**
	 * @brief Gets the number of items in this stack.
	 *
	 * @return The number of items in this stack.
	 */
	[[nodiscard]] SizeType Num() const
	{
		return m_Values.Num();
	}

	/**
	 * @brief Peeks at the value on top of this stack.
	 *
	 * @return A pointer to the value on top of this stack.
	 */
	[[nodiscard]] ConstPointerType Peek() const
	{
		if (m_Values.IsEmpty())
		{
			return nullptr;
		}

		return &m_Values.Last();
	}

	/**
	 * @brief Pops the top value off of this stack.
	 *
	 * Will assert if this stack does not have any values to pop off.
	 *
	 * @return The top value off of this stack.
	 */
	[[nodiscard]] ElementType Pop()
	{
		UM_ASSERT(CanPop(), "Attempting to pop value off of empty stack");

		return m_Values.TakeLast();
	}

private:

	ArrayType m_Values;
};
#pragma once

#include "Containers/Optional.h"
#include "Engine/CoreTypes.h"
#include "Memory/UniquePtr.h"
#include "Misc/Badge.h"
#include "Templates/ComparisonTraits.h"
#include "Templates/IsCallable.h"
#include "Templates/TypeTraits.h"
#include <initializer_list>

template<typename T>
class TLinkedList;

namespace Private
{
	/**
	 * @brief Defines a node for a singly-linked list.
	 *
	 * @tparam T The type of the node's value.
	 */
	template<typename T>
	class TLinkedListNode
	{
	public:

		using ElementType = T;

		/** @brief The node after this one in the chain. */
		TUniquePtr<TLinkedListNode> NextNode;

		/** @brief This node's value. */
		ElementType Value;

		/**
		 * @brief Sets default values for this node's properties.
		 *
		 * @param value The node's value.
		 */
		explicit TLinkedListNode(const ElementType& value)
			: Value { value }
		{
		}

		/**
		 * @brief Sets default values for this node's properties.
		 *
		 * @param value The node's value.
		 */
		explicit TLinkedListNode(ElementType&& value)
			: Value { MoveTemp(value) }
		{
		}

		/**
		 * @brief Constructs a unique pointer to a linked list node.
		 *
		 * @tparam ConstructTypes The types of the arguments to forward to the node value's constructor.
		 * @param args The arguments to forward to the node value's constructor.
		 * @return The constructed node.
		 */
		template<typename... ConstructTypes>
		[[nodiscard]] static TUniquePtr<TLinkedListNode> ConstructUnique(ConstructTypes&& ... args)
		{
			ElementType value = FMemory::ConstructObject<ElementType>(Forward<ConstructTypes>(args)...);
			return MakeUnique<TLinkedListNode>(MoveTemp(value));
		}
	};

	/**
	 * @brief Defines an iterator for a linked list.
	 *
	 * @tparam IsConst True if the iterator is for constant elements, otherwise false.
	 */
	template<typename T, bool IsConst>
	class TLinkedListIterator
	{
	public:

		using NodeType = ConditionalAddConst<IsConst, TLinkedListNode<T>>;
		using ElementType = ConditionalAddConst<IsConst, T>;
		using PointerType = AddPointer<ElementType>;
		using ReferenceType = AddLValueReference<ElementType>;
		using ListType = ConditionalAddConst<IsConst, TLinkedList<T>>;

		/**
		 * @brief Sets default values for this iterator's properties.
		 */
		TLinkedListIterator() = default;

		/**
		 * @brief Sets default values for this iterator's properties.
		 *
		 * @param list The list being iterated.
		 * @param previousNode The "previous" node.
		 * @param currentNode The "current" node.
		 */
		TLinkedListIterator(ListType* list, NodeType* previousNode, NodeType* currentNode)
			: m_List { list }
			, m_PreviousNode { previousNode }
			, m_CurrentNode { currentNode }
		{
		}

		/**
		 * @brief Checks to see if this linked list iterator belongs to the given linked list.
		 *
		 * @param linkedList The linked list.
		 * @return True if this linked list belongs to \p linkedList, otherwise false.
		 */
		[[nodiscard]] bool BelongsTo(const ListType& linkedList) const
		{
			return &linkedList == m_List;
		}

		/**
		 * @brief Checks to see if this iterator is equivalent to another.
		 *
		 * @param other The other iterator.
		 * @return True if this iterator is equivalent to \p other, otherwise false.
		 */
		[[nodiscard]] bool Equals(const TLinkedListIterator& other) const
		{
			return m_List == other.m_List &&
			       m_PreviousNode == other.m_PreviousNode &&
			       m_CurrentNode == other.m_CurrentNode;
		}

		/**
		 * @brief Gets the current node.
		 *
		 * @return The current node.
		 */
		[[nodiscard]] NodeType* GetCurrentNode(TBadge<ListType>) const
		{
			return m_CurrentNode;
		}

		/**
		 * @brief Gets the previous node.
		 *
		 * @return The previous node.
		 */
		[[nodiscard]] NodeType* GetPreviousNode(TBadge<ListType>) const
		{
			return m_PreviousNode;
		}

		/**
		 * @brief Returns a value indicating whether or not this iterator is valid.
		 *
		 * @return True if this iterator is valid, otherwise false.
		 */
		[[nodiscard]] bool IsValid() const
		{
			return m_CurrentNode != nullptr;
		}

		/**
		 * @brief Moves this iterator to the next valid element.
		 */
		void MoveNext()
		{
			if (m_CurrentNode == nullptr)
			{
				return;
			}

			m_PenultimateNode = m_PreviousNode;
			m_PreviousNode = m_CurrentNode;
			m_CurrentNode = m_CurrentNode->NextNode.Get();
		}

		/**
		 * @brief Notifies this linked list iterator that the head node was removed.
		 *
		 * @param newHead The new head node.
		 */
		void NotifyHeadRemoved(TBadge<ListType>, NodeType* newHead)
		{
			m_CurrentNode = newHead;
			m_PreviousNode = nullptr;
			m_PenultimateNode = nullptr;
		}

		/**
		 * @brief Notifies this linked list iterator that the current node was removed.
		 */
		void NotifyCurrentRemoved(TBadge<ListType>)
		{
			m_CurrentNode = m_PreviousNode;
			m_PreviousNode = m_PenultimateNode;
			m_PenultimateNode = nullptr;
		}

		/**
		 * @brief Removes the current node that this iterator points to.
		 */
		void Remove() requires(IsConst == false);

		/**
		 * @brief Checks to see if this iterator is equivalent to another.
		 *
		 * @param other The other iterator.
		 * @return True if this iterator is equivalent to \p other, otherwise false.
		 */
		[[nodiscard]] bool operator==(const TLinkedListIterator& other) const
		{
			return Equals(other);
		}

		/**
		 * @brief Checks to see if this iterator is not equivalent to another.
		 *
		 * @param other The other iterator.
		 * @return True if this iterator is not equivalent to \p other, otherwise false.
		 */
		[[nodiscard]] bool operator!=(const TLinkedListIterator& other) const
		{
			return Equals(other) == false;
		}

		/**
		 * @brief Returns a value indicating whether or not this iterator is valid.
		 *
		 * @return True if this iterator is valid, otherwise false.
		 */
		[[nodiscard]] operator bool() const
		{
			return IsValid();
		}

		/**
		 * @brief Moves this iterator to the next available element.
		 *
		 * @return This iterator.
		 */
		TLinkedListIterator& operator++()
		{
			MoveNext();
			return *this;
		}

		/**
		 * @brief Moves this iterator to the next available element.
		 *
		 * @return A copy of this iterator before incrementing.
		 */
		TLinkedListIterator operator++(int)
		{
			TLinkedListIterator returnValue = *this;
			MoveNext();
			return returnValue;
		}

		/**
		 * @brief Gets the current value this iterator points to.
		 *
		 * @return The current value this iterator points to.
		 */
		PointerType operator->() const
		{
			return &m_CurrentNode->Value;
		}

		/**
		 * @brief Gets the current value this iterator points to.
		 *
		 * @return The current value this iterator points to.
		 */
		ReferenceType operator*() const
		{
			return m_CurrentNode->Value;
		}

	private:

		ListType* m_List = nullptr;
		NodeType* m_PenultimateNode = nullptr;
		NodeType* m_PreviousNode = nullptr;
		NodeType* m_CurrentNode = nullptr;
	};
}

/**
 * @brief Defines a linked list.
 *
 * @tparam T The contained type.
 */
template<typename T>
class TLinkedList
{
	friend Private::TLinkedListNode<T>;

public:

	using NodeType = Private::TLinkedListNode<T>;
	using ElementType = typename NodeType::ElementType;
	using IteratorType = Private::TLinkedListIterator<ElementType, false>;
	using ConstIteratorType = Private::TLinkedListIterator<ElementType, true>;
	using PointerType = typename IteratorType::PointerType;
	using ConstPointerType = typename ConstIteratorType::PointerType;
	using ReferenceType = typename IteratorType::ReferenceType;
	using ConstReferenceType = typename ConstIteratorType::ReferenceType;
	using SizeType = int32;
	using ComparisonTraits = TComparisonTraits<ElementType>;

	/**
	 * @brief Sets default values for this linked list's properties.
	 */
	TLinkedList() = default;

	/**
	 * @brief Constructs a linked list from an initializer list.
	 *
	 * @param initializer The initializer list.
	 */
	TLinkedList(std::initializer_list<T> initializer)
	{
		// NOTE Initializer lists only allow accessing values by const ref
		for (const T& value : initializer)
		{
			Add(value);
		}
	}

	/**
	 * @brief Creates a new linked list, copying the data from another.
	 *
	 * @param other The other linked list to copy data from.
	 */
	TLinkedList(const TLinkedList& other)
	{
		CopyFrom(other);
	}

	/**
	 * @brief Creates a new linked list, transferring ownership of another's data.
	 *
	 * @param other The linked list whose data is to be transferred.
	 */
	TLinkedList(TLinkedList&& other) noexcept
	{
		MoveFrom(MoveTemp(other));
	}

	/**
	 * @brief Destroys this linked list.
	 */
	~TLinkedList()
	{
		Clear();
	}

	/**
	 * @brief Adds the given value to this linked list.
	 *
	 * @param value The value to add.
	 */
	void Add(const ElementType& value)
	{
		AppendValue(value);
	}

	/**
	 * @brief Adds the given value to this linked list.
	 *
	 * @param value The value to add.
	 */
	void Add(ElementType&& value)
	{
		AppendValue(MoveTemp(value));
	}

	/**
	 * @brief Clears this linked list.
	 */
	void Clear()
	{
		m_Head.Reset();
		m_Tail = nullptr;
		m_NumElements = 0;
	}

	/**
	 * @brief Checks to see if this linked list contains a value using the given predicate to check for the value.
	 *
	 * @tparam PredicateType The type of the predicate.
	 * @param predicate The predicate function to use to check for value equality.
	 * @return True if this linked list contains a value as indicated by \p predicate, otherwise false.
	 */
	template<typename PredicateType>
	[[nodiscard]] bool ContainsByPredicate(PredicateType predicate) const
		requires TIsCallable<bool, PredicateType, ConstReferenceType>::Value
	{
		const NodeType* currentNode = m_Head.Get();
		while (currentNode != nullptr)
		{
			if (predicate(currentNode->Value))
			{
				return true;
			}
			currentNode = currentNode->NextNode.Get();
		}
		return false;
	}

	/**
	 * @brief Checks to see if this linked list contains the given value to search for.
	 *
	 * @param value The value to search for.
	 * @return True if this linked list contains \p value, otherwise false.
	 */
	[[nodiscard]] bool Contains(const ElementType& value) const
	{
		return ContainsByPredicate([&value](const ElementType& containedValue)
		{
			return ComparisonTraits::Equals(value, containedValue);
		});
	}

	/**
	 * @brief Creates a non-mutating iterator for this linked list.
	 *
	 * @return A non-mutating iterator for this linked list.
	 */
	[[nodiscard]] ConstIteratorType CreateConstIterator() const
	{
		return ConstIteratorType { this, nullptr, m_Head.Get() };
	}

	/**
	 * @brief Creates a non-mutating iterator for this linked list.
	 *
	 * @return A non-mutating iterator for this linked list.
	 */
	[[nodiscard]] ConstIteratorType CreateIterator() const
	{
		return ConstIteratorType { this, nullptr, m_Head.Get() };
	}

	/**
	 * @brief Creates an iterator for this linked list.
	 *
	 * @return An iterator for this linked list.
	 */
	[[nodiscard]] IteratorType CreateIterator()
	{
		return IteratorType { this, nullptr, m_Head.Get() };
	}

	/**
	 * @brief Constructs a new element in-place at the end of this linked list.
	 *
	 * @tparam ConstructTypes The types of the arguments to forward to the new element's constructor.
	 * @param args The arguments to forward to the new element's constructor.
	 * @return The newly added element.
	 */
	template<typename... ConstructTypes>
	[[nodiscard]] ElementType& Emplace(ConstructTypes&&... args)
	{
		TUniquePtr<NodeType> valueNode = NodeType::ConstructUnique(Forward<ConstructTypes>(args)...);
		AppendNode(MoveTemp(valueNode));

		return m_Tail->Value;
	}

	/**
	 * @brief Checks to see if this linked list is empty.
	 *
	 * @return True if this linked is empty, otherwise false.
	 */
	[[nodiscard]] bool IsEmpty() const
	{
		return m_Head.IsNull();
	}

	/**
	 * @brief Iterates all elements in this linked list.
	 *
	 * @tparam IteratorType The iterator type.
	 * @param iterator The iterator.
	 */
	template<typename IteratorType>
	void Iterate(IteratorType iterator) const
		requires TIsCallable<EIterationDecision, IteratorType, ConstReferenceType>::Value
	{
		const NodeType* currentNode = m_Head.Get();
		while (currentNode != nullptr)
		{
			if (iterator(currentNode->Value) == EIterationDecision::Break)
			{
				break;
			}
			currentNode = currentNode->NextNode.Get();
		}
	}

	/**
	 * @brief Iterates all elements in this linked list.
	 *
	 * @tparam IteratorType The iterator type.
	 * @param iterator The iterator.
	 */
	template<typename IteratorType>
	void Iterate(IteratorType iterator)
		requires TIsCallable<EIterationDecision, IteratorType, ReferenceType>::Value
	{
		NodeType* currentNode = m_Head.Get();
		while (currentNode != nullptr)
		{
			if (iterator(currentNode->Value) == EIterationDecision::Break)
			{
				break;
			}
			currentNode = currentNode->NextNode.Get();
		}
	}

	/**
	 * @brief Gets the number of elements in this linked list.
	 *
	 * @return The number of elements in this linked list.
	 */
	[[nodiscard]] SizeType Num() const
	{
		return m_NumElements;
	}

	/**
	 * @brief Removes all elements from this linked list matching the given predicate.
	 *
	 * @tparam PredicateType The type of the predicate.
	 * @param predicate The predicate function to use to check for value equality.
	 * @return True if any values were removed, otherwise false.
	 */
	template<typename PredicateType>
	[[maybe_unused]] bool RemoveByPredicate(PredicateType predicate)
	{
		bool removedSomething = false;
		for (IteratorType iter = CreateIterator(); iter; ++iter)
		{
			if (predicate(*iter))
			{
				removedSomething |= Remove(iter);
			}
		}
		return removedSomething;
	}

	/**
	 * @brief Removes the value at the given iterator.
	 *
	 * @param iter The iterator.
	 * @return True if the value was removed (guaranteed as long as this list is not empty and the iterator is valid), otherwise false.
	 */
	[[maybe_unused]] bool Remove(IteratorType& iter)
	{
		if (IsEmpty() || iter.IsValid() == false)
		{
			return false;
		}

		UM_ASSERT(iter.BelongsTo(*this), "Attempting to remove from linked list using iterator for a different list");

		// Check if we're deleting the head node
		constexpr TBadge<TLinkedList<T>> badge;
		if (iter.GetCurrentNode(badge) == m_Head.Get())
		{
			// NOTE Cannot combine these calls as the destruction of m_Head in the move assignment will
			//      destroy the rest of the nodes in the list
			TUniquePtr<NodeType> newHead = MoveTemp(m_Head->NextNode);
			m_Head = MoveTemp(newHead);
			--m_NumElements;

			if (m_NumElements == 0)
			{
				m_Tail = nullptr;
			}

			iter.NotifyHeadRemoved(badge, m_Head.Get());
		}
		else
		{
			NodeType* currentNode = iter.GetCurrentNode(badge);
			NodeType* previousNode = iter.GetPreviousNode(badge);

			// Check if we're deleting the tail node
			if (m_Tail == currentNode)
			{
				m_Tail = previousNode;
			}

			TUniquePtr<NodeType> newNext = MoveTemp(currentNode->NextNode);
			previousNode->NextNode = MoveTemp(newNext);
			--m_NumElements;

			iter.NotifyCurrentRemoved(badge);
		}

		return true;
	}

	/**
	 * @brief Removes the given value from this linked list.
	 *
	 * @param value The value to remove.
	 * @return True if any elements were removed, otherwise false.
	 */
	[[maybe_unused]] bool Remove(const ElementType& value)
	{
		for (IteratorType iter = CreateIterator(); iter; ++iter)
		{
			if (ComparisonTraits::Equals(*iter, value))
			{
				return Remove(iter);
			}
		}
		return false;
	}

	/**
	 * @brief Copies another linked list's elements.
	 *
	 * @param other The other linked list to copy.
	 * @return This linked list.
	 */
	TLinkedList& operator=(const TLinkedList& other)
	{
		if (&other != this)
		{
			CopyFrom(other);
		}
		return *this;
	}

	/**
	 * @brief Assumes ownership of another linked list's elements.
	 *
	 * @param other The linked list whose elements should be moved.
	 * @return This linked list.
	 */
	TLinkedList& operator=(TLinkedList&& other) noexcept
	{
		if (&other != this)
		{
			MoveFrom(MoveTemp(other));
		}
		return *this;
	}

	// STL compatibility BEGIN
	IteratorType      begin()       { return CreateIterator(); }
	ConstIteratorType begin() const { return CreateIterator(); }
	IteratorType      end()         { return IteratorType { this, m_Tail, nullptr }; }
	ConstIteratorType end()   const { return ConstIteratorType { this, m_Tail, nullptr }; }
	// STL compatibility END

private:

	/**
	 * @brief Appends the given node to this linked list.
	 *
	 * @param node The node to append.
	 */
	void AppendNode(TUniquePtr<NodeType> node)
	{
		if (m_Head.IsNull())
		{
			m_Head = MoveTemp(node);
			m_Tail = m_Head.Get();
		}
		else
		{
			m_Tail->NextNode = MoveTemp(node);
			m_Tail = m_Tail->NextNode.Get();
		}

		++m_NumElements;
	}

	/**
	 * @brief Appends the given value to this linked list.
	 *
	 * @param value The value to append.
	 */
	void AppendValue(ElementType value)
	{
		TUniquePtr<NodeType> valueNode = MakeUnique<NodeType>(MoveTemp(value));
		AppendNode(MoveTemp(valueNode));
	}

	/**
	 * @brief Copies another linked list's values.
	 *
	 * @param other The other linked list.
	 */
	void CopyFrom(const TLinkedList& other)
	{
		Clear();

		if (other.IsEmpty())
		{
			return;
		}

		// Copy the first element
		m_Head = MakeUnique<NodeType>(other.m_Head->Value);
		m_Tail = &m_Head;

		if (other.m_Head->NextNode.IsNull())
		{
			return;
		}

		// Copy the rest of the elements
		const NodeType* nodeToCopy = other.m_Head->NextNode.Get();
		NodeType* nodeToAppendTo = m_Head.Get();
		while (nodeToCopy != nullptr)
		{
			nodeToAppendTo->NextNode = MakeUnique<NodeType>(nodeToCopy->Value);
			m_Tail = nodeToAppendTo->NextNode.Get();

			nodeToAppendTo = m_Tail;
			nodeToCopy = nodeToCopy->NextNode.Get();
		}
	}

	/**
	 * @brief Assumes ownership of another linked list's values.
	 *
	 * @param other The other linked list.
	 */
	void MoveFrom(TLinkedList&& other)
	{
		m_Head = MoveTemp(other.m_Head);
		m_Tail = other.m_Tail;
		m_NumElements = other.m_NumElements;

		other.m_Tail = nullptr;
		other.m_NumElements = 0;
	}

	// TODO Implement a free-list allocator?
	TUniquePtr<NodeType> m_Head;
	NodeType* m_Tail = nullptr;
	SizeType m_NumElements = 0;
};

template<typename T>
struct TIsZeroConstructible<TLinkedList<T>> : TIsZeroConstructible<TUniquePtr<typename TLinkedList<T>::NodeType>>
{
};

namespace Private
{
	template<typename T, bool IsConst>
	void TLinkedListIterator<T, IsConst>::Remove()
		requires(IsConst == false)
	{
		if (m_List == nullptr)
		{
			return;
		}

		m_List->Remove(*this);
	}
}
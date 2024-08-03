#pragma once

/**
 * @brief Defines a "badge" that can only be created by the specified template type.
 *
 * @tparam FriendType The friend type.
 */
template<typename FriendType>
class TBadge final
{
	friend FriendType;

public:

	constexpr ~TBadge() { }

private:

	constexpr TBadge() = default;
};
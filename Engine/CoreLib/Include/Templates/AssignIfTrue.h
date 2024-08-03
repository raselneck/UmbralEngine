#pragma once

#include "Templates/Move.h"

template<typename T, bool Condition>
class TAssignIfTrue
{
public:

	void operator()(T& target, const T& value) const
	{
		target = value;
	}

	void operator()(T& target, T&& value) const
	{
		target = MoveTemp(value);
	}
};

template<typename T>
class TAssignIfTrue<T, false>
{
public:

	void operator()(T&, const T&) const
	{
	}

	void operator()(T&, T&&) const
	{
	}
};
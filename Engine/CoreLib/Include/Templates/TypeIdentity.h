#pragma once

// https://en.cppreference.com/w/cpp/types/type_identity

template<typename T>
struct TTypeIdentity
{
	using Type = T;
};
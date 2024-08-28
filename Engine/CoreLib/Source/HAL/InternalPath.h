#pragma once

#include "Containers/StringView.h"

namespace Private
{
	/**
	 * @brief Gets a temporary null-terminated string view.
	 *
	 * The string view returned by this function will be a valid copy of \p value until the next
	 * time that this function is called on the same thread. The string view is guaranteed to be
	 * null-terminated, so it will be suitable to be provided to native file system functions
	 * that expect a null-terminated, raw C string.
	 *
	 * @param value The string view to get a temporary clone of.
	 * @return The null-terminated string view.
	 */
	[[nodiscard]] FStringView GetTemporaryNullTerminatedStringView(FStringView value);
}
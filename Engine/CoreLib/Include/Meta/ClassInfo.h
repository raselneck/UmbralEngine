#pragma once

#include "Misc/Badge.h"
#include "Meta/StructInfo.h"
#include "Templates/IsConstructible.h"

/**
 * @brief A function to construct a default instance of a class.
 */
using FConstructClassFunction = void*(*)(TBadge<class FClassInfo>);

/**
 * @brief A function to construct default instance of a class at a location.
 */
using FConstructClassAtLocationFunction = void(*)(TBadge<class FClassInfo>, void* location);

/**
 * @brief Defines meta information about a class.
 */
class FClassInfo : public FStructInfo
{
public:

	/**
	 * @brief Sets default values for this class info's properties.
	 *
	 * @param name The class's name.
	 * @param size The class's size.
	 * @param alignment The class's alignment.
	 * @param baseType The class's base type.
	 */
	FClassInfo(FStringView name, int32 size, int32 alignment, const FClassInfo* baseType);

	/**
	 * @brief Destroys this class info.
	 */
	virtual ~FClassInfo() override = default;

	/**
	 * @brief Gets a value indicating whether or not a default instance of this class can be constructed.
	 *
	 * @return True if a default instance of this class can be constructed, otherwise false.
	 */
	[[nodiscard]] bool CanConstruct() const;

	/**
	 * @brief Gets a value indicating whether or not a default instance of this class can be constructed at a specific location.
	 *
	 * @return True if a default instance of this class can be constructed at a specific location, otherwise false.
	 */
	[[nodiscard]] bool CanConstructAtLocation() const;

	/**
	 * @brief Constructs a default instance of this class.
	 *
	 * @return The constructed instance if possible, otherwise nullptr.
	 */
	[[nodiscard]] void* Construct() const;

	/**
	 * @brief Constructs a default instance of this class at the given memory location.
	 *
	 * The \p location is expected to have been allocated with at least enough space
	 * to accommodate an object of this type. You can use GetSize() to get the size of
	 * an instance of this class to help.
	 *
	 * @param location The memory location.
	 */
	void ConstructAtLocation(void* location) const;

	/**
	 * @brief Sets the function to construct a default instance of this class.
	 *
	 * @param function The function.
	 */
	void SetConstructClassFunction(FConstructClassFunction function);

	/**
	 * @brief Sets the function to construct a default instance of this class at a given memory location.
	 *
	 * @param function The function.
	 */
	void SetConstructClassAtLocationFunction(FConstructClassAtLocationFunction function);

private:

	FConstructClassFunction m_ConstructFunction = nullptr;
	FConstructClassAtLocationFunction m_ConstructAtLocationFunction = nullptr;
};
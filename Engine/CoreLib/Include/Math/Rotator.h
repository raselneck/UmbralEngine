#pragma once

#include "Math/Quaternion.h"

/**
 * @brief Defines a 3D rotation in terms of Euler angles (in degrees).
 */
class FRotator
{
public:

	/** @brief A rotator with zero rotation. */
	static const FRotator Identity;

	/** @brief The rotation's yaw, or rotation around the Y-axis. */
	float Yaw = 0.0f;

	/** @brief The rotation's pitch, or rotation around the X-axis. */
	float Pitch = 0.0f;

	/** @brief The rotation's roll, or rotation around the Z-axis. */
	float Roll = 0.0f;

	/**
	 * @brief Sets default values for this rotator's properties.
	 */
	constexpr FRotator() = default;

	/**
	 * @brief Sets default values for this rotator's properties.
	 *
	 * @param yaw The initial value for the yaw component of this rotator.
	 * @param pitch The initial value for the pitch component of this rotator.
	 * @param roll The initial value for the roll component of this rotator.
	 */
	constexpr FRotator(const float yaw, const float pitch, const float roll)
		: Yaw { yaw }
		, Pitch { pitch }
		, Roll { roll }
	{
	}

	/**
	 * @brief Sets default values for this rotator's properties.
	 *
	 * @param value The quaternion expressing the desired rotation.
	 */
	[[nodiscard]] static FRotator FromQuaternion(const FQuaternion& value);

	/**
	 * @brief Gets the pointer to this matrix's values.
	 *
	 * @return The pointer to this matrix's values.
	 */
	[[nodiscard]] const float* GetValuePtr() const;

	/**
	 * @brief Gets the pointer to this matrix's values.
	 *
	 * @return The pointer to this matrix's values.
	 */
	[[nodiscard]] float* GetValuePtr();

	/**
	 * @brief Retrieves a quaternion representing the same rotation as this rotator.
	 *
	 * @return A quaternion representing the same rotation as this rotator.
	 */
	[[nodiscard]] FQuaternion ToQuaternion() const;
};
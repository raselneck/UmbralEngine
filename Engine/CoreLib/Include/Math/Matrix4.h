#pragma once

#include "Containers/Optional.h"
#include "Containers/StaticArray.h"
#include "Math/Matrix3.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Misc/StringFormatting.h"
#include "Templates/TypeTraits.h"

class FRotator;
class FQuaternion;

/**
 * @brief Represents the right-handed 4x4 floating point matrix, which can store translation, scale, and rotation information.
 */
class FMatrix4
{
public:

	using ArrayType = TStaticArray<float, 16>;

	/** @brief The 4x4 identity matrix. */
	static const FMatrix4 Identity;

	/** @brief The first row and first column value. */
	float M11 = 0.0f;

	/** @brief The first row and second column value. */
	float M12 = 0.0f;

	/** @brief The first row and third column value. */
	float M13 = 0.0f;

	/** @brief The first row and fourth column value. */
	float M14 = 0.0f;

	/** @brief The second row and first column value. */
	float M21 = 0.0f;

	/** @brief The second row and second column value. */
	float M22 = 0.0f;

	/** @brief The second row and third column value. */
	float M23 = 0.0f;

	/** @brief The second row and fourth column value. */
	float M24 = 0.0f;

	/** @brief The third row and first column value. */
	float M31 = 0.0f;

	/** @brief The third row and second column value. */
	float M32 = 0.0f;

	/** @brief The third row and third column value. */
	float M33 = 0.0f;

	/** @brief The third row and fourth column value. */
	float M34 = 0.0f;

	/** @brief The fourth row and first column value. */
	float M41 = 0.0f;

	/** @brief The fourth row and second column value. */
	float M42 = 0.0f;

	/** @brief The fourth row and third column value. */
	float M43 = 0.0f;

	/** @brief The fourth row and fourth column value. */
	float M44 = 0.0f;

	// clang-format off

	/**
	 * @brief Sets default values for this 4x4 matrix's properties.
	 */
	constexpr FMatrix4() = default;

	/**
	 * @brief Sets default values for this 4x4 matrix's properties.
	 *
	 * @param m11 The initial value for the first column, first row.
	 * @param m12 The initial value for the first column, second row.
	 * @param m13 The initial value for the first column, third row.
	 * @param m14 The initial value for the first column, fourth row.
	 * @param m21 The initial value for the second column, first row.
	 * @param m22 The initial value for the second column, second row.
	 * @param m23 The initial value for the second column, third row.
	 * @param m24 The initial value for the second column, fourth row.
	 * @param m31 The initial value for the third column, first row.
	 * @param m32 The initial value for the third column, second row.
	 * @param m33 The initial value for the third column, third row.
	 * @param m34 The initial value for the third column, fourth row.
	 * @param m41 The initial value for the fourth column, first row.
	 * @param m42 The initial value for the fourth column, second row.
	 * @param m43 The initial value for the fourth column, third row.
	 * @param m44 The initial value for the fourth column, fourt row.
	 */
	constexpr FMatrix4(float m11, float m12, float m13, float m14,
	                   float m21, float m22, float m23, float m24,
	                   float m31, float m32, float m33, float m34,
	                   float m41, float m42, float m43, float m44)
		: M11 { m11 }, M12 { m12 }, M13 { m13 }, M14 { m14 }
		, M21 { m21 }, M22 { m22 }, M23 { m23 }, M24 { m24 }
		, M31 { m31 }, M32 { m32 }, M33 { m33 }, M34 { m34 }
		, M41 { m41 }, M42 { m42 }, M43 { m43 }, M44 { m44 }
	{
	}

	// clang-format on

	// TODO Add

	/**
	 * @brief Creates a spherical billboard that rotates around a specified object position.
	 *
	 * @param objectPosition The object's position.
	 * @param cameraPosition The camera's position.
	 * @param cameraUp The camera up direction. Expected to be normalized.
	 * @param cameraForward The camera forward direction. Expected to be normalized.
	 * @param result The created billboard matrix.
	 */
	static void CreateBillboard(const FVector3& objectPosition,
	                            const FVector3& cameraPosition,
	                            const FVector3& cameraUp,
	                            const TOptional<FVector3>& cameraForward,
	                            FMatrix4& result);

	/**
	 * @brief Creates a spherical billboard that rotates around a specified object position.
	 *
	 * @param objectPosition The object's position.
	 * @param cameraPosition The camera's position.
	 * @param cameraUp The camera up direction. Expected to be normalized.
	 * @param cameraForward The camera forward direction. Expected to be normalized.
	 * @return The created billboard matrix.
	 */
	static FMatrix4 CreateBillboard(const FVector3& objectPosition,
	                                const FVector3& cameraPosition,
	                                const FVector3& cameraUp,
	                                const TOptional<FVector3>& cameraForward)
	{
		FMatrix4 result;
		CreateBillboard(objectPosition, cameraPosition, cameraUp, cameraForward, result);
		return result;
	}

	/**
	 * @brief Creates a cylindrical billboard that rotates around a specified axis.
	 *
	 * @param objectPosition Position of the object the billboard will rotate around.
	 * @param cameraPosition Position of the camera.
	 * @param rotateAxis Axis to rotate the billboard around. Expected to be normalized.
	 * @param cameraForward The forward vector of the camera. Expected to be normalized.
	 * @param objectForward The forward vector of the object. Expected to be normalized.
	 * @param result The created billboard matrix.
	 */
	static void CreateConstrainedBillboard(const FVector3& objectPosition,
	                                       const FVector3& cameraPosition,
	                                       const FVector3& rotateAxis,
	                                       const TOptional<FVector3>& cameraForward,
	                                       const TOptional<FVector3>& objectForward,
	                                       FMatrix4& result);

	/**
	 * @brief Creates a cylindrical billboard that rotates around a specified axis.
	 *
	 * @param objectPosition Position of the object the billboard will rotate around.
	 * @param cameraPosition Position of the camera.
	 * @param rotateAxis Axis to rotate the billboard around.
	 * @param cameraForward The forward vector of the camera.
	 * @param objectForward The forward vector of the object.
	 * @return The created billboard matrix.
	 */
	static FMatrix4 CreateConstrainedBillboard(const FVector3& objectPosition,
	                                           const FVector3& cameraPosition,
	                                           const FVector3& rotateAxis,
	                                           const TOptional<FVector3>& cameraForward,
	                                           const TOptional<FVector3>& objectForward)
	{
		FMatrix4 result;
		CreateConstrainedBillboard(objectPosition, cameraPosition, rotateAxis, cameraForward, objectForward,
		                           result);
		return result;
	}

	/**
	 * @brief Creates a new FMatrix4 that rotates around an arbitrary vector.
	 *
	 * @param axis The axis to rotate around. Expected to be normalized.
	 * @param angle The angle, in radians, to rotate around the axis.
	 * @param result The created FMatrix4.
	 */
	static void CreateFromAxisAngle(const FVector3& axis, float angle, FMatrix4& result);

	/**
	 * @brief Creates a new FMatrix4 that rotates around an arbitrary vector.
	 *
	 * @param axis The axis to rotate around. Expected to be normalized.
	 * @param angle The angle, in radians, to rotate around the axis.
	 * @return The created FMatrix4.
	 */
	static FMatrix4 CreateFromAxisAngle(const FVector3& axis, const float angle)
	{
		FMatrix4 result;
		CreateFromAxisAngle(axis, angle, result);
		return result;
	}

	/**
	 * @brief Creates a rotation FMatrix4 from a FRotator.
	 *
	 * @param value The FRotator to create the FMatrix4 from.
	 * @param result The created FMatrix4.
	 */
	static void CreateFromRotator(const FRotator& value, FMatrix4& result);

	/**
	 * @brief Creates a rotation FMatrix4 from a FRotator.
	 *
	 * @param value The FRotator to create the FMatrix4 from.
	 * @return The created FMatrix4.
	 */
	static FMatrix4 CreateFromRotator(const FRotator& value)
	{
		FMatrix4 result;
		CreateFromRotator(value, result);
		return result;
	}

	/**
	 * @brief Creates a rotation FMatrix4 from a FQuaternion.
	 *
	 * @param value The quaternion to create the FMatrix4 from.
	 * @param result The created FMatrix4.
	 */
	static void CreateFromQuaternion(const FQuaternion& value, FMatrix4& result);

	/**
	 * @brief Creates a rotation FMatrix4 from a quaternion.
	 *
	 * @param value The quaternion to create the FMatrix4 from.
	 * @return The created FMatrix4.
	 */
	static FMatrix4 CreateFromQuaternion(const FQuaternion& value)
	{
		FMatrix4 result;
		CreateFromQuaternion(value, result);
		return result;
	}

	/**
	 * @brief Fills in a rotation matrix from a specified yaw, pitch, and roll.
	 *
	 * @param yaw The angle of rotation, in radians, around the Y-axis.
	 * @param pitch The angle of rotation, in radians, around the X-axis.
	 * @param roll The angle of rotation, in radians, around the Z-axis.
	 * @param result An existing matrix filled in to represent the specified yaw, pitch, and roll.
	 */
	static void CreateFromYawPitchRoll(float yaw, float pitch, float roll, FMatrix4& result);

	/**
	 * @brief Fills in a rotation matrix from a specified yaw, pitch, and roll.
	 *
	 * @param yaw The angle of rotation, in radians, around the Y-axis.
	 * @param pitch The angle of rotation, in radians, around the X-axis.
	 * @param roll The angle of rotation, in radians, around the Z-axis.
	 * @return A new matrix filled in to represent the specified yaw, pitch, and roll.
	 */
	static FMatrix4 CreateFromYawPitchRoll(const float yaw, const float pitch, const float roll)
	{
		FMatrix4 result;
		CreateFromYawPitchRoll(yaw, pitch, roll, result);
		return result;
	}

	/**
	 * @brief Creates a view matrix.
	 *
	 * @param cameraPosition The position of the camera.
	 * @param cameraTarget The position of the camera to look at.
	 * @param cameraUp The camera's local up vector.
	 * @param result The created view matrix.
	 */
	static void CreateLookAt(const FVector3& cameraPosition,
	                         const FVector3& cameraTarget,
	                         const FVector3& cameraUp,
	                         FMatrix4& result);

	/**
	 * @brief Creates a view matrix.
	 *
	 * @param cameraPosition The position of the camera.
	 * @param cameraTarget The position of the camera to look at.
	 * @param cameraUp The camera's local up vector.
	 * @return The created view matrix.
	 */
	static FMatrix4 CreateLookAt(const FVector3& cameraPosition,
	                             const FVector3& cameraTarget,
	                             const FVector3& cameraUp)
	{
		FMatrix4 result;
		CreateLookAt(cameraPosition, cameraTarget, cameraUp, result);
		return result;
	}

	/**
	 * @brief Builds an orthogonal projection matrix.
	 *
	 * @param width Width ofthe view volume.
	 * @param height Height of the view volume.
	 * @param zNearPlane Minimum depth of the view volume.
	 * @param zFarPlane Maximum depth of the view volume.
	 * @param result The projection matrix.
	 */
	static void CreateOrthographic(float width, float height,
	                               float zNearPlane, float zFarPlane,
	                               FMatrix4& result);

	/**
	 * @brief Builds an orthogonal projection matrix.
	 *
	 * @param width Width of the view volume.
	 * @param height Height of the view volume.
	 * @param zNearPlane Minimum depth of the view volume.
	 * @param zFarPlane Maximum depth of the view volume.
	 * @return The projection matrix.
	 */
	static FMatrix4 CreateOrthographic(const float width, const float height,
	                                   const float zNearPlane, const float zFarPlane)
	{
		FMatrix4 result;
		CreateOrthographic(width, height, zNearPlane, zFarPlane, result);
		return result;
	}

	/**
	 * @brief Builds a customized, orthogonal projection matrix.
	 *
	 * @param left The left of the view volume.
	 * @param right The right of the view volume.
	 * @param bottom The bottom of the view volume.
	 * @param top The top of the view volume.
	 * @param zNearPlane Minimum depth of the view volume.
	 * @param zFarPlane Maximum depth of the view volume.
	 * @param result The projection matrix.
	 */
	static void CreateOrthographicOffCenter(float left, float right,
	                                        float bottom, float top,
	                                        float zNearPlane, float zFarPlane,
	                                        FMatrix4& result);

	/**
	 * @brief Builds a customized, orthogonal projection matrix.
	 *
	 * @param left The left of the view volume.
	 * @param right The right of the view volume.
	 * @param bottom The bottom of the view volume.
	 * @param top The top of the view volume.
	 * @param zNearPlane Minimum depth of the view volume.
	 * @param zFarPlane Maximum depth of the view volume.
	 * @return The projection matrix.
	 */
	static FMatrix4 CreateOrthographicOffCenter(const float left, const float right,
	                                            const float bottom, const float top,
	                                            const float zNearPlane, const float zFarPlane)
	{
		FMatrix4 result;
		CreateOrthographicOffCenter(left, right, bottom, top, zNearPlane, zFarPlane, result);
		return result;
	}

	/**
	 * @brief Builds a customized, orthogonal projection matrix.
	 *
	 * @param left The left of the view volume.
	 * @param right The right of the view volume.
	 * @param bottom The bottom of the view volume.
	 * @param top The top of the view volume.
	 * @param result The projection matrix.
	 */
	static void CreateOrthographicOffCenter(const float left, const float right,
	                                        const float bottom, const float top,
	                                        FMatrix4& result)
	{
		CreateOrthographicOffCenter(left, right, bottom, top, 0.0f, 1.0f, result);
	}

	/**
	 * @brief Builds a customized, orthogonal projection matrix.
	 *
	 * @param left The left of the view volume.
	 * @param right The right of the view volume.
	 * @param bottom The bottom of the view volume.
	 * @param top The top of the view volume.
	 * @return The projection matrix.
	 */
	static FMatrix4 CreateOrthographicOffCenter(float left, float right, float bottom, float top)
	{
		FMatrix4 result;
		CreateOrthographicOffCenter(left, right, bottom, top, 0.0f, 1.0f, result);
		return result;
	}

	/**
	 * @brief Builds a perspective projection matrix based on a field of view.
	 *
	 * @param fieldOfView Field of view in radians.
	 * @param aspectRatio Aspect ratio, defined as view space width divided by height.
	 * @param nearPlaneDistance Distance to the near view plane.
	 * @param farPlaneDistance Distance to the far view plane.
	 * @param result The perspective projection matrix.
	 */
	static void CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio,
	                                         float nearPlaneDistance, float farPlaneDistance,
	                                         FMatrix4& result);

	/**
	 * @brief Builds a perspective projection matrix based on a field of view.
	 *
	 * @param fieldOfView Field of view in radians.
	 * @param aspectRatio Aspect ratio, defined as view space width divided by height.
	 * @param nearPlaneDistance Distance to the near view plane.
	 * @param farPlaneDistance Distance to the far view plane.
	 * @return The perspective projection matrix.
	 */
	static FMatrix4 CreatePerspectiveFieldOfView(const float fieldOfView, const float aspectRatio,
	                                             const float nearPlaneDistance, const float farPlaneDistance)
	{
		FMatrix4 result;
		CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance, result);
		return result;
	}

	/**
	 * @brief Creates a rotation transformation matrix representing rotation around the X axis.
	 *
	 * @param angle The angle of rotation, in degrees.
	 * @param result The result matrix.
	 */
	static void CreateRotationX(float angle, FMatrix4& result);

	/**
	 * @brief Creates a rotation transformation matrix representing rotation around the X axis.
	 *
	 * @param angle The angle of rotation, in degrees.
	 * @returns The transformation matrix.
	 */
	static FMatrix4 CreateRotationX(const float angle)
	{
		FMatrix4 result;
		CreateRotationX(angle, result);
		return result;
	}

	/**
	 * @brief Creates a rotation transformation matrix representing rotation around the Y axis.
	 *
	 * @param angle The angle of rotation, in degrees.
	 * @param result The result matrix.
	 */
	static void CreateRotationY(float angle, FMatrix4& result);

	/**
	 * @brief Creates a rotation transformation matrix representing rotation around the Y axis.
	 *
	 * @param angle The angle of rotation, in degrees.
	 * @returns The transformation matrix.
	 */
	static FMatrix4 CreateRotationY(const float angle)
	{
		FMatrix4 result;
		CreateRotationY(angle, result);
		return result;
	}

	/**
	 * @brief Creates a rotation transformation matrix representing rotation around the Z axis.
	 *
	 * @param angle The angle of rotation, in degrees.
	 * @param result The result matrix.
	 */
	static void CreateRotationZ(float angle, FMatrix4& result);

	/**
	 * @brief Creates a rotation transformation matrix representing rotation around the Z axis.
	 *
	 * @param angle The angle of rotation, in degrees.
	 * @returns The transformation matrix.
	 */
	static FMatrix4 CreateRotationZ(const float angle)
	{
		FMatrix4 result;
		CreateRotationZ(angle, result);
		return result;
	}

	/**
	 * @brief Creates a scaling transformation matrix.
	 *
	 * @param x The scale along the X axis.
	 * @param y The scale along the Y axis.
	 * @param z The scale along the Z axis.
	 * @param result The result matrix.
	 */
	static constexpr void CreateScale(const float x, const float y, const float z, FMatrix4& result)
	{
		// clang-format off
		result.M11 = x;    result.M12 = 0.0f; result.M13 = 0.0f; result.M14 = 0.0f;
		result.M21 = 0.0f; result.M22 = y;    result.M23 = 0.0f; result.M24 = 0.0f;
		result.M31 = 0.0f; result.M32 = 0.0f; result.M33 = z;    result.M34 = 0.0f;
		result.M41 = 0.0f; result.M42 = 0.0f; result.M43 = 0.0f; result.M34 = 1.0f;
		// clang-format on
	}

	/**
	 * @brief Creates a scaling transformation matrix.
	 *
	 * @param x The scale along the X axis.
	 * @param y The scale along the Y axis.
	 * @param z The scale along the Z axis.
	 * @returns The transformation matrix.
	 */
	static constexpr FMatrix4 CreateScale(const float x, const float y, const float z)
	{
		FMatrix4 result;
		result.M11 = x;
		result.M22 = y;
		result.M33 = z;
		result.M44 = 1.0f;
		return result;
	}

	/**
	 * @brief Creates a scaling transformation matrix.
	 *
	 * @param scale The scale.
	 * @param result The result matrix.
	 */
	static constexpr void CreateScale(const FVector3& scale, FMatrix4& result)
	{
		CreateScale(scale.X, scale.Y, scale.Z, result);
	}

	/**
	 * @brief Creates a scaling transformation matrix.
	 *
	 * @param scale The scale.
	 * @returns The transformation matrix.
	 */
	static constexpr FMatrix4 CreateScale(const FVector3& scale)
	{
		FMatrix4 result;
		result.M11 = scale.X;
		result.M22 = scale.Y;
		result.M33 = scale.Z;
		result.M44 = 1.0f;
		return result;
	}

	/**
	 * @brief Creates a translation transformation matrix.
	 *
	 * @param x The position along the X axis.
	 * @param y The position along the Y axis.
	 * @param z The position along the Z axis.
	 * @param result The result matrix.
	 */
	static constexpr void CreateTranslation(const float x, const float y, const float z, FMatrix4& result)
	{
		// clang-format off
		result.M11 = 1.0;  result.M12 = 0.0f; result.M13 = 0.0f; result.M14 = 0.0f;
		result.M21 = 0.0f; result.M22 = 1.0;  result.M23 = 0.0f; result.M24 = 0.0f;
		result.M31 = 0.0f; result.M32 = 0.0f; result.M33 = 1.0f; result.M34 = 0.0f;
		result.M41 = x;    result.M42 = y;    result.M43 = z;    result.M34 = 1.0f;
		// clang-format on
	}

	/**
	 * @brief Creates a translation transformation matrix.
	 *
	 * @param x The position along the X axis.
	 * @param y The position along the Y axis.
	 * @param z The position along the Z axis.
	 * @returns The transformation matrix.
	 */
	static constexpr FMatrix4 CreateTranslation(const float x, const float y, const float z)
	{
		FMatrix4 result;
		result.M11 = 1.0f;
		result.M22 = 1.0f;
		result.M33 = 1.0f;
		result.M41 = x;
		result.M42 = y;
		result.M43 = z;
		result.M44 = 1.0f;
		return result;
	}

	/**
	 * @brief Creates a translation transformation matrix.
	 *
	 * @param position The position.
	 * @param result The result matrix.
	 */
	static constexpr void CreateTranslation(const FVector3& position, FMatrix4& result)
	{
		CreateTranslation(position.X, position.Y, position.Z, result);
	}

	/**
	 * @brief Creates a translation transformation matrix.
	 *
	 * @param position The position.
	 * @returns The transformation matrix.
	 */
	static constexpr FMatrix4 CreateTranslation(const FVector3& position)
	{
		return CreateTranslation(position.X, position.Y, position.Z);
	}

	// TODO Divide

	/**
	 * @brief Gets the backward vector defined by this matrix.
	 *
	 * @return The backward vector.
	 */
	[[nodiscard]] FVector3 GetBackward() const
	{
		return FVector3::CreateNormalized(M31, M32, M33);
	}

	// TODO GetDeterminant

	/**
	 * @brief Gets the down vector defined by this matrix.
	 *
	 * @return The down vector.
	 */
	[[nodiscard]] FVector3 GetDown() const
	{
		return FVector3::CreateNormalized(-M21, -M22, -M23);
	}

	/**
	 * @brief Gets the forward vector defined by this FMatrix4.
	 *
	 * @return The forward vector.
	 */
	[[nodiscard]] FVector3 GetForward() const
	{
		return FVector3::CreateNormalized(-M31, -M32, -M33);
	}

	// TODO GetInverse

	/**
	 * @brief Gets the left vector defined by this FMatrix4.
	 *
	 * @return The left vector.
	 */
	[[nodiscard]] FVector3 GetLeft() const
	{
		return FVector3::CreateNormalized(-M11, -M12, -M13);
	}

	/**
	 * @brief Gets the right vector defined by this FMatrix4.
	 *
	 * @return The right vector.
	 */
	[[nodiscard]] FVector3 GetRight() const
	{
		return FVector3::CreateNormalized(M11, M12, M13);
	}

	/**
	 * @brief Gets the translation vector defined by this FMatrix4.
	 *
	 * @return The translation vector.
	 */
	[[nodiscard]] constexpr FVector3 GetTranslation() const
	{
		return FVector3 { M41, M42, M43 };
	}

	// TODO GetTransposed

	/**
	 * @brief Gets the up vector defined by this FMatrix4.
	 *
	 * @return The up vector.
	 */
	[[nodiscard]] FVector3 GetUp() const
	{
		return FVector3::CreateNormalized(M21, M22, M23);
	}

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

	// TODO Invert

	/**
	 * @brief Multiplies two matrices together.
	 *
	 * @param first The first matrix.
	 * @param second The second matrix.
	 * @param result The result matrix to populate.
	 */
	static void Multiply(const FMatrix4& first, const FMatrix4& second, FMatrix4& result);

	/**
	 * @brief Multiplies two matrices together.
	 *
	 * @param first The first matrix.
	 * @param second The second matrix.
	 * @returns The product matrix.
	 */
	[[nodiscard]] static FMatrix4 Multiply(const FMatrix4& first, const FMatrix4& second)
	{
		FMatrix4 result;
		Multiply(first, second, result);
		return result;
	}

	// TODO Multiply with FQuaternion
	// TODO Multiply with FVector4
	// TODO Multiply with scalar

	/**
	 * @brief Sets this matrix's values to represent the identity matrix.
	 */
	void SetIdentity();

	// TODO SetTranslation

	// TODO Subtract

	/**
	 * @brief Gets an array with values equivalent to this matrix.
	 *
	 * @param result The array to copy this matrix's values to.
	 */
	void ToArray(ArrayType& result) const;

	/**
	 * @brief Gets an array with values equivalent to this matrix.
	 *
	 * @return An array with values equivalent to this matrix.
	 */
	[[nodiscard]] ArrayType ToArray() const
	{
		ArrayType result;
		ToArray(result);
		return result;
	}

	/**
	 * @brief Converts this 4x4 matrix to a 3x3 normal matrix.
	 *
	 * @return This 4x4 matrix as a 3x3 normal matrix.
	 */
	[[nodiscard]] FMatrix3 ToNormalMatrix() const;

	// TODO Transpose

	// TODO Comment
	FMatrix4& operator*=(const FMatrix4& other)
	{
		const FMatrix4 thisCopy = *this;
		Multiply(thisCopy, other, *this);
		return *this;
	}

	// TODO Comment
	friend FMatrix4 operator*(FMatrix4 first, const FMatrix4& second)
	{
		first *= second;
		return first;
	}
};

template<>
struct TFormatter<FMatrix4>
{
	void BuildString(const FMatrix4& value, FStringBuilder& builder);
	bool Parse(FStringView formatString);
};
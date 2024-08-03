#pragma once

#include "Containers/Any.h"
#include "Containers/Array.h"
#include "Containers/Function.h"
#include "Engine/Error.h"
#include "Meta/AttributeInfo.h"
#include "Misc/EnumMacros.h"

/**
 * @brief An enumeration of flags that can describe a function.
 */
enum class EFunctionFlags : uint32
{
	None        = 0,
	Static      = (1 << 0),
	Free        = (1 << 1),
	Instance    = (1 << 2),
	Const       = (1 << 3),
	Public      = (1 << 4),
	Protected   = (1 << 5),
	Private     = (1 << 6),
	Virtual     = (1 << 7)
};

ENUM_FLAG_OPERATORS(EFunctionFlags)

/**
 * @brief Defines meta information about a function parameter.
 */
class FFunctionParameterInfo : public FAttributeCollectionInfo
{
public:

	/**
	 * @brief Sets default values for this parameter's properties.
	 *
	 * @param name The parameter's name.
	 * @param type The parameter's type.
	 */
	FFunctionParameterInfo(FStringView name, const FTypeInfo* type);

	/**
	 * @brief Destroys this function parameter information.
	 */
	virtual ~FFunctionParameterInfo() override = default;

	/**
	 * @brief Gets this parameter's name.
	 *
	 * @return This parameter's name.
	 */
	[[nodiscard]] FStringView GetName() const
	{
		return m_Name;
	}

	/**
	 * @brief Gets this parameter's type.
	 *
	 * @return This parameter's type.
	 */
	[[nodiscard]] const FTypeInfo* GetType() const
	{
		return m_Type;
	}

private:

	FStringView m_Name;
	const FTypeInfo* m_Type = nullptr;
};

/**
 * @brief Defines meta information about a function.
 */
class FFunctionInfo : public FAttributeCollectionInfo
{
public:

	/**
	 * @brief The callback signature for invoking functions.
	 */
	using InvokeFunction = TFunction<FAny(const void*, TSpan<FAny>)>;

	/**
	 * @brief Sets default values for this function's properties.
	 *
	 * @param name The function's name.
	 * @param returnType The function's return type.
	 * @param flags The function's flags.
	 */
	FFunctionInfo(FStringView name, const FTypeInfo* returnType, EFunctionFlags flags);

	/**
	 * @brief Destroys this function information.
	 */
	virtual ~FFunctionInfo() override = default;

	/**
	 * @brief Adds a parameter to this function info.
	 *
	 * @param name The parameter's name.
	 * @param type The parameter's type.
	 */
	void AddParameter(FStringView name, const FTypeInfo* type);

	/**
	 * @brief Gets the number of parameters this function accepts.
	 *
	 * @return The number of parameters this function accepts.
	 */
	[[nodiscard]] int32 GetNumParameters() const
	{
		return m_Parameters.Num();
	}

	/**
	 * @brief Gets this function's name.
	 *
	 * @return This function's name.
	 */
	[[nodiscard]] FStringView GetName() const
	{
		return m_Name;
	}

	/**
	 * @brief Gets the parameter at the given index.
	 *
	 * @param index The index.
	 * @return The parameter at \p index, or nullptr if none exists.
	 */
	[[nodiscard]] const FFunctionParameterInfo* GetParameter(int32 index) const;

	/**
	 * @brief Gets this function's parameters.
	 *
	 * @return This function's parameters.
	 */
	[[nodiscard]] TSpan<const FFunctionParameterInfo> GetParameters() const;

	/**
	 * @brief Gets this function's return type.
	 *
	 * @return This function's return type.
	 */
	[[nodiscard]] const FTypeInfo* GetReturnType() const
	{
		return m_ReturnType;
	}

	/**
	 * @brief Checks to see if this function has any of the given flags.
	 *
	 * @param flags The flags to check for.
	 * @return True if this function has the given flags, otherwise false.
	 */
	[[nodiscard]] bool HasFlag(const EFunctionFlags flags) const
	{
		return ::HasFlag(m_Flags, flags);
	}

	/**
	 * @brief Attempts to invoke this function. Will only work if this is a static or free function.
	 *
	 * @param parameters The function parameters.
	 * @return The function's return value.
	 */
	[[nodiscard]] TErrorOr<FAny> Invoke(TSpan<FAny> parameters);

	/**
	 * @brief Attempts to invoke this function. If this is a static or free function, then \p instance will be
	 *        ignored. Will not work if this is a const instance function.
	 *
	 * @param instance The object instance to use when invoking the function.
	 * @param parameters The function parameters.
	 * @return The function's return value.
	 */
	[[nodiscard]] TErrorOr<FAny> Invoke(void* instance, TSpan<FAny> parameters);

	/**
	 * @brief Attempts to invoke this function. If this is a static or free function, then \p instance will be ignored.
	 *
	 * @param instance The object instance to use when invoking the function.
	 * @param parameters The function parameters.
	 * @return The function's return value.
	 */
	[[nodiscard]] TErrorOr<FAny> Invoke(const void* instance, TSpan<FAny> parameters);

	/**
	 * @brief Sets the function that can be used to invoke this function.
	 *
	 * @param invokeFunction The invoke function.
	 */
	void SetInvokeFunction(InvokeFunction invokeFunction);

private:

	InvokeFunction m_InvokeFunction;
	TArray<FFunctionParameterInfo> m_Parameters;
	FStringView m_Name;
	const FTypeInfo* m_ReturnType = nullptr;
	EFunctionFlags m_Flags = EFunctionFlags::None;
};
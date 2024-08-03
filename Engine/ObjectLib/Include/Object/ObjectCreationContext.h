#pragma once

#include "Containers/Any.h"
#include "Containers/HashMap.h"
#include "Containers/StringView.h"

/**
 * @brief Defines an object creation context, which can define arbitrary parameters.
 */
class FObjectCreationContext
{
public:

	using HashMapType = THashMap<FStringView, FAny>;

	UM_DISABLE_COPY(FObjectCreationContext);
	UM_DEFAULT_MOVE(FObjectCreationContext);

	/**
	 * @brief Sets default values for this object creation context's properties.
	 */
	FObjectCreationContext() = default;

	/**
	 * @brief Destroys this object creation context.
	 */
	~FObjectCreationContext() = default;

	/**
	 * @brief Gets the parameter with the given name.
	 *
	 * @tparam T The parameter's value type.
	 * @param name The parameter's name.
	 * @return The parameter's value.
	 */
	template<typename T>
	[[nodiscard]] T* GetParameter(const FStringView name) const
	{
		FAny* value = GetParameterAsAny(name);
		if (value == nullptr)
		{
			return nullptr;
		}

		return value->GetValue<T>();
	}

	/**
	 * @brief Gets the parameter. Will assert if a parameter with the given name does not exist.
	 *
	 * @tparam T The parameter's value type.
	 * @param name The parameter's name.
	 * @return The parameter's value.
	 */
	template<typename T>
	[[nodiscard]] T& GetParameterChecked(const FStringView name) const
	{
		T* value = GetParameter<T>(name);
		UM_ASSERT(value != nullptr, "A parameter with the given name does not exist");

		return *value;
	}

	/**
	 * @brief Checks to see if this creation context has a parameter with the given name.
	 *
	 * @param name The parameter name.
	 * @return True if there was a parameter with the given name, otherwise false.
	 */
	[[nodiscard]] bool HasParameter(FStringView name) const;

	/**
	 * @brief Sets a parameter.
	 *
	 * @tparam T The value's type.
	 * @param name The parameter's name.
	 * @param value The parameter's value.
	 * @return True if the parameter was set, otherwise false.
	 */
	template<typename T>
	[[maybe_unused]] bool SetParameter(FStringView name, T value)
	{
		FAny opaqueValue;
		opaqueValue.Emplace<T>(MoveTemp(value));

		return SetParameterAsAny(name, MoveTemp(opaqueValue));
	}

#if WITH_TESTING
	/**
	 * @brief Gets this context's parameter hash map.
	 *
	 * @return This context's parameter hash map.
	 */
	[[nodiscard]] const HashMapType& DebugGetParameters() const
	{
		return m_Parameters;
	}
#endif

private:

	/**
	 * @brief Gets the parameter with the given name.
	 *
	 * @param name The parameter's name.
	 * @return The parameter's value, or nullptr if a parameter with the given \p name was not found.
	 */
	[[nodiscard]] FAny* GetParameterAsAny(FStringView name) const;

	/**
	 * @brief Sets a parameter.
	 *
	 * @param name The parameter's name.
	 * @param value The parameter's value.
	 * @return True if the parameter was set, otherwise false.
	 */
	[[nodiscard]] bool SetParameterAsAny(FStringView name, FAny value);

	HashMapType m_Parameters;
};
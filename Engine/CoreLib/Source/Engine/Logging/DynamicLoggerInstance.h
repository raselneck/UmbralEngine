#pragma once

#include "Containers/Array.h"
#include "Engine/Logging/LogListener.h"
#include "Memory/UniquePtr.h"

/**
 * @brief Defines a logger instance that accepts a dynamic number of log listeners.
 */
class FDynamicLoggerInstance : public Private::ILoggerInstance
{
public:

	/** @inheritdoc */
	virtual bool Initialize() override;

	/**
	 * @brief Checks to see if this dynamic logger instance is initialized.
	 *
	 * @return True if this dynamic logger instance is initialized, otherwise false.
	 */
	[[nodiscard]] bool IsInitialized() const
	{
		return m_Listeners.Num() > 0;
	}

	/** @inheritdoc */
	virtual void Flush() override;

	/** @inheritdoc */
	virtual void Shutdown() override;

	/** @inheritdoc */
	virtual void WriteImpl(ELogLevel logLevel, FStringView message, TSpan<Private::FStringFormatArgument> messageArgs) override;

private:

	TArray<TUniquePtr<ILogListener>> m_Listeners;
};
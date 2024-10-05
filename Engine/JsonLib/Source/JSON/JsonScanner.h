#pragma once

#include "Parsing/Scanner.h"

/**
 * @brief Defines a JSON token scanner.
 */
class FJsonScanner : public FScanner
{
public:

	/**
	 * @brief Sets default values for this scanner's properties.
	 */
	FJsonScanner();

	/**
	 * @brief Destroys this JSON token scanner.
	 */
	virtual ~FJsonScanner() override = default;

protected:

	/** @inheritdoc FScanner::TryScanTokenFromCurrentPosition() */
	virtual bool TryScanTokenFromCurrentPosition() override;
};

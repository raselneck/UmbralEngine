#pragma once

#include "Engine/IntTypes.h"
#include "Engine/Platform.h"

/**
 * @brief The default main entry point. This is defined by Umbral.
 */
int UMBRAL_EXPORT main(int, char**);

/**
 * @brief This is the main entry point to be implemented by applications using Umbral.
 *
 * @return The application's exit code.
 */
extern "C" int32 UmbralMain();
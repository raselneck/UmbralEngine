#pragma once

#include "Containers/Span.h"
#include "Containers/String.h"
#include "Engine/Error.h"

namespace USL
{
	/**
	 * @brief Attempts to convert a SPIR-V blob to GLSL source code.
	 *
	 * @param spirvBlob The SPIR-V blob.
	 * @return The resulting GLSL source code, or the error that was encountered.
	 */
	TErrorOr<FString> ConvertSpirvToGlsl(TSpan<const uint8> blob);

	/**
	 * @brief Attempts to convert a SPIR-V blob to GLSL source code.
	 *
	 * @param spirvBlob The SPIR-V blob.
	 * @return The resulting GLSL source code, or the error that was encountered.
	 */
	TErrorOr<FString> ConvertSpirvToGlsl(TSpan<const uint32> blob);
}
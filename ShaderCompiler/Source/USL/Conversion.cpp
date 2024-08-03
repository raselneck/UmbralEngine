#include "USL/Conversion.h"
#include "Templates/NumericLimits.h"
#include <spirv_glsl.hpp>

static constexpr usize MaxShaderSourceLength = static_cast<usize>(TNumericLimits<FString::SizeType>::MaxValue);

TErrorOr<FString> USL::ConvertSpirvToGlsl(const TSpan<const uint8> blob)
{
	if (blob.IsEmpty())
	{
		return MAKE_ERROR("Given empty SPIR-V blob");
	}

	if (blob.Num() % 4 != 0)
	{
		return MAKE_ERROR("SPIR-V blobs must have a size divisible by 4; given size is {}", blob.Num());
	}

	const TSpan<const uint32> spirvBlob { reinterpret_cast<const uint32*>(blob.GetData()), blob.Num() / 4 };
	return ConvertSpirvToGlsl(spirvBlob);
}

TErrorOr<FString> USL::ConvertSpirvToGlsl(TSpan<const uint32> blob)
{
	const uint32* blobData = blob.GetData();
	const int32 blobLength = blob.Num();

	spirv_cross::CompilerGLSL compiler { blobData, static_cast<size_t>(blobLength) };
	compiler.set_common_options(spirv_cross::CompilerGLSL::Options
	{
		.version = 310,
		.es = true,
		.enable_420pack_extension = false,
		.vertex {
			.default_float_precision = spirv_cross::CompilerGLSL::Options::Highp,
			.default_int_precision = spirv_cross::CompilerGLSL::Options::Highp,
		},
		.fragment {
			.default_float_precision = spirv_cross::CompilerGLSL::Options::Highp,
			.default_int_precision = spirv_cross::CompilerGLSL::Options::Highp,
		}
	});

	// TODO Can the compiler throw exceptions?

	const std::string compiledShaderSource = compiler.compile();
	if (compiledShaderSource.size() > MaxShaderSourceLength)
	{
		return MAKE_ERROR("Converted shader source length is too large");
	}

	return FString { compiledShaderSource.data(), static_cast<FString::SizeType>(compiledShaderSource.length()) };
}
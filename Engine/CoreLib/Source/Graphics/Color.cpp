#include "Graphics/Color.h"
#include "Graphics/LinearColor.h"
#include "Math/Vector4.h"
#include "Misc/StringBuilder.h"

template<typename ResultType>
static ResultType ConvertColorToFourComponentValue(const FColor& value)
{
	return ResultType
	{
		Private::ByteToNormalizedFloat(value.R),
		Private::ByteToNormalizedFloat(value.G),
		Private::ByteToNormalizedFloat(value.B),
		Private::ByteToNormalizedFloat(value.A)
	};
}

FLinearColor FColor::ToLinearColor() const
{
	return ConvertColorToFourComponentValue<FLinearColor>(*this);
}

#define PACK_COLOR_COMPONENTS(_1, _2, _3, _4) \
	(static_cast<uint32>(_1) << 24) | \
	(static_cast<uint32>(_2) << 16) | \
	(static_cast<uint32>(_3) <<  8) | \
	(static_cast<uint32>(_4) <<  0)

uint32 FColor::PackARGB() const
{
	return PACK_COLOR_COMPONENTS(A, R, G, B);
}

uint32 FColor::PackRGBA() const
{
	return PACK_COLOR_COMPONENTS(R, G, B, A);
}

#undef PACK_COLOR_COMPONENTS

FVector4 FColor::ToVector4() const
{
	return ConvertColorToFourComponentValue<FVector4>(*this);
}

void TFormatter<FColor>::BuildString(const FColor& value, FStringBuilder& builder)
{
	builder << "(R="_sv << static_cast<uint64>(value.R)
	        << ", G="_sv << static_cast<uint64>(value.G)
	        << ", B="_sv << static_cast<uint64>(value.B)
	        << ", A="_sv << static_cast<uint64>(value.A)
	        << ")"_sv;
}

bool TFormatter<FColor>::Parse(FStringView formatString)
{
	return formatString.IsEmpty();
}
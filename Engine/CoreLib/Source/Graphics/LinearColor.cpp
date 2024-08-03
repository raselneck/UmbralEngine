#include "Graphics/Color.h"
#include "Graphics/LinearColor.h"
#include "Math/Vector4.h"
#include "Misc/StringBuilder.h"

bool FLinearColor::IsNearlyEqual(const FLinearColor& other) const
{
	return FMath::IsNearlyEqual(R, other.R) &&
	       FMath::IsNearlyEqual(G, other.G) &&
	       FMath::IsNearlyEqual(B, other.B) &&
	       FMath::IsNearlyEqual(A, other.A);
}

FColor FLinearColor::ToColor() const
{
	return FColor
	{
		Private::NormalizedFloatToByte(R),
		Private::NormalizedFloatToByte(G),
		Private::NormalizedFloatToByte(B),
		Private::NormalizedFloatToByte(A)
	};
}

FVector4 FLinearColor::ToVector4() const
{
	return FVector4 { R, G, B, A };
}

void TFormatter<FLinearColor>::BuildString(const FLinearColor& value, FStringBuilder& builder)
{
	builder.Append("(R="_sv)
	       .Append(value.R, MakeOptional<int32>(3))
	       .Append(", G="_sv)
	       .Append(value.G, MakeOptional<int32>(3))
	       .Append(", B="_sv)
	       .Append(value.B, MakeOptional<int32>(3))
	       .Append(", A="_sv)
	       .Append(value.A, MakeOptional<int32>(3))
	       .Append(")"_sv);
}

bool TFormatter<FLinearColor>::Parse(FStringView formatString)
{
	return formatString.IsEmpty();
}
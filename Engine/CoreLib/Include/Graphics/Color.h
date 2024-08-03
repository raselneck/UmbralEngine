#pragma once

#include "Engine/IntTypes.h"
#include "Misc/StringFormatting.h"
#include "Templates/IsZeroConstructible.h"

class FLinearColor;
class FVector4;

/**
 * @brief Defines a color with byte values for each component.
 */
class FColor
{
public:

	/** @brief This color's red component. */
	uint8 R = 0;

	/** @brief This color's green component. */
	uint8 G = 0;

	/** @brief This color's blue component. */
	uint8 B = 0;

	/** @brief This color's alpha component. */
	uint8 A = 0;

	/**
	 * @brief Sets default values for this color.
	 */
	constexpr FColor() = default;

	/**
	 * @brief Sets default values for this color's properties.
	 *
	 * @param r The initial red component value.
	 * @param g The initial green component value.
	 * @param b The initial blue component value.
	 */
	constexpr FColor(uint8 r, uint8 g, uint8 b)
		: R { r }
		, G { g }
		, B { b }
		, A { 255 }
	{
	}

	/**
	 * @brief Sets default values for this color's properties.
	 *
	 * @param r The initial red component value.
	 * @param g The initial green component value.
	 * @param b The initial blue component value.
	 * @param a The initial alpha component value.
	 */
	constexpr FColor(uint8 r, uint8 g, uint8 b, uint8 a)
		: R { r }
		, G { g }
		, B { b }
		, A { a }
	{
	}

	/**
	 * @brief Converts this color to a linear color.
	 *
	 * @return This color as a linear color.
	 */
	[[nodiscard]] FLinearColor ToLinearColor() const;

	/**
	 * @brief Packs this color into a 32-bit unsigned integer in ARGB format.
	 *
	 * @return This color packed into a 32-bit unsigned integer.
	 */
	[[nodiscard]] uint32 PackARGB() const;

	/**
	 * @brief Packs this color into a 32-bit unsigned integer in RGBA format.
	 *
	 * @return This color packed into a 32-bit unsigned integer.
	 */
	[[nodiscard]] uint32 PackRGBA() const;

	/**
	 * @brief Converts this color to a four component vector.
	 *
	 * @return This color as a four component vector.
	 */
	[[nodiscard]] FVector4 ToVector4() const;

	/**
	 * @brief Checks to see if this color is equivalent to another color.
	 *
	 * @param other The other color.
	 * @returns True if this color is equivalent to \p other, otherwise false.
	 */
	constexpr bool operator==(const FColor other) const
	{
		return R == other.R &&
		       G == other.G &&
		       B == other.B &&
		       A == other.A;
	}

	/**
	 * @brief Checks to see if this color is not equivalent to another color.
	 *
	 * @param other The other color.
	 * @returns True if this color is not equivalent to \p other, otherwise false.
	 */
	constexpr bool operator!=(const FColor other) const
	{
		return R != other.R ||
		       G != other.G ||
		       B != other.B ||
		       A == other.A;
	}
};

template<>
struct TIsZeroConstructible<FColor> : FTrueType
{
};

// NOTE: Color values painstakingly extracted from Wikipedia https://en.wikipedia.org/wiki/X11_color_names
namespace EColor
{
	constexpr FColor AliceBlue         { 240, 248, 255 };
	constexpr FColor AntiqueWhite      { 250, 235, 215 };
	constexpr FColor Aqua              {   0, 255, 255 };
	constexpr FColor Aquamarine        { 127, 255, 212 };
	constexpr FColor Azure             { 240, 255, 255 };
	constexpr FColor Beige             { 245, 245, 220 };
	constexpr FColor Bisque            { 255, 228, 196 };
	constexpr FColor Black             {   0,   0,   0 };
	constexpr FColor BlanchedAlmond    { 255, 235, 205 };
	constexpr FColor Blue              {   0,   0, 255 };
	constexpr FColor BlueViolet        { 138,  43, 226 };
	constexpr FColor Brown             { 165,  42,  42 };
	constexpr FColor Burlywood         { 222, 184, 135 };
	constexpr FColor CadetBlue         {  95, 158, 160 };
	constexpr FColor Chartreuse        { 127, 255,   0 };
	constexpr FColor Chocolate         { 210, 105,  30 };
	constexpr FColor Coral             { 255, 127,  80 };
	constexpr FColor CornflowerBlue    { 100, 149, 237 };
	constexpr FColor Cornsilk          { 255, 248, 220 };
	constexpr FColor Crimson           { 220,  20,  60 };
	constexpr FColor Cyan              {   0, 255, 255 };
	constexpr FColor DarkBlue          {   0,   0, 139 };
	constexpr FColor DarkCyan          {   0, 139, 139 };
	constexpr FColor DarkGoldenrod     { 184, 134,  11 };
	constexpr FColor DarkGray          { 169, 169, 169 };
	constexpr FColor DarkGreen         {   0, 100,   0 };
	constexpr FColor DarkKhaki         { 189, 183, 107 };
	constexpr FColor DarkMagenta       { 139,   0, 139 };
	constexpr FColor DarkOliveGreen    {  85, 107,  47 };
	constexpr FColor DarkOrange        { 255, 140,   0 };
	constexpr FColor DarkOrchid        { 153,  50, 204 };
	constexpr FColor DarkRed           { 139,   0,   0 };
	constexpr FColor DarkSalmon        { 233, 150, 122 };
	constexpr FColor DarkSeaGreen      { 143, 188, 143 };
	constexpr FColor DarkSlateBlue     {  72,  61, 139 };
	constexpr FColor DarkSlateGray     {  47,  79,  79 };
	constexpr FColor DarkTurquoise     {   0, 206, 209 };
	constexpr FColor DarkViolet        { 148,   0, 211 };
	constexpr FColor DeepPink          { 255,  20, 147 };
	constexpr FColor DeepSkyBlue       {   0, 191, 255 };
	constexpr FColor DimGray           { 105, 105, 105 };
	constexpr FColor DodgerBlue        {  30, 144, 255 };
	constexpr FColor Eigengrau         {  22,  22,  29 };
	constexpr FColor Firebrick         { 178,  34,  34 };
	constexpr FColor FloralWhite       { 255, 250, 240 };
	constexpr FColor ForestGreen       {  34, 139,  34 };
	constexpr FColor Magenta           { 255,   0, 255 };
	constexpr FColor Gainsboro         { 220, 220, 220 };
	constexpr FColor GhostWhite        { 248, 248, 255 };
	constexpr FColor Gold              { 255, 215,   0 };
	constexpr FColor Goldenrod         { 218, 165,  32 };
	constexpr FColor Gray              { 190, 190, 190 };
	constexpr FColor Green             {   0, 128,   0 };
	constexpr FColor GreenYellow       { 173, 255,  47 };
	constexpr FColor Honeydew          { 240, 255, 240 };
	constexpr FColor HotPink           { 255, 105, 180 };
	constexpr FColor IndianRed         { 205,  92,  92 };
	constexpr FColor Indigo            {  75,   0, 130 };
	constexpr FColor Ivory             { 255, 255, 240 };
	constexpr FColor Khaki             { 240, 230, 140 };
	constexpr FColor Lavender          { 230, 230, 250 };
	constexpr FColor LavenderBlush     { 255, 240, 245 };
	constexpr FColor LawnGreen         { 124, 252,   0 };
	constexpr FColor LemonChiffon      { 255, 250, 205 };
	constexpr FColor LightBlue         { 173, 216, 230 };
	constexpr FColor LightCoral        { 240, 128, 128 };
	constexpr FColor LightCyan         { 224, 255, 255 };
	constexpr FColor LightGoldenrod    { 250, 250, 210 };
	constexpr FColor LightGray         { 211, 211, 211 };
	constexpr FColor LightGreen        { 144, 238, 144 };
	constexpr FColor LightPink         { 255, 182, 193 };
	constexpr FColor LightSalmon       { 255, 160, 122 };
	constexpr FColor LightSeaGreen     {  32, 178, 170 };
	constexpr FColor LightSkyBlue      { 135, 206, 250 };
	constexpr FColor LightSlateGray    { 119, 136, 153 };
	constexpr FColor LightSteelBlue    { 176, 196, 222 };
	constexpr FColor LightYellow       { 255, 255, 224 };
	constexpr FColor Lime              {   0, 255,   0 };
	constexpr FColor LimeGreen         {  50, 205,  50 };
	constexpr FColor Linen             { 250, 240, 230 };
	constexpr FColor Fuchsia           { 255,   0, 255 };
	constexpr FColor Maroon            { 176,  48,  96 };
	constexpr FColor MediumAquamarine  { 102, 205, 170 };
	constexpr FColor MediumBlue        {   0,   0, 205 };
	constexpr FColor MediumOrchid      { 186,  85, 211 };
	constexpr FColor MediumPurple      { 147, 112, 219 };
	constexpr FColor MediumSeaGreen    {  60, 179, 113 };
	constexpr FColor MediumSlateBlue   { 123, 104, 238 };
	constexpr FColor MediumSpringGreen {   0, 250, 154 };
	constexpr FColor MediumTurquoise   {  72, 209, 204 };
	constexpr FColor MediumVioletRed   { 199,  21, 133 };
	constexpr FColor MidnightBlue      {  25,  25, 112 };
	constexpr FColor MintCream         { 245, 255, 250 };
	constexpr FColor MistyRose         { 255, 228, 225 };
	constexpr FColor Moccasin          { 255, 228, 181 };
	constexpr FColor NavajoWhite       { 255, 222, 173 };
	constexpr FColor Navy              {   0,   0, 128 };
	constexpr FColor OldLace           { 253, 245, 230 };
	constexpr FColor Olive             { 128, 128,   0 };
	constexpr FColor OliveDrab         { 107, 142,  35 };
	constexpr FColor Orange            { 255, 165,   0 };
	constexpr FColor OrangeRed         { 255,  69,   0 };
	constexpr FColor Orchid            { 218, 112, 214 };
	constexpr FColor PaleGoldenrod     { 238, 232, 170 };
	constexpr FColor PaleGreen         { 152, 251, 152 };
	constexpr FColor PaleTurquoise     { 175, 238, 238 };
	constexpr FColor PaleVioletRed     { 219, 112, 147 };
	constexpr FColor PapayaWhip        { 255, 239, 213 };
	constexpr FColor PeachPuff         { 255, 218, 185 };
	constexpr FColor Peru              { 205, 133,  63 };
	constexpr FColor Pink              { 255, 192, 203 };
	constexpr FColor Plum              { 221, 160, 221 };
	constexpr FColor PowderBlue        { 176, 224, 230 };
	constexpr FColor Purple            { 160,  32, 240 };
	constexpr FColor RebeccaPurple     { 102,  51, 153 };
	constexpr FColor Red               { 255,   0,   0 };
	constexpr FColor RosyBrown         { 188, 143, 143 };
	constexpr FColor RoyalBlue         {  65, 105, 225 };
	constexpr FColor SaddleBrown       { 139,  69,  19 };
	constexpr FColor Salmon            { 250, 128, 114 };
	constexpr FColor SandyBrown        { 244, 164,  96 };
	constexpr FColor SeaGreen          {  46, 139,  87 };
	constexpr FColor Seashell          { 255, 245, 238 };
	constexpr FColor Sienna            { 160,  82,  45 };
	constexpr FColor Silver            { 192, 192, 192 };
	constexpr FColor SkyBlue           { 135, 206, 235 };
	constexpr FColor SlateBlue         { 106,  90, 205 };
	constexpr FColor SlateGray         { 112, 128, 144 };
	constexpr FColor Snow              { 255, 250, 250 };
	constexpr FColor SpringGreen       {   0, 255, 127 };
	constexpr FColor SteelBlue         {  70, 130, 180 };
	constexpr FColor Tan               { 210, 180, 140 };
	constexpr FColor Teal              {   0, 128, 128 };
	constexpr FColor Thistle           { 216, 191, 216 };
	constexpr FColor Tomato            { 255,  99,  71 };
	constexpr FColor Turquoise         {  64, 224, 208 };
	constexpr FColor Violet            { 238, 130, 238 };
	constexpr FColor WebGreen          {   0, 255,   0 };
	constexpr FColor WebGray           { 128, 128, 128 };
	constexpr FColor WebMaroon         { 128,   0,   0 };
	constexpr FColor WebPurple         { 128,   0, 128 };
	constexpr FColor Wheat             { 245, 222, 179 };
	constexpr FColor White             { 255, 255, 255 };
	constexpr FColor WhiteSmoke        { 245, 245, 245 };
	constexpr FColor Yellow            { 255, 255,   0 };
	constexpr FColor YellowGreen       { 154, 205,  50 };
}

template<>
struct TFormatter<FColor>
{
	void BuildString(const FColor& value, FStringBuilder& builder);
	bool Parse(FStringView formatString);
};
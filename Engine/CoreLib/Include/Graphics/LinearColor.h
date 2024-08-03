#pragma once

#include "Containers/StaticArray.h"
#include "Graphics/Color.h"
#include "Math/Math.h"
#include "Misc/StringFormatting.h"

class FVector4;

namespace Private
{
	constexpr TStaticArray<float, 256> ByteToNormalizedFloatTable
	{{
		0.0000000f, 0.0039216f, 0.0078431f, 0.0117647f, 0.0156863f, 0.0196078f, 0.0235294f, 0.0274510f,
		0.0313725f, 0.0352941f, 0.0392157f, 0.0431373f, 0.0470588f, 0.0509804f, 0.0549020f, 0.0588235f,
		0.0627451f, 0.0666667f, 0.0705882f, 0.0745098f, 0.0784314f, 0.0823529f, 0.0862745f, 0.0901961f,
		0.0941176f, 0.0980392f, 0.1019608f, 0.1058824f, 0.1098039f, 0.1137255f, 0.1176471f, 0.1215686f,
		0.1254902f, 0.1294118f, 0.1333333f, 0.1372549f, 0.1411765f, 0.1450980f, 0.1490196f, 0.1529412f,
		0.1568627f, 0.1607843f, 0.1647059f, 0.1686275f, 0.1725490f, 0.1764706f, 0.1803922f, 0.1843137f,
		0.1882353f, 0.1921569f, 0.1960784f, 0.2000000f, 0.2039216f, 0.2078431f, 0.2117647f, 0.2156863f,
		0.2196078f, 0.2235294f, 0.2274510f, 0.2313725f, 0.2352941f, 0.2392157f, 0.2431373f, 0.2470588f,
		0.2509804f, 0.2549020f, 0.2588235f, 0.2627451f, 0.2666667f, 0.2705882f, 0.2745098f, 0.2784314f,
		0.2823529f, 0.2862745f, 0.2901961f, 0.2941176f, 0.2980392f, 0.3019608f, 0.3058824f, 0.3098039f,
		0.3137255f, 0.3176471f, 0.3215686f, 0.3254902f, 0.3294118f, 0.3333333f, 0.3372549f, 0.3411765f,
		0.3450980f, 0.3490196f, 0.3529412f, 0.3568627f, 0.3607843f, 0.3647059f, 0.3686275f, 0.3725490f,
		0.3764706f, 0.3803922f, 0.3843137f, 0.3882353f, 0.3921569f, 0.3960784f, 0.4000000f, 0.4039216f,
		0.4078431f, 0.4117647f, 0.4156863f, 0.4196078f, 0.4235294f, 0.4274510f, 0.4313725f, 0.4352941f,
		0.4392157f, 0.4431373f, 0.4470588f, 0.4509804f, 0.4549020f, 0.4588235f, 0.4627451f, 0.4666667f,
		0.4705882f, 0.4745098f, 0.4784314f, 0.4823529f, 0.4862745f, 0.4901961f, 0.4941176f, 0.4980392f,
		0.5019608f, 0.5058824f, 0.5098039f, 0.5137255f, 0.5176471f, 0.5215686f, 0.5254902f, 0.5294118f,
		0.5333333f, 0.5372549f, 0.5411765f, 0.5450980f, 0.5490196f, 0.5529412f, 0.5568627f, 0.5607843f,
		0.5647059f, 0.5686275f, 0.5725490f, 0.5764706f, 0.5803922f, 0.5843137f, 0.5882353f, 0.5921569f,
		0.5960784f, 0.6000000f, 0.6039216f, 0.6078431f, 0.6117647f, 0.6156863f, 0.6196078f, 0.6235294f,
		0.6274510f, 0.6313725f, 0.6352941f, 0.6392157f, 0.6431373f, 0.6470588f, 0.6509804f, 0.6549020f,
		0.6588235f, 0.6627451f, 0.6666667f, 0.6705882f, 0.6745098f, 0.6784314f, 0.6823529f, 0.6862745f,
		0.6901961f, 0.6941176f, 0.6980392f, 0.7019608f, 0.7058824f, 0.7098039f, 0.7137255f, 0.7176471f,
		0.7215686f, 0.7254902f, 0.7294118f, 0.7333333f, 0.7372549f, 0.7411765f, 0.7450980f, 0.7490196f,
		0.7529412f, 0.7568627f, 0.7607843f, 0.7647059f, 0.7686275f, 0.7725490f, 0.7764706f, 0.7803922f,
		0.7843137f, 0.7882353f, 0.7921569f, 0.7960784f, 0.8000000f, 0.8039216f, 0.8078431f, 0.8117647f,
		0.8156863f, 0.8196078f, 0.8235294f, 0.8274510f, 0.8313725f, 0.8352941f, 0.8392157f, 0.8431373f,
		0.8470588f, 0.8509804f, 0.8549020f, 0.8588235f, 0.8627451f, 0.8666667f, 0.8705882f, 0.8745098f,
		0.8784314f, 0.8823529f, 0.8862745f, 0.8901961f, 0.8941176f, 0.8980392f, 0.9019608f, 0.9058824f,
		0.9098039f, 0.9137255f, 0.9176471f, 0.9215686f, 0.9254902f, 0.9294118f, 0.9333333f, 0.9372549f,
		0.9411765f, 0.9450980f, 0.9490196f, 0.9529412f, 0.9568627f, 0.9607843f, 0.9647059f, 0.9686275f,
		0.9725490f, 0.9764706f, 0.9803922f, 0.9843137f, 0.9882353f, 0.9921569f, 0.9960784f, 1.0000000f
	}};

	/**
	 * @brief Converts a byte to a normalized float value.
	 *
	 * @param value The byte value to convert.
	 * @return The equivalent normalized float value.
	 */
	constexpr float ByteToNormalizedFloat(const uint8 value)
	{
		return ByteToNormalizedFloatTable[value];
	}

	/**
	 * @brief Converts a normalized float value to a byte.
	 *
	 * If the float value is outside of the range [0, 1] then the closest
	 * extreme in that range is returned.
	 *
	 * @param value The float value to convert.
	 * @return The equivalent byte value.
	 */
	constexpr uint8 NormalizedFloatToByte(const float value)
	{
		if (value < 0.0f)
		{
			return 0;
		}
		if (value > 1.0f)
		{
			return 255;
		}
		return static_cast<uint8>(value * 255.0f);
	}
}

/**
 * @brief Defines a color with float values for each component.
 */
class FLinearColor
{
public:

	/** @brief This color's red component. */
	float R = 0.0f;

	/** @brief This color's green component. */
	float G = 0.0f;

	/** @brief This color's blue component. */
	float B = 0.0f;

	/** @brief This color's alpha component. */
	float A = 0.0f;

	/**
	 * @brief Sets default values for this linear color's properties.
	 */
	constexpr FLinearColor() = default;

	/**
	 * @brief  Sets default values for this linear color's properties.
	 *
	 * @param r The initial value for this linear color's red component.
	 * @param g The initial value for this linear color's green component.
	 * @param b The initial value for this linear color's blue component.
	 */
	constexpr FLinearColor(float r, float g, float b)
		: R { r }
		, G { g }
		, B { b }
		, A { 1.0f }
	{
	}

	/**
	 * @brief  Sets default values for this linear color's properties.
	 *
	 * @param r The initial value for this linear color's red component.
	 * @param g The initial value for this linear color's green component.
	 * @param b The initial value for this linear color's blue component.
	 * @param a The initial value for this linear color's alpha component.
	 */
	constexpr FLinearColor(float r, float g, float b, float a)
		: R { r }
		, G { g }
		, B { b }
		, A { a }
	{
	}

	/**
	 * @brief  Sets default values for this linear color's properties.
	 *
	 * @param value The color to copy.
	 */
	constexpr explicit FLinearColor(const FColor value)
		: R { Private::ByteToNormalizedFloat(value.R) }
		, G { Private::ByteToNormalizedFloat(value.G) }
		, B { Private::ByteToNormalizedFloat(value.B) }
		, A { Private::ByteToNormalizedFloat(value.A) }
	{
	}

	/**
	 * @brief Checks to see if this linear color is nearly equal to another.
	 *
	 * @param other The other linear color.
	 * @return True if this linear color is nearly equal to \p other, otherwise false.
	 */
	[[nodiscard]] bool IsNearlyEqual(const FLinearColor& other) const;

	/**
	 * @brief Converts this linear color to a color.
	 *
	 * @return This linear color as a color.
	 */
	[[nodiscard]] FColor ToColor() const;

	/**
	 * @brief Converts this linear color to a four component vector.
	 *
	 * @return This linear color as a four component vector.
	 */
	[[nodiscard]] FVector4 ToVector4() const;

	/**
	 * @brief Checks to see if this color is equivalent to another color.
	 *
	 * @param other The other color.
	 * @returns True if this color is equivalent to \p other, otherwise false.
	 */
	bool operator==(const FLinearColor& other) const
	{
		return IsNearlyEqual(other);
	}

	/**
	 * @brief Checks to see if this color is not equivalent to another color.
	 *
	 * @param other The other color.
	 * @returns True if this color is not equivalent to \p other, otherwise false.
	 */
	bool operator!=(const FLinearColor& other) const
	{
		return IsNearlyEqual(other) == false;
	}
};

template<>
struct TIsZeroConstructible<FLinearColor> : FTrueType
{
};

namespace ELinearColor
{
	constexpr FLinearColor AliceBlue         { EColor::AliceBlue         };
	constexpr FLinearColor AntiqueWhite      { EColor::AntiqueWhite      };
	constexpr FLinearColor Aqua              { EColor::Aqua              };
	constexpr FLinearColor Aquamarine        { EColor::Aquamarine        };
	constexpr FLinearColor Azure             { EColor::Azure             };
	constexpr FLinearColor Beige             { EColor::Beige             };
	constexpr FLinearColor Bisque            { EColor::Bisque            };
	constexpr FLinearColor Black             { EColor::Black             };
	constexpr FLinearColor BlanchedAlmond    { EColor::BlanchedAlmond    };
	constexpr FLinearColor Blue              { EColor::Blue              };
	constexpr FLinearColor BlueViolet        { EColor::BlueViolet        };
	constexpr FLinearColor Brown             { EColor::Brown             };
	constexpr FLinearColor Burlywood         { EColor::Burlywood         };
	constexpr FLinearColor CadetBlue         { EColor::CadetBlue         };
	constexpr FLinearColor Chartreuse        { EColor::Chartreuse        };
	constexpr FLinearColor Chocolate         { EColor::Chocolate         };
	constexpr FLinearColor Coral             { EColor::Coral             };
	constexpr FLinearColor CornflowerBlue    { EColor::CornflowerBlue    };
	constexpr FLinearColor Cornsilk          { EColor::Cornsilk          };
	constexpr FLinearColor Crimson           { EColor::Crimson           };
	constexpr FLinearColor Cyan              { EColor::Cyan              };
	constexpr FLinearColor DarkBlue          { EColor::DarkBlue          };
	constexpr FLinearColor DarkCyan          { EColor::DarkCyan          };
	constexpr FLinearColor DarkGoldenrod     { EColor::DarkGoldenrod     };
	constexpr FLinearColor DarkGray          { EColor::DarkGray          };
	constexpr FLinearColor DarkGreen         { EColor::DarkGreen         };
	constexpr FLinearColor DarkKhaki         { EColor::DarkKhaki         };
	constexpr FLinearColor DarkMagenta       { EColor::DarkMagenta       };
	constexpr FLinearColor DarkOliveGreen    { EColor::DarkOliveGreen    };
	constexpr FLinearColor DarkOrange        { EColor::DarkOrange        };
	constexpr FLinearColor DarkOrchid        { EColor::DarkOrchid        };
	constexpr FLinearColor DarkRed           { EColor::DarkRed           };
	constexpr FLinearColor DarkSalmon        { EColor::DarkSalmon        };
	constexpr FLinearColor DarkSeaGreen      { EColor::DarkSeaGreen      };
	constexpr FLinearColor DarkSlateBlue     { EColor::DarkSlateBlue     };
	constexpr FLinearColor DarkSlateGray     { EColor::DarkSlateGray     };
	constexpr FLinearColor DarkTurquoise     { EColor::DarkTurquoise     };
	constexpr FLinearColor DarkViolet        { EColor::DarkViolet        };
	constexpr FLinearColor DeepPink          { EColor::DeepPink          };
	constexpr FLinearColor DeepSkyBlue       { EColor::DeepSkyBlue       };
	constexpr FLinearColor DimGray           { EColor::DimGray           };
	constexpr FLinearColor DodgerBlue        { EColor::DodgerBlue        };
	constexpr FLinearColor Eigengrau         { EColor::Eigengrau         };
	constexpr FLinearColor Firebrick         { EColor::Firebrick         };
	constexpr FLinearColor FloralWhite       { EColor::FloralWhite       };
	constexpr FLinearColor ForestGreen       { EColor::ForestGreen       };
	constexpr FLinearColor Magenta           { EColor::Magenta           };
	constexpr FLinearColor Gainsboro         { EColor::Gainsboro         };
	constexpr FLinearColor GhostWhite        { EColor::GhostWhite        };
	constexpr FLinearColor Gold              { EColor::Gold              };
	constexpr FLinearColor Goldenrod         { EColor::Goldenrod         };
	constexpr FLinearColor Gray              { EColor::Gray              };
	constexpr FLinearColor Green             { EColor::Green             };
	constexpr FLinearColor GreenYellow       { EColor::GreenYellow       };
	constexpr FLinearColor Honeydew          { EColor::Honeydew          };
	constexpr FLinearColor HotPink           { EColor::HotPink           };
	constexpr FLinearColor IndianRed         { EColor::IndianRed         };
	constexpr FLinearColor Indigo            { EColor::Indigo            };
	constexpr FLinearColor Ivory             { EColor::Ivory             };
	constexpr FLinearColor Khaki             { EColor::Khaki             };
	constexpr FLinearColor Lavender          { EColor::Lavender          };
	constexpr FLinearColor LavenderBlush     { EColor::LavenderBlush     };
	constexpr FLinearColor LawnGreen         { EColor::LawnGreen         };
	constexpr FLinearColor LemonChiffon      { EColor::LemonChiffon      };
	constexpr FLinearColor LightBlue         { EColor::LightBlue         };
	constexpr FLinearColor LightCoral        { EColor::LightCoral        };
	constexpr FLinearColor LightCyan         { EColor::LightCyan         };
	constexpr FLinearColor LightGoldenrod    { EColor::LightGoldenrod    };
	constexpr FLinearColor LightGray         { EColor::LightGray         };
	constexpr FLinearColor LightGreen        { EColor::LightGreen        };
	constexpr FLinearColor LightPink         { EColor::LightPink         };
	constexpr FLinearColor LightSalmon       { EColor::LightSalmon       };
	constexpr FLinearColor LightSeaGreen     { EColor::LightSeaGreen     };
	constexpr FLinearColor LightSkyBlue      { EColor::LightSkyBlue      };
	constexpr FLinearColor LightSlateGray    { EColor::LightSlateGray    };
	constexpr FLinearColor LightSteelBlue    { EColor::LightSteelBlue    };
	constexpr FLinearColor LightYellow       { EColor::LightYellow       };
	constexpr FLinearColor Lime              { EColor::Lime              };
	constexpr FLinearColor LimeGreen         { EColor::LimeGreen         };
	constexpr FLinearColor Linen             { EColor::Linen             };
	constexpr FLinearColor Fuchsia           { EColor::Fuchsia           };
	constexpr FLinearColor Maroon            { EColor::Maroon            };
	constexpr FLinearColor MediumAquamarine  { EColor::MediumAquamarine  };
	constexpr FLinearColor MediumBlue        { EColor::MediumBlue        };
	constexpr FLinearColor MediumOrchid      { EColor::MediumOrchid      };
	constexpr FLinearColor MediumPurple      { EColor::MediumPurple      };
	constexpr FLinearColor MediumSeaGreen    { EColor::MediumSeaGreen    };
	constexpr FLinearColor MediumSlateBlue   { EColor::MediumSlateBlue   };
	constexpr FLinearColor MediumSpringGreen { EColor::MediumSpringGreen };
	constexpr FLinearColor MediumTurquoise   { EColor::MediumTurquoise   };
	constexpr FLinearColor MediumVioletRed   { EColor::MediumVioletRed   };
	constexpr FLinearColor MidnightBlue      { EColor::MidnightBlue      };
	constexpr FLinearColor MintCream         { EColor::MintCream         };
	constexpr FLinearColor MistyRose         { EColor::MistyRose         };
	constexpr FLinearColor Moccasin          { EColor::Moccasin          };
	constexpr FLinearColor NavajoWhite       { EColor::NavajoWhite       };
	constexpr FLinearColor Navy              { EColor::Navy              };
	constexpr FLinearColor OldLace           { EColor::OldLace           };
	constexpr FLinearColor Olive             { EColor::Olive             };
	constexpr FLinearColor OliveDrab         { EColor::OliveDrab         };
	constexpr FLinearColor Orange            { EColor::Orange            };
	constexpr FLinearColor OrangeRed         { EColor::OrangeRed         };
	constexpr FLinearColor Orchid            { EColor::Orchid            };
	constexpr FLinearColor PaleGoldenrod     { EColor::PaleGoldenrod     };
	constexpr FLinearColor PaleGreen         { EColor::PaleGreen         };
	constexpr FLinearColor PaleTurquoise     { EColor::PaleTurquoise     };
	constexpr FLinearColor PaleVioletRed     { EColor::PaleVioletRed     };
	constexpr FLinearColor PapayaWhip        { EColor::PapayaWhip        };
	constexpr FLinearColor PeachPuff         { EColor::PeachPuff         };
	constexpr FLinearColor Peru              { EColor::Peru              };
	constexpr FLinearColor Pink              { EColor::Pink              };
	constexpr FLinearColor Plum              { EColor::Plum              };
	constexpr FLinearColor PowderBlue        { EColor::PowderBlue        };
	constexpr FLinearColor Purple            { EColor::Purple            };
	constexpr FLinearColor RebeccaPurple     { EColor::RebeccaPurple     };
	constexpr FLinearColor Red               { EColor::Red               };
	constexpr FLinearColor RosyBrown         { EColor::RosyBrown         };
	constexpr FLinearColor RoyalBlue         { EColor::RoyalBlue         };
	constexpr FLinearColor SaddleBrown       { EColor::SaddleBrown       };
	constexpr FLinearColor Salmon            { EColor::Salmon            };
	constexpr FLinearColor SandyBrown        { EColor::SandyBrown        };
	constexpr FLinearColor SeaGreen          { EColor::SeaGreen          };
	constexpr FLinearColor Seashell          { EColor::Seashell          };
	constexpr FLinearColor Sienna            { EColor::Sienna            };
	constexpr FLinearColor Silver            { EColor::Silver            };
	constexpr FLinearColor SkyBlue           { EColor::SkyBlue           };
	constexpr FLinearColor SlateBlue         { EColor::SlateBlue         };
	constexpr FLinearColor SlateGray         { EColor::SlateGray         };
	constexpr FLinearColor Snow              { EColor::Snow              };
	constexpr FLinearColor SpringGreen       { EColor::SpringGreen       };
	constexpr FLinearColor SteelBlue         { EColor::SteelBlue         };
	constexpr FLinearColor Tan               { EColor::Tan               };
	constexpr FLinearColor Teal              { EColor::Teal              };
	constexpr FLinearColor Thistle           { EColor::Thistle           };
	constexpr FLinearColor Tomato            { EColor::Tomato            };
	constexpr FLinearColor Turquoise         { EColor::Turquoise         };
	constexpr FLinearColor Violet            { EColor::Violet            };
	constexpr FLinearColor WebGreen          { EColor::WebGreen          };
	constexpr FLinearColor WebGray           { EColor::WebGray           };
	constexpr FLinearColor WebMaroon         { EColor::WebMaroon         };
	constexpr FLinearColor WebPurple         { EColor::WebPurple         };
	constexpr FLinearColor Wheat             { EColor::Wheat             };
	constexpr FLinearColor White             { EColor::White             };
	constexpr FLinearColor WhiteSmoke        { EColor::WhiteSmoke        };
	constexpr FLinearColor Yellow            { EColor::Yellow            };
	constexpr FLinearColor YellowGreen       { EColor::YellowGreen       };
}

template<>
struct TFormatter<FLinearColor>
{
	void BuildString(const FLinearColor& value, FStringBuilder& builder);
	bool Parse(FStringView formatString);
};
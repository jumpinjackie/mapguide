//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef COLOR_H_
#define COLOR_H_

#include "MathHelper.h"
#include <sstream>
#include <iomanip>

///<summary>
/// Class for managing each channel of a color.
/// Color takes a string or a integer, separates it to four channels.
/// Color also provides function to return the combined value of the four channels.
/// The string's format should be "AARRGGBB".
/// The integer's format should be 0xAARRGGBB.
///</summary>
class Color
{
public:

    ///<summary>
    /// Each color has four channels. This enum enumerates those four channels.
    /// It is very helpful and convenient for uesrs to distinguish those four channels.
    ///</summary>
    enum ChannelType
    {
        Alpha,
        Red,
        Green,
        Blue
    };

    ///<summary>
    /// At most time, the color is stored in an 32 bit integer.
    /// Each 8 bit stands for one channel.
    /// So it enumerates each channel's offset in an integer.
    ///</summary>
    enum ChannelOffsetType
    {
        AlphaOffset = 24,
        RedOffset   = 16,
        GreenOffset = 8,
        BlueOffset  = 0,
    };

    ///<summary>
    /// Default constructor. The default color is (0x00000000) transparent black.
    ///</summary>
    inline               Color();

    ///<summary>
    /// Separate the parameter to four channels and store them.
    ///</summary>
    inline explicit      Color(unsigned int ARGB);

    ///<summary>
    /// Initialize the four channels using the four parameters.
    ///</summary>
    inline               Color(unsigned char a, unsigned char r, unsigned char g, unsigned char b);

    ///<summary>
    /// Separate the parameter to four channels and store them.
    ///</summary>
    inline void          SetARGB(unsigned int ARGB);

    ///<summary>
    /// Set the four channels using the four parameters.
    ///</summary>
    inline void          SetARGB(unsigned char a, unsigned char r, unsigned char g, unsigned char b);

    ///<summary>
    /// Merge the four channels to an integer and return it.
    ///</summary>
    inline unsigned int  GetARGB() const;

    ///<summary>
    /// Return alpha channel's value.
    ///</summary>
    inline unsigned char GetA() const;

    ///<summary>
    /// Return red channel's value.
    ///</summary>
    inline unsigned char GetR() const;

    ///<summary>
    /// Return green channel's value.
    ///</summary>
    inline unsigned char GetG() const;

    ///<summary>
    /// Return blue channel's value.
    ///</summary>
    inline unsigned char GetB() const;

    ///<summary>
    /// Set the parameter to alpha channel's value.
    ///</summary>
    inline void          SetA(unsigned char alpha);

    ///<summary>
    /// Set the parameter to red channel's value.
    ///</summary>
    inline void          SetR(unsigned char red);

    ///<summary>
    /// Set the parameter to green channel's value.
    ///</summary>
    inline void          SetG(unsigned char green);

    ///<summary>
    /// Set the parameter to blue channel's value.
    ///</summary>
    inline void          SetB(unsigned char blue);

    ///<summary>
    /// Round the parameter to integer, then set it to alpha channel's value.
    ///</summary>
    inline void          SetA(const double &alpha);

    ///<summary>
    /// Round the parameter to integer, then set it to red channel's value.
    ///</summary>
    inline void          SetR(const double &red);

    ///<summary>
    /// Round the parameter to integer, then set it to green channel's value.
    ///</summary>
    inline void          SetG(const double &green);

    ///<summary>
    /// Round the parameter to integer, then set it to blue channel's value.
    ///</summary>
    inline void          SetB(const double &blue);

    ///<summary>
    /// Reset all the four channels to kChannelNull.
    ///</summary>
    inline void          SetNull();

    ///<summary>
    /// Reset the alpha channel's value to kChannelNull.
    ///</summary>
    inline void          SetTransparent();

    ///<summary>
    /// If all the four channels are zero, it returns true. Otherwise, it returns false.
    ///</summary>
    inline bool          IsNull() const;

    ///<summary>
    /// If the alpha channel is zero, it returns true. Otherwise, it returns false.
    ///</summary>
    inline bool          IsTransparent() const;

    ///<summary>
    /// Check whether it is equal to another color object.
    ///</summary>
    inline bool          operator == (const Color &comp) const;

    ///<summary>
    /// Check whether it is different from another color object.
    ///</summary>
    inline bool          operator != (const Color &comp) const;

    ///<summary>
    /// Static function returns a reference to a static null object.
    ///</summary>
    inline static const Color& GetNullColor();

    ///<summary>
    /// The max value of a channel, which is used in 'and' bit operation.
    ///</summary>
    const static unsigned char kChannelMask;

    ///<summary>
    /// The null value of a channel, when all the four channels are null,
    /// then the color is null.
    ///</summary>
    const static unsigned char kChannelNull;

    ///<summary>
    /// The max value of a channel.
    ///</summary>
    const static unsigned char kChannelMax;

    ///<summary>
    /// The min value of a channel.
    ///</summary>
    const static unsigned char kChannelMin;

private:
    unsigned char              m_alpha;  // alpha channel.
    unsigned char              m_red;    // red channel.
    unsigned char              m_green;  // green channel.
    unsigned char              m_blue;   // blue channel.

};


// Forward declaration of two global operators.

/// Read the color object from an input stream.
template<class Elem, class Tr> std::basic_istream<Elem, Tr>& operator >> (std::basic_istream<Elem, Tr> &is, Color &color);

/// Write the color object to an output stream.
template<class Elem, class Tr> std::basic_ostream<Elem, Tr>& operator << (std::basic_ostream<Elem, Tr> &os, const Color &color);


//
// Color
//

Color::Color()
{
    SetNull();
}

Color::Color(unsigned int ARGB)
{
    SetARGB(ARGB);
}

Color::Color(unsigned char a, unsigned char r, unsigned char g, unsigned char b)
{
    m_alpha = a;
    m_red   = r;
    m_green = g;
    m_blue  = b;
}

void Color::SetARGB(unsigned int ARGB)
{
    m_alpha = (unsigned char)(ARGB >> AlphaOffset) & kChannelMask;
    m_red   = (unsigned char)(ARGB >> RedOffset  ) & kChannelMask;
    m_green = (unsigned char)(ARGB >> GreenOffset) & kChannelMask;
    m_blue  = (unsigned char)(ARGB >> BlueOffset ) & kChannelMask;
}

void Color::SetARGB(unsigned char a, unsigned char r, unsigned char g, unsigned char b)
{
    m_alpha = a;
    m_red   = r;
    m_green = g;
    m_blue  = b;
}

unsigned int Color::GetARGB() const
{
    return ((m_alpha << AlphaOffset) | (m_red << RedOffset) | (m_green << GreenOffset) | (m_blue << BlueOffset));
}

bool Color::operator == (const Color &comp) const
{
    return ((comp.m_alpha == m_alpha) && (comp.m_red == m_red) && (comp.m_green == m_green) && (comp.m_blue == m_blue));
}

bool Color::operator != (const Color &comp) const
{
    return !(*this == comp);
}

unsigned char Color::GetA() const
{
    return m_alpha;
}

unsigned char Color::GetR() const
{
    return m_red;
}

unsigned char Color::GetG() const
{
    return m_green;
}

unsigned char Color::GetB() const
{
    return m_blue;
}

void Color::SetA(unsigned char alpha)
{
    m_alpha = alpha;
}

void Color::SetR(unsigned char red)
{
    m_red = red;
}

void Color::SetG(unsigned char green)
{
    m_green = green;
}

void Color::SetB(unsigned char blue)
{
    m_blue = blue;
}

void Color::SetA(const double &alpha)
{
    assert(alpha >= kChannelMin && alpha <= kChannelMax);

    m_alpha = (unsigned char)Double2Int(alpha);
}

void Color::SetR(const double &red)
{
    assert(red >= kChannelMin && red <= kChannelMax);

    m_red = (unsigned char)Double2Int(red);
}

void Color::SetG(const double &green)
{
    assert(green >= kChannelMin && green <= kChannelMax);

    m_green = (unsigned char)Double2Int(green);
}

void Color::SetB(const double &blue)
{
    assert(blue >= kChannelMin && blue <= kChannelMax);

    m_blue = (unsigned char)Double2Int(blue);
}

void Color::SetNull()
{
    m_alpha = kChannelNull;
    m_red   = kChannelNull;
    m_green = kChannelNull;
    m_blue  = kChannelNull;
}

bool Color::IsNull() const
{
    return ((kChannelNull == m_alpha) && (kChannelNull == m_red) && (kChannelNull == m_green) && (kChannelNull == m_blue));
}

void Color::SetTransparent()
{
    m_alpha = kChannelNull;
}

bool Color::IsTransparent() const
{
    return (m_alpha == kChannelNull);
}

const Color& Color::GetNullColor()
{
    static Color nullColor;
    nullColor.SetNull();
    return nullColor;
}

/// Read the color object from an input stream.
template<class Elem, class Tr>
std::basic_istream<Elem, Tr>& operator >> (std::basic_istream<Elem, Tr> &is, Color &color)
{
    // Save flags
    std::ios_base::fmtflags flag = is.flags();

    // Get 8 characters.
    Elem ch[8] = {0};
    int k;
    for (k = 0; k < 8; ++k)
    {
        if (!(is >> ch[k]))
            break;
        // Translate char / wchar to integer. eg. '0' -> 0, 'F' -> 15.
        if (ch[k] >= Elem('0') && ch[k] <= Elem('9'))
        {
            ch[k] -= Elem('0');
        }
        else if (ch[k] >= Elem('A') && ch[k] <= Elem('F'))
        {
            ch[k] -= (Elem('A') - 10);
        }
        else if (ch[k] >= Elem('a') && ch[k] <= Elem('f'))
        {
            ch[k] -= (Elem('a') - 10);
        }
        else
        {
            // Put back this character.
            is.putback(ch[k]);
            // Indicates reading the stream encounter an bad data.
            is.setstate(std::ios_base::badbit);
            break;
        }
    }
    unsigned int value = 0;
    for (int i = 0; i < k; ++i)
    {
        value <<= 4;
        value |= (0xF & ch[i]);
    }
    if (k <= 6)
        value |= 0xFF000000; // There is no ALPHA value, set it to 0xFF silently.
    color.SetARGB(value);

    if (is.eof() && k > 0)
        is.clear();

    // Restore flags, and reserve the iostate.
    // Thus caller can check the iostate when that function returns.
    is.flags(flag);
    return is;
}

/// Write the color object to an output stream.
template<class Elem, class Tr>
std::basic_ostream<Elem, Tr>& operator << (std::basic_ostream<Elem, Tr> &os, const Color &color)
{
    // Save flags.
    std::ios_base::fmtflags flag = os.flags();

    os << std::hex     << std::uppercase
       << std::setw(2) << std::setfill(Elem('0')) << (unsigned int)color.GetA() // Don't ouptut char or unsigned char.
       << std::setw(2) << std::setfill(Elem('0')) << (unsigned int)color.GetR() // So cast the type to unsigned int.
       << std::setw(2) << std::setfill(Elem('0')) << (unsigned int)color.GetG()
       << std::setw(2) << std::setfill(Elem('0')) << (unsigned int)color.GetB();

    // Restore flags.
    os.flags(flag);
    return os;
}

#endif

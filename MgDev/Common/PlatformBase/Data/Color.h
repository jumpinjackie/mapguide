//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef _MG_COLOR
#define _MG_COLOR

/// \defgroup MgColor MgColor
/// \ingroup Common_Module
/// \{

class MgColor;
template class MG_PLATFORMBASE_API Ptr<MgColor>;

/////////////////////////////////////////////////////////////////
/// \brief
/// This class stores Alpha, Red, Green and Blue component of a Color.
///
class MG_PLATFORMBASE_API MgColor : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgColor)

PUBLISHED_API:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructor.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgColor(string color);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgColor(String color);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgColor(string color);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param color (String/string)
    /// Color specified as 0xRRGGBBAA
    /// RR - Red component
    /// GG - Green component
    /// BB - Blue Component
    /// AA - Alpha component (optional)
    ///
    /// \return
    /// Nothing
    ///
    MgColor(CREFSTRING color);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructor.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgColor(short red, short green, short blue);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgColor(short red, short green, short blue);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgColor(int red, int green, int blue);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param red (short/int)
    /// Red value. This value must be in the range of 0 to 255 inclusive.
    /// \param green (short/int)
    /// Green value. This value must be in the range of 0 to 255 inclusive.
    /// \param blue (short/int)
    /// Blue value. This value must be in the range of 0 to 255 inclusive.
    ///
    /// \return
    /// Nothing
    ///
    MgColor(INT16 red, INT16 green, INT16 blue);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructor.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgColor(short red, short green, short blue, short alpha);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgColor(short red, short green, short blue, short alpha);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgColor(int red, int green, int blue, int alpha);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param red (short/int)
    /// Red value. This value must be in the range of 0 to 255 inclusive.
    /// \param green (short/int)
    /// Green value. This value must be in the range of 0 to 255 inclusive.
    /// \param blue (short/int)
    /// Blue value. This value must be in the range of 0 to 255 inclusive.
    /// \param alpha (short/int)
    /// Alpha value
    ///
    /// \return
    /// Nothing
    ///
    MgColor(INT16 red, INT16 green, INT16 blue, INT16 alpha);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns Color specified as 0xRRGGBBAA
    /// RR - Red component
    /// GG - Green component
    /// BB - Blue Component
    /// AA - Alpha component (optional)
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetColor();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetColor();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetColor();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// return the color value
    ///
    STRING GetColor();   /// __get

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// return the color red value
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// short GetRed();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// short GetRed();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetRed();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// return the red value
    ///
    INT16 GetRed(); /// __get

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// return the color blue value
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// short GetBlue();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// short GetBlue();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetBlue();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// return the blue value
    ///
    INT16 GetBlue(); /// __get

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// return the color green value
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// short GetGreen();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// short GetGreen();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetGreen();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// return the green value
    ///
    INT16 GetGreen(); /// __get

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// return the color alpha value
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// short GetAlpha();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// short GetAlpha();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetAlpha();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// return the alpha value
    ///
    INT16 GetAlpha(); /// __get

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes data to a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserializes data from a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    ///
    virtual void Deserialize(MgStream* stream);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Default constructor
    ///
    MgColor();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Virtual destructor
    ///
    virtual ~MgColor();

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class Id.
    ///
    /// \return
    /// Returns the integer value of the class Id.
    ///
    INT32 GetClassId();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Self destructing method.
    ///
    virtual void Dispose();

private:

    INT16 m_red;
    INT16 m_green;
    INT16 m_blue;
    INT16 m_alpha;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_Data_Color;
};
/// \}

#endif

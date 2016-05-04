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

#ifndef _MGCOORDINATE_H_
#define _MGCOORDINATE_H_

class MgCoordinate;
template class MG_GEOMETRY_API Ptr<MgCoordinate>;

/// \defgroup MgCoordinate MgCoordinate
/// \ingroup Geometry_Module_classes
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// MgCoordinate is an abstract base class used to represent the
/// Cartesian coordinates of a geometry.
///
/// \remarks
/// Concrete classes derived
/// from this abstract class are MgCoordinateXY, MgCoordinateXYM,
/// MgCoordinateXYZ, and MgCoordinateXYZM.
/// \n
/// Coordinates by default have X and Y ordinates and can
/// optionally have additional ordinates to specify elevation
/// (Z), and measure (M). MgCoordinate objects are directly used
/// to construct MgPoint geometries and MgArcSegment geometry
/// components and indirectly to construct all other geometries.
///
/// \note
/// This is a base class, not designed to be used directly. Use
/// the derived classes.
///
class MG_GEOMETRY_API MgCoordinate : public MgSerializable
{
PUBLISHED_API:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the dimensions supported by this coordinate as a bit
    /// mask. See MgCoordinateDimension for a specification of the
    /// values which can be found in the bit mask.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetDimension();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetDimension();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetDimension();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns a bit mask of OR'd MgCoordinateDimension values that
    /// defines the dimensions supported by this coordinate.
    ///
    ///
    virtual INT32 GetDimension() = 0;  /// __get

    ////////////////////////////////////////
    /// \brief
    /// Gets the X value of this coordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual double GetX();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual double GetX();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual double GetX();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the X value.
    ///
    ///
    virtual double GetX() = 0;  /// __get

    ////////////////////////////////////////
    /// \brief
    /// Gets the Y value of this coordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual double GetY();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual double GetY();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual double GetY();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the Y value.
    ///
    ///
    virtual double GetY() = 0;  /// __get

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Z value of this coordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual double GetZ();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual double GetZ();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual double GetZ();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the Z value or the value of MgCoordinate::NoZ if not
    /// set.
    ///
    ///
    virtual double GetZ() = 0;  /// __get

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the M value of this coordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual double GetM();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual double GetM();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual double GetM();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the M value or the value of MgCoordinate::NoM if not
    /// set.
    ///
    ///
    virtual double GetM() = 0;  /// __get

EXTERNAL_API:
    ////////////////////////////////////////////////////////////
    /// \brief
    /// The value of this constant is NAN. In a PHP context, the
    /// value is 1.\#QNAN . A call to the GetZ() method on an
    /// MgCoordinate with no Z value returns the noM value.
    ///
    ///
   static double const NoZ;

    ////////////////////////////////////////////////////////////
    /// \brief
    /// The value of this constant is NAN. In a PHP context, the
    /// value is 1.\#QNAN . A call to the GetM() method on an
    /// MgCoordinate with no M value returns the noM value.
    ///
    ///
    static double const NoM;

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    ///
    virtual INT32 GetClassId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a copy of this object
    ///
    virtual MgCoordinate* Copy() = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Test 2 coordinates for equality
    ///
    virtual bool Equals(MgCoordinate* coord) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Convert to XML representation
    ///
    virtual void ToXml(string &str) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Convert to AWKT representation
    ///
    virtual void ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly) = 0;

    ////////////////////////////////////////
    /// \brief
    /// Sets the X value of this coordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetX(double x);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetX(double x);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetX(double x);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Sets the X value.
    ///
    ///
    virtual void SetX(double x) = 0;  /// __get

    ////////////////////////////////////////
    /// \brief
    /// Sets the Y value of this coordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetY(double y);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetY(double y);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetY(double y);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Sets the Y value.
    ///
    ///
    virtual void SetY(double y) = 0;  /// __get

    ////////////////////////////////////////
    /// \brief
    /// Sets the Z value of this coordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetZ(double z);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetZ(double z);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetZ(double z);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Sets the Z value.
    ///
    ///
    virtual void SetZ(double z) = 0;  /// __get

    ////////////////////////////////////////
    /// \brief
    /// Sets the M value of this coordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetM(double m);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetM(double m);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetM(double m);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Sets the M value.
    ///
    ///
    virtual void SetM(double m) = 0;  /// __get

CLASS_ID:
    static const INT32 m_cls_id = Geometry_Coordinate;
};
/// \}

#endif //_MGCOORDINATE_H_

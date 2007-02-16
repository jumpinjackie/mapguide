//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef _MGCOORDINATESYSTEMFACTORY_H_
#define _MGCOORDINATESYSTEMFACTORY_H_

/// \defgroup MgCoordinateSystemFactory MgCoordinateSystemFactory
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// Supports the creation of MgCoordinateSystem objects based on
/// a OpenGIS Well-Known Text (WKT) definition of the coordinate
/// system.
///

class MG_GEOMETRY_API MgCoordinateSystemFactory : public MgDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemFactory)

PUBLISHED_API:
    ///////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgCoordinateSystemFactory object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  MgCoordinateSystemFactory();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  MgCoordinateSystemFactory();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  MgCoordinateSystemFactory();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// $coordSysFactory = new MgCoordinateSystemFactory();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    MgCoordinateSystemFactory();
    //////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgCoordinateSystem instance given a definition of
    /// the coordinate system in OpenGIS Well-Known Text (WKT)
    /// format. See \link WKTofSRS WKT of Spatial Reference System \endlink
    /// and \link CSTestValues Coordinate System Test Values \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual MgCoordinateSystem Create(string srsWkt);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual MgCoordinateSystem Create(String srsWkt);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual MgCoordinateSystem Create(string srsWkt);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param srsWkt (String/string)
    /// A string defining the coordinate system
    /// in OpenGIS WKT format.
    ///
    /// \return
    /// Returns an MgCoordinateSystem instance that corresponds to
    /// the specified WKT definition.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// $wkt = 'PROJCS["UTM Zone 18 (NAD 83)", GEOGCS ["NAD 83
    /// (Continental US)", DATUM ["NAD 83 (Continental US)", SPHEROID
    /// ["GRS 80", 6378137, 298.257222101]], PRIMEM [ "Greenwich",
    /// 0.000000 ], UNIT ["Decimal Degree", 0.01745329251994330]],
    /// PROJECTION ["Transverse Mercator"], PARAMETER
    /// ["Scale_Factor", 0.999600], PARAMETER ["Central_Meridian",
    /// -75.000000], PARAMETER ["False_Easting", 500000.000000],
    /// UNIT ["Meter", 1.000000000000]]';
    ///
    /// $coordSys = $coordSysFactory($wkt);
    /// \htmlinclude ExampleBottom.html
    ///
    virtual MgCoordinateSystem* Create(CREFSTRING srsWkt);
INTERNAL_API:
    virtual ~MgCoordinateSystemFactory();

protected:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId();

    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose();

/// Data Members
CLASS_ID:

    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemFactory;

private:

    // cached coord sys objects -- so that we don't have to match
    // every time someone asks for the same SRS conversion
    std::map<STRING, MgCoordinateSystem*> m_hCSCache;

    // Needed for thread-safety
    ACE_Recursive_Thread_Mutex m_mutex;
};
/// \}

#endif //_MGCOORDINATESYSTEMFACTORY_H_

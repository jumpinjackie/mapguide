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

#ifndef _MGTRANSFORM_H_
#define _MGTRANSFORM_H_

class MgEnvelope;

class MgTransform;
template class MG_GEOMETRY_API Ptr<MgTransform>;

/// \defgroup MgTransform MgTransform
/// \ingroup Geometry_Module_classes
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// MgTransform is an abstract class that can be implemented to
/// provide linear or projected coordinate transformations. There
/// are currently no concrete classes.
///
class MG_GEOMETRY_API MgTransform : public MgGuardDisposable
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the X and Y ordinates and returns a new coordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCoordinate Transform(double x, double y);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCoordinate Transform(double x, double y);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCoordinate Transform(double x, double y);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param x (double)
    /// The X ordinate to transform.
    /// \param y (double)
    /// The Y ordinate to transform.
    ///
    /// \return
    /// A new MgCoordinate defined by the transformed X and Y ordinates.
    ///
    virtual MgCoordinate* Transform(double x, double y) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the X, Y, and Z ordinates and returns a new coordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCoordinate Transform(double x, double y, double z);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCoordinate Transform(double x, double y, double z);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCoordinate Transform(double x, double y, double z);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param x (double)
    /// The X ordinate to transform.
    /// \param y (double)
    /// The Y ordinate to transform.
    /// \param z (double)
    /// The Z ordinate to transform.
    ///
    /// \return
    /// A new MgCoordinate defined by the transformed X, Y, and Z ordinates.
    ///
    virtual MgCoordinate* Transform(double x, double y, double z) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the coordinate and returns a new coordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCoordinate Transform(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCoordinate Transform(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCoordinate Transform(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coordinate (MgCoordinate)
    /// The input MgCoordinate to transform.
    ///
    /// \return
    /// A new MgCoordinate transformed from the specified coordinate.
    ///
    virtual MgCoordinate* Transform(MgCoordinate* coordinate) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the envelope and returns a new envelope.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgEnvelope Transform(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgEnvelope Transform(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgEnvelope Transform(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param envelope (MgEnvelope)
    /// The input MgEnvelope to transform.
    ///
    /// \return
    /// A new MgEnvelope transformed from the specified envelope.
    ///
    virtual MgEnvelope* Transform(MgEnvelope* envelope) = 0;
};
/// \}

#endif //_MGTRANSFORM_H_

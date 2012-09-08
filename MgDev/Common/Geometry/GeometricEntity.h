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

#ifndef MGGEOMETRICENTITY_H_
#define MGGEOMETRICENTITY_H_

class MgPoint;

class MgGeometricEntity;
template class MG_GEOMETRY_API Ptr<MgGeometricEntity>;

/// \defgroup MgGeometricEntity MgGeometricEntity
/// \ingroup Geometry_Module_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// MgGeometricEntity is an abstract base class that is the root
/// of all geometry and geometry component classes.
///
/// \remarks
/// Geometric entities are immutable once constructed.
///
class MG_GEOMETRY_API MgGeometricEntity : public MgSerializable
{
PUBLISHED_API:
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the geometric dimension of this entity, indicating
    /// whether the entity is defined by points, curves, or regions,
    /// that is, is zero, one, or two dimensional.
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
    /// Returns one of the values defined in MgGeometricDimension.
    ///
    virtual INT32 GetDimension() = 0;  /// __get

    ////////////////////////////////////////////////////
    /// \brief
    /// Tests whether this entity is empty or not.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool IsEmpty();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean IsEmpty();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool IsEmpty();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// True if the geometry is empty, false otherwise.
    ///
    virtual bool IsEmpty() = 0;

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Tests whether this entity is simple or not. Simple entities
    /// do not contain any points of self-tangency or self
    /// intersection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool IsSimple();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean IsSimple();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool IsSimple();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// True if the geometry is simple, false otherwise.
    ///
    /// \note
    /// Optional for MapGuide.
    ///
    virtual bool IsSimple();

    /////////////////////////////////////////////////////////
    /// \brief
    /// Tests whether the entity's boundary encloses an area.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool IsClosed();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean IsClosed();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool IsClosed();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// True if the geometry is closed, false otherwise.
    ///
    virtual bool IsClosed() = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Tests whether the coordinates given to construct the entity represent
    /// a valid Geometry or GeometryComponent.
    ///
    /// Note: Optional for MapGuide.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool IsValid();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean IsValid();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool IsValid();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// True if the geometry is valid, false otherwise.
    ///
    virtual bool IsValid();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets this geometric entity's bounding box.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgEnvelope Envelope();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgEnvelope Envelope();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgEnvelope Envelope();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgEnvelope object representing the bounding box.
    ///
    virtual MgEnvelope* Envelope();

    /////////////////////////////////////////////////////
    /// \brief
    /// Returns a copy of this geometric entity.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometricEntity Copy();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometricEntity Copy();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometricEntity Copy();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// An MgGeometricEntity that is a copy of this one.
    ///
    virtual MgGeometricEntity* Copy() = 0;

    /////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a transformed copy of this geometric entity.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometricEntity Transform(MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometricEntity Transform(MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometricEntity Transform(MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param transform (MgTransform)
    /// The MgTransform to use in constructing a copy
    /// of this geometric entity.
    ///
    /// \return
    /// An MgGeometricEntity that is a copy of this one with each
    /// coordinate transformed.
    ///
    virtual MgGeometricEntity* Transform(MgTransform* transform) = 0;

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the area for this geometric entity or zero for
    /// MgLineString and MgPoint objects. Calculates the area for
    /// each MgPolygon in an MgMultiPolygon or for each
    /// MgCurvePolygon in an MgMultiCurvePolygon and returns the sum
    /// of areas.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual double GetArea();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual double GetArea();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual double GetArea();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The area of this geometric entity.
    ///
    virtual double GetArea();  /// __get

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Calculates and returns the length for this geometric entity.
    /// Defaults to zero for MgPoint objects.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual double GetLength();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual double GetLength();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual double GetLength();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Length of this geometric entity
    ///
    virtual double GetLength();  /// __get

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Calculates and returns the centroid of this geometric entity.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgPoint GetCentroid();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgPoint GetCentroid();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgPoint GetCentroid();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// A centroid point
    ///
    virtual MgPoint* GetCentroid();  /// __get

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Return a path iterator for this geometric entity
    ///
    /// \return
    /// MgGeometricPathIterator
    ///
    virtual MgGeometricPathIterator* GetPath();

    virtual INT32 GetEntityType() = 0;

    /// TODO: Adding default implementation in base class for now
    /// TODO: When we have actual implementation it should be pure virtual
    /// TODO: function.
    virtual void ToXml(std::string& /*str*/) {}

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Convert to AWKT representation
    ///
    virtual void ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Convert to AWKT representation
    ///
    virtual STRING ToAwkt(bool is2dOnly) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns an iterator over the coordinates included in this
    /// geometric entity.
    ///
    /// \return
    /// The coordinates of this geometric entity
    ///
    virtual MgCoordinateIterator* GetCoordinates() = 0;

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Calculate the envelope for this geometry
    ///
    virtual MgEnvelope* ComputeEnvelope() = 0;

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_GeometricEntity;

protected:

    Ptr<MgEnvelope> m_envelope;
};
/// \}

#endif //_MGGEOMETRICENTITY_H_

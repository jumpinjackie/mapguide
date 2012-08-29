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

#ifndef _MGENVELOPE_H_
#define _MGENVELOPE_H_

class MgEnvelope;
template class MG_GEOMETRY_API Ptr<MgEnvelope>;

/// \defgroup MgEnvelope MgEnvelope
/// \ingroup Geometry_Module_classes
/// \{

//////////////////////////////////////////////////////////////
/// \brief
/// MgEnvelope represents a bounding box defined in terms of a
/// lower left coordinate and an upper right coordinate.
///
class MG_GEOMETRY_API MgEnvelope : public MgSerializable
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgEnvelope)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs and initializes  an "null" envelope.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgEnvelope();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgEnvelope();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgEnvelope();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgEnvelope();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs and initializes  an envelope for a region defined by one
    /// coordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgEnvelope(MgCoordinate coord);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgEnvelope(MgCoordinate coord);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgEnvelope(MgCoordinate coord);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coord (MgCoordinate)
    /// The MgCoordinate to use in defining the region.
    ///
    MgEnvelope(MgCoordinate* coord);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs and initializes  an envelope for a region defined by two
    /// coordinates.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgEnvelope(MgCoordinate coord1, MgCoordinate coord2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgEnvelope(MgCoordinate coord1, MgCoordinate coord2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgEnvelope(MgCoordinate coord1, MgCoordinate coord2);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coord1 (MgCoordinate)
    /// The first MgCoordinate to use in defining the region.
    /// \param coord2 (MgCoordinate)
    /// The second MgCoordinate to use in defining the region.
    ///
    MgEnvelope(MgCoordinate* coord1, MgCoordinate* coord2);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs and initializes  an envelope for a region defined by two
    /// coordinates , given as four doubles
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgEnvelope(double xMin, double yMin, double xMax, double yMax);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgEnvelope(double xMin, double yMin, double xMax, double yMax);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgEnvelope(double xMin, double yMin, double xMax, double yMax);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param xMin (double)
    /// Left coordinate
    /// \param yMin (double)
    /// Bottom coordinate
    /// \param xMax (double)
    /// Right coordinate
    /// \param yMax (double)
    /// Top coordinate
    ///
    MgEnvelope(double xMin, double yMin, double xMax, double yMax);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs and initializes  an envelope that is a copy of another.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgEnvelope(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgEnvelope(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgEnvelope(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param envelope (MgEnvelope)
    /// The MgEnvelope to copy.
    ///
    MgEnvelope(MgEnvelope* envelope);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the lower left coordinate of the envelope.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgCoordinate GetLowerLeftCoordinate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgCoordinate GetLowerLeftCoordinate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgCoordinate GetLowerLeftCoordinate();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// An MgCoordinate that defines the lower left corner of the envelope.
    ///
    MgCoordinate* GetLowerLeftCoordinate();  /// __get

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the upper right coordinate of the envelope.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgCoordinate GetUpperRightCoordinate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgCoordinate GetUpperRightCoordinate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgCoordinate GetUpperRightCoordinate();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// An MgCoordinate that defines the upper right corner of the envelope.
    ///
    MgCoordinate* GetUpperRightCoordinate();  /// __get

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the difference between the maximum and minimum X values.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// double GetWidth();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// double GetWidth();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// double GetWidth();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// A double representing the width of the envelope.
    ///
    double GetWidth();  /// __get

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the difference between the maximum and minimum Y values.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// double GetHeight();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// double GetHeight();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// double GetHeight();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// A double representing the height of the envelope.
    ///
    double GetHeight();  /// __get

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the difference between the maximum and minimum Z values.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// double GetDepth();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// double GetDepth();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// double GetDepth();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// A double representing the depth of the envelope.
    ///
    double GetDepth();  /// __get

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Tests whether this Envelope is "null" or not.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool IsNull();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean IsNull();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool IsNull();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// True if this envelope is "null", false otherwise.
    ///
    bool IsNull();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Makes this envelope a "null" envelope.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void MakeNull();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void MakeNull();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void MakeNull();
    /// \htmlinclude SyntaxBottom.html
    ///
    void MakeNull();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enlarges the boundary of the envelope so that it contains the given
    /// coordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void ExpandToInclude(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void ExpandToInclude(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void ExpandToInclude(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coordinate (MgCoordinate)
    /// The MgCoordinate to expand this envelope to include.
    ///
    void ExpandToInclude(MgCoordinate* coordinate);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enlarges the boundary of the envelope so that it contains the given
    /// envelope.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void ExpandToInclude(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void ExpandToInclude(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void ExpandToInclude(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param envelope (MgEnvelope)
    /// The MgEnvelope to expand this envelope to include.
    ///
    void ExpandToInclude(MgEnvelope* envelope);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Tests whether the given coordinate lies in or on this envelope.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Contains(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean Contains(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Contains(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coordinate (MgCoordinate)
    /// The MgCoordinate to test containment of.
    ///
    /// \return
    /// True if the coordinate is contained in this envelope, false otherwise.
    ///
    bool Contains(MgCoordinate* coordinate);

    //////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Tests whether the given envelope lies completely inside this
    /// Envelope (inclusive of the boundary).
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Contains(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean Contains(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Contains(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param envelope (MgEnvelope)
    /// The MgEnvelope to test containment of.
    ///
    /// \return
    /// True if the envelope argument is contained in this envelope,
    /// false otherwise.
    ///
    bool Contains(MgEnvelope* envelope);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Tests if the region defined by another envelope overlaps (intersects)
    /// the region of this envelope.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Intersects(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean Intersects(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Intersects(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param envelope (MgEnvelope)
    /// The MgEnvelope to test for intersection.
    ///
    /// \return
    /// True if the envelopes intersect, false otherwise.
    ///
    bool Intersects(MgEnvelope* envelope);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a transformed copy of this envelope.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgEnvelope Transform(MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgEnvelope Transform(MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgEnvelope Transform(MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param transform (MgTransform)
    /// The MgTransform to use in constructing a copy of this envelope.
    ///
    /// \return
    /// An MgEnvelope that is a copy of this one but whose region is
    /// transformed.
    ///
    MgEnvelope* Transform(MgTransform* transform);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

    virtual void ToXml(string& str);
    MgByteReader* ToXml();

    void Grow(double offset);

protected:

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_Envelope;

protected:
    Ptr<MgCoordinate>   m_lowerLeft;
    Ptr<MgCoordinate>   m_upperRight;
};
/// \}

#endif //_MGENVELOPE_H_

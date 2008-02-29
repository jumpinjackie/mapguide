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

#ifndef _CENVELOPE_H_
#define _CENVELOPE_H_

namespace CSLibrary
{
//////////////////////////////////////////////////////////////
/// <summary>
/// CEnvelope represents a bounding box defined in terms of a
/// lower left coordinate and an upper right coordinate.
/// </summary>

class COORDINATE_SYSTEM_API CEnvelope
{
public:
    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Constructs and intializes an "null" envelope.
    ///</summary>
    CEnvelope();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Constructs and intializes an envelope for a region defined by one
    /// coordinate.
    ///</summary>
    ///<param name="double x">
    /// The x coordinate to use in defining the region.
    ///</param>
    ///<param name="double y">
    /// The y coordinate to use in defining the region.
    ///</param>
    CEnvelope(double x, double y);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Constructs and intializes an envelope for a region defined by one
    /// coordinate.
    ///</summary>
    ///<param name="double x">
    /// The x coordinate to use in defining the region.
    ///</param>
    ///<param name="double y">
    /// The y coordinate to use in defining the region.
    ///</param>
    ///<param name="double z">
    /// The z coordinate to use in defining the region.
    ///</param>
    CEnvelope(double x, double y, double z);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Constructs and intializes an envelope for a region defined by two
    /// coordinates.
    ///</summary>
    ///<param name="double x1">
    /// The first x coordinate to use in defining the region.
    ///</param>
    ///<param name="double y1">
    /// The first y coordinate to use in defining the region.
    ///</param>
    ///<param name="double x2">
    /// The second x coordinate to use in defining the region.
    ///</param>
    ///<param name="double y2">
    /// The second y coordinate to use in defining the region.
    ///</param>
    CEnvelope(double x1, double y1, double x2, double y2);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Constructs and intializes an envelope for a region defined by two
    /// coordinates.
    ///</summary>
    ///<param name="double x1">
    /// The first x coordinate to use in defining the region.
    ///</param>
    ///<param name="double y1">
    /// The first y coordinate to use in defining the region.
    ///</param>
    ///<param name="double z1">
    /// The first z coordinate to use in defining the region.
    ///</param>
    ///<param name="double x2">
    /// The second x coordinate to use in defining the region.
    ///</param>
    ///<param name="double y2">
    /// The second y coordinate to use in defining the region.
    ///</param>
    ///<param name="double z2">
    /// The second z coordinate to use in defining the region.
    ///</param>
    CEnvelope(double x1, double y1, double z1, double x2, double y2, double z2);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Constructs and intializes an envelope that is a copy of another.
    ///</summary>
    ///<param name="CEnvelope envelope">
    /// The CEnvelope to copy.
    ///</param>
    CEnvelope(CEnvelope* envelope);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the lower left x coordinate of the envelope.
    ///</summary>
    ///<returns>
    /// A double that defines the lower left corner x coordinate of the envelope.
    ///</returns>
    double GetLowerLeftXCoordinate() { return m_lowerLeftX; }

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the lower left y coordinate of the envelope.
    ///</summary>
    ///<returns>
    /// A double that defines the lower left corner y coordinate of the envelope.
    ///</returns>
    double GetLowerLeftYCoordinate() { return m_lowerLeftY; }

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the lower left z coordinate of the envelope.
    ///</summary>
    ///<returns>
    /// A double that defines the lower left corner z coordinate of the envelope.
    ///</returns>
    double GetLowerLeftZCoordinate() { return m_lowerLeftZ; }

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the upper right x coordinate of the envelope.
    ///</summary>
    ///<returns>
    /// A double that defines the upper right corner x coordinate of the envelope.
    ///</returns>
    double GetUpperRightXCoordinate() { return m_upperRightX; }

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the upper right y coordinate of the envelope.
    ///</summary>
    ///<returns>
    /// A double that defines the upper right corner y coordinate of the envelope.
    ///</returns>
    double GetUpperRightYCoordinate() { return m_upperRightY; }

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the upper right z coordinate of the envelope.
    ///</summary>
    ///<returns>
    /// A double that defines the upper right corner z coordinate of the envelope.
    ///</returns>
    double GetUpperRightZCoordinate() { return m_upperRightZ; }

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the difference between the maximum and minimum X values.
    ///</summary>
    ///<returns>
    /// A double representing the width of the envelope.
    ///</returns>
    double GetWidth();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the difference between the maximum and minimum Y values.
    ///</summary>
    ///<returns>
    /// A double representing the height of the envelope.
    ///</returns>
    double GetHeight();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the difference between the maximum and minimum Z values.
    ///</summary>
    ///<returns>
    /// A double representing the depth of the envelope.
    ///</returns>
    double GetDepth();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Tests whether this Envelope is "null" or not.
    ///</summary>
    ///<returns>
    /// True if this envelope is "null", false otherwise.
    ///</returns>
    bool IsNull();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Makes this envelope a "null" envelope.
    ///</summary>
    void MakeNull();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Gets the Dimension of this Envelope.
    ///</summary>
    ///<returns>
    /// The # of dimensions.
    ///</returns>
    int GetDimension();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Enlarges the boundary of the envelope so that it contains the given
    /// coordinate.
    ///</summary>
    ///<param name="double x">
    /// The x coordinate to expand this envelope to include.
    ///</param>
    ///<param name="double y">
    /// The y coordinate to expand this envelope to include.
    ///</param>
    void ExpandToInclude(double x, double y);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Enlarges the boundary of the envelope so that it contains the given
    /// coordinate.
    ///</summary>
    ///<param name="double x">
    /// The x coordinate to expand this envelope to include.
    ///</param>
    ///<param name="double y">
    /// The y coordinate to expand this envelope to include.
    ///</param>
    ///<param name="double z">
    /// The z coordinate to expand this envelope to include.
    ///</param>
    void ExpandToInclude(double x, double y, double z);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Enlarges the boundary of the envelope so that it contains the given
    /// envelope.
    ///</summary>
    ///<param name="CEnvelope envelope">
    /// The CEnvelope to expand this envelope to include.
    ///</param>
    void ExpandToInclude(CEnvelope* envelope);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Tests whether the given coordinate lies in or on this envelope.
    ///</summary>
    ///<param name="double x">
    /// The x coordinate to test containment of.
    ///</param>
    ///<param name="double y">
    /// The y coordinate to test containment of.
    ///</param>
    ///<returns>
    /// True if the coordinate is contained in this envelope, false otherwise.
    ///</returns>
    bool Contains(double x, double y);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Tests whether the given coordinate lies in or on this envelope.
    ///</summary>
    ///<param name="double x">
    /// The x coordinate to test containment of.
    ///</param>
    ///<param name="double y">
    /// The y coordinate to test containment of.
    ///</param>
    ///<param name="double z">
    /// The z coordinate to test containment of.
    ///</param>
    ///<returns>
    /// True if the coordinate is contained in this envelope, false otherwise.
    ///</returns>
    bool Contains(double x, double y, double z);

    //////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Tests whether the given envelope lies completely inside this
    /// Envelope (inclusive of the boundary).
    /// </summary>
    /// <param name="CEnvelope envelope">The CEnvelope to test containment of. </param>
    /// <returns>
    /// True if the envelope argument is contained in this envelope,
    /// false otherwise.
    /// </returns>
    bool Contains(CEnvelope* envelope);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Tests if the region defined by another envelope overlaps (intersects)
    /// the region of this envelope.
    ///</summary>
    ///<param name="CEnvelope envelope">
    /// The CEnvelope to test for intersection.
    ///</param>
    ///<returns>
    /// True if the envelopes intersect, false otherwise.
    ///</returns>
    bool Intersects(CEnvelope* envelope);

protected:
    double m_lowerLeftX;
    double m_lowerLeftY;
    double m_lowerLeftZ;
    double m_upperRightX;
    double m_upperRightY;
    double m_upperRightZ;
};

} // End of namespace

#endif //_CENVELOPE_H_

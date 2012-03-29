//
//  Copyright (C) 2005-2011 by Autodesk, Inc.
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

#ifndef MGSIZE2D_H_
#define MGSIZE2D_H_

/// <summary>
/// A class that represents a two-dimensional size, height and width.
/// </summary>
class MG_PLATFORMBASE_API MgSize2D : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgSize2D)

INTERNAL_API:
    //-------------------------------------------------------
    // Constructors
    //-------------------------------------------------------

    /// <summary>
    /// Initializes a new instance of the MgSize2D class with
    /// the specified components.
    /// </summary>
    /// <param name="height">height component of the size.</param>
    /// <param name="width">width component of the size.</param>
    MgSize2D(const double height, const double width);

    /// <summary>
    /// Copy constructor
    /// </summary>
    /// <param name="vec">The size to be copied.</param>
    MgSize2D(const MgSize2D& size);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgSize2D object.
    ///
    MgSize2D();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destroys an MgSize2D object.
    ///
    ~MgSize2D();

    //-------------------------------------------------------
    // MgSize2D implementation
    //-------------------------------------------------------

    /// <summary>
    /// Returns whether this size is equal to the supplied size.
    /// </summary>
    /// <param name="vec">The size to compare to.</param>
    /// <param name="tolerance">The tolerance to use when comparing.</param>
    /// <returns>True if the sizes are equal; otherwise false.</returns>
    bool isEqualTo(const MgSize2D& size, double tolerance = 1.0e-12) const;

    //-------------------------------------------------------
    // Operator overloads
    //-------------------------------------------------------

    /// <summary>
    /// Copy operator
    /// </summary>
    /// <param name="vec">The size to copy.</param>
    /// <returns>A reference to this size.</returns>
    MgSize2D& operator=(const MgSize2D& size);

    /// <summary>
    /// Operator that determines whether two sizes are unequal.  Two sizes
    /// are equal if they have the same components.
    /// </summary>
    /// <param name="vec">The size to compare to.</param>
    /// <returns>True if the size are equal; otherwise false.</returns>
    bool operator==(const MgSize2D& size) const;

    /// <summary>
    /// Operator that determines whether two sizes are unequal.  Two sizes
    /// are unequal if they have one or more different components.
    /// </summary>
    /// <param name="vec">The size to compare to.</param>
    /// <returns>True if the size are unequal; otherwise false.</returns>
    bool operator!=(const MgSize2D& size) const;

    //-------------------------------------------------------
    // Accessor methods
    //-------------------------------------------------------

    /// <summary>
    /// Gets the height component of the size.
    /// </summary>
    /// <returns>The height value of the size.</returns>
    double GetHeight() const;

    /// <summary>
    /// Gets the width component of the size.
    /// </summary>
    /// <returns>The width value of the size.</returns>
    double GetWidth() const;

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes data to TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserializes data from TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

protected:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class ID.
    ///
    /// \return
    /// The integer value.
    ///
    virtual INT32 GetClassId();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Disposes this object.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_Data_Size2D;

private:
    //-------------------------------------------------------
    // Variables
    //-------------------------------------------------------

    double m_height;
    double m_width;
};

#endif

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

#ifndef MGVECTOR3D_H_
#define MGVECTOR3D_H_

/// <summary>
/// A class that represents a three-dimensional vector.
/// </summary>
class MG_PLATFORMBASE_API MgVector3D : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgVector3D)

INTERNAL_API:
    //-------------------------------------------------------
    // Constructors
    //-------------------------------------------------------

    /// <summary>
    /// Initializes a new instance of the MgVector3D class with
    /// the specified components.
    /// </summary>
    /// <param name="X">X component of the vector.</param>
    /// <param name="Y">Y component of the vector.</param>
    /// <param name="Z">Z component of the vector.</param>
    MgVector3D(const double X, const double Y, const double Z);

    /// <summary>
    /// Copy constructor
    /// </summary>
    /// <param name="vec">The vector to be copied.</param>
    MgVector3D(const MgVector3D& vec);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgVector3D object.
    ///
    MgVector3D();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destroys an MgVector3D object.
    ///
    ~MgVector3D();

    //-------------------------------------------------------
    // MgVector3D implementation
    //-------------------------------------------------------

    /// <summary>
    /// Returns whether this vector is equal to the supplied vector.
    /// </summary>
    /// <param name="vec">The vector to compare to.</param>
    /// <param name="tolerance">The tolerance to use when comparing.</param>
    /// <returns>True if the vectors are equal; otherwise false.</returns>
    bool isEqualTo(const MgVector3D& vec, double tolerance = 1.0e-12) const;

    /// <summary>
    /// Returns the normal of the vector.
    /// </summary>
    /// <returns>A new normalized vector.</returns>
    MgVector3D normal(const double& tolerance = 1.0e-12) const;

    /// <summary>
    /// Normalize the vector.  This ensures the vector has unit length.
    /// </summary>
    /// <returns>A reference to this vector after normalization.</returns>
    MgVector3D& normalize(const double& tolerance = 1.0e-12);

    /// <summary>
    /// Returns the length of this vector.
    /// </summary>
    double length() const;

    /// <summary>
    /// Returns the length squared of this vector.
    /// </summary>
    double lengthSqrd() const;

    /// <summary>
    /// Returns the dot product of this vector with the supplied vector.
    /// </summary>
    /// <param name="vec">The vector to perform the dot product with.</param>
    double dotProduct(const MgVector3D& vec) const;

    /// <summary>
    /// Returns the cross product of this vector with the supplied vector.
    /// </summary>
    /// <param name="vec">The vector to perform the cross product with.</param>
    MgVector3D crossProduct(const MgVector3D& vec) const;

    //-------------------------------------------------------
    // Operator overloads
    //-------------------------------------------------------

    /// <summary>
    /// Copy operator
    /// </summary>
    /// <param name="vec">The vector to copy.</param>
    /// <returns>A reference to this vector.</returns>
    MgVector3D& operator=(const MgVector3D& vec);

    /// <summary>
    /// Operator that determines whether two vectors are equal.  Two vectors
    /// are equal if they have the same components.
    /// </summary>
    /// <param name="vec">The vector to compare to.</param>
    /// <returns>True if the vectors are equal; otherwise false.</returns>
    bool operator==(const MgVector3D& vec) const;

    /// <summary>
    /// Operator that determines whether two vectors are unequal.  Two vectors
    /// are unequal if they have one or more different components.
    /// </summary>
    /// <param name="vec">The vector to compare to.</param>
    /// <returns>True if the vectors are unequal; otherwise false.</returns>
    bool operator!=(const MgVector3D& vec) const;

    /// <summary>
    /// Operator that scales a vector.
    /// </summary>
    /// <param name="scale">The scale factor for the vector.</param>
    /// <returns>The scaled vector.</returns>
    MgVector3D operator*(const double scale) const;

    /// <summary>
    /// Operator that scales a vector.
    /// </summary>
    /// <param name="scale">The scale factor for the vector.</param>
    /// <param name="vec">The vector to scale.</param>
    /// <returns>The scaled vector.</returns>
    friend MgVector3D operator*(const double scale, const MgVector3D& vec)
    {
        return MgVector3D(vec.x * scale, vec.y * scale, vec.z * scale);
    }

    /// <summary>
    /// Operator that inverse-scales a vector.
    /// </summary>
    /// <param name="scale">The scale factor for the vector.</param>
    /// <returns>The scaled vector.</returns>
    MgVector3D operator/(const double scale) const;

    /// <summary>
    /// Operator that adds two vectors.
    /// </summary>
    /// <param name="vec">The vector to add.</param>
    /// <returns>The result of adding the specified vector to this vector.</returns>
    MgVector3D operator+(const MgVector3D& vec) const;

    /// <summary>
    /// Operator that subtracts two vectors.
    /// </summary>
    /// <param name="vec">The vector to subtract.</param>
    /// <returns>The result of subtracting the specified vector from this vector.</returns>
    MgVector3D operator-(const MgVector3D& vec) const;

    /// <summary>
    /// Operator that negates a vector.
    /// </summary>
    /// <returns>The negated vector.</returns>
    MgVector3D operator-() const;

    /// <summary>
    /// Operator to access x, y, z via indexer.  0 represents the X component of
    /// the vector, 1 represents the Y component of the vector, and 2 represents
    /// the Z component of the vector.
    /// </summary>
    /// <returns>The indexed x, y, or z value.</returns>
    double operator[](const unsigned int i) const;

    /// <summary>
    /// Operator to access x, y, z via indexer.  0 represents the X component of
    /// the vector, 1 represents the Y component of the vector, and 2 represents
    /// the Z component of the vector.
    /// </summary>
    /// <returns>The indexed x, y, or z value.</returns>
    double& operator[](const unsigned int i);

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
    static const INT32 m_cls_id = PlatformBase_Data_Vector3D;

private:
    //-------------------------------------------------------
    // Variables
    //-------------------------------------------------------

    // the components
    double x;
    double y;
    double z;
};

#endif

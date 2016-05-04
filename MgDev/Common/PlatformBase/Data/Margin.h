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

#ifndef MgMargin_H_
#define MgMargin_H_

/// <summary>
/// A class that represents a margin, top, left, right and bottom.
/// </summary>
class MG_PLATFORMBASE_API MgMargin : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgMargin)

INTERNAL_API:
    //-------------------------------------------------------
    // Constructors
    //-------------------------------------------------------

    /// <summary>
    /// Initializes a new instance of the MgMargin class with
    /// the specified components.
    /// </summary>
    /// <param name="left">left component of the margin.</param>
    /// <param name="top">top component of the margin.</param>
    /// <param name="right">right component of the margin.</param>
    /// <param name="botton">bottom component of the margin.</param>
    MgMargin(const double left, const double top, const double right, const double bottom);

    /// <summary>
    /// Copy constructor
    /// </summary>
    /// <param name="margin">The margin to be copied.</param>
    MgMargin(const MgMargin& margin);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgMargin object.
    ///
    MgMargin();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destroys an MgMargin object.
    ///
    ~MgMargin();

    //-------------------------------------------------------
    // MgMargin implementation
    //-------------------------------------------------------

    /// <summary>
    /// Returns whether this margin is equal to the supplied margin.
    /// </summary>
    /// <param name="margin">The margin to compare to.</param>
    /// <param name="tolerance">The tolerance to use when comparing.</param>
    /// <returns>True if the margins are equal; otherwise false.</returns>
    bool isEqualTo(const MgMargin& margin, double tolerance = 1.0e-12) const;

    //-------------------------------------------------------
    // Operator overloads
    //-------------------------------------------------------

    /// <summary>
    /// Copy operator
    /// </summary>
    /// <param name="margin">The margin to copy.</param>
    /// <returns>A reference to this margin.</returns>
    MgMargin& operator=(const MgMargin& margin);

    /// <summary>
    /// Operator that determines whether two margin are unequal.  Two margin
    /// are equal if they have the same components.
    /// </summary>
    /// <param name="margin">The margin to compare to.</param>
    /// <returns>True if the margin are equal; otherwise false.</returns>
    bool operator==(const MgMargin& margin) const;

    /// <summary>
    /// Operator that determines whether two margin are unequal.  Two margin
    /// are unequal if they have one or more different components.
    /// </summary>
    /// <param name="margin">The margin to compare to.</param>
    /// <returns>True if the margin are unequal; otherwise false.</returns>
    bool operator!=(const MgMargin& margin) const;

    //-------------------------------------------------------
    // Accessor methods
    //-------------------------------------------------------

    /// <summary>
    /// Gets the left component of the margin.
    /// </summary>
    /// <returns>The left margin value.</returns>
    double GetLeft() const;

    /// <summary>
    /// Gets the right component of the margin.
    /// </summary>
    /// <returns>The right margin value.</returns>
    double GetRight() const;

    /// <summary>
    /// Gets the top component of the margin.
    /// </summary>
    /// <returns>The top margin value.</returns>
    double GetTop() const;

    /// <summary>
    /// Gets the bottom component of the margin.
    /// </summary>
    /// <returns>The bottom margin value.</returns>
    double GetBottom() const;

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
    static const INT32 m_cls_id = PlatformBase_Data_Margin;

private:
    //-------------------------------------------------------
    // Variables
    //-------------------------------------------------------

    double m_left;
    double m_right;
    double m_top;
    double m_bottom;
};

#endif

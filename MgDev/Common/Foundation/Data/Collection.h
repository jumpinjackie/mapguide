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

#ifndef _MG_COLLECTION_H_
#define _MG_COLLECTION_H_

/// \defgroup MgCollection MgCollection
/// \ingroup Collections_Module
/// \{

class MgByteReader;

/////////////////////////////////////////////////////////////////
/// \brief
/// MgCollection is an abstract class for defining standard collection access
/// for all collection classes.
///
/// \note
/// Collections are NOT thread safe and ordered in the sequence of add operation.
class MG_FOUNDATION_API MgCollection : public MgSerializable
{
PUBLISHED_API:
    /// \brief
    /// Gets the number of items in the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetCount() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetCount() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetCount() const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the number of items in the collection.
    ///
    virtual INT32 GetCount() const = 0;

    /// \brief
    /// Removes all items from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Clear();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Clear();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Clear();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void Clear() = 0;

    /// \brief
    /// Removes the specified item from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void RemoveAt(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void RemoveAt(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void RemoveAt(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void RemoveAt(INT32 index) = 0;

EXTERNAL_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Creates an XML document representing the collection.
    ///
    /// \return
    /// A pointer to an MgByteReader object.
    ///
    virtual MgByteReader* ToXml() = 0;

INTERNAL_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Destructor.  Provides cleanup for derived classes.
    ///
    virtual ~MgCollection();
};
/// \}

#endif

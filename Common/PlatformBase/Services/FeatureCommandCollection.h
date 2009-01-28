//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef _MG_FEATURE_COMMAND_COLLECTION_H
#define _MG_FEATURE_COMMAND_COLLECTION_H

/// \defgroup MgFeatureCommandCollection MgFeatureCommandCollection
/// \ingroup Collections_Module
/// \{

class MgFeatureCommandCollection;
template class MG_PLATFORMBASE_API Ptr<MgFeatureCommandCollection>;

/// \brief
/// Represents a collection of feature commands.
///
/// \remarks
/// Feature commands are used to
/// perform insert, update, and delete operations on an Fdo data source.  This
/// collection is an argument to MgFeatureService::UpdateFeatures.  The feature
/// commands are executed in the order they appear in the collection.  Additional
/// arguments to UpdateFeatures control whether or not the commands are executed
/// as part of a transaction.
///
class MG_PLATFORMBASE_API  MgFeatureCommandCollection : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgFeatureCommandCollection)

PUBLISHED_API:
    /// \brief
    /// Gets the number of commands in the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetCount() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetCount() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetCount() const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns number of commands in the collection
    ///
    INT32 GetCount() const;

    /// \brief
    /// Gets the command in the collection at the specified index.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgFeatureCommand GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgFeatureCommand GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgFeatureCommand GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    ///
    /// \return
    /// Returns the command in the collection at the specified index
    ///
    /// \exception MgIndexOutOfRangeException if index is out of range.
    ///
    MgFeatureCommand* GetItem(INT32 index) const;

    /// \brief
    /// Sets the command in the collection at the specified index to the specified command.
    /// If there was already command assigned to specified index, it will be overwritten.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetItem(int index, MgFeatureCommand command);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetItem(int index, MgFeatureCommand command);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetItem(int index, MgFeatureCommand command);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    /// \param command (MgFeatureCommand)
    /// Input command
    ///
    /// \return
    /// Returns nothing
    ///
    /// \exception MgIndexOutOfRangeException if index is out of range.
    ///
    void SetItem(INT32 index, MgFeatureCommand* command);

    /////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified command to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int Add(MgFeatureCommand command);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int Add(MgFeatureCommand command);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int Add(MgFeatureCommand command);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param command (MgFeatureCommand)
    /// One of type MgDeleteFeatures, MgInsertFeatures,
    /// or MgUpdateFeatures
    ///
    /// \return
    /// Returns nothing.
    ///
    void Add(MgFeatureCommand* command);

    /// \brief
    /// Inserts the specified command at the specified index within the collection.
    /// Items following the insertion point are moved down to accommodate the new command.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Insert(int index, MgFeatureCommand command);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Insert(int index, MgFeatureCommand command);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Insert(int index, MgFeatureCommand command);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    /// \param command (MgFeatureCommand)
    /// Input command
    ///
    /// \return
    /// Returns nothing
    ///
    /// \exception MgIndexOutOfRangeException if index is out of range.
    ///
    void Insert(INT32 index, MgFeatureCommand* command);

    /// \brief
    /// Removes all commands from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Clear();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Clear();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Clear();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing
    ///
    void Clear();

    /// \brief
    /// Removes the specified command from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Remove(MgFeatureCommand command);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// bool Remove(MgFeatureCommand command);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Remove(MgFeatureCommand command);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param command (MgFeatureCommand)
    /// Input command
    ///
    /// \return
    /// Returns true if removal was successful.
    ///

    bool Remove(MgFeatureCommand* command);

    /// \brief
    /// Removes the specified command from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void RemoveAt(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void RemoveAt(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void RemoveAt(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    ///
    /// \return
    /// Returns nothing
    ///
    /// \exception MgIndexOutOfRangeException if index is out of range.
    ///
    void RemoveAt(INT32 index);

    /// \brief
    /// Returns true if the collection contains the specified command, false otherwise.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Contains(const MgFeatureCommand command) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean Contains(const MgFeatureCommand command) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Contains(const MgFeatureCommand command) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param command (MgFeatureCommand)
    /// Input command
    ///
    /// \return
    /// Returns true if the collection contains the specified command, false otherwise
    ///
    bool Contains(const MgFeatureCommand* command) const;

    /// \brief
    /// Returns the index of the specified command in the collection or -1 if the command does not exist.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int IndexOf(const MgFeatureCommand command) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int IndexOf(const MgFeatureCommand command) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int IndexOf(const MgFeatureCommand command) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param command (MgFeatureCommand)
    /// Input command
    ///
    /// \return
    /// Returns the index of the specified command in the collection or -1 if the command does not exist
    ///
    INT32 IndexOf(const MgFeatureCommand* command) const;

    /// \brief
    /// Constructor.  Creates an empty collection of feature commands.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgFeatureCommandCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgFeatureCommandCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgFeatureCommandCollection();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgFeatureCommandCollection();

EXTERNAL_API:

    /// \brief
    /// Destructor.  Deletes contained feature commands.
    ///
    ~MgFeatureCommandCollection();

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes data to a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserializes data from a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// Returns the integer value.
    ///
    INT32 GetClassId();

    virtual void Dispose();

private:

    Ptr<MgDisposableCollection> m_dCollection;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_FeatureCommandCollection;
};
/// \}

#endif // _MGFEATUREWRITER_H

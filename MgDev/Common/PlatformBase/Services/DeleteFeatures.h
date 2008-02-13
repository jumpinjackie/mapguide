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

/// \defgroup MgDeleteFeatures MgDeleteFeatures
/// \ingroup Feature_Service_classes
/// \{

#ifndef _MGDELETEFEATURES_H
#define _MGDELETEFEATURES_H

class MgDeleteFeatures;
template class MG_PLATFORMBASE_API Ptr<MgDeleteFeatures>;

/////////////////////////////////////////////////////////////////
/// \brief
/// A feature command that features from a datastore. The feature
/// class name and filter text define feature(s) to delete. This
/// class is added to an MgFeatureCommandCollection object, which
/// is then passed in a call to the
/// MgFeatureService::UpdateFeatures method.
///
class MG_PLATFORMBASE_API  MgDeleteFeatures : public MgFeatureCommand
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgDeleteFeatures)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgDeleteFeatures object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgDeleteFeatures(string className, string filterText);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgDeleteFeatures(String className, String filterText);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgDeleteFeatures(string className, string filterText);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param className (String/string)
    /// The name of the class from which the
    /// data is to be deleted.
    /// \param filterText (String/string)
    /// The filter used to select features
    /// for deletion.
    ///
    /// \return
    /// Returns nothing.
    ///
    MgDeleteFeatures(   CREFSTRING className,
                        CREFSTRING filterText   );

    //////////////////////////////////////////////////
    /// \brief
    /// Gets the type of the command, that is,
    /// MgFeatureCommandType::DeleteFeatures.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetCommandType() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetCommandType() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetCommandType() const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns MgFeatureCommandType::DeleteFeatures.
    ///
    INT32 GetCommandType() const;  /// __get

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the feature class name of the feature objects to be deleted.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetFeatureClassName() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetFeatureClassName() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetFeatureClassName() const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the feature class name of the feature objects to be deleted
    ///
    STRING GetFeatureClassName() const;  /// __get

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the filter used to select the features to be deleted
    /// from the datastore.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetFilterText() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetFilterText() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetFilterText() const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the filter used to select the features to be deleted
    /// from the datastore.
    ///
    STRING GetFilterText() const;  /// __get

EXTERNAL_API:

    /// \brief
    /// Destructor
    ///
    ~MgDeleteFeatures();

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

    MgDeleteFeatures() {}

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// Returns the integer value.
    ///
    INT32 GetClassId();

    virtual void Dispose();

CLASS_ID:

    static const INT32 m_cls_id = PlatformBase_FeatureService_DeleteFeatures;

private:

    STRING m_className;
    STRING m_filterText;
};
/// \}

#endif // _MGDELETEFEATURES_H

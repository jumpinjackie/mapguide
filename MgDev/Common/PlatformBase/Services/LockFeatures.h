//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef _MGLOCKFEATURES_H
#define _MGLOCKFEATURES_H

class MgLockFeatures;
template class MG_PLATFORMBASE_API Ptr<MgLockFeatures>;

/// \defgroup MgLockFeatures MgLockFeatures
/// \ingroup Feature_Service_classes
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates an insert feature data command. You must add this
/// command to an MgFeatureCommandCollection object and pass the
/// latter in a call to MgFeatureService::UpdateFeatures.
///
/// \remarks
///
class MG_PLATFORMBASE_API MgLockFeatures : public MgFeatureCommand
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgLockFeatures);

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a lock feature comand
    MgLockFeatures(CREFSTRING className, CREFSTRING filterText);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the type of the command, that is,
    /// MgFeatureCommandType::LockFeatures.
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
    /// Returns MgFeatureCommandType::LockFeatures.
    ///
    virtual INT32 GetCommandType() const;    /// __get

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the feature class.
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
    /// Returns the name of the feature class.
    ///
    STRING GetFeatureClassName() const;   /// __get

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the filter text.
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
    /// Returns the filter text.
    ///
    STRING GetFilterText() const;   /// __get

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

protected:
    MgLockFeatures();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// Returns the integer value.
    ///
    INT32 GetClassId();

    void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_LockFeatures;

private:
    STRING m_className;
    STRING m_filterText;
};
/// \}

#endif

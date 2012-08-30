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

#ifndef DESKTOP_DWF_VERSION_H
#define DEKSTOP_DWF_VERSION_H

/// \ingroup Desktop_Misc_Module
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// Specifies the \link DWF DWF \endlink schema and file version for
/// all Mapping Service operations.
///
/// \remarks
/// These parameters should be set by the client so the MapGuide
/// server generates client-side compatible DWFs.
///
class MG_DESKTOP_API MgdDwfVersion : public MgSerializable
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgdDwfVersion)

PUBLISHED_API:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a DWF specification.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgdDwfVersion(string fileVersion, string schemaVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgdDwfVersion(String fileVersion, String schemaVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgdDwfVersion(string fileVersion, string schemaVersion);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param fileVersion (String/string)
    /// Supported client-side DWF file
    /// version.
    /// \param schemaVersion (String/string)
    /// Supported client-side DWF schema
    /// version.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// $dwfVersion = new MgdDwfVersion('6.01','1.2');
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    MgdDwfVersion(CREFSTRING fileVersion, CREFSTRING schemaVersion);

    //////////////////////////////////////////////
    /// \brief
    /// Gets the DWF file version string.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetFileVersion();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetFileVersion();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetFileVersion();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the DWF file version as a string.
    ///
    STRING GetFileVersion();

    ////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the supported client-side file version for DWF.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetFileVersion(string fileVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetFileVersion(String fileVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetFileVersion(string fileVersion);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param fileVersion (String/string)
    /// Supported client-side file version of
    /// DWF.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetFileVersion(CREFSTRING fileVersion);

    ////////////////////////////////////////////////
    /// \brief
    /// Gets the DWF schema version.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetSchemaVersion();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetSchemaVersion();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetSchemaVersion();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the DWF schema version as a string.
    ///
    STRING GetSchemaVersion();

    ////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the supported client-side schema version for DWF.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetSchemaVersion(string schemaVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetSchemaVersion(String schemaVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetSchemaVersion(string schemaVersion);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param schemaVersion (String/string)
    /// Input supported client-side schema
    /// version
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetSchemaVersion(CREFSTRING schemaVersion);

protected:

    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose()
    {
        delete this;
    }

    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId() { return m_cls_id; }

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_MappingService_DwfVersion;

INTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Default constructor to shut Ptr<> up.
    ///
    MgdDwfVersion();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to a stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from a stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

private:
    STRING  m_fileVersion;
    STRING  m_schemaVersion;
};
/// \}

#endif // DESKTOP_DWF_VERSION_H

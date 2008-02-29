//
//  Copyright (C) 1996-2008 by Autodesk, Inc.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE CORRECTNESS
//  OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE IT. AUTODESK
//  PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY DISCLAIMS ANY
//  LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL DAMAGES FOR ERRORS,
//  OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject to
//  restrictions set forth in FAR 52.227-19 (Commercial Computer Software
//  Restricted Rights) and DFAR 252.227-7013(c)(1)(ii) (Rights in Technical
//  Data and Computer Software), as applicable.
//


#ifndef _DWFTK_PACKAGE_VERSION_EXTENSION_H
#define _DWFTK_PACKAGE_VERSION_EXTENSION_H

///
///\file        dwf/package/writer/PackageVersionExtension.h
///\brief       This file contains the DWFPackageVersionExtension interface declaration.
///

#ifndef DWFTK_READ_ONLY

#ifdef minor
#undef minor
#endif

#ifdef major
#undef major
#endif

#include "dwfcore/String.h"
#include "dwfcore/Exception.h"
using namespace DWFCore;

#include "dwf/Toolkit.h"
#include "dwf/package/utility/DWFPackageFileDescriptor.h"
namespace DWFToolkit
{

//
// fwd decl
//
class DWFInterface;
class DWFSection;
class DWFGlobalSection;
class DWFPackageWriter;
class DWFXMLSerializer;
class DWFPackageFileDescriptor;


///
///\ingroup     dwfpackage
///
///\class       DWFPackageVersionExtension   dwf/package/writer/PackageVersionExtension.h     "dwf/package/writer/PackageVersionExtension.h"
///\brief       This is the plug-in interface for the DWFPackageWriter.
///\since       7.0.1
///
///             This interface is intended to be used to modify the publish-time
///             behavior of the DWFPackageWriter; specifically to capture and
///             implement version-specific differences in the DWF.  
///             As a general rule, version variations may be created to support
///             a new feature or introduce new functionality.  Eventually, these
///             might be removed from the format entirely but the ability
///             to produce the package variations must continue to exist.
///
///             The interface works by mimicking some of the DWFPackageWriter interface.
///             The writer, when used with an extension, will first make the matching
///             call on the extension. This methods must return a boolean value
///             that communicates the continuation intent back to the writer.  
///             If \e true, the writer will continue to process its own call path after
///             returning from the extension.
///             If \e false, the writer will return immediately from its own call path
///             after returning from the extension.
///
class DWFPackageVersionExtension 
{

public:

    ///
    ///         Destructor
    /// 
    ///\throw   None
    ///
    _DWFTK_API
    virtual ~DWFPackageVersionExtension()
        throw(){;}

    ///
    ///         Returns the DWF File Format major version for this extension.
    ///
    ///\return  The integer value of the major format version.
    ///\throw   None
    ///
    _DWFTK_API
    virtual unsigned short major()
        throw() = 0;

    ///
    ///         Returns the DWF File Format minor version for this extension.
    ///
    ///\return  The integer value of the minor format version.
    ///\throw   None
    ///
    _DWFTK_API
    virtual unsigned short minor()
        throw() = 0;

    ///
    ///\copydoc DWFPackageWriter::addSection()
    ///
    _DWFTK_API
    virtual bool addSection( DWFSection*   pSection,
                             DWFInterface* pInterface = NULL )
        throw( DWFException ) = 0;

    ///
    ///\copydoc DWFPackageWriter::addGlobalSection()
    ///
    _DWFTK_API
    virtual bool addGlobalSection( DWFGlobalSection* pSection )
        throw( DWFException ) = 0;

    ///
    ///\copydoc DWFPackageWriter::write()
    ///
    _DWFTK_API
    virtual bool write( const DWFString& zSourceProductVendor = L"",
                        const DWFString& zSourceProductName = L"",
                        const DWFString& zSourceProductVersion = L"",
                        const DWFString& zDWFProductVendor = L"",
                        const DWFString& zDWFProductVersion = L"",
                        DWFZipFileDescriptor::teFileMode eCompressionMode = DWFZipFileDescriptor::eZipSmallest )
        throw( DWFException ) = 0;


    ///
    ///         Hook provided just before the <i>manifest.xml</i> document is created and published.
    ///
    ///\param   rPackageWriter      The object managing the creation of the DWF package file.
    ///\param   rPackageDescriptor  The file descriptor of the DWF package file being created.
    ///\param   rXMLSerializer      The object being used to create the XML data.
    ///\param   zPackagePassword    If the entire package is being password protected
    ///                             or if this extension is adding a password protected file,
    ///                             this password will be used.
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual void prewriteManifest( DWFPackageWriter&            rPackageWriter, 
                                   DWFPackageFileDescriptor&    rPackageDescriptor,
                                   DWFXMLSerializer&            rXMLSerializer,
                                   const DWFString&             rPackagePassword )
        throw( DWFException ) = 0;

    ///
    ///         Hook provided after the <i>manifest.xml</i> document is created and published.
    ///
    ///\param   rPackageWriter      The object managing the creation of the DWF package file.
    ///\param   rPackageDescriptor  The file descriptor of the DWF package file being created.
    ///\param   rXMLSerializer      The object being used to create the XML data.
    ///\param   zPackagePassword    If the entire package is being password protected
    ///                             or if this extension is adding a password protected file,
    ///                             this password will be used.
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual void postwriteManifest( DWFPackageWriter&            rPackageWriter, 
                                    DWFPackageFileDescriptor&    rPackageDescriptor,
                                    DWFXMLSerializer&            rXMLSerializer,
                                    const DWFString&             rPackagePassword )
        throw( DWFException ) = 0;

    ///
    ///         Hook provided after all sections have been published.
    ///
    ///\param   rPackageWriter      The object managing the creation of the DWF package file.
    ///\param   rPackageDescriptor  The file descriptor of the DWF package file being created.
    ///\param   rXMLSerializer      The object being used to create the XML data.
    ///\param   zPackagePassword    If the entire package is being password protected
    ///                             or if this extension is adding a password protected file,
    ///                             this password will be used.
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual void postwriteSections( DWFPackageWriter&            rPackageWriter, 
                                    DWFPackageFileDescriptor&    rPackageDescriptor,
                                    DWFXMLSerializer&            rXMLSerializer,
                                    const DWFString&             rPackagePassword )
        throw( DWFException ) = 0;

protected:

    ///
    ///         Constructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    DWFPackageVersionExtension()
        throw(){;}
};

}


#endif
#endif


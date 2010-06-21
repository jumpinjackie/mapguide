//
//  Copyright (C) 1996-2010 by Autodesk, Inc.
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


#ifndef _DWFTK_PACKAGE_WRITER_H
#define _DWFTK_PACKAGE_WRITER_H

///
///\file        dwf/package/writer/PackageWriter.h
///\brief       This file contains the DWFPackageWriter class declaration.
///


#ifndef DWFTK_READ_ONLY



#include "dwfcore/STL.h"
#include "dwfcore/UUID.h"
#include "dwfcore/OutputStream.h"
using namespace DWFCore;

#include "dwf/Toolkit.h"
#include "dwf/package/XML.h"
#include "dwf/package/Constants.h"
#include "dwf/package/writer/DWFXMLSerializer.h"
#include "dwf/package/writer/PackageVersionExtension.h"
#include "dwf/package/utility/DWFPackageFileDescriptor.h"


namespace DWFToolkit
{

//
// fwd decl
//
class DWFManifest;
class DWFResource;
class DWFInterface;
class DWFSection;
class DWFGlobalSection;
class DWFPropertyContainer;

///
///\ingroup     dwfpackage
///
///\class       DWFPackageWriter   dwf/package/writer/PackageWriter.h     "dwf/package/writer/PackageWriter.h"
///\brief       This class is used to publish DWF package files.
///\since       7.0.1
///
///\todo        Add direct publishing to DWFOutputStream in addition to DWFFile respresentation.
///
class DWFPackageWriter      _DWFTK_EXPORT_API_MEMORY_CONTROL_BASE
{

public:

        ///
        ///\brief   These enumerations are used during the serialization process
        ///         to alter the content produced by the serializable objects.
        ///
    typedef enum
    {
        eManifest           = 0x10,
        eDescriptor         = 0x40,
        eObjectDefinition   = 0x80

    } teDocumentType;

        ///
        ///\brief   These enumerations are used during the serialization process
        ///         to alter the content produced by the serializable objects.
        ///
    typedef enum
    {
        eNone               = 0x00,
        eEPlot              = 0x01,
        eEModel             = 0x02,
        eGlobal             = 0x04

    } teSectionType;

public:

    ///
    ///         Constructor
    ///
    ///\param   rDWFPackageFile         Identifies the DWF file to publish.
    ///\param   zDWFPackagePassword     An optional password for protecting the DWF package file.
    ///\param   pVersionExtension       An optional plug-in for publishing package versions different from the curent one.
    ///\throw   None
    ///
    _DWFTK_API
    DWFPackageWriter( const DWFFile&                rDWFPackageFile,
                      const DWFString&              zDWFPackagePassword = L"",
                      DWFPackageVersionExtension*   pVersionExtension = NULL )
        throw();

    ///
    ///         Destructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    virtual ~DWFPackageWriter()
        throw();

    ///
    ///         Adds a section to the DWF package.
    ///
    ///\param   pSection                The section to publish into the package.
    ///                                 Ownership of this section pointer will be claimed automatically.
    ///                                 This means that, left as is, the pointer will be deleted
    ///                                 once the package is published.  
    ///                                 In order to prevent this, the caller can explicitly reclaim
    ///                                 ownership of the section by calling DWFSection::own().
    ///
    ///\param   pInterface              This object should be provided if \a pSection is a custom type that
    ///                                 should be declared in the manifest (it will be ignored for known types.)
    ///                                 Note that the DWF Specification dictates that at least one interface
    ///                                 be declared in the manifest.  If none exist, an exception will be thrown at write time.
    ///                                 This pointer will be deleted with the \b DWFCORE_FREE_OBJECT macro.
    ///\throw   DWFException
    ///
    _DWFTK_API
    void addSection( DWFSection*    pSection,
                     DWFInterface*  pInterface = NULL )
        throw( DWFException );

    ///
    ///         Adds a type global section to the DWF package.
    ///
    ///\param   pSection                The global section to publish into the package.
    ///                                 Ownership of this section pointer will be claimed automatically.
    ///                                 This means that, left as is, the pointer will be deleted
    ///                                 once the package is published.  
    ///                                 In order to prevent this, the caller can explicitly reclaim
    ///                                 ownership of the section by calling \a DWFSection::own().
    ///\throw   DWFException
    ///
    _DWFTK_API
    void addGlobalSection( DWFGlobalSection* pSection )
        throw( DWFException );

    ///
    ///         Adds an interface to the DWF package manifest.
    /// 
    ///         This method is provided in addition to the \a addSection() method
    ///         as it may be necessary to, in the case of dumb package copying
    ///         for example, to include this information in the target package
    ///         without having any ability to corelate the source sections
    ///         and interfaces.  
    ///         This pointer will be deleted with the \b DWFCORE_FREE_OBJECT macro.
    ///
    ///         This method will check for duplicate interfaces and
    ///         if \a pInternface is a duplicate, it will
    ///         be deleted and the previously stored one will be returned.
    ///
    ///\throw   DWFException
    ///
    _DWFTK_API
    DWFInterface* addInterface( DWFInterface* pInterface )
        throw( DWFException );

    ///
    ///         Returns the property container interface
    ///         for the package manifest.  
    ///
    ///         This can be used to add package-wide properties to the DWF.
    ///
    ///\return  An interface with which to add package manifest properties.
    ///\throw   DWFException
    ///
    _DWFTK_API
    DWFPropertyContainer& getManifestProperties()
        throw( DWFException );

    ///
    ///         Writes out the DWF package.
    ///
    ///         Any sections and other objects owned by the writer and/or internal manifest
    ///         will be deleted at the completion of this method.  Be sure to claim ownership
    ///         of any such objects that are necessary to survive after this call.
    ///
    ///         This method exposes the standard DWF package properties directly.
    ///         It is highly recommended that at DWF publishers set these correctly with as
    ///         much detail as possible.
    ///
    ///         Setting standard DWF package properties:
    ///         \code
    ///         Autodesk DWF Writer is a printer driver DLL that is used to generate DWF packages
    ///         through the Windows printing system, it is a component that another application
    ///         would use (indirectly) to publish the DWF.  The following is an example of how
    ///         it might populate these properties when printing from Microsoft Word:
    ///
    ///         zSourceProductVendor    = L"Microsoft Corporation"          <--- from Word 2003
    ///         zSourceProductName      = L"Microsoft Office 2003"          <--- from Word 2003
    ///         zSourceProductVersion   = L"11.0.5604"                      <--- from Word 2003
    ///         zDWFProductVendor       = L"Autodesk, Inc."                 <--- from DWF Writer 2
    ///         zDWFProductVersion      = L"2.0.4.0"                        <--- from DWF Writer 2
    ///         \endcode
    ///
    ///\param   zSourceProductVendor    A standard property detailing the vendor of the software
    ///                                 that produced the original content and used the DWF product
    ///                                 in order to produce the DWF package.
    ///\param   zSourceProductName      A standard property detailing the name of the software
    ///                                 that produced the original content and used the DWF product
    ///                                 in order to produce the DWF package.
    ///\param   zSourceProductVersion   A standard property detailing the version of the software
    ///                                 that produced the original content and used the DWF product
    ///                                 in order to produce the DWF package.
    ///\param   zDWFProductVendor       A standard property that details the vendor of the software component
    ///                                 that was used by the source product to publish the DWF package.
    ///\param   zDWFProductVersion      A standard property that details the version of the software component
    ///                                 that was used by the source product to publish the DWF package.
    ///\param   eCompressionMode        An enumeration that adjusts the zip library compression options.
    ///
    ///\throw   DWFException
    ///
    _DWFTK_API
    void write( const DWFString&                 zSourceProductVendor = L"",
                const DWFString&                 zSourceProductName = L"",
                const DWFString&                 zSourceProductVersion = L"",
                const DWFString&                 zDWFProductVendor = L"",
                const DWFString&                 zDWFProductVersion = L"",
                DWFZipFileDescriptor::teFileMode eCompressionMode = DWFZipFileDescriptor::eZipSmallest )
        throw( DWFException );

private:

    //
    //
    //
    void _serializeResource( const DWFString&   zSection,
                             DWFResource&       rResource )
        throw( DWFException );

    //
    // 
    //
    void _ensureManifest()
        throw( DWFException );

private:

    DWFFile                     _rDWFPackage;
    DWFString                   _zDWFPassword;

    DWFUUID                     _oUUID;
    DWFXMLSerializer*           _pXMLSerializer;
    DWFPackageVersionExtension* _pVersionExtension;

    DWFPackageFileDescriptor*   _pPackageDescriptor;
    DWFManifest*                _pPackageManifest;

    double                      _nNextPlot;

    bool                        _bEPlotGlobalAdded;
    bool                        _bEModelGlobalAdded;

    size_t                      _nEPlotSectionCount;
    size_t                      _nEModelSectionCount;

    vector<DWFSection*>         _oSections;
    vector<DWFGlobalSection*>   _oGlobalSections;

private:

    DWFPackageWriter();
    DWFPackageWriter( const DWFPackageWriter& );
    DWFPackageWriter& operator=( const DWFPackageWriter& );
};

}

#endif
#endif


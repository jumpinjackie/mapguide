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


#ifndef _DWFTK_VERSION_H
#define _DWFTK_VERSION_H


///
///\file        dwf/Version.h
///\brief       This file contains DWF file format version information and macros 
///             for use in toolkit client code.
///



//
    // IMPORTANT: Toolkit client code should NOT use these internal macros.
//
    // Doxygen should skip these.
//

#define _DWF_FORMAT_VERSION_MAJOR                   6       // Internal Use Only
#define _DWF_FORMAT_VERSION_MINOR                  11       // Internal Use Only
#define _DWF_FORMAT_VERSION_EXTRA                   0       // Internal Use Only -- Reserved



///
///\defgroup    DWFFileFormat       DWF File Format
///             These macros are available for referencing the current and
///             historical revisions of the DWF File Format.
///
///\note        The DWF File Format is different from the DWF Toolkit version.
///

///
///\ingroup     DWFFileFormat
///\brief       The current major revision of the DWF File Format.
///
///             This macro should be used by client code wishing to reference
///             the current major version of the DWF file format.
///
///             A major version change indicates a significant structural
///             and or content shift in the file.  Generally speaking,
///             while the toolkit will always attempt to provide as much
///             backwards compatability as possible, it may do so, in order
///             to account for such changes, with a new collection of data and API
///             objects.  A major version change will almost definitely result in
///             a port of the toolkit client code.
///
#define _DWF_FORMAT_VERSION_CURRENT_MAJOR           _DWF_FORMAT_VERSION_MAJOR
///
///\ingroup     DWFFileFormat
///\brief       The current minor revision of the DWF File Format.
///
///             This macro should be used by client code wishing to reference
///             the current minor version of the DWF file format.
///
///             A minor version change usually indicates that some content or structure
///             was adding or modified in a way that would not impair existing DWF processors.
///             Extensions or additions to schemas, introductions of new section types and
///             document and elements are all examples of minor revision triggers.
///             
///             Toolkit client code should always perform a minor version check to 
///             ensure 100% compatability and in the case that an increment occurs
///             at least notify the end user that some new data may exist that the
///             current product may not be able to handle/display/etc.
///
#define _DWF_FORMAT_VERSION_CURRENT_MINOR           _DWF_FORMAT_VERSION_MINOR
///
///\ingroup     DWFFileFormat
///\brief       The current DWF File Format version number as a composite integer value.
///
///             The current value of this macro is <b>611</b>.
///
#define _DWF_FORMAT_VERSION_CURRENT                 ((_DWF_FORMAT_VERSION_CURRENT_MAJOR*100) + _DWF_FORMAT_VERSION_CURRENT_MINOR)
///
///\ingroup     DWFFileFormat
///\brief       The current DWF File Format version number as a composite floating point value.
///
///             The current value of this macro is <b>6.11</b>.
///
#define _DWF_FORMAT_VERSION_CURRENT_FLOAT           (float)(_DWF_FORMAT_VERSION_CURRENT_MAJOR + (_DWF_FORMAT_VERSION_CURRENT_MINOR/100.0))
///
///\ingroup     DWFFileFormat
///\brief       The current DWF File Format version number as an ASCII character string.
///
#define _DWF_FORMAT_VERSION_CURRENT_STRING          "6.11"
///
///\ingroup     DWFFileFormat
///\brief       The current DWF File Format version number as a wide character string.
///
#define _DWF_FORMAT_VERSION_CURRENT_WIDE_STRING     L"6.11"

///
///\defgroup    DWFFileFormatEComposite     EComposite Version
///\ingroup     DWFFileFormat
///             These macros are available for referencing the version
///             of the file format that introduced dwfs that contain both ePlot 
///             and eModel sections
///             

///
///\ingroup     DWFFileFormatEComposite
///\brief       The major revision of the DWF File Format introducing eComposite dwfs.
///
#define _DWF_FORMAT_VERSION_ECOMPOSITE_MAJOR          6
///
///\ingroup     DWFFileFormatEComposite
///\brief       The minor revision of the DWF File Format introducing eComposite dwfs.
///
#define _DWF_FORMAT_VERSION_ECOMPOSITE_MINOR          11
///
///\ingroup     DWFFileFormatEComposite
///\brief       The version number of the DWF File Format introducing eComposite dwfs as a composite integer value.
///
///             The value of this macro is <b>611</b>.
///
#define _DWF_FORMAT_VERSION_ECOMPOSITE                ((_DWF_FORMAT_VERSION_ECOMPOSITE_MAJOR*100) + _DWF_FORMAT_VERSION_ECOMPOSITE_MINOR)
///
///\ingroup     DWFFileFormatEComposite
///\brief       The version number of the DWF File Format introducing eComposite dwfs as a composite floating point value.
///
///             The value of this macro is <b>6.11</b>.
///
#define _DWF_FORMAT_VERSION_ECOMPOSITE_FLOAT          (float)(_DWF_FORMAT_VERSION_ECOMPOSITE_MAJOR + (_DWF_FORMAT_VERSION_ECOMPOSITE_MINOR/100.0))
///
///\ingroup     DWFFileFormatEComposite
///\brief       The version number of the DWF File Format introducing eComposite dwfs as an ASCII character string.
///
#define _DWF_FORMAT_VERSION_ECOMPOSITE_STRING         "6.11"
///
///\ingroup     DWFFileFormatEComposite
///\brief       The version number of the DWF File Format introducing eComposite dwfs as a wide character string.
///
#define _DWF_FORMAT_VERSION_ECOMPOSITE_WIDE_STRING    L"6.11"


///
///\defgroup    DWFFileFormatTI     TYPEINFO Version
///\ingroup     DWFFileFormat
///             These macros are available for referencing the version
///             of the file format that introduced the TYPEINFO file.
///             
///             The TYPEINFO file must be placed as the very first file
///             in the DWF package archive.  It's presence can be detected
///             in the binary stream directly after the header for immediate
///             recognition and processing.  It must be named the same as the
///             DWFInterface identifier that it represents.
///

///
///\ingroup     DWFFileFormatTI
///\brief       The major revision of the DWF File Format introducing the TYPEINFO file.
///
#define _DWF_FORMAT_VERSION_TYPEINFO_MAJOR          6
///
///\ingroup     DWFFileFormatTI
///\brief       The minor revision of the DWF File Format introducing the TYPEINFO file.
///
#define _DWF_FORMAT_VERSION_TYPEINFO_MINOR          1
///
///\ingroup     DWFFileFormatTI
///\brief       The version number of the DWF File Format introducing the TYPEINFO file as a composite integer value.
///
///             The value of this macro is <b>601</b>.
///
#define _DWF_FORMAT_VERSION_TYPEINFO                ((_DWF_FORMAT_VERSION_TYPEINFO_MAJOR*100) + _DWF_FORMAT_VERSION_TYPEINFO_MINOR)
///
///\ingroup     DWFFileFormatTI
///\brief       The version number of the DWF File Format introducing the TYPEINFO file as a composite floating point value.
///
///             The value of this macro is <b>6.01</b>.
///
#define _DWF_FORMAT_VERSION_TYPEINFO_FLOAT          (float)(_DWF_FORMAT_VERSION_TYPEINFO_MAJOR + (_DWF_FORMAT_VERSION_TYPEINFO_MINOR/100.0))
///
///\ingroup     DWFFileFormatTI
///\brief       The version number of the DWF File Format introducing the TYPEINFO file as an ASCII character string.
///
#define _DWF_FORMAT_VERSION_TYPEINFO_STRING         "6.01"
///
///\ingroup     DWFFileFormatTI
///\brief       The version number of the DWF File Format introducing the TYPEINFO file as a wide character string.
///
#define _DWF_FORMAT_VERSION_TYPEINFO_WIDE_STRING    L"6.01"


///
///\defgroup    DWFFileFormatPkg    Package Version
///\ingroup     DWFFileFormat
///             These macros are available for referencing the version
///             of the file format that introduced the zip package file format.
///             

///
///\ingroup     DWFFileFormatPkg
///\brief       The major revision of the DWF File Format introducing the zip package structure.
///
#define _DWF_FORMAT_VERSION_PACKAGE_MAJOR           6
///
///\ingroup     DWFFileFormatPkg
///\brief       The minor revision of the DWF File Format introducing the zip package structure.
///
#define _DWF_FORMAT_VERSION_PACKAGE_MINOR           0
///
///\ingroup     DWFFileFormatPkg
///\brief       The version number of the DWF File Format introducing the zip package structure as a composite integer value.
///
///             The value of this macro is <b>600</b>.
///
#define _DWF_FORMAT_VERSION_PACKAGE                 ((_DWF_FORMAT_VERSION_PACKAGE_MAJOR*100) + _DWF_FORMAT_VERSION_PACKAGE_MINOR)
///
///\ingroup     DWFFileFormatPkg
///\brief       The version number of the DWF File Format introducing the zip package structure as a composite floating point value.
///
///             The value of this macro is <b>6.0</b>.
///
#define _DWF_FORMAT_VERSION_PACKAGE_FLOAT           (float)(_DWF_FORMAT_VERSION_PACKAGE_MAJOR + (_DWF_FORMAT_VERSION_PACKAGE_MINOR/100.0))
///
///\ingroup     DWFFileFormatPkg
///\brief       The version number of the DWF File Format introducing the zip package structure as an ASCII character string.
///
#define _DWF_FORMAT_VERSION_PACKAGE_STRING          "6.0"
///
///\ingroup     DWFFileFormatPkg
///\brief       The version number of the DWF File Format introducing the zip package structure as a wide character string.
///
#define _DWF_FORMAT_VERSION_PACKAGE_WIDE_STRING     L"6.0"

///
///\defgroup    DWFFileFormatAlias  Version Aliases
///\ingroup     DWFFileFormat
///             These macros are available for referencing versions
///             of the file format that coincide with other introductory
///             features.
///             

///
///\ingroup     DWFFileFormatAlias
///\brief       The version number of the DWF File Format introducing composite dwfs
///
#define _DWF_FORMAT_VERSION_INTRO_ECOMPOSITE        _DWF_FORMAT_VERSION_ECOMPOSITE

///
///\ingroup     DWFFileFormatAlias
///\brief       The version number of the DWF File Format introducing 3D graphics streams.
///
#define _DWF_FORMAT_VERSION_INTRO_3D                _DWF_FORMAT_VERSION_TYPEINFO



///
///\defgroup    DWFManifestFormat   Package Manifest Schema Version
///\ingroup     DWFFileFormat
///             These macros are available for referencing the current version
///             of the schema for the manifest document.
///  
    
///
///\ingroup     DWFManifestFormat
///\brief       The major revision of the DWF package manfiest document schema.
///
#define _DWF_FORMAT_MANIFEST_VERSION_MAJOR              6
///
///\ingroup     DWFManifestFormat
///\brief       The minor revision of the DWF package manfiest document schema.
///
#define _DWF_FORMAT_MANIFEST_VERSION_MINOR              0
///
///\ingroup     DWFManifestFormat
///\brief       The current version of the DWF package manfiest document schema as a composite integer value.
///
///             The current value of this macro is <b>600</b>.
///
#define _DWF_FORMAT_MANIFEST_VERSION_CURRENT            ((_DWF_FORMAT_MANIFEST_VERSION_MAJOR*100) + _DWF_FORMAT_MANIFEST_VERSION_MINOR)
///
///\ingroup     DWFManifestFormat
///\brief       The current version of the DWF package manfiest document schema as a composite floating point value.
///
///             The current value of this macro is <b>6.0</b>.
///
#define _DWF_FORMAT_MANIFEST_VERSION_CURRENT_FLOAT      (float)(_DWF_FORMAT_MANIFEST_VERSION_MAJOR + (_DWF_FORMAT_MANIFEST_VERSION_MINOR/100.0))
///
///\ingroup     DWFManifestFormat
///\brief       The current version of the DWF package manfiest document schema as an ASCII character string.
///
#define _DWF_FORMAT_MANIFEST_VERSION_CURRENT_STRING     "6.0"

///
///\defgroup    DWFEPlotFormat      2D EPlot Section Descriptor Schema Version
///\ingroup     DWFFileFormat
///             These macros are available for referencing the current version
///             of the schema for the EPlot section descriptor documents.
/// 

///
///\ingroup     DWFEPlotFormat
///\brief       The major revision of the EPlot section descriptor document schema.
///
#define _DWF_FORMAT_EPLOT_VERSION_MAJOR                 1
///
///\ingroup     DWFEPlotFormat
///\brief       The minor revision of the EPlot section descriptor document schema.
///
#define _DWF_FORMAT_EPLOT_VERSION_MINOR                 20
///
///\ingroup     DWFEPlotFormat
///\brief       The current version of the EPlot section descriptor document schema as a composite integer value.
///
///             The current value of this macro is <b>120</b>.
///
#define _DWF_FORMAT_EPLOT_VERSION_CURRENT               ((_DWF_FORMAT_EPLOT_VERSION_MAJOR*100) + _DWF_FORMAT_EPLOT_VERSION_MINOR)
///
///\ingroup     DWFEPlotFormat
///\brief       The current version of the EPlot section descriptor document schema as a composite floating point value.
///
///             The current value of this macro is <b>1.2</b>.
///
#define _DWF_FORMAT_EPLOT_VERSION_CURRENT_FLOAT         (float)(_DWF_FORMAT_EPLOT_VERSION_MAJOR + (_DWF_FORMAT_EPLOT_VERSION_MINOR/100.0))
///
///\ingroup     DWFEPlotFormat
///\brief       The current version of the EPlot section descriptor document schema as an ASCII character string.
///
#define _DWF_FORMAT_EPLOT_VERSION_CURRENT_STRING        "1.2"
///
///\ingroup     DWFEPlotFormat
///\brief       The EPlot section type as an ASCII character string.
///
#define _DWF_FORMAT_EPLOT_TYPE_STRING                   "com.autodesk.dwf.ePlot"
///
///\ingroup     DWFEPlotFormat
///\brief       The EPlot section type as a wide character string.
///
#define _DWF_FORMAT_EPLOT_TYPE_WIDE_STRING             L"com.autodesk.dwf.ePlot"
///
///\ingroup     DWFEPlotFormat
///\brief       The Global EPlot section type as an ASCII character string.
///
#define _DWF_FORMAT_EPLOT_GLOBAL_TYPE_STRING            "com.autodesk.dwf.ePlotGlobal"
///
///\ingroup     DWFEPlotFormat
///\brief       The Global EPlot section type as a wide character string.
///
#define _DWF_FORMAT_EPLOT_GLOBAL_TYPE_WIDE_STRING      L"com.autodesk.dwf.ePlotGlobal"

///
///\defgroup    DWFEModelFormat      3D EModel Section Descriptor Schema Version
///\ingroup     DWFFileFormat
///             These macros are available for referencing the current version
///             of the schema for the EModel section descriptor documents.
/// 

///
///\ingroup     DWFEModelFormat
///\brief       The major revision of the EModel section descriptor document schema.
///
#define _DWF_FORMAT_EMODEL_VERSION_MAJOR                1
///
///\ingroup     DWFEModelFormat
///\brief       The minor revision of the EModel section descriptor document schema.
///
#define _DWF_FORMAT_EMODEL_VERSION_MINOR                0
///
///\ingroup     DWFEModelFormat
///\brief       The current version of the EModel section descriptor document schema as a composite integer value.
///
///             The current value of this macro is <b>100</b>.
///
#define _DWF_FORMAT_EMODEL_VERSION_CURRENT              ((_DWF_FORMAT_EMODEL_VERSION_MAJOR*100) + _DWF_FORMAT_EMODEL_VERSION_MINOR)
///
///\ingroup     DWFEModelFormat
///\brief       The current version of the EModel section descriptor document schema as a composite floating point value.
///
///             The current value of this macro is <b>1.0</b>.
///
#define _DWF_FORMAT_EMODEL_VERSION_CURRENT_FLOAT        (float)(_DWF_FORMAT_EMODEL_VERSION_MAJOR + (_DWF_FORMAT_EMODEL_VERSION_MINOR/100.0))
///
///\ingroup     DWFEModelFormat
///\brief       The current version of the EModel section descriptor document schema as an ASCII character string.
///
#define _DWF_FORMAT_EMODEL_VERSION_CURRENT_STRING       "1.0"
///
///\ingroup     DWFEModelFormat
///\brief       The EModel section type as an ASCII character string.
///
#define _DWF_FORMAT_EMODEL_TYPE_STRING                  "com.autodesk.dwf.eModel"
///
///\ingroup     DWFEModelFormat
///\brief       The EModel section type as a wide character string.
///
#define _DWF_FORMAT_EMODEL_TYPE_WIDE_STRING            L"com.autodesk.dwf.eModel"
///
///\ingroup     DWFEModelFormat
///\brief       The Global EModel section type as an ASCII character string.
///
#define _DWF_FORMAT_EMODEL_GLOBAL_TYPE_STRING           "com.autodesk.dwf.eModelGlobal"
///
///\ingroup     DWFEModelFormat
///\brief       The Global EModel section type as a wide character string.
///
#define _DWF_FORMAT_EMODEL_GLOBAL_TYPE_WIDE_STRING     L"com.autodesk.dwf.eModelGlobal"

//
//
// DWF Toolkit Versioning
//
//

#define _DWFTK_VERSION_MAJOR                        7           // Binary interface compatability (not compatable across revisions)
#define _DWFTK_VERSION_MINOR                        1           // Binary interface compatability (changes backwards compatable across revisions)
#define _DWFTK_VERSION_AGE                          1           // Binary interface compatability (not interface changes, internal changes only)
#define _DWFTK_VERSION_BUILD                        287         // Internal build difference

#define _DWFTK_VERSION_ECHO( s )                    #s
#define _DWFTK_VERSION_STRING_( a, b, c, d )        _DWFTK_VERSION_ECHO(a) "." _DWFTK_VERSION_ECHO(b) "." _DWFTK_VERSION_ECHO(c) "." _DWFTK_VERSION_ECHO(d) "\0"
#define _DWFTK_VERSION_STRING                       _DWFTK_VERSION_STRING_( _DWFTK_VERSION_MAJOR, _DWFTK_VERSION_MINOR, _DWFTK_VERSION_AGE, _DWFTK_VERSION_BUILD )


#endif

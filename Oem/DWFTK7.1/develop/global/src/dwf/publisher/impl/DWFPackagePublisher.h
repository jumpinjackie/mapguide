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

#ifndef _DWFTK_DWF_PACKAGE_PUBLISHER_H
#define _DWFTK_DWF_PACKAGE_PUBLISHER_H


///
///\file        dwf/publisher/impl/DWFPackagePublisher.h
///\brief       This file contains the DWFPackagePublisher class declaration.
///


#ifndef DWFTK_READ_ONLY


#include "dwfcore/File.h"
using namespace DWFCore;

#include "dwf/Toolkit.h"
#include "dwf/package/Section.h"
#include "dwf/package/ObjectDefinitionResource.h"
#include "dwf/package/writer/PackageWriter.h"
#include "dwf/publisher/Image.h"
#include "dwf/publisher/Publisher.h"
#include "dwf/publisher/model/Model.h"
#include "dwf/publisher/plot/Plot.h"

namespace DWFToolkit
{

///
///\ingroup         dwfpublish
///
///\class       DWFPackagePublisher   dwf/publisher/impl/DWFPackagePublisher.h   "dwf/publisher/impl/DWFPackagePublisher.h"
///\brief       An publisher implementation for producing DWF package files.
///\since       7.0.1
///
///             This is a reference implementation of the DWFPublisher base class for creating DWF packages.
///             This implementation was developed as part of the 3D Publishing API and, as such, produces
///             only 3D DWF 6.01 packages.  
///
///             Due to the composite nature of the the visitors, even this class can be used to extract
///             additional data to other output pipes during the creation of the DWF package.
///
///\todo        Implement a full version for composite DWF packages.
///
class DWFPackagePublisher : public DWFPublisher
                          , public DWFPublishedObject::Visitor
                          , public DWFPropertyVisitor
                          , public DWFEmbeddedFontVisitor

                          //_DWFTK_EXPORT_API_MEMORY_CONTROL_BASE_MEMBER
{

public:

    ///
    ///\brief   The name of the default model view (see DWFModel::createView).
    ///
    static const wchar_t* const kz_NamedView_Default;
    ///
    ///\brief   The name of the DWFProperty that captures the handedness heuristic (see DWFModel::getPolygonHandedness)
    ///
    static const wchar_t* const kz_PropName_PolygonHandedness;
    ///
    ///\brief   The name of the DWFProperty that captures the lighting preference (see DWFModel::useModelLighting)
    ///
    static const wchar_t* const kz_PropName_UseDefaultLighting;
    ///
    ///\brief   The name of the DWFProperty that captures the feature edge preference (see DWFModel::useSilhouetteEdges)
    ///
    static const wchar_t* const kz_PropName_UseSilhouetteEdges;
    ///
    ///\brief   The name of the DWFProperty that captures the edge color preference (see DWFModel::getEdgeColor)
    ///
    static const wchar_t* const kz_PropName_EdgeColor;

    ///
    ///\brief   The name of the DWFPropertyContainer that bundles the default (initial) model view.
    ///
    static const wchar_t* const kz_PropSetName_DefaultView;
    ///
    ///\brief   The name of the DWFProperty that decribes the default camera projection.
    ///
    static const wchar_t* const kz_PropName_CameraProjection;
    ///
    ///\brief   The name of the DWFProperty that decribes the default camera position.
    ///
    static const wchar_t* const kz_PropName_CameraPosition;
    ///
    ///\brief   The name of the DWFProperty that decribes the default camera target.
    ///
    static const wchar_t* const kz_PropName_CameraTarget;
    ///
    ///\brief   The name of the DWFProperty that decribes the default camera up vector.
    ///
    static const wchar_t* const kz_PropName_CameraUpVector;
    ///
    ///\brief   The name of the DWFProperty that decribes the default camera field.
    ///
    static const wchar_t* const kz_PropName_CameraField;

    ///
    ///\brief   The name of the DWFProperty that identifies an object.
    ///
    static const wchar_t* const kz_PropName_Published_Object;
    ///
    ///\brief   The DWFProperty category that marks it as hidden.
    ///
    static const wchar_t* const kz_PropCategory_Hidden;

public:

    ///
    ///         Constructor
    ///
    ///         This documentation has been copied from DWFPackageWriter::write():
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
    ///\param   rDWFFile                    The destination DWF package file name.
    ///\param   zPassword                   An optional password to protect the DWF package.
    ///\param   zSourceProductVendor        A standard property detailing the vendor of the software
    ///                                     that the produced the original content and used the DWF product
    ///                                     in order to produce the DWF package.
    ///\param   zSourceProductName          A standard property detailing the name of the software
    ///                                     that the produced the original content and used the DWF product
    ///                                     in order to produce the DWF package.
    ///\param   zSourceProductVersion       A standard property detailing the version of the software
    ///                                     that the produced the original content and used the DWF product
    ///                                     in order to produce the DWF package.
    ///\param   zPublisherProductVendor     A standard property that details the vendor of the software component
    ///                                     that was used by the source product to publish the DWF package.
    ///\param   zPublisherProductVersion    A standard property that details the version of the software component
    ///                                     that was used by the source product to publish the DWF package.
    ///\throw   DWFException
    ///
    _DWFTK_API
    DWFPackagePublisher( const DWFFile&   rDWFFile,
                         const DWFString& zPassword                = "",
                         const DWFString& zSourceProductVendor     = "",
                         const DWFString& zSourceProductName       = "",
                         const DWFString& zSourceProductVersion    = "",
                         const DWFString& zPublisherProductVendor  = "",
                         const DWFString& zPublisherProductVersion = "" )
        throw();

    ///
    ///         Destructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    virtual ~DWFPackagePublisher()
        throw();

    ///
    ///         Compose all of the publishable section content and graphics data
    ///         provided to this object and publish a DWF package file.
    ///
    ///\throw   DWFException
    ///
    _DWFTK_API
    void publish()
        throw( DWFException );

    ///
    ///\copydoc DWFPublisher::getPublishedObjectVisitor
    ///
    _DWFTK_API
    DWFPublishedObject::Visitor* getPublishedObjectVisitor()
        throw( DWFException )
    {
        return this;
    }

    ///
    ///\copydoc DWFPublisher::getPropertyVisitor
    ///
    _DWFTK_API
    DWFPropertyVisitor* getPropertyVisitor()
        throw( DWFException )
    {
        return this;
    }

    ///
    ///\copydoc DWFPublisher::getEmbeddedFontVisitor
    ///
    _DWFTK_API
    DWFEmbeddedFontVisitor* getEmbeddedFontVisitor()
        throw( DWFException )
    {
        return this;
    }

    ///
    ///\copydoc DWFPublishedObject::Visitor::visitPublishedObject
    ///
    _DWFTK_API
    void visitPublishedObject( DWFPublishedObject& rObject )
        throw( DWFException );

    ///
    ///\copydoc DWFPropertyVisitor::visitProperty
    ///
    _DWFTK_API
    void visitProperty( DWFProperty& rProperty )
        throw( DWFException );

    ///
    ///\copydoc DWFPropertyVisitor::visitPropertyContainer
    ///
    _DWFTK_API
    void visitPropertyContainer( DWFPropertyContainer& rPropertyContainer )
        throw( DWFException );

    ///
    ///\copydoc DWFEmbeddedFontVisitor::visitEmbeddedFont
    ///
    _DWFTK_API
    void visitEmbeddedFont( const DWFEmbeddedFont& rFont )
        throw( DWFException );

    ///
    ///\copydoc DWFPublisher::preprocess
    ///
    _DWFTK_API
    void preprocess( DWFPublishable* pContent )
        throw( DWFException );


    ///
    ///\copydoc DWFPublisher::postprocess
    ///
    _DWFTK_API
    void postprocess( DWFPublishable* pContent )
        throw( DWFException );

private:

    void _preprocessModel( DWFModel* pW3DModel )
        throw( DWFException );

    void _postprocessModel( DWFModel* pW3DModel )
        throw( DWFException );

    void _preprocessPlot( DWFPlot* pW2DPlot )
        throw( DWFException );

    void _postprocessPlot( DWFPlot* pW2DPlot )
        throw( DWFException );

    /***********************************************
    void _preprocessGeneric( DWFGeneric* pGeneric )
        throw( DWFException );

    void _postprocessGeneric( DWFGeneric* pGeneric )
        throw( DWFException );
    ***********************************************/

    void _definePublishedObject( DWFPublishedObject&        rPublishedObject,
                                 DWFDefinedObject**         ppDefinedObject,
                                 DWFDefinedObjectInstance** ppDefinedInstance )
        throw( DWFException );

    DWFDefinedObject* _findDefinedObject( DWFPublishedObject& rPublishedObject ) const
        throw();

private:

    unsigned int                        _nModels;
    unsigned int                        _nNextInstanceID;

    unsigned int                        _nPlots;

    DWFString                           _zSourceProductVendor;
    DWFString                           _zSourceProductName;
    DWFString                           _zSourceProductVersion;
    DWFString                           _zPublisherProductVendor;
    DWFString                           _zPublisherProductVersion;
    DWFPackageWriter                    _oPackageWriter;
    DWFEmbeddedFont::tList              _oEmbeddedFonts;

    DWFSection*                         _pCurrentSection;
    DWFObjectDefinitionResource*        _pCurrentObjectDefinition;

private:

    DWFPackagePublisher( const DWFPackagePublisher& );
    DWFPackagePublisher& operator=( const DWFPackagePublisher& );
};

    //
    // required for win32 dll external linkage
    //
#ifdef  _DWFCORE_WIN32_SYSTEM
#ifndef DWFTK_STATIC

_declspec(selectany) const wchar_t* const DWFPackagePublisher::kz_PropName_PolygonHandedness =   L"_PolygonHandedness";
_declspec(selectany) const wchar_t* const DWFPackagePublisher::kz_PropName_UseDefaultLighting =  L"_UseDefaultLighting";
_declspec(selectany) const wchar_t* const DWFPackagePublisher::kz_PropName_UseSilhouetteEdges =  L"_UseSilhouetteEdges";
_declspec(selectany) const wchar_t* const DWFPackagePublisher::kz_PropName_EdgeColor =           L"_EdgeColor";

_declspec(selectany) const wchar_t* const DWFPackagePublisher::kz_NamedView_Default =            L"default";
_declspec(selectany) const wchar_t* const DWFPackagePublisher::kz_PropSetName_DefaultView =      L"_DefaultView";
_declspec(selectany) const wchar_t* const DWFPackagePublisher::kz_PropName_CameraProjection =    L"_CameraProjection";
_declspec(selectany) const wchar_t* const DWFPackagePublisher::kz_PropName_CameraPosition =      L"_CameraPosition";
_declspec(selectany) const wchar_t* const DWFPackagePublisher::kz_PropName_CameraTarget =        L"_CameraTarget";
_declspec(selectany) const wchar_t* const DWFPackagePublisher::kz_PropName_CameraUpVector =      L"_CameraUpVector";
_declspec(selectany) const wchar_t* const DWFPackagePublisher::kz_PropName_CameraField =         L"_CameraField";

_declspec(selectany) const wchar_t* const DWFPackagePublisher::kz_PropName_Published_Object =    L"_name";
_declspec(selectany) const wchar_t* const DWFPackagePublisher::kz_PropCategory_Hidden =          L"hidden";

#endif
#endif

}



#endif
#endif


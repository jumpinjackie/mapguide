//
//  Copyright (C) 1996-2009 by Autodesk, Inc.
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


#ifndef _DWFTK_CONSTANTS_H
#define _DWFTK_CONSTANTS_H


///
///\file        dwf/package/Constants.h
///\brief       This file contains all DWF XML string declarations.
///


#include "dwfcore/Core.h"
#include "dwf/Toolkit.h"


///
///\ingroup     dwfpackage
///
///\class       DWFXML  dwf/package/Constants.h     "dwf/package/Constants.h"
///\brief       This class scopes the DWF XML strings known to the toolkit.
///\since       7.0.1
///
class DWFXML
{

public:

    ///
    ///\brief   "dwf:"
    ///
    static const char* const kzNamespace_DWF;
    ///
    ///\brief   "ePlot:"
    ///
    static const char* const kzNamespace_EPlot;
    ///
    ///\brief   "eModel:"
    ///
    static const char* const kzNamespace_EModel;
    ///
    ///\brief   "eCommon:"
    ///
    static const char* const kzNamespace_ECommon;

    ///
    ///\brief   "Toc"
    ///
    static const char* const kzElement_TOC;
    ///
    ///\brief   "Page"
    ///
    static const char* const kzElement_Page;
    ///
    ///\brief   "Paper"
    ///
    static const char* const kzElement_Paper;
    ///
    ///\brief   "Space"
    ///
    static const char* const kzElement_Space;
    ///
    ///\brief   "Units"
    ///
    static const char* const kzElement_Units;
    ///
    ///\brief   "Global"
    ///
    static const char* const kzElement_Global;
    ///
    ///\brief   "Source"
    ///
    static const char* const kzElement_Source;
    ///
    ///\brief   "Object"
    ///
    static const char* const kzElement_Object;
    ///
    ///\brief   "Objects"
    ///
    static const char* const kzElement_Objects;
    ///
    ///\brief   "Section"
    ///
    static const char* const kzElement_Section;
    ///
    ///\brief   "Sections"
    ///
    static const char* const kzElement_Sections;
    ///
    ///\brief   "Manifest"
    ///
    static const char* const kzElement_Manifest;
    ///
    ///\brief   "Bookmark"
    ///
    static const char* const kzElement_Bookmark;
    ///
    ///\brief   "Property"
    ///
    static const char* const kzElement_Property;
    ///
    ///\brief   "Resource"
    ///
    static const char* const kzElement_Resource;
///
///\brief   "Instance"
///
   	static const char* const kzElement_Instance;
///
///\brief   "Instances"
///
   	static const char* const kzElement_Instances;
///
///\brief   "Resources"
///
   	static const char* const kzElement_Resources;
///
///\brief   "Interface"
///
    static const char* const kzElement_Interface;
    ///
    ///\brief   "Interfaces"
    ///
    static const char* const kzElement_Interfaces;
    ///
    ///\brief   "Properties"
    ///
    static const char* const kzElement_Properties;
    ///
    ///\brief   "Dependency"
    ///
    static const char* const kzElement_Dependency;
    ///
    ///\brief   "Dependencies"
    ///
    static const char* const kzElement_Dependencies;
    ///
    ///\brief   "FontResource"
    ///
    static const char* const kzElement_FontResource;
    ///
    ///\brief   "ImageResource"
    ///
    static const char* const kzElement_ImageResource;
    ///
    ///\brief   "GraphicResource"
    ///
    static const char* const kzElement_GraphicResource;
    ///
    ///\brief   "ObjectDefinition"
    ///
    static const char* const kzElement_ObjectDefinition;
    ///
    ///\brief   "PageObjectDefinition"
    ///
    static const char* const kzElement_PageObjectDefinition;
    ///
    ///\brief   "SpaceObjectDefinition"
    ///
    static const char* const kzElement_SpaceObjectDefinition;
    ///
    ///\brief   "GlobalObjectDefinition"
    ///
    static const char* const kzElement_GlobalObjectDefinition;

    ///
    ///\brief   "id"
    ///
    static const char* const kzAttribute_ID;
    ///
    ///\brief   "name"
    ///
    static const char* const kzAttribute_Name;
    ///
    ///\brief   "type"
    ///
    static const char* const kzAttribute_Type;
    ///
    ///\brief   "href"
    ///
    static const char* const kzAttribute_HRef;
    ///
    ///\brief   "refs"
    ///
    static const char* const kzAttribute_Refs;
    ///
    ///\brief   "mime"
    ///
    static const char* const kzAttribute_MIME;
    ///
    ///\brief   "role"
    ///
    static const char* const kzAttribute_Role;
    ///
    ///\brief   "clip"
    ///
    static const char* const kzAttribute_Clip;
    ///
    ///\brief   "show"
    ///
    static const char* const kzAttribute_Show;
    ///
    ///\brief   "size"
    ///
    static const char* const kzAttribute_Size;
    ///
    ///\brief   "nodes"
    ///
    static const char* const kzAttribute_Nodes;
    ///
    ///\brief   "value"
    ///
    static const char* const kzAttribute_Value;
    ///
    ///\brief   "units"
    ///
    static const char* const kzAttribute_Units;
    ///
    ///\brief   "title"
    ///
    static const char* const kzAttribute_Title;
    ///
    ///\brief   "color"
    ///
    static const char* const kzAttribute_Color;
    ///
    ///\brief   "width"
    ///
    static const char* const kzAttribute_Width;
    ///
    ///\brief   "height"
    ///
    static const char* const kzAttribute_Height;
    ///
    ///\brief   "zorder"
    ///
    static const char* const kzAttribute_ZOrder;
    ///
    ///\brief   "object"
    ///
    static const char* const kzAttribute_Object;
    ///
    ///\brief   "author"
    ///
    static const char* const kzAttribute_Author;
    ///
    ///\brief   "extents"
    ///
    static const char* const kzAttribute_Extents;
    ///
    ///\brief   "version"
    ///
    static const char* const kzAttribute_Version;
    ///
    ///\brief   "request"
    ///
    static const char* const kzAttribute_Request;
    ///
    ///\brief   "scanned"
    ///
    static const char* const kzAttribute_Scanned;
    ///
    ///\brief   "children"
    ///
    static const char* const kzAttribute_Children;
    ///
    ///\brief   "provider"
    ///
    static const char* const kzAttribute_Provider;
    ///
    ///\brief   "objectId"
    ///
    static const char* const kzAttribute_ObjectID;
    ///
    ///\brief   "category"
    ///
    static const char* const kzAttribute_Category;
    ///
    ///\brief   "transform"
    ///
    static const char* const kzAttribute_Transform;
    ///
    ///\brief   "plotOrder"
    ///
    static const char* const kzAttribute_PlotOrder;
    ///
    ///\brief   "colorDepth"
    ///
    static const char* const kzAttribute_ColorDepth;
    ///
    ///\brief   "privilege"
    ///
    static const char* const kzAttribute_Privilege;
    ///
    ///\brief   "description"
    ///
    static const char* const kzAttribute_Description;
    ///
    ///\brief   "orientation"
    ///
    static const char* const kzAttribute_Orientation;
    ///
    ///\brief   "logfontName"
    ///
    static const char* const kzAttribute_LogfontName;
    ///
    ///\brief   "invertColors"
    ///
    static const char* const kzAttribute_InvertColors;
    ///
    ///\brief   "creationTime"
    ///
    static const char* const kzAttribute_CreationTime;
    ///
    ///\brief   "canonicalName"
    ///
    static const char* const kzAttribute_CanonicalName;
    ///
    ///\brief   "characterCode"
    ///
    static const char* const kzAttribute_CharacterCode;
    ///
    ///\brief   "parentObjectId"
    ///
    static const char* const kzAttribute_ParentObjectID;
    ///
    ///\brief   "originalExtents"
    ///
    static const char* const kzAttribute_OriginalExtents;
    ///
    ///\brief   "modificationTime"
    ///
    static const char* const kzAttribute_ModificationTime;
    ///
    ///\brief   "scannedResolution"
    ///
    static const char* const kzAttribute_ScannedResolution;
    ///
    ///\brief   "effectiveResolution"
    ///
    static const char* const kzAttribute_EffectiveResolution;



    ///
    ///\brief   L"2d streaming graphics"
    ///
    static const wchar_t* const kzRole_Graphics2d;
    ///
    ///\brief   L"3d streaming graphics"
    ///
    static const wchar_t* const kzRole_Graphics3d;
    ///
    ///\brief   L"descriptor"
    ///
    static const wchar_t* const kzRole_Descriptor;
    ///
    ///\brief   L"thumbnail"
    ///
    static const wchar_t* const kzRole_Thumbnail;
    ///
    ///\brief   L"2d vector overlay"
    ///
    static const wchar_t* const kzRole_Graphics2dOverlay;
    ///
    ///\brief   L"2d vector markup"
    ///
    static const wchar_t* const kzRole_Graphics2dMarkup;
    ///
    ///\brief   L"RML markup"
    ///
    static const wchar_t* const kzRole_RmlMarkup;
    ///
    ///\brief   L"preview"
    ///
    static const wchar_t* const kzRole_Preview;
    ///
    ///\brief   L"overlay preview"
    ///
    static const wchar_t* const kzRole_OverlayPreview;
    ///
    ///\brief   L"markup preview"
    ///
    static const wchar_t* const kzRole_MarkupPreview;
    ///
    ///\brief   L"raster overlay"
    ///
    static const wchar_t* const kzRole_RasterOverlay;
    ///
    ///\brief   L"raster markup"
    ///
    static const wchar_t* const kzRole_RasterMarkup;
    ///
    ///\brief   L"font"
    ///
    static const wchar_t* const kzRole_Font;
    ///
    ///\brief   L"metadata"
    ///
    static const wchar_t* const kzRole_Metadata;
    ///
    ///\brief   L"package signature"
    ///
    static const wchar_t* const kzRole_PackageSignature;
    ///
    ///\brief   L"section signature"
    ///
    static const wchar_t* const kzRole_SectionSignature;
    ///
    ///\brief   L"fileset signature"
    ///
    static const wchar_t* const kzRole_FilesetSignature;
    ///
    ///\brief   L"object definition"
    ///
    static const wchar_t* const kzRole_ObjectDefinition;
    ///
    ///\brief   L"markup object definition"
    ///
    static const wchar_t* const kzRole_MarkupObjectDefinition;
    ///
    ///\brief   L"texture"
    ///
    static const wchar_t* const kzRole_Texture;

};

    //
    // required for win32 dll external linkage
    //
#ifdef  _DWFCORE_WIN32_SYSTEM
#ifndef DWFTK_STATIC

_declspec(selectany) const char* const DWFXML::kzNamespace_DWF                   = "dwf:";
_declspec(selectany) const char* const DWFXML::kzNamespace_EPlot                 = "ePlot:";
_declspec(selectany) const char* const DWFXML::kzNamespace_EModel                = "eModel:";
_declspec(selectany) const char* const DWFXML::kzNamespace_ECommon               = "eCommon:";

_declspec(selectany) const char* const DWFXML::kzElement_TOC                     = "Toc";
_declspec(selectany) const char* const DWFXML::kzElement_Page                    = "Page";
_declspec(selectany) const char* const DWFXML::kzElement_Paper                   = "Paper";
_declspec(selectany) const char* const DWFXML::kzElement_Space                   = "Space";
_declspec(selectany) const char* const DWFXML::kzElement_Units                   = "Units";
_declspec(selectany) const char* const DWFXML::kzElement_Global                  = "Global";
_declspec(selectany) const char* const DWFXML::kzElement_Source                  = "Source";
_declspec(selectany) const char* const DWFXML::kzElement_Object                  = "Object";
_declspec(selectany) const char* const DWFXML::kzElement_Objects                 = "Objects";
_declspec(selectany) const char* const DWFXML::kzElement_Section                 = "Section";
_declspec(selectany) const char* const DWFXML::kzElement_Sections                = "Sections";
_declspec(selectany) const char* const DWFXML::kzElement_Manifest                = "Manifest";
_declspec(selectany) const char* const DWFXML::kzElement_Bookmark                = "Bookmark";
_declspec(selectany) const char* const DWFXML::kzElement_Property                = "Property";
_declspec(selectany) const char* const DWFXML::kzElement_Resource                = "Resource";
_declspec(selectany) const char* const DWFXML::kzElement_Resources               = "Resources";
_declspec(selectany) const char* const DWFXML::kzElement_Instance                = "Instance";
_declspec(selectany) const char* const DWFXML::kzElement_Instances               = "Instances";
_declspec(selectany) const char* const DWFXML::kzElement_Interface               = "Interface";
_declspec(selectany) const char* const DWFXML::kzElement_Interfaces              = "Interfaces";
_declspec(selectany) const char* const DWFXML::kzElement_Properties              = "Properties";
_declspec(selectany) const char* const DWFXML::kzElement_Dependency              = "Dependency";
_declspec(selectany) const char* const DWFXML::kzElement_Dependencies            = "Dependencies";
_declspec(selectany) const char* const DWFXML::kzElement_FontResource            = "FontResource";
_declspec(selectany) const char* const DWFXML::kzElement_ImageResource           = "ImageResource";
_declspec(selectany) const char* const DWFXML::kzElement_GraphicResource         = "GraphicResource";
_declspec(selectany) const char* const DWFXML::kzElement_ObjectDefinition        = "ObjectDefinition";
_declspec(selectany) const char* const DWFXML::kzElement_PageObjectDefinition    = "PageObjectDefinition";
_declspec(selectany) const char* const DWFXML::kzElement_SpaceObjectDefinition   = "SpaceObjectDefinition";
_declspec(selectany) const char* const DWFXML::kzElement_GlobalObjectDefinition  = "GlobalObjectDefinition";

_declspec(selectany) const char* const DWFXML::kzAttribute_ID                    = "id";
_declspec(selectany) const char* const DWFXML::kzAttribute_Name                  = "name";
_declspec(selectany) const char* const DWFXML::kzAttribute_Type                  = "type";
_declspec(selectany) const char* const DWFXML::kzAttribute_HRef                  = "href";
_declspec(selectany) const char* const DWFXML::kzAttribute_Refs                  = "refs";
_declspec(selectany) const char* const DWFXML::kzAttribute_MIME                  = "mime";
_declspec(selectany) const char* const DWFXML::kzAttribute_Role                  = "role";
_declspec(selectany) const char* const DWFXML::kzAttribute_Clip                  = "clip";
_declspec(selectany) const char* const DWFXML::kzAttribute_Show                  = "show";
_declspec(selectany) const char* const DWFXML::kzAttribute_Size                  = "size";
_declspec(selectany) const char* const DWFXML::kzAttribute_Nodes                 = "nodes";
_declspec(selectany) const char* const DWFXML::kzAttribute_Value                 = "value";
_declspec(selectany) const char* const DWFXML::kzAttribute_Units                 = "units";
_declspec(selectany) const char* const DWFXML::kzAttribute_Title                 = "title";
_declspec(selectany) const char* const DWFXML::kzAttribute_Color                 = "color";
_declspec(selectany) const char* const DWFXML::kzAttribute_Width                 = "width";
_declspec(selectany) const char* const DWFXML::kzAttribute_Height                = "height";
_declspec(selectany) const char* const DWFXML::kzAttribute_ZOrder                = "zOrder";
_declspec(selectany) const char* const DWFXML::kzAttribute_Object                = "object";
_declspec(selectany) const char* const DWFXML::kzAttribute_Author                = "author";
_declspec(selectany) const char* const DWFXML::kzAttribute_Extents               = "extents";
_declspec(selectany) const char* const DWFXML::kzAttribute_Version               = "version";
_declspec(selectany) const char* const DWFXML::kzAttribute_Request               = "request";
_declspec(selectany) const char* const DWFXML::kzAttribute_Scanned               = "scanned";
_declspec(selectany) const char* const DWFXML::kzAttribute_Children              = "children";
_declspec(selectany) const char* const DWFXML::kzAttribute_Provider              = "provider";
_declspec(selectany) const char* const DWFXML::kzAttribute_ObjectID              = "objectId";
_declspec(selectany) const char* const DWFXML::kzAttribute_Category              = "category";
_declspec(selectany) const char* const DWFXML::kzAttribute_Transform             = "transform";
_declspec(selectany) const char* const DWFXML::kzAttribute_PlotOrder             = "plotOrder";
_declspec(selectany) const char* const DWFXML::kzAttribute_ColorDepth            = "colorDepth";
_declspec(selectany) const char* const DWFXML::kzAttribute_Privilege             = "privilege";
_declspec(selectany) const char* const DWFXML::kzAttribute_Description           = "description";
_declspec(selectany) const char* const DWFXML::kzAttribute_Orientation           = "orientation";
_declspec(selectany) const char* const DWFXML::kzAttribute_LogfontName           = "logfontName";
_declspec(selectany) const char* const DWFXML::kzAttribute_InvertColors          = "invertColors";
_declspec(selectany) const char* const DWFXML::kzAttribute_CreationTime          = "creationTime";
_declspec(selectany) const char* const DWFXML::kzAttribute_CanonicalName         = "canonicalName";
_declspec(selectany) const char* const DWFXML::kzAttribute_CharacterCode         = "characterCode";
_declspec(selectany) const char* const DWFXML::kzAttribute_ParentObjectID        = "parentObjectId";
_declspec(selectany) const char* const DWFXML::kzAttribute_OriginalExtents       = "originalExtents";
_declspec(selectany) const char* const DWFXML::kzAttribute_ModificationTime      = "modificationTime";
_declspec(selectany) const char* const DWFXML::kzAttribute_ScannedResolution     = "scannedResolution";
_declspec(selectany) const char* const DWFXML::kzAttribute_EffectiveResolution   = "effectiveResolution";

_declspec(selectany) const wchar_t* const DWFXML::kzRole_Graphics2d              = L"2d streaming graphics";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_Graphics3d              = L"3d streaming graphics";   
_declspec(selectany) const wchar_t* const DWFXML::kzRole_Descriptor              = L"descriptor";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_Thumbnail               = L"thumbnail";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_Graphics2dOverlay       = L"2d vector overlay";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_Graphics2dMarkup        = L"2d vector markup";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_RmlMarkup               = L"RML markup";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_Preview                 = L"preview";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_OverlayPreview          = L"overlay preview";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_MarkupPreview           = L"markup preview";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_RasterOverlay           = L"raster overlay";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_RasterMarkup            = L"raster markup";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_Font                    = L"font";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_Metadata                = L"metadata";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_PackageSignature        = L"package signature";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_SectionSignature        = L"section signature";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_FilesetSignature        = L"fileset signature";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_ObjectDefinition        = L"object definition";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_MarkupObjectDefinition  = L"markup object definition";
_declspec(selectany) const wchar_t* const DWFXML::kzRole_Texture                 = L"texture";

#endif
#endif

#endif

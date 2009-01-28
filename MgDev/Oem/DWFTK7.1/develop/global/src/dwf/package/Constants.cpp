//
//  Copyright (C) 2003-2009 by Autodesk, Inc.
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


#include "dwf/package/Constants.h"


#if defined(DWFTK_STATIC) || !defined(_DWFCORE_WIN32_SYSTEM)

//DNT_Start
const char* const DWFXML::kzNamespace_DWF                   = "dwf:";
const char* const DWFXML::kzNamespace_EPlot                 = "ePlot:";
const char* const DWFXML::kzNamespace_EModel                = "eModel:";
const char* const DWFXML::kzNamespace_ECommon               = "eCommon:";

const char* const DWFXML::kzElement_TOC                     = "Toc";
const char* const DWFXML::kzElement_Page                    = "Page";
const char* const DWFXML::kzElement_Paper                   = "Paper";
const char* const DWFXML::kzElement_Space                   = "Space";
const char* const DWFXML::kzElement_Units                   = "Units";
const char* const DWFXML::kzElement_Global                  = "Global";
const char* const DWFXML::kzElement_Source                  = "Source";
const char* const DWFXML::kzElement_Object                  = "Object";
const char* const DWFXML::kzElement_Objects                 = "Objects";
const char* const DWFXML::kzElement_Section                 = "Section";
const char* const DWFXML::kzElement_Sections                = "Sections";
const char* const DWFXML::kzElement_Manifest                = "Manifest";
const char* const DWFXML::kzElement_Bookmark                = "Bookmark";
const char* const DWFXML::kzElement_Property                = "Property";
const char* const DWFXML::kzElement_Resource                = "Resource";
const char* const DWFXML::kzElement_Resources               = "Resources";
const char* const DWFXML::kzElement_Instance                = "Instance";
const char* const DWFXML::kzElement_Instances               = "Instances";
const char* const DWFXML::kzElement_Interface               = "Interface";
const char* const DWFXML::kzElement_Interfaces              = "Interfaces";
const char* const DWFXML::kzElement_Properties              = "Properties";
const char* const DWFXML::kzElement_Dependency              = "Dependency";
const char* const DWFXML::kzElement_Dependencies            = "Dependencies";
const char* const DWFXML::kzElement_FontResource            = "FontResource";
const char* const DWFXML::kzElement_ImageResource           = "ImageResource";
const char* const DWFXML::kzElement_GraphicResource         = "GraphicResource";
const char* const DWFXML::kzElement_ObjectDefinition        = "ObjectDefinition";
const char* const DWFXML::kzElement_PageObjectDefinition    = "PageObjectDefinition";
const char* const DWFXML::kzElement_SpaceObjectDefinition   = "SpaceObjectDefinition";
const char* const DWFXML::kzElement_GlobalObjectDefinition  = "GlobalObjectDefinition";

const char* const DWFXML::kzAttribute_ID                    = "id";
const char* const DWFXML::kzAttribute_Name                  = "name";
const char* const DWFXML::kzAttribute_Type                  = "type";
const char* const DWFXML::kzAttribute_HRef                  = "href";
const char* const DWFXML::kzAttribute_Refs                  = "refs";
const char* const DWFXML::kzAttribute_MIME                  = "mime";
const char* const DWFXML::kzAttribute_Role                  = "role";
const char* const DWFXML::kzAttribute_Clip                  = "clip";
const char* const DWFXML::kzAttribute_Show                  = "show";
const char* const DWFXML::kzAttribute_Size                  = "size";
const char* const DWFXML::kzAttribute_Nodes                 = "nodes";
const char* const DWFXML::kzAttribute_Value                 = "value";
const char* const DWFXML::kzAttribute_Units                 = "units";
const char* const DWFXML::kzAttribute_Title                 = "title";
const char* const DWFXML::kzAttribute_Color                 = "color";
const char* const DWFXML::kzAttribute_Width                 = "width";
const char* const DWFXML::kzAttribute_Height                = "height";
const char* const DWFXML::kzAttribute_ZOrder                = "zOrder";
const char* const DWFXML::kzAttribute_Object                = "object";
const char* const DWFXML::kzAttribute_Author                = "author";
const char* const DWFXML::kzAttribute_Extents               = "extents";
const char* const DWFXML::kzAttribute_Version               = "version";
const char* const DWFXML::kzAttribute_Request               = "request";
const char* const DWFXML::kzAttribute_Scanned               = "scanned";
const char* const DWFXML::kzAttribute_Children              = "children";
const char* const DWFXML::kzAttribute_Provider              = "provider";
const char* const DWFXML::kzAttribute_ObjectID              = "objectId";
const char* const DWFXML::kzAttribute_Category              = "category";
const char* const DWFXML::kzAttribute_Transform             = "transform";
const char* const DWFXML::kzAttribute_PlotOrder             = "plotOrder";
const char* const DWFXML::kzAttribute_ColorDepth            = "colorDepth";
const char* const DWFXML::kzAttribute_Privilege             = "privilege";
const char* const DWFXML::kzAttribute_Description           = "description";
const char* const DWFXML::kzAttribute_Orientation           = "orientation";
const char* const DWFXML::kzAttribute_LogfontName           = "logfontName";
const char* const DWFXML::kzAttribute_InvertColors          = "invertColors";
const char* const DWFXML::kzAttribute_CreationTime          = "creationTime";
const char* const DWFXML::kzAttribute_CanonicalName         = "canonicalName";
const char* const DWFXML::kzAttribute_CharacterCode         = "characterCode";
const char* const DWFXML::kzAttribute_ParentObjectID        = "parentObjectId";
const char* const DWFXML::kzAttribute_OriginalExtents       = "originalExtents";
const char* const DWFXML::kzAttribute_ModificationTime      = "modificationTime";
const char* const DWFXML::kzAttribute_ScannedResolution     = "scannedResolution";
const char* const DWFXML::kzAttribute_EffectiveResolution   = "effectiveResolution";

const wchar_t* const DWFXML::kzRole_Graphics2d              = L"2d streaming graphics";
const wchar_t* const DWFXML::kzRole_Graphics3d              = L"3d streaming graphics";   
const wchar_t* const DWFXML::kzRole_Descriptor              = L"descriptor";
const wchar_t* const DWFXML::kzRole_Thumbnail               = L"thumbnail";
const wchar_t* const DWFXML::kzRole_Graphics2dOverlay       = L"2d vector overlay";
const wchar_t* const DWFXML::kzRole_Graphics2dMarkup        = L"2d vector markup";
const wchar_t* const DWFXML::kzRole_RmlMarkup               = L"RML markup";
const wchar_t* const DWFXML::kzRole_Preview                 = L"preview";
const wchar_t* const DWFXML::kzRole_OverlayPreview          = L"overlay preview";
const wchar_t* const DWFXML::kzRole_MarkupPreview           = L"markup preview";
const wchar_t* const DWFXML::kzRole_RasterOverlay           = L"raster overlay";
const wchar_t* const DWFXML::kzRole_RasterMarkup            = L"raster markup";
const wchar_t* const DWFXML::kzRole_Font                    = L"font";
const wchar_t* const DWFXML::kzRole_Metadata                = L"metadata";
const wchar_t* const DWFXML::kzRole_PackageSignature        = L"package signature";
const wchar_t* const DWFXML::kzRole_SectionSignature        = L"section signature";
const wchar_t* const DWFXML::kzRole_FilesetSignature        = L"fileset signature";
const wchar_t* const DWFXML::kzRole_ObjectDefinition        = L"object definition";
const wchar_t* const DWFXML::kzRole_MarkupObjectDefinition  = L"markup object definition";
const wchar_t* const DWFXML::kzRole_Texture                 = L"texture";
//DNT_End

#endif


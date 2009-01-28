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

#include "./EMapConstants.h"
using namespace DWFToolkit::DWFEMapExtension;


const char* const DWFEMAPXML::kzNamespace_EMap                          = "eMap:";
const char* const DWFEMAPXML::kzElement_Map                             = "Map";
const char* const DWFEMAPXML::kzElement_Name                            = "Name";
const char* const DWFEMAPXML::kzElement_Background                      = "Background";
const char* const DWFEMAPXML::kzElement_Color                           = "Color";
const char* const DWFEMAPXML::kzElement_CoordinateSpace                 = "CoordinateSpace";
const char* const DWFEMAPXML::kzElement_Type                            = "Type";
const char* const DWFEMAPXML::kzElement_ScaleFactor                     = "ScaleFactor";
const char* const DWFEMAPXML::kzElement_Bounds                          = "Bounds";
const char* const DWFEMAPXML::kzElement_Units                           = "Units";
const char* const DWFEMAPXML::kzElement_MinX                            = "MinX";
const char* const DWFEMAPXML::kzElement_MaxX                            = "MaxX";
const char* const DWFEMAPXML::kzElement_MinY                            = "MinY";
const char* const DWFEMAPXML::kzElement_MaxY                            = "MaxY";
const char* const DWFEMAPXML::kzElement_InitialView                     = "InitialView";
const char* const DWFEMAPXML::kzElement_InitialBounds                   = "InitialBounds";
const char* const DWFEMAPXML::kzElement_CenterX                         = "CenterX";
const char* const DWFEMAPXML::kzElement_CenterY                         = "CenterY";
const char* const DWFEMAPXML::kzElement_Scale                           = "Scale";
const char* const DWFEMAPXML::kzElement_Label                           = "Label";
const char* const DWFEMAPXML::kzElement_Layer                           = "Layer";
const char* const DWFEMAPXML::kzElement_Layers                          = "Layers";
const char* const DWFEMAPXML::kzElement_LayerGroup                      = "LayerGroup";
const char* const DWFEMAPXML::kzElement_LayerGroups                     = "LayerGroups";
const char* const DWFEMAPXML::kzElement_ScaleRange                      = "ScaleRange";
const char* const DWFEMAPXML::kzElement_ScaleRanges                     = "ScaleRanges";
const char* const DWFEMAPXML::kzElement_UIGraphic                       = "UIGraphic";
const char* const DWFEMAPXML::kzElement_UIGraphics                      = "UIGraphics";
const char* const DWFEMAPXML::kzElement_MapObjectDefinition             = "MapObjectDefinition";

const char* const DWFEMAPXML::kzElement_MapTransaction                  = "MapTransaction";
const char* const DWFEMAPXML::kzElement_Commands                        = "Commands";
const char* const DWFEMAPXML::kzElement_AddLayer                        = "AddLayer";
const char* const DWFEMAPXML::kzElement_RemoveLayer                     = "RemoveLayer";
const char* const DWFEMAPXML::kzElement_UpdateLayer                     = "UpdateLayer";
const char* const DWFEMAPXML::kzElement_AddLayerGroup                   = "AddLayerGroup";
const char* const DWFEMAPXML::kzElement_RemoveLayerGroup                = "RemoveLayerGroup";
const char* const DWFEMAPXML::kzElement_UpdateLayerGroup                = "UpdateLayerGroup";
const char* const DWFEMAPXML::kzElement_UpdateUIGraphic                 = "UpdateUIGraphic";
const char* const DWFEMAPXML::kzElement_MapTransactionObjectDefinition  = "MapTransactionObjectDefinition";

const char* const DWFEMAPXML::kzAttribute_Uri                           = "uri";
const char* const DWFEMAPXML::kzAttribute_Index                         = "index";
const char* const DWFEMAPXML::kzAttribute_Visible                       = "visible";
const char* const DWFEMAPXML::kzAttribute_Editable                      = "editable";
const char* const DWFEMAPXML::kzAttribute_MinScale                      = "minScale";
const char* const DWFEMAPXML::kzAttribute_MaxScale                      = "maxScale";
const char* const DWFEMAPXML::kzAttribute_ObjectID                      = "objectId";
const char* const DWFEMAPXML::kzAttribute_Collapsed                     = "collapsed";
const char* const DWFEMAPXML::kzAttribute_Selectable                    = "selectable";
const char* const DWFEMAPXML::kzAttribute_MapObjectID                   = "mapObjectId";
const char* const DWFEMAPXML::kzAttribute_GroupObjectID                 = "groupObjectId";
const char* const DWFEMAPXML::kzAttribute_ShowInLegend                  = "showInLegend";
const char* const DWFEMAPXML::kzAttribute_InsertAfter                   = "insertAfter";
const char* const DWFEMAPXML::kzAttribute_SequenceID                    = "sequenceId";

const wchar_t* const DWFEMAPXML::kzRole_LegendImage                     = L"legend image";
const wchar_t* const DWFEMAPXML::kzRole_ObservationMesh                 = L"observation mesh";

// End of file

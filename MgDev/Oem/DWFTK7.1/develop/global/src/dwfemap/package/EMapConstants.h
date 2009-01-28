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
#ifndef _DWFEMAPTK_EMAPCONSTANTS_H
#define _DWFEMAPTK_EMAPCONSTANTS_H

#include "../EMapToolkit.h"

namespace DWFToolkit { namespace  DWFEMapExtension
{
    //--------------------------------------------------------------------
    // DESCRIPTION:
    // Class DWFEMAPXML is organizational namespace for all the static
    // strings that represent namespaces, elements, attributes, and
    // roles as defined by the DWF EMap XML Schema.
    //--------------------------------------------------------------------
    class DWFEMAPXML
    {
    public:
        static const char* const kzNamespace_EMap;

        static const char* const kzElement_Map;
        static const char* const kzElement_Name;
        static const char* const kzElement_Background;
        static const char* const kzElement_Color;
        static const char* const kzElement_CoordinateSpace;
        static const char* const kzElement_Type;
        static const char* const kzElement_ScaleFactor;
        static const char* const kzElement_Bounds;
        static const char* const kzElement_Units;
        static const char* const kzElement_MinX;
        static const char* const kzElement_MaxX;
        static const char* const kzElement_MinY;
        static const char* const kzElement_MaxY;
        static const char* const kzElement_InitialView;
        static const char* const kzElement_InitialBounds;
        static const char* const kzElement_CenterX;
        static const char* const kzElement_CenterY;
        static const char* const kzElement_Scale;
        static const char* const kzElement_Label;
        static const char* const kzElement_Layer;
        static const char* const kzElement_Layers;
        static const char* const kzElement_LayerGroup;
        static const char* const kzElement_LayerGroups;
        static const char* const kzElement_ScaleRange;
        static const char* const kzElement_ScaleRanges;
        static const char* const kzElement_UIGraphic;
        static const char* const kzElement_UIGraphics;
        static const char* const kzElement_MapObjectDefinition;

        static const char* const kzElement_MapTransaction;
        static const char* const kzElement_Commands;
        static const char* const kzElement_AddLayer;
        static const char* const kzElement_RemoveLayer;
        static const char* const kzElement_UpdateLayer;
        static const char* const kzElement_AddLayerGroup;
        static const char* const kzElement_RemoveLayerGroup;
        static const char* const kzElement_UpdateLayerGroup;
        static const char* const kzElement_UpdateUIGraphic;
        static const char* const kzElement_MapTransactionObjectDefinition;

        static const char* const kzAttribute_Uri;
        static const char* const kzAttribute_Index;
        static const char* const kzAttribute_Visible;
        static const char* const kzAttribute_Editable;
        static const char* const kzAttribute_MinScale;
        static const char* const kzAttribute_MaxScale;
        static const char* const kzAttribute_ObjectID;
        static const char* const kzAttribute_Collapsed;
        static const char* const kzAttribute_Selectable;
        static const char* const kzAttribute_MapObjectID;
        static const char* const kzAttribute_GroupObjectID;
        static const char* const kzAttribute_ShowInLegend;
        static const char* const kzAttribute_InsertAfter;
        static const char* const kzAttribute_SequenceID;

        static const wchar_t* const kzRole_LegendImage;
        static const wchar_t* const kzRole_ObservationMesh;
    };
}}

#endif // _DWFEMAPTK_EMAPCONSTANTS_H
// End of file.

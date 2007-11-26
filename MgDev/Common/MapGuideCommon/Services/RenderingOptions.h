//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef MG_RENDERING_OPTIONS_H
#define MG_RENDERING_OPTIONS_H

/// \cond INTERNAL

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Options for Rendering Service.
///
class MG_MAPGUIDE_API MgRenderingOptions: public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgRenderingOptions)
PUBLISHED_API:
    enum
    {
        RenderSelection = 1,    // Renders the selected feature(s)
        RenderLayers = 2,       // Renders the features on the map
        KeepSelection = 4       // Renders the selected feature(s) even if they are outside the current scale
    };
    MgRenderingOptions(CREFSTRING format, INT32 behavior, MgColor* selectionColor);
    STRING GetImageFormat();
    INT32 GetBehavior();
    MgColor* GetSelectionColor();
INTERNAL_API:
    virtual void Serialize(MgStream* stream);
    virtual void Deserialize(MgStream* stream);
    MgRenderingOptions();
    virtual ~MgRenderingOptions();
protected:
    INT32 GetClassId();
    virtual void Dispose();
private:
    STRING m_format;
    INT32 m_behavior;
    Ptr<MgColor> m_selectionColor;
CLASS_ID:
    static const INT32 m_cls_id = MapGuide_RenderingService_RenderingOptions;
};
/// \endcond

#endif

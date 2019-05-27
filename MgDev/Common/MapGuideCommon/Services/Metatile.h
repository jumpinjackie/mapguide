//
//  Copyright (C) 2004-2019 by Autodesk, Inc.
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

#ifndef METATILE_H
#define METATILE_H

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// \brief
/// Defines a raw image frame buffer from the result of a meta-tile
/// rendering operation
///
/// \since 3.3
class MG_MAPGUIDE_API MgMetatile : public MgSerializable
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgMetatile)

EXTERNAL_API:

    /// \brief
    /// Gets the meta-tile content. If no meta-tiling is performed, this is the tile image. Otherwise,
    /// it is the raw image frame buffer (for sub-dividing into originally requested tile images)
    MgByteReader* GetImage();

    /// \brief
    /// Gets the originally requested width of this tile
    INT32 GetRequestedWidth();

    /// \brief
    /// Gets the originally requested height of this tile
    INT32 GetRequestedHeight();

    /// \brief
    /// Gets the width of this metatile
    INT32 GetWidth();

    /// \brief
    /// Gets the height of this metatile
    
    INT32 GetHeight();

    /// \brief
    /// Gets the meta-tiling factor.
    INT32 GetMetaTilingFactor();

    /// \brief
    /// Gets the originally requested tile image format
    STRING GetTileImageFormat();

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
    static const INT32 m_cls_id = MapGuide_RenderingService_Metatile;

INTERNAL_API:
    MgMetatile();
    MgMetatile(MgByteReader* image, INT32 width, INT32 height, INT32 requestedWidth, INT32 requestedHeight, CREFSTRING imageFormat, INT32 metaTilingFactor);
    virtual ~MgMetatile();

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
    Ptr<MgByteReader> m_image;
    INT32 m_width;
    INT32 m_height;
    INT32 m_requestedWidth;
    INT32 m_requestedHeight;
    INT32 m_metaTilingFactor;
    STRING m_imageFormat;
};
/// \endcond

#endif
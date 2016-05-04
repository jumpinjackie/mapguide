//
//  Copyright (C) 2010-2011 by Autodesk, Inc.
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

#ifndef TILEWATERMARKPOSITION_H_
#define TILEWATERMARKPOSITION_H_

#include "MdfModel.h"
#include "WatermarkPosition.h"
#include "WatermarkXOffset.h"
#include "WatermarkYOffset.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The TileWatermarkPosition class is one concrete implementation of WatermarkPosition.
    // It will repeat the source both in X and Y dimension.  Tile is the unit of repeat.
    // Within tile it uses X-Y as the position of watermark.
    //------------------------------------------------------------------------
    class MDFMODEL_API TileWatermarkPosition : public WatermarkPosition
    {
    public:
        // Construction, destruction, initialization.
        TileWatermarkPosition();
        virtual ~TileWatermarkPosition();

        // Operations

        // Property: TileWidth
        double GetTileWidth() const;
        void SetTileWidth(const double& dTileWidth);

        // Property: TileHeight
        double GetTileHeight() const;
        void SetTileHeight(const double& dTileHeight);

        // Property: HorizontalPosition
        const WatermarkXOffset* GetHorizontalPosition() const;
        WatermarkXOffset* GetHorizontalPosition();
        void AdoptHorizontalPosition(WatermarkXOffset* pHorizontalPosition);
        WatermarkXOffset* OrphanHorizontalPosition();

        // Property: VerticalPosition
        const WatermarkYOffset* GetVerticalPosition() const;
        WatermarkYOffset* GetVerticalPosition();
        void AdoptVerticalPosition(WatermarkYOffset* pVerticalPosition);
        WatermarkYOffset* OrphanVerticalPosition();

        virtual bool Equals(WatermarkPosition* another);

//#ifdef _WIN32
#if _DEBUG
        // for memory leak testing
        static int DumpMemoryLeaks();
#endif
//#endif

    private:
        // Hidden TileWatermarkPosition copy constructor and assignment operator.
        TileWatermarkPosition(const TileWatermarkPosition&);
        TileWatermarkPosition& operator=(const TileWatermarkPosition&);

        // Data members
        // See corresponding properties for descriptions
        double m_tileWidth;
        double m_tileHeight;
        WatermarkXOffset* m_horizontalPosition;
        WatermarkYOffset* m_verticalPosition;

        static const double doubleTolerance;
    };

END_NAMESPACE_MDFMODEL
#endif // TILEWATERMARKPOSITION_H_

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

#ifndef XYWATERMARKPOSITION_H_
#define XYWATERMARKPOSITION_H_

#include "MdfModel.h"
#include "WatermarkPosition.h"
#include "WatermarkXOffset.h"
#include "WatermarkYOffset.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The XYWatermarkPosition class is one concrete implementation of WatermarkPosition.
    // It uses X-Y as the position of watermark.
    //------------------------------------------------------------------------
    class MDFMODEL_API XYWatermarkPosition : public WatermarkPosition
    {
    public:
        // Construction, destruction, initialization.
        XYWatermarkPosition();
        virtual ~XYWatermarkPosition();

        // Operations

        // Property: XPosition
        const WatermarkXOffset* GetXPosition() const;
        WatermarkXOffset* GetXPosition();
        void AdoptXPosition(WatermarkXOffset* pXPosition);
        WatermarkXOffset* OrphanXPosition();

        // Property: YPosition
        const WatermarkYOffset* GetYPosition() const;
        WatermarkYOffset* GetYPosition();
        void AdoptYPosition(WatermarkYOffset* pYPosition);
        WatermarkYOffset* OrphanYPosition();

        virtual bool Equals(WatermarkPosition* another);

//#ifdef _WIN32
#if _DEBUG
        // for memory leak testing
        static int DumpMemoryLeaks();
#endif
//#endif

    private:
        // Hidden XYWatermarkPosition copy constructor and assignment operator.
        XYWatermarkPosition(const XYWatermarkPosition&);
        XYWatermarkPosition& operator=(const XYWatermarkPosition&);

        // Data members
        // See corresponding properties for descriptions
        WatermarkXOffset* m_XPosition;
        WatermarkYOffset* m_YPosition;
    };

END_NAMESPACE_MDFMODEL
#endif // XYWATERMARKPOSITION_H_

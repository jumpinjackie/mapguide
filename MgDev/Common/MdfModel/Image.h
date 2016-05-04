//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#ifndef IMAGE_H_
#define IMAGE_H_

#include "MdfModel.h"
#include "GraphicElement.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION: Building block of the graphics of a SymbolDefinition.
    //-------------------------------------------------------------------------
    class MDFMODEL_API Image : public GraphicElement
    {
    public:
        // Construction, destruction, initialization
        Image();
        virtual ~Image();

        const MdfString& GetContent() const;
        void SetContent(const MdfString& content);

        const MdfString& GetResourceId() const;
        void SetResourceId(const MdfString& resourceId);

        const MdfString& GetLibraryItemName() const;
        void SetLibraryItemName(const MdfString& libraryItemName);

        const MdfString& GetSizeX() const;
        void SetSizeX(const MdfString& sizeX);

        const MdfString& GetSizeY() const;
        void SetSizeY(const MdfString& sizeY);

        const MdfString& GetSizeScalable() const;
        void SetSizeScalable(const MdfString& sizeScalable);

        const MdfString& GetAngle() const;
        void SetAngle(const MdfString& angle);

        const MdfString& GetPositionX() const;
        void SetPositionX(const MdfString& positionX);

        const MdfString& GetPositionY() const;
        void SetPositionY(const MdfString& positionY);

        virtual void AcceptVisitor(IGraphicElementVisitor& igeVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        Image(const Image&);
        Image& operator=(const Image&);

        // Data members
        MdfString m_sContent;
        MdfString m_sResourceId;
        MdfString m_sLibraryItemName;
        MdfString m_sSizeX;
        MdfString m_sSizeY;
        MdfString m_sSizeScalable;
        MdfString m_sAngle;
        MdfString m_sPositionX;
        MdfString m_sPositionY;
    };

END_NAMESPACE_MDFMODEL
#endif // IMAGE_H_

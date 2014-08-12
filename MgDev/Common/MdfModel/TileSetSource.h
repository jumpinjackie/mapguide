//
//  Copyright (C) 2004-2014 by Autodesk, Inc.
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
#ifndef TILESETSOURCE_H_
#define TILESETSOURCE_H_

#include "MdfModel.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    class MDFMODEL_API TileSetSource : public MdfRootObject
    {
    public:
        TileSetSource();
        virtual ~TileSetSource();

        // Property : ResourceId
        const MdfString& GetResourceId() const;
        void SetResourceId(const MdfString&  strResourceId);

    private:
        // Hidden copy constructor and assignment operator.
        TileSetSource(const TileSetSource&);
        TileSetSource& operator=(const TileSetSource&);

        MdfString m_resourceId;
    };

END_NAMESPACE_MDFMODEL

#endif //TILESETSOURCE_H_
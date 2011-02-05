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

#ifndef SE_SYMBOLMANAGER_H_
#define SE_SYMBOLMANAGER_H_

#include "StylizationAPI.h"
#include "RendererStyles.h"

namespace MdfModel
{
    class SymbolDefinition;
}

using namespace MdfModel;


struct ImageData
{
    ImageData() :
        size(0),
        data(NULL),
        format(RS_ImageFormat_Unknown),
        width(-1),
        height(-1)
    {
    }

    int size;
    unsigned char* data;
    RS_ImageFormat format;
    int width;
    int height;
};


class SE_SymbolManager
{
public:
    virtual SymbolDefinition* GetSymbolDefinition(const wchar_t* resourceId) = 0;
    virtual bool GetImageData(const wchar_t* resourceId, const wchar_t* resourceName, ImageData& imageData) = 0;
    virtual STYLIZATION_API bool GetImageData(const wchar_t* base64Data, const int size, ImageData& imageData);
};

#endif

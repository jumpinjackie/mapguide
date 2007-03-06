//
//  Copyright (C) 2007 by Autodesk, Inc.
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

#ifndef SE_SYMBOLMANAGER_H
#define SE_SYMBOLMANAGER_H

namespace MdfModel
{
    class SymbolDefinition;
};

class SE_SymbolManager
{
public:
    virtual MdfModel::SymbolDefinition* GetSymbolDefinition(const wchar_t* resource) = 0;
    virtual unsigned char* GetImageData(const wchar_t* resource, int& length) = 0;
};

#endif

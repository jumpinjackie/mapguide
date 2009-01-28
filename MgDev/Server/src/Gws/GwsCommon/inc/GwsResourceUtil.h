//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef GWSRESOURCEMODULE_H
#define GWSRESOURCEMODULE_H

class CGwsResourceModule
{
public:
    CGwsResourceModule(const wchar_t* fileName);
    virtual ~CGwsResourceModule();

    GWS_COMMON_API
    static bool GetString(unsigned int id, std::wstring &str);
    GWS_COMMON_API
    static void GwsStatusMessage (EGwsStatus fes, std::wstring &str);
    GWS_COMMON_API
    static void FeatureStatusMessage (EGwsFeatureStatus fstat, std::wstring & str);

private:
#ifdef _WIN32
    static HMODULE shMod;
#endif

};

#endif // GWSRESOURCEMODULE_H

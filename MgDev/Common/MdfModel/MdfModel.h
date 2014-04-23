//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#pragma once 

#ifndef MDFMODEL_H_
#define MDFMODEL_H_

//-------------------------------------------------------------------------
// DESCRIPTION
// The following ifdef block is the standard way of creating macros which
// make exporting from a DLL simpler. All files within this DLL are
// compiled with the MDFMODEL_EXPORTS symbol defined on the command line.
// This symbol should not be defined on any project that uses this DLL.
// This way any other project whose source files include this file see
// MDFMODEL_API functions as being imported from a DLL, whereas this DLL
// sees symbols defined with this macro as being exported.
//-------------------------------------------------------------------------
#ifdef _WIN32
#   ifdef MDFMODEL_EXPORTS
#       define MDFMODEL_API __declspec(dllexport)
#       define EXPIMP_TEMPLATE
#   else
#       define MDFMODEL_API __declspec(dllimport)
#       define EXPIMP_TEMPLATE extern
#   endif
#else
#   define MDFMODEL_API
#   define EXPIMP_TEMPLATE
#endif

// exporting of template classes for collections
// suppresses MS specific (non-ANSI) code warning
#pragma warning(disable: 4231)

// ignore warnings about using deprecated methods
#pragma warning(disable: 4996)

// ignore warning about needing to have dll-interface to be used by clients export
#pragma warning(disable: 4251)

#define MDFMODEL_NAMESPACE          MdfModel
#define BEGIN_NAMESPACE_MDFMODEL    namespace MdfModel {
#define END_NAMESPACE_MDFMODEL      }

#include <string>

BEGIN_NAMESPACE_MDFMODEL

    // the MdfString is a std::wstring.
    // It might be swapped out  at a later date for a const wchar_t* .
    typedef std::wstring MdfString; // wide string for unicode support
    
END_NAMESPACE_MDFMODEL

using namespace MDFMODEL_NAMESPACE;

#endif // MDFMODEL_H_

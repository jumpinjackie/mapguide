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

#ifndef MGSETLOCALE_H_
#define MGSETLOCALE_H_

/// \cond INTERNAL
// This class should be used instead of ::setlocale(int category, const char * locale)
// in order to temporarily set the locale for an operation.  The reason being:
// this class will automatically reset the locale back to what it was before
// this object was created whether an exception occurs in the block of code where
// it is active or not.
//
// Usage:
//    {
//        MgSetLocale mgSetLocale(LC_NUMERIC, "");  // create a stack instance
//        ...code that works in the new locale...
//    }
//    <-- at the point where execution resumes after the block, the old locale
//        is restored whether an exception had occurred in the above code or not.
class MgSetLocale {

public:

    MgSetLocale(int category, const char * locale);
    ~MgSetLocale();

protected:
    std::string m_strOldLocale;
    std::string m_strNewLocale;
    int m_category;
};
/// \endcond

#endif

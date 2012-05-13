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

#ifndef MG_UNIQUE_FUNCTION_H
#define MG_UNIQUE_FUNCTION_H

#include <vector>
#include <map>
using namespace std;

//=============================================================================
//  This matrix template is designed to be used with built-in arithmetic types.
//  It is of fixed size; it is constructed with one memory allocation.
template <typename T>
class MgUniqueFunction
{
    public:
        MgUniqueFunction() {}
        ~MgUniqueFunction() {}

        static void Execute(const std::vector<T>& in, std::vector<T>& out)
        {
            int cnt = (int)in.size();
            if (cnt <= 0) { return; } // Nothing to do, we just send back Property Definition to clients from reader

            std::map<T, char> mMap;
            for (int i=0; i<cnt; i++) mMap[in[i]] = '7'; //7 is just arbitrary placeholder

            out.reserve(mMap.size());
            typename std::map<T, char>::iterator iter2;
            for (iter2 = mMap.begin(); iter2 != mMap.end(); iter2++)
                out.push_back(iter2->first);
        }
};

#endif  // DM_MATRIX_H

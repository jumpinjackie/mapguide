//
//  Copyright (C) 2011 by Autodesk, Inc.
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

#ifndef PROFILERESULT_H_
#define PROFILERESULT_H_

#include "MdfModel.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The ProfileResult class is a root document node of the MdfModel. Through
    // its interface, one can get access to all the data in the ProfileResult.
    //------------------------------------------------------------------------
    class MDFMODEL_API ProfileResult : public MdfRootObject
    {
    public:

        // Defines the profile result type
        enum ProfileResultType
        {
            ProfileRenderMap,
            ProfileRenderDynamicOverlay
        };

        // Construction, destruction, initialization.
        ProfileResult();
        virtual ~ProfileResult();

        // Attribute : ProfileResultType
        ProfileResultType GetProfileResultType() const;
        void SetProfileResultType(ProfileResultType type);

//#ifdef _WIN32
#if _DEBUG
        // for memory leak testing
        static int DumpMemoryLeaks();
#endif
//#endif

    private:
        // Hidden ProfileResult copy constructor and assignment operator.
        ProfileResult(const ProfileResult&);
        ProfileResult& operator=(const ProfileResult&);

        // profile result type attribute
        ProfileResultType m_eProfileResultType;
    };

END_NAMESPACE_MDFMODEL
#endif // PROFILERESULT_H_

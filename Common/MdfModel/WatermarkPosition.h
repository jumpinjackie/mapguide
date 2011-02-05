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

#ifndef WATERMARKPOSITION_H_
#define WATERMARKPOSITION_H_

#include "MdfModel.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The WatermarkPosition class is the abstract class of position.
    // It doesn't contain any field inside itself.
    //------------------------------------------------------------------------
    class MDFMODEL_API WatermarkPosition : public MdfRootObject
    {
    public:
        // Destruction
        virtual ~WatermarkPosition();

        virtual bool Equals(WatermarkPosition* another) = 0;

    protected:
        // Construction, initialization
        // Default constructor is protected to make this class abstract.
        WatermarkPosition();
    };

END_NAMESPACE_MDFMODEL
#endif // WATERMARKPOSITION_H_

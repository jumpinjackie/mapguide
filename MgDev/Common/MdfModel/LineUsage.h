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

#ifndef LINEUSAGE_H_
#define LINEUSAGE_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "Path.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    //-------------------------------------------------------------------------
    class MDFMODEL_API LineUsage : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization
        LineUsage();
        virtual ~LineUsage();

        const MdfString& GetAngleControl() const;
        void SetAngleControl(const MdfString& angleControl);

        const MdfString& GetUnitsControl() const;
        void SetUnitsControl(const MdfString& unitsControl);

        const MdfString& GetVertexControl() const;
        void SetVertexControl(const MdfString& vertexControl);

        const MdfString& GetAngle() const;
        void SetAngle(const MdfString& angle);

        const MdfString& GetStartOffset() const;
        void SetStartOffset(const MdfString& startOffset);

        const MdfString& GetEndOffset() const;
        void SetEndOffset(const MdfString& endOffset);

        const MdfString& GetRepeat() const;
        void SetRepeat(const MdfString& repeat);

        const MdfString& GetVertexAngleLimit() const;
        void SetVertexAngleLimit(const MdfString& vertexAngleLimit);

        const MdfString& GetVertexJoin() const;
        void SetVertexJoin(const MdfString& vertexJoin);

        Path* GetDefaultPath();
        void AdoptDefaultPath(Path* defaultPath);
        Path* OrphanDefaultPath();

    private:
        // Hidden copy constructor and assignment operator.
        LineUsage(const LineUsage&);
        LineUsage& operator=(const LineUsage&);

        // Data members
        MdfString m_sAngleControl;
        MdfString m_sUnitsControl;
        MdfString m_sVertexControl;
        MdfString m_sAngle;
        MdfString m_sStartOffset;
        MdfString m_sEndOffset;
        MdfString m_sRepeat;
        MdfString m_sVertexAngleLimit;
        MdfString m_sVertexJoin;
        Path* m_pDefaultPath;
    };

END_NAMESPACE_MDFMODEL
#endif // LINEUSAGE_H_

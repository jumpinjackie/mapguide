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
#include "Usage.h"
#include "Path.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    //-------------------------------------------------------------------------
    class MDFMODEL_API LineUsage : public Usage
    {
    public:
        enum UnitsControl
        {
            Absolute,
            Parametric
        };

        enum VertexControl
        {
            NoOverlap,
            OverlapDirect,
            OverlapNoWrap,
            OverlapWrap
        };

        // Construction, destruction, initialization
        LineUsage();
        virtual ~LineUsage();

        AngleControl GetAngleControl() const;
        void SetAngleControl(AngleControl angleControl);

        UnitsControl GetUnitsControl() const;
        void SetUnitsControl(UnitsControl unitsControl);

        VertexControl GetVertexControl() const;
        void SetVertexControl(VertexControl vertexControl);

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

        Path* GetDefaultPath();
        void AdoptDefaultPath(Path* defaultPath);
        Path* OrphanDefaultPath();

        virtual void AcceptVisitor(IUsageVisitor& iuVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        LineUsage(const LineUsage&);
        LineUsage& operator=(const LineUsage&);

        // Data members
        AngleControl m_eAngleControl;
        UnitsControl m_eUnitsControl;
        VertexControl m_eVertexControl;

        MdfString m_sAngle;
        MdfString m_sStartOffset;
        MdfString m_sEndOffset;
        MdfString m_sRepeat;
        MdfString m_sVertexAngleLimit;

        Path* m_pDefaultPath;
    };

END_NAMESPACE_MDFMODEL
#endif // LINEUSAGE_H_

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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GwsQueryEngineImp.h"


/////////////////////////////////////////////////////////////////////
//
// class CGwsRightNestedLoopJoinQueryResults
//
/////////////////////////////////////////////////////////////////////

CGwsRightNestedLoopJoinQueryResults::CGwsRightNestedLoopJoinQueryResults ()
{
    m_bClosed = true;
}


CGwsRightNestedLoopJoinQueryResults::~CGwsRightNestedLoopJoinQueryResults () throw()
{
}


EGwsStatus CGwsRightNestedLoopJoinQueryResults::InitializeReader (
    IGWSQuery                  * query,
    CGwsPreparedQuery          * prepquery,
    FdoStringCollection        * joincols,
    bool                        bScrollable
)
{
    m_bClosed = true;
    return CGwsRightJoinQueryResults::InitializeReader (query, prepquery, joincols, bScrollable);
}


bool CGwsRightNestedLoopJoinQueryResults::ReadNext()
{
    return CGwsRightJoinQueryResults::ReadNext ();
}


void CGwsRightNestedLoopJoinQueryResults::Close ()
{
    if (! m_bClosed)
        CGwsRightJoinQueryResults::Close ();
    m_bClosed = true;
}


EGwsStatus CGwsRightNestedLoopJoinQueryResults::SetRelatedValues (
    const GWSFeatureId & vals
)
{
    EGwsStatus stat = eGwsOk;

    try {
        Close ();

        FdoPtr<FdoFilter> pFilter;

        for (int idx = 0; idx < m_joincols->GetCount (); idx ++) {
            FdoString          * propname = m_joincols->GetString (idx);
            FdoPtr<FdoDataValue> val = vals.GetItem (idx);

            FdoPtr<FdoIdentifier> pFeatureNameProperty = FdoIdentifier::Create(propname);

            // Get the data type of the primary property
            FdoDataType dtPrimary = val->GetDataType();

            // Get the data type of the secondary property
            FdoDataType dtSecondary = dtPrimary;
            if (m_prepquery)
            {
                FdoPtr<CGwsQueryResultDescriptors> extFeatDsc;
                m_prepquery->DescribeResults((IGWSExtendedFeatureDescription**)&extFeatDsc);
                CGwsPropertyDesc propDesc = extFeatDsc->GetPropertyDescriptor(propname);
                dtSecondary = propDesc.m_dataprop;
            }

            // Determine which comparison operator to use
            FdoComparisonOperations comparisonOp = FdoComparisonOperations_EqualTo;
            switch (dtPrimary)
            {
                case FdoDataType_Byte:
                case FdoDataType_Int16:
                case FdoDataType_Int32:
                case FdoDataType_Int64:
                case FdoDataType_Decimal:
                case FdoDataType_Single:
                case FdoDataType_Double:
                    switch (dtSecondary)
                    {
                        case FdoDataType_Byte:
                        case FdoDataType_Int16:
                        case FdoDataType_Int32:
                        case FdoDataType_Int64:
                        case FdoDataType_Decimal:
                        case FdoDataType_Single:
                        case FdoDataType_Double:
                            comparisonOp = FdoComparisonOperations_EqualTo;
                            break;

                        case FdoDataType_String:
                            comparisonOp = FdoComparisonOperations_Like;
                            break;
                    }
                    break;

                case FdoDataType_String:
                    switch (dtSecondary)
                    {
                        case FdoDataType_String:
                            comparisonOp = FdoComparisonOperations_EqualTo;
                            break;

                        case FdoDataType_Byte:
                        case FdoDataType_Int16:
                        case FdoDataType_Int32:
                        case FdoDataType_Int64:
                        case FdoDataType_Decimal:
                        case FdoDataType_Single:
                        case FdoDataType_Double:
                            // if the string represents a number, or if it does not contain wildcard characters, use like, else do nothing
                            FdoStringP strPrimVal = val->ToString();
                            FdoStringP percent = "%";         // NOXLATE
                            FdoStringP underscore = "_";      // NOXLATE
                            FdoStringP bracket = "[";         // NOXLATE
                            FdoStringP bracketCaret = "[^";   // NOXLATE

                            // Remove the single quote from the string
                            FdoStringP quote = "'";           // NOXLATE
                            FdoStringP emptyString = "";      // NOXLATE
                            strPrimVal = strPrimVal.Replace(quote, emptyString);

                            if (strPrimVal.IsNumber())
                            {
                                FdoPtr<FdoDoubleValue> dval = FdoDoubleValue::Create();
                                dval->SetDouble(strPrimVal.ToDouble());
                                val = dval;
                                comparisonOp = FdoComparisonOperations_EqualTo;
                            }
                            else if (!strPrimVal.Contains(percent)
                                     && !strPrimVal.Contains(underscore)
                                     && !strPrimVal.Contains(bracket)
                                     && !strPrimVal.Contains(bracketCaret))
                            {
                                comparisonOp = FdoComparisonOperations_Like;
                            }
                    }
                    break;
            }

            if (idx == 0) {
                // Create the comparison condidtion
                pFilter = FdoComparisonCondition::Create(pFeatureNameProperty,
                                                             comparisonOp,
                                                             val);
            } else {
                FdoPtr<FdoFilter> pRhsFilter = FdoComparisonCondition::Create(pFeatureNameProperty,
                                                                              comparisonOp,
                                                                              val);
                FdoPtr<FdoFilter> pCombinedFilter = FdoFilter::Combine(pFilter,
                                                                       FdoBinaryLogicalOperations_And,
                                                                       pRhsFilter);
                pFilter = pCombinedFilter;
            }
        }

        m_prepquery->SetFilter (pFilter);
        IGWSFeatureIterator * fiter = NULL;
        stat = m_prepquery->Execute (& fiter);
        if (IGWSException::IsError (stat)) {
            PushStatus (stat);
            return stat;
        }
        m_reader = dynamic_cast<FdoIFeatureReader*> (fiter);
        m_bClosed = false;

        stat = CGwsRightJoinQueryResults::SetRelatedValues (vals);

    } catch (FdoException *e) {
        PushFdoException (eGwsFailed, e);
        e->Release();
        stat = eGwsFailed;
    }
    return stat;
}

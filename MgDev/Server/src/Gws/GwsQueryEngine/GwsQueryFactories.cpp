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
IGWSFeatureQueryDefinition * IGWSFeatureQueryDefinition::Create (
    FdoIdentifierCollection *    sellist,
    const GWSQualifiedName & classname,
    FdoFilter             *  filter
)
{
    IGWSFeatureQueryDefinition * pQdef;
    pQdef = new GWSFeatureQueryDefinition (sellist, classname, filter);
    pQdef->AddRef ();

    return pQdef;
}


IGWSLeftJoinQueryDefinition * IGWSLeftJoinQueryDefinition::Create (
    const FdoString     * joinName,
    const FdoString     * joinDelimiter,
    bool                  forceOneToOne,
    IGWSQueryDefinition *    left_qdef,
    IGWSQueryDefinition *    right_qdef,
    FdoStringCollection    * left_attrs,
    FdoStringCollection    * right_attrs
)
{
    IGWSLeftJoinQueryDefinition * pQdef;
    pQdef = new GWSLeftJoinQueryDefinition (joinName,
                                            joinDelimiter,
                                            forceOneToOne,
                                            left_qdef,
                                            right_qdef,
                                            left_attrs,
                                            right_attrs);
    pQdef->AddRef ();
    return pQdef;
}


IGWSEqualJoinQueryDefinition * IGWSEqualJoinQueryDefinition::Create (
    const FdoString     * joinName,
    const FdoString     * joinDelimiter,
    bool                  forceOneToOne,
    IGWSQueryDefinition *    left_qdef,
    IGWSQueryDefinition *    right_qdef,
    FdoStringCollection    * left_attrs,
    FdoStringCollection    * right_attrs
)
{
    IGWSEqualJoinQueryDefinition * pQdef;
    pQdef = new GWSEqualJoinQueryDefinition (joinName,
                                             joinDelimiter,
                                             forceOneToOne,
                                             left_qdef,
                                             right_qdef,
                                             left_attrs,
                                             right_attrs);
    pQdef->AddRef ();
    return pQdef;
}


IGWSQuery * IGWSQuery::Create (
    IGWSConnectionPool  * pool,
    IGWSQueryDefinition * qdef,
    IGWSObject          * owner
)
{
    CGwsFeatureSourceQuery * query  =
         (CGwsFeatureSourceQuery *)
            CGwsFeatureSourceQuery::CreateInstance<CGwsFeatureSourceQuery>(owner);

    query->Initialize (pool, qdef);
    return query;
}

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

#ifndef MGOGCFILTERUTIL_H
#define MGOGCFILTERUTIL_H

#include "ServerFeatureDllExport.h"

//fwd declare
namespace xercesc_2_7
{
    class DOMElement;
}
using namespace xercesc_2_7;

class MgCoordinateSystemTransform;


class MG_SERVER_FEATURE_API MgOgcFilterUtil
{
public:

    STRING Ogc2FdoFilter(CREFSTRING ogcFilter, MgCoordinateSystemTransform* xform, CREFSTRING geomProp, MgPropertyDefinitionCollection* propertyDefs);

private:

    int xmlcmp(const wchar_t* s1, const wchar_t* s2);

    STRING process_element(DOMElement* root);
    STRING process_inner_element(DOMElement* root);
    void process_two_elements(DOMElement* root, REFSTRING left, REFSTRING right);
    STRING process_binary_logical(DOMElement* root);
    STRING process_unary_logical(DOMElement* root);
    STRING process_comparison(DOMElement* root);
    STRING process_like(DOMElement* root);
    STRING process_null(DOMElement* root);
    STRING process_between(DOMElement* root);
    STRING process_identifier(DOMElement* root);
    STRING process_literal(DOMElement* root);
    STRING process_binary_arithmetic(DOMElement* root);

    STRING process_bbox(DOMElement* root);
    STRING process_binary_spatial(DOMElement* root);

    STRING process_geometry(DOMElement* root);
    STRING process_linestring(DOMElement* root);
    STRING process_linearring(DOMElement* root);
    STRING process_polygon(DOMElement* root);
    STRING process_coordinates(DOMElement* root);
    STRING process_poslist(DOMElement* root);
    STRING process_pos(DOMElement* root);
    STRING process_box(DOMElement* root);
    STRING process_envelope(DOMElement* root);

    STRING extract_content(DOMElement* root);

    void xform_box(double& minx, double& miny, double& maxx, double& maxy);
    STRING xform_coords(CREFSTRING coords);

private:

    //state variables
    Ptr<MgCoordinateSystemTransform> m_xform;
    Ptr<MgPropertyDefinitionCollection> m_propertyDefs;
    STRING m_geomProp;
    STRING m_propName;

};

#endif

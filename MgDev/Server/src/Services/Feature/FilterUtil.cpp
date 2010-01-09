//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#include "MapGuideCommon.h"
#include "FilterUtil.h"
#include "System/XmlDefs.h"
#include "System/XmlUtil.h"
#include "CoordinateSystem/CoordinateSystemTransform.h"

//compare strings that represent XML element names
//ignoring case and any schema name prefix
int MgOgcFilterUtil::xmlcmp(const wchar_t* s1, const wchar_t* s2)
{
    const wchar_t* id1 = wcsstr(s1, L":");
    if (id1)
        id1++;
    else
        id1 = s1;

    const wchar_t* id2 = wcsstr(s2, L":");
    if (id2)
        id2++;
    else
        id2 = s2;

    return _wcsicmp(id1, id2);
}

STRING MgOgcFilterUtil::Ogc2FdoFilter(CREFSTRING ogcFilter, MgCoordinateSystemTransform* xform, CREFSTRING geomProp,
                                      MgPropertyDefinitionCollection* propertyDefs)
{
    STRING fdoFilter = L"";

    //we will use the transform to transform geometry
    //from map coordinate system to layer/FDO data source coordinate system
    m_xform = SAFE_ADDREF(xform);
    m_propertyDefs = SAFE_ADDREF(propertyDefs);
    m_geomProp = geomProp;
    m_propName = L"";

    //convert to utf-8
    string utffilter = MgUtil::WideCharToMultiByte(ogcFilter);

    //parse into DOM
    MgXmlUtil xmlUtil;
    xmlUtil.ParseString(utffilter.c_str());
    DOMElement* root = xmlUtil.GetRootNode();

    //verify it's a filter
    STRING rootName = X2W(root->getTagName());
    assert(xmlcmp(rootName.c_str(), L"Filter") == 0);

    //get top node of the filter
    DOMNode* child = root->getFirstChild();

    while (0 != child)
    {
        if(child->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            fdoFilter = process_element((DOMElement*)child);
            break;
        }

        child = child->getNextSibling();
    }

    return fdoFilter;
}


STRING MgOgcFilterUtil::process_element(DOMElement* root)
{
    STRING name = X2W(root->getTagName());
    const wchar_t* wnm = name.c_str();

    //binary logical
    if (xmlcmp(wnm, L"AND") == 0 ||
        xmlcmp(wnm, L"OR") == 0)
    {
        return process_binary_logical(root);
    }
    //unary logical
    else if (xmlcmp(wnm, L"NOT") == 0)
    {
        return process_unary_logical(root);
    }
    //binary comparison condition
    else if (xmlcmp(wnm, L"PropertyIsEqualTo") == 0 ||
             xmlcmp(wnm, L"PropertyIsNotEqualTo") == 0 ||
             xmlcmp(wnm, L"PropertyIsLessThan") == 0 ||
             xmlcmp(wnm, L"PropertyIsGreaterThan") == 0 ||
             xmlcmp(wnm, L"PropertyIsLessThanOrEqualTo") == 0 ||
             xmlcmp(wnm, L"PropertyIsGreaterThanOrEqualTo") == 0)
    {
        return process_comparison(root);
    }
    else if (xmlcmp(wnm, L"PropertyIsLike") == 0)
    {
        return process_like(root);
    }
    else if (xmlcmp(wnm, L"PropertyIsNull") == 0)
    {
        return process_null(root);
    }
    else if (xmlcmp(wnm, L"PropertyIsBetween") == 0)
    {
        return process_between(root);
    }
    //spatial operations
    else if (xmlcmp(wnm, L"BBOX") == 0)
    {
        return process_bbox(root);
    }
    else if (xmlcmp(wnm, L"Distance") == 0 ||
             xmlcmp(wnm, L"Beyond") == 0)
    {
        //TODO:
    }
    else if (xmlcmp(wnm, L"Intersects") == 0 ||
             xmlcmp(wnm, L"Within") == 0 ||
             xmlcmp(wnm, L"Contains") == 0 ||
             xmlcmp(wnm, L"Crosses") == 0 ||
             xmlcmp(wnm, L"Overlaps") == 0 ||
             xmlcmp(wnm, L"Touches") == 0 ||
             xmlcmp(wnm, L"Disjoint") == 0)

    {
        return process_binary_spatial(root);
    }
    //simple arithmetic
    else if (xmlcmp(wnm, L"Add") == 0 ||
             xmlcmp(wnm, L"Sub") == 0 ||
             xmlcmp(wnm, L"Mul") == 0 ||
             xmlcmp(wnm, L"Div") == 0)
    {
        return process_binary_arithmetic(root);
    }
    //identifiers and constants
    else if (xmlcmp(wnm, L"PropertyName") == 0)
    {
        m_propName = process_identifier(root);
        return m_propName;
    }
    else if (xmlcmp(wnm, L"Literal") == 0)
    {
        return process_literal(root);
    }
    else if (xmlcmp(wnm, L"Box") == 0)
    {
        return process_box(root);
    }
    else if (xmlcmp(wnm, L"Envelope") == 0)
    {
        return process_envelope(root);
    }
    //gml geometry -- TODO
    else if (xmlcmp(wnm, L"LineString") == 0 ||
             xmlcmp(wnm, L"Polygon") == 0 ||
             xmlcmp(wnm, L"Point") == 0 ||
             xmlcmp(wnm, L"LinearRing") == 0)
    {
        return process_geometry(root);
    }
    else if (xmlcmp(wnm, L"coordinates") == 0)
    {
        return process_coordinates(root);
    }
    else if (xmlcmp(wnm, L"posList") == 0)
    {
        return process_poslist(root);
    }
    else if (xmlcmp(wnm, L"pos") == 0)
    {
        process_pos(root);
    }

    return L""; //error or unsupported
}

STRING MgOgcFilterUtil::process_inner_element(DOMElement* root)
{
    //processes the first element that's nested inside the given element
    //it's a way of skipping an element that's only informative rather
    //than containing actual data we need to think about

    DOMNode* ichild = root->getFirstChild();
    while(0 != ichild)
    {
        if(ichild->getNodeType() == DOMNode::ELEMENT_NODE)
            return process_element((DOMElement*)ichild);

        ichild = ichild->getNextSibling();
    }

    return L"";
}

void MgOgcFilterUtil::process_two_elements(DOMElement* root, REFSTRING left, REFSTRING right)
{
    //evaluates the first two elements of a tag
    //useful when processing binary operators

    int count = 0;

    DOMNode* child = root->getFirstChild();

    while(0 != child)
    {
        if(child->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            switch (count)
            {
                case 0: left = process_element((DOMElement*)child); break;
                case 1: right = process_element((DOMElement*)child); break;
                case 2: return;
            }

            count++;
        }

        child = child->getNextSibling();
    }
}

//returns the contents of the given node as string
//e.g. for <g>hello</g> returns hello
STRING MgOgcFilterUtil::extract_content(DOMElement* root)
{
    DOMNode* child = root->getFirstChild();
    while(0 != child)
    {
        if(child->getNodeType() == DOMNode::TEXT_NODE)
            return X2W(child->getNodeValue());

        child = child->getNextSibling();
    }

    return L"";
}


STRING MgOgcFilterUtil::process_binary_logical(DOMElement* root)
{
    STRING name = X2W(root->getTagName());
    const wchar_t* wnm = name.c_str();
    STRING op = L" OR ";

    //determine operation type
    if (xmlcmp(wnm, L"And") == 0)
        op = L" AND ";
    else if (xmlcmp(wnm, L"Or") == 0)
        op = L" OR ";
    else
        assert(false); //should not get here!

    bool first = true;
    STRING ret = L"(";

    //Binary logical ops aren't really binary in OGC.
    //They can have any number of operands... so we loop
    DOMNode* ichild = root->getFirstChild();
    while(0 != ichild)
    {
        if(ichild->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            if (!first)
                ret += op;
            else
                first = false;

            ret += process_element((DOMElement*)ichild);
        }

        ichild = ichild->getNextSibling();
    }

    ret += L")";

    return ret;
}

STRING MgOgcFilterUtil::process_unary_logical(DOMElement* root)
{
    return L"NOT " + process_inner_element(root);
}

STRING MgOgcFilterUtil::process_comparison(DOMElement* root)
{
    STRING name = X2W(root->getTagName());
    const wchar_t* wnm = name.c_str();
    STRING op = L" = ";

    //determine operation type
    if (xmlcmp(wnm, L"PropertyIsEqualTo") == 0)
        op = L" = ";
    else if (xmlcmp(wnm, L"PropertyIsNotEqualTo") == 0)
        op = L" != ";
    else if (xmlcmp(wnm, L"PropertyIsLessThan") == 0)
        op = L" < ";
    else if (xmlcmp(wnm, L"PropertyIsGreaterThan") == 0)
        op = L" > ";
    else if (xmlcmp(wnm, L"PropertyIsLessThanOrEqualTo") == 0)
        op = L" <= ";
    else if (xmlcmp(wnm, L"PropertyIsGreaterThanOrEqualTo") == 0)
        op = L" >= ";
    else
        assert(false); //should not get here!

    STRING left;
    STRING right;

    process_two_elements(root, left, right);

    return L"(" + left + op + right + L")";
}

STRING MgOgcFilterUtil::process_binary_arithmetic(DOMElement* root)
{
    STRING name = X2W(root->getTagName());
    const wchar_t* wnm = name.c_str();
    STRING op = L" = ";

    //determine operation type
    if (xmlcmp(wnm, L"Add") == 0)
        op = L" + ";
    else if (xmlcmp(wnm, L"Sub") == 0)
        op = L" - ";
    else if (xmlcmp(wnm, L"Mul") == 0)
        op = L" * ";
    else if (xmlcmp(wnm, L"Div") == 0)
        op = L" / ";
    else
        assert(false); //should not get here!

    STRING left;
    STRING right;

    process_two_elements(root, left, right);

    return L"(" + left + op + right + L")";
}

STRING MgOgcFilterUtil::process_like(DOMElement* root)
{
    STRING left;
    STRING right;

    process_two_elements(root, left, right);

    return L"(" + left + L" LIKE " + right + L")";
}

STRING MgOgcFilterUtil::process_null(DOMElement* root)
{
    return L"(" + process_inner_element(root) + L" NULL)";
}


STRING MgOgcFilterUtil::process_between(DOMElement* root)
{
    STRING name;
    STRING min;
    STRING max;
    int count = 0;

    DOMNode* child = root->getFirstChild();

    while(0 != child)
    {
        if(child->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            DOMElement* elt = (DOMElement*)child;

            STRING cname = X2W(elt->getTagName());

            if (xmlcmp(cname.c_str(), L"LowerBoundary") == 0)
            {
                //internally the LowerBoundary element (version 1.1)
                //will contain a <Literal> element which
                //we need to get to and then process
                min = process_inner_element(elt);
            }
            else if (xmlcmp(cname.c_str(), L"UpperBoundary") == 0)
            {
                //internally the UpperBoundary element (version 1.1)
                //will contain a <Literal> element which
                //we need to get to and then process
                min = process_inner_element(elt);
            }
            else //PropertyName expected, but go through general process_element pipeline anyway
            {
                name = process_element(elt);
            }
        }

        child = child->getNextSibling();
    }

    return L"((" + name + L" >= " + min + L") AND (" + name + L" <= " + max + L"))";
}


STRING MgOgcFilterUtil::process_identifier(DOMElement* root)
{
    //get contents of <PropertyName> xml tag
    STRING propName = extract_content(root);

    //convert to FDO-like property name by converting
    //"Person/Address/City" to "Person.Address.City"
    for (size_t i=0; i<propName.length(); i++)
        if (propName[i] == L'/')
            propName[i] = L'.';

    return propName;
}

STRING MgOgcFilterUtil::process_literal(DOMElement* root)
{
    //get contents of tag. Usually the tag is named <Literal>
    //but it can be <LowerBound> or <UpperBound>
    STRING contents = extract_content(root);

    //we need to determine the data type to decide whether or not the
    //literal value needs to be enclosed in quotes. We attempt to do this
    //by looking up the data type for the last property name encountered.
    //TODO - Come up with a more robust way to determine the data type
    //being compared to the literal value. The current approach only
    //works if the property name precedes the literal value.
    bool requiresQuotes = true;
    if(!m_propName.empty() && m_propertyDefs.p != NULL)
    {
        Ptr<MgPropertyDefinition> propDef = m_propertyDefs->FindItem(m_propName);
        if (propDef->GetPropertyType () == MgFeaturePropertyType::DataProperty)
        {
            // We found a matching data property
            MgDataPropertyDefinition* dataProp = static_cast<MgDataPropertyDefinition*>(propDef.p);
            if(dataProp != NULL && dataProp->GetDataType() != MgPropertyType::String)
            {
                requiresQuotes = false;
            }
        }
    }

    STRING literalValue;
    literalValue.reserve(contents.length() + 3);

    if(requiresQuotes)
    {
        literalValue += L"'";
    }

    //copy characters one by one, replacing special characters along the way
    for (size_t i=0; i<contents.length(); i++)
    {
        if (contents[i] == L'\'')
            literalValue += L"''";
        else
            literalValue += contents[i];
    }

    if(requiresQuotes)
    {
        literalValue += L"'";
    }

    return literalValue;
}

STRING MgOgcFilterUtil::process_bbox(DOMElement* root)
{
    STRING left;
    STRING right;

    process_two_elements(root, left, right);

    //this assumes the gml:Box tag is parsed as a WKT polygon for use
    //in FDO spatial conditions which expect that

    if(left.empty() && !right.empty())
    {
        // If we have an empty property name, use the geometry property
        left = m_geomProp;
    }
    else if(!left.empty() && right.empty())
    {
        // If we have no property name element at all, use the geometry
        // property and shift the envelope part to the right
        right = left;
        left = m_geomProp;
    }

    return L"(" + left + L" ENVELOPEINTERSECTS " + right + L")";
}

STRING MgOgcFilterUtil::process_binary_spatial(DOMElement* root)
{
    STRING name = X2W(root->getTagName());
    const wchar_t* wnm = name.c_str();
    STRING op = L"INTERSECTS";

    //not strictly needed as operator names are identical
    if (xmlcmp(wnm, L"Intersects") == 0)
        op = L"INTERSECTS";
    else if (xmlcmp(wnm, L"Within") == 0)
        op = L"WITHIN";
    else if (xmlcmp(wnm, L"Contains") == 0)
        op = L"CONTAINS";
    else if (xmlcmp(wnm, L"Crosses") == 0)
        op = L"CROSSES";
    else if (xmlcmp(wnm, L"Overlaps") == 0)
        op = L"OVERLAPS";
    else if (xmlcmp(wnm, L"Touches") == 0)
        op = L"TOUCHES";
    else if (xmlcmp(wnm, L"Disjoint") == 0)
        op = L"DISJOINT";
    else
        assert(false);


    STRING left;
    STRING right;

    process_two_elements(root, left, right);

    return L"(" + left + L" " + op + L" " + right + L")";
}

STRING MgOgcFilterUtil::process_envelope(DOMElement* root)
{
    double minx = -DBL_MAX;
    double miny = -DBL_MAX;
    double maxx = +DBL_MAX;
    double maxy = +DBL_MAX;

    DOMNode* child = root->getFirstChild();

    while (0 != child)
    {
        if(child->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            DOMElement* elt = (DOMElement*)child;
            STRING nm = X2W(elt->getTagName());

            if (xmlcmp(nm.c_str(), L"lowerCorner") == 0)
            {
                wstring sll = extract_content(elt);
                swscanf(sll.c_str(), L"%lf %lf", &minx, &miny);
            }
            else if (xmlcmp(nm.c_str(), L"upperCorner") == 0)
            {
                wstring sur = extract_content(elt);
                swscanf(sur.c_str(), L"%lf %lf", &maxx, &maxy);
            }
        }

        child = child->getNextSibling();
    }

    //transform the coordinates in case we were given a coord sys transformation
    xform_box(minx, miny, maxx, maxy);

    //we convert gml envelope to a polygon for FDO purposes

    wchar_t poly[512];
    swprintf(poly, 512, L"GeomFromText('POLYGON((%g %g,%g %g,%g %g,%g %g,%g %g))')", minx, miny, maxx, miny, maxx, maxy, minx, maxy, minx, miny);

    return STRING(poly);
}


STRING MgOgcFilterUtil::process_box(DOMElement* root)
{
    //convert gml:Box to WKT geometry
    //Box is no longer used in filters, in preference to Envelope,
    //but let's support it anyways

    STRING coords = process_inner_element(root);

    //we convert gml box to a polygon -- unfortunately that requires us
    //to parse the coordinates so that we can reorganize them into a polygon
    //for now, let's do something simple

    double minx = -DBL_MAX;
    double miny = -DBL_MAX;
    double maxx = +DBL_MAX;
    double maxy = +DBL_MAX;
    int ret = swscanf(coords.c_str(), L"%lf %lf,%lf %lf", &minx, &miny, &maxx, &maxy);

    //transform the coordinates in case we were given a coord sys transformation
    xform_box(minx, miny, maxx, maxy);

    wchar_t poly[512];
    swprintf(poly, 512, L"GeomFromText('POLYGON((%g %g,%g %g,%g %g,%g %g,%g %g))')", minx, miny, maxx, miny, maxx, maxy, minx, maxy, minx, miny);

    return STRING(poly);
}

STRING MgOgcFilterUtil::process_coordinates(DOMElement* root)
{
    //converts a gml::coordinates tag to WKT coordinate string

    //get contents of tag -- it is a list of x,y pairs separated by space
    //e.g. "1,2 3,4 5,6
    STRING in = extract_content(root);

    STRING out = L"";
    out.reserve(in.length());

    //the output is a list of coordinate pairs separated by comma,
    //e.g. "1 2, 3 4, 5 6"

    bool seen_comma = false;
    bool seen_space = false;

    for (size_t i = 0; i<in.length(); i++)
    {
        wchar_t wc = in[i];

        switch (wc)
        {
        case L',': seen_comma = true;break;
        case L' ': seen_space = true;break;
        default:
            {
                if (seen_comma)
                    out += L' ';
                else if (seen_space)
                    out += L',';

                seen_comma = false;
                seen_space = false;

                out += wc;
            }
            break;
        }

    }

    //transform the coordinates in case we were given a coord sys transformation
    out = xform_coords(out);

    return out;
}

STRING MgOgcFilterUtil::process_poslist(DOMElement* root)
{
    int dim = 2; //dimensionality

    STRING dimstr = X2W(root->getAttribute(W2X(L"dimension")));

    if (!dimstr.empty())
    {
        int ndim = 0;
        int ret = swscanf(dimstr.c_str(), L"%d", &ndim);
        if (ret == 1 && ndim != 0) dim = ndim;
    }

    STRING in = extract_content(root);

    //Coordinates in a posList are separated by white space
    //and no commas, unlike the gml coordinates element.
    //We need to insert commas in the correct places for our output string
    //by taking into account the dimensionality

    STRING out = L"";
    out.reserve(in.length());

    wchar_t sep = 0;
    int count = 0;

    for (size_t i = 0; i<in.length(); i++)
    {
        wchar_t wc = in[i];

        switch (wc)
        {
        case L' ': //TODO: other white space chars
            {
                if (!sep)
                {
                    if (count % dim == 0)
                        sep = L','; //separates tuples
                    else
                        sep = L' '; //separates coordinates in a tuple
                }
            }
            break;
        default:
            {
                if (sep)
                {
                    if (count)
                        out += sep;

                    count++;
                    sep = 0;
                }

                out += wc;
            }
            break;
        }
    }

    //transform the coordinates in case we were given a coord sys transformation
    out = xform_coords(out);

    return out;
}

STRING MgOgcFilterUtil::process_pos(DOMElement* root)
{
    //for a pos element, we can just return the contents of the tag, since
    //the caller will be adding those up to make a list of all the positions
    //separated by commas
    return extract_content(root);
}


STRING MgOgcFilterUtil::process_geometry(DOMElement* root)
{
    //convert gml:_Geometry to WKT geometry

    STRING name = X2W(root->getTagName());
    const wchar_t* wnm = name.c_str();

    STRING geom = L"GeomFromText('";

    if (xmlcmp(wnm, L"LineString") == 0)
        geom += process_linestring(root);
    else if (xmlcmp(wnm, L"Polygon") == 0)
        geom += process_polygon(root);
    else if (xmlcmp(wnm, L"LinearRing") == 0)
        //must return, since we do want the GeomFromText here -- LinearRings are components of other stuff
        return process_linearring(root);
    else
        assert(false);

    geom += L"')";

    return geom;
}

STRING MgOgcFilterUtil::process_linestring(DOMElement* root)
{
    return L"LINESTRING " + process_linearring(root);
}

STRING MgOgcFilterUtil::process_linearring(DOMElement* root)
{
    //just return the coordinate list, enclosed in ()
    //to make things easier when building up polygons
    STRING out = L"(";

    bool first = true;

    DOMNode* child = root->getFirstChild();
    while(0 != child)
    {
        if(child->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            if (!first)
                out += L","; //if there are multiple elements, they must be gml:pos which we separate by ,
            else
                first = false;

            out += process_element((DOMElement*)child);
        }

        child = child->getNextSibling();
    }

    out += L")"; //close off coordinate collection

    return out;
}

STRING MgOgcFilterUtil::process_polygon(DOMElement* root)
{
    STRING out = L"POLYGON (";

    bool first = true;

    DOMNode* child = root->getFirstChild();
    while(0 != child)
    {
        //multiple elements here can be gml:interior and
        //gml:exterior (or outerBoundaryIs) -- we can ignore
        //these and go right in there to get the linear rings
        //TODO: do we care to specify which ring is the exterior?
        if(child->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            if (!first)
                out += L",";
            else
                first = false;

            out += process_inner_element((DOMElement*)child);
        }

        child = child->getNextSibling();
    }

    out += L")"; //close off coordinate collection

    return out;
}

void MgOgcFilterUtil::xform_box(double& minx, double& miny, double& maxx, double& maxy)
{
    //transforms an envelope to the FDO coordinate system, in case it was given in
    //a different coord sys (for example the map's coord sys)
    if (m_xform.p)
    {
        Ptr<MgCoordinate> c1 = new MgCoordinateXY(minx, miny);
        Ptr<MgCoordinate> c2 = new MgCoordinateXY(maxx, maxy);
        Ptr<MgEnvelope> e = new MgEnvelope(c1, c2);

        e = m_xform->Transform(e);

        c1 = e->GetLowerLeftCoordinate();
        c2 = e->GetUpperRightCoordinate();

        minx = c1->GetX();
        miny = c1->GetY();
        maxx = c2->GetX();
        maxy = c2->GetY();
    }
}

STRING MgOgcFilterUtil::xform_coords(CREFSTRING coords)
{
    //transforms a coolection of coordinates to the FDO coordinate system,
    //in case it was given in a different coord sys (for example the map's coord sys)

    //case of identity transform
    if (!m_xform.p)
        return coords;

    size_t len = coords.length();
    STRING ret;
    ret.reserve(len);

    STRING tmp;
    tmp.reserve(30);

    double tuple[4];
    tuple[0] = tuple[1] = tuple[2] = tuple[3] = 0;
    wchar_t dtos[64];
    int count = 0;

    for (size_t i=0; i<len + 1; i++)
    {
        //the last time we iterate, we insert an artificial
        //comma separator so that the last coordinate is processed
        wchar_t wc = (i == len) ? L',' : coords[i];

        switch (wc)
        {
        case L',' :
            {
                if (tmp.length())
                {
                    //parse a coordinate if non-white space
                    //chars were seen before this comma
                    swscanf(tmp.c_str(), L"%lf", &tuple[count++]);
                    tmp.clear();
                }

                if (count >= 2)
                {
                    m_xform->TransformM(&tuple[0], &tuple[1], &tuple[2], &tuple[3]);

                    //write out transformed data to the output string
                    for (int j=0; j<count; j++)
                    {
                        swprintf(dtos, 64, L"%g", tuple[j]);
                        ret += dtos;

                        if (j < count - 1)
                            ret += L" ";
                        else if (i != len)
                            ret += L",";
                    }

                    count = 0;
                }
            }
            break;
        case L' ' :
            {
                if (tmp.length())
                {
                    //parse a coordinate if non-white space
                    //chars were seen before this space
                    swscanf(tmp.c_str(), L"%lf", &tuple[count++]);
                    tmp.clear();
                }
            }
            break;
        default:
            tmp += wc;
            break;
        }
    }

    return ret;
}


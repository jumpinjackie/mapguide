//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "OgcFramework.h"
#include "WmsLayerDefinitions.h"

/*
Here's just one of what we'll be processing (we'll get a collection of these):

<ResourceDocument>
    <ResourceId>Library://Samples/Sheboygan/Layers/Hydrography.LayerDefinition</ResourceId>
    <Depth>4</Depth>
    <Owner>Administrator</Owner>
    <CreatedDate>2005-12-06T00:01:15Z</CreatedDate>
    <ModifiedDate>2005-12-06T01:29:29Z</ModifiedDate>
    <ResourceDocumentHeader xmlns:xsd="http://www.w3.org/2001/XMLSchema"
                            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                            xsi:noNamespaceSchemaLocation="ResourceDocumentHeader-1.0.0.xsd">
        <Metadata>
            <Simple>
                <Property">
                    <Name>opaque</Name>
                    <Value>0</Value>
                </Property>
                <Property">
                    <Name>Title</Name>
                    <Value>HYDROGRAPHY</Value>
                </Property>
                <Property">
                    <Name>queryable</Name>
                    <Value>0</Value>
                </Property>
                <Property">
                    <Name>KeywordList</Name>
                    <Value>MapGuide</Value>
                </Property>
                <Property">
                    <Name>MetadataURL</Name>
                    <Value>www.autodesk.com</Value>
                </Property>
                <Property">
                    <Name>BoundingBox</Name>
                    <Value>-87,44,-87,45</Value>
                </Property>
            </Simple>
        </Metadata>
    </ResourceDocumentHeader>
</ResourceDocument>
*/

MgWmsLayerDefinitions::MgWmsLayerDefinitions(MgXmlParser& Input)
:   m_XmlInput(Input)
,   m_bOk(false)

{
    //
    Input.SetOptions(keSkipWhitespace|keSkipComments|keSkipProcessingInstructions);

    // If there's a next (ie, First) element, and it's the expected beginning element
    // let's see what it is...
    if(Input.Next() && Input.Current().Type() == keBeginElement) {
        // Is it a ResourceList element?
        MgXmlBeginElement& Begin = (MgXmlBeginElement&)Input.Current();
        m_bOk = Begin.Name() == _("ResourceList");

        // Position ourselves over the next thing, the anticipated ResourceDocument element
        Input.Next();
    }
}

bool MgWmsLayerDefinitions::Next()
{
    if(!m_bOk)
        return false;

    if(m_XmlInput.Current().Type() == keBeginElement) {
        MgXmlBeginElement& Begin = (MgXmlBeginElement&)m_XmlInput.Current();
        m_bOk = Begin.Name() == _("ResourceDocument");
    }
    else
        m_bOk = false;

    return m_bOk;
}

void MgWmsLayerDefinitions::GenerateDefinitions(MgUtilDictionary& Dictionary)
{
    MgXmlSynchronizeOnElement ResourceDocument(m_XmlInput,_("ResourceDocument"));
    if(!ResourceDocument.AtBegin())
        return; // Something is wrong.  We leave.

    while(!ResourceDocument.AtEnd()) {
        STRING sValue; // basic_string
        if(GetElementContents(_("ResourceId"),sValue)) {
            // Okay, the ResourceId is too decorated for our purposes;
            // the outside world doesn't need to know (and clutter up
            // URL command lines with) this syntactic "punctuation" so
            // we just get rid of it.
            // Remove the Library prefix, if present.
            if(sValue.find(_("Library://")) == 0)
                sValue = sValue.substr(10);
            // Remove the LayerDefinition suffix, if present.
            int iEnd = sValue.find(_(".LayerDefinition"));
            if(iEnd >= 0)
                sValue.resize(iEnd);
            // There, that's our Layer Name.
            Dictionary.AddDefinition(_("Layer.Name"),sValue);

            // Until we have "Friendly Name" support, the
            // friendly name will simply be the layer name sans
            // path.
            iEnd = sValue.find_last_of('/');
            if(iEnd >= 0)
                sValue = sValue.substr(iEnd+1); // one past the slash.

            // That's our Layer Title,
            // Note that subsequently-found metadata may override this
            // definition with a real title... one that the user actually
            // wants.  This just provides a default in case no such
            // friendly name exists.... that keeps the list of layer names
            // from being a list of empty strings.
            Dictionary.AddDefinition(_("Layer.Title"),sValue);
        }
        else if(!GetMetadataDefinitions(Dictionary)) {
          SkipElement(NULL);
        }
    }

}

bool MgWmsLayerDefinitions::GetElementContents(CPSZ pszElementName,STRING& sValue)
{
    MgXmlSynchronizeOnElement Element(m_XmlInput,pszElementName);
    if(!Element.AtBegin())
        return false;

    if(m_XmlInput.Current().Type() == keText) {
        sValue = m_XmlInput.Current().Contents();
        return true;
    }

    return false;
}

bool MgWmsLayerDefinitions::GetMetadataDefinitions(MgUtilDictionary& Dictionary)
{
//    STRING sDebug = m_XmlInput.Current().Contents();
    // We're looking for a <ResourceDocumentHeader ...>
    MgXmlSynchronizeOnElement ElementResourceDocumentHeader(m_XmlInput,_("ResourceDocumentHeader"));
    if(!ElementResourceDocumentHeader.AtBegin())
        return false;

    // And inside that, there's a <Metadata ...>
    MgXmlSynchronizeOnElement ElementMetadata(m_XmlInput,_("Metadata"));
    if(!ElementMetadata.AtBegin())
        return false;

    // And inside *that*, we hope there's a <Simple...>
    MgXmlSynchronizeOnElement ElementSimple(m_XmlInput,_("Simple"));
    if(!ElementSimple.AtBegin())
        return false;

    // And once we're here, we hope to find a grunch of <Property...> elements
    while(!ElementSimple.AtEnd()) {
        MgXmlSynchronizeOnElement ElementProperty(m_XmlInput,_("Property"));
        if(ElementProperty.AtBegin()) {
            // Each of which consist of <Name> and <Value> pairs...
            STRING sName;
            STRING sValue;
            if(GetElementContents(_("Name"),sName) && GetElementContents(_("Value"),sValue)) {
                STRING sDefinitionName =  _("Layer.") + sName;

                //----------------------------------------------------------------------
                // If it starts and ends with escaped angled brackets, let's assume it's
                // "corrupted" XML that simply needs unescaping.
                //
                // TODO: This is not meant to be a long-term solution; it just overcomes
                // a current schema restriction on metadata consisting of mixed content.
                int iLt =sValue.find(_("&lt;"));
                int iGt = sValue.rfind(_("&gt;"));
                int iLen = sValue.length();
                if(sValue.find(_("&lt;")) == 0 && sValue.rfind(_("&gt;")) == sValue.length() - 4) {
                  int iPos;
                  while((iPos = sValue.find(_("&lt;"))) >= 0)
                    sValue = sValue.substr(0,iPos) + _("<") + sValue.substr(iPos+4);
                  while((iPos = sValue.find(_("&gt;"))) >= 0)
                    sValue = sValue.substr(0,iPos) + _(">") + sValue.substr(iPos+4);
                  while((iPos = sValue.find(_("\x201d"))) >= 0)
                    sValue = sValue.substr(0,iPos) + _("\"") + sValue.substr(iPos+1);
                }
                //----------------------------------------------------------------------

                Dictionary.AddDefinition(sDefinitionName,sValue);
            }
        }
    }

    return true;
}


bool MgWmsLayerDefinitions::SkipElement(CPSZ pszElementName)
{
    STRING sName;
    if(pszElementName == NULL && m_XmlInput.Current().Type() == keBeginElement) {
        MgXmlBeginElement& Begin = (MgXmlBeginElement&)m_XmlInput.Current();
        sName = Begin.Name();
        pszElementName = sName.c_str();
    }

    MgXmlSynchronizeOnElement Whatever(m_XmlInput,pszElementName);

    return Whatever.AtBegin();
}

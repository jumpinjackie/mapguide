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
#include "WfsFeatureDefinitions.h"

/*
  This source file starts out looking exactly like MgWfsFeatureDefinitions.cpp/.h
  but (a) since it represents a different object, and (b) the implementation is
  expected to devolve into two considerably different things, it's broken out
  into separate code.
*/

MgWfsFeatureDefinitions::MgWfsFeatureDefinitions(MgResourceService* pResourceService,MgFeatureService* pFeatureService)
:   m_pResourceService(pResourceService)
,   m_pFeatureService(pFeatureService)
,   m_sSubsetOfTypes(_("\n"))
,   m_bOk(false)
{

    /*
    // NOTE: It would be cool if this logic could exist on the server, allowing only
    // one round-trip between webtier and server for this entire aggregated bit of info.
    */

    // Create MgResourceIdentifier
    MgResourceIdentifier mgrIdentifier(_("Library://"));

    // Run API command
    STRING sType = _("FeatureSource");
    INT32 keProperties = MgResourceHeaderProperties::Metadata;
    STRING sDontCare(_(""));
    Ptr<MgByteReader> Result =
          pResourceService->EnumerateResources(&mgrIdentifier, // "Library://"
                                                -1,             // Infinite depth
                                                sType,          // "LayerDefinition"
                                                keProperties,   // want metadata, not security
                                                sDontCare,      // start date; don't care
                                                sDontCare);     // end date; don't care

    STRING sLayers = Result->ToString();
    MgXmlParser Input(sLayers.c_str());
    Input.SetOptions(keSkipWhitespace|keSkipComments|keSkipProcessingInstructions);

    // Scoot us up to the first element
    Input.Next();

    // Is it a ResourceList element?
    MgXmlSynchronizeOnElement ResourceList(Input,_("ResourceList"));
    if(!ResourceList.AtBegin())
        return;

    // Everything will be put into this thing
    // though we don't get things in the right order,
    // so some juggling is called for.
    CStringStream oTheWholeEnchilada;

    while(!ResourceList.AtEnd()) {
        // And just inside of that, is it a ResourceDocument element?
        MgXmlSynchronizeOnElement ResourceDocument(Input,_("ResourceDocument"));
        if(!ResourceDocument.AtBegin())
            return; // Something is wrong.  We leave.

        STRING sResource;
        STRING sSource;

        CStringStream oDefinitions;
        // We're looking specifically for ResourceId elements.
        while(!ResourceDocument.AtEnd()) {
            if(GetElementContents(Input,_("ResourceId"),sResource)) {
                // Okay, the ResourceId is too decorated for our purposes;
                // the outside world doesn't need to know (and clutter up
                // URL command lines with) this syntactic "punctuation" so
                // we just get rid of it.
                // Remove the Library prefix, if present.
                sSource = sResource;
                if(sSource.find(_("Library://")) == 0)
                    sSource = sSource.substr(10);
                // Remove the LayerDefinition suffix, if present.
                int iEnd = sSource.find(_(".FeatureSource"));
                if(iEnd >= 0)
                    sSource.resize(iEnd);
                // There, that's our Layer Source.
                AddDefinition(oDefinitions,_("Source"),sSource.c_str());
                AddDefinition(oDefinitions,_("Source.id"),sResource.c_str());


            }
            else if(GetMetadataDefinitions(Input,oDefinitions)) {
            }
            else {
              SkipElement(Input,NULL);
            }
        } // while not at end of ResourceDocument

        // Now, given the feature source, we need to find out stuff about
        // it...
        MgResourceIdentifier idResource(sResource);

        // And what classes we can expect to find.
        // That's done indirectly by enumerating the schemas the source reports...
        STRING sDontCare(_(""));
        Ptr<MgFeatureSchemaCollection> pSchemas = this->m_pFeatureService->DescribeSchema(&idResource,sDontCare);

        for(INT32 iSchema=0; iSchema<pSchemas->GetCount(); iSchema++) {
            Ptr<MgFeatureSchema> pSchema = pSchemas->GetItem(iSchema);
            Ptr<MgClassDefinitionCollection> pClasses = pSchema->GetClasses();
            // ... and by the classes each schema reports.
            for(INT32 iClass=0; iClass<pClasses->GetCount(); iClass++) {
                Ptr<MgClassDefinition> pClass = pClasses->GetItem(iClass);

                // What internally is known as the name is really
                // the human-readable thing, which in OGC parlance is the title.
                STRING sTitle = pClass->GetName();

                // And what OGC wants to call a Name is really the internal
                // cookie.  We use the stripped down resource name (source)
                // plus the human-readable name
                STRING sName = sSource+_(":")+sTitle;

                // Now, spew out everything we know about this class.
                STRING sFeatureNameAttr(_("id='"));
                sFeatureNameAttr += sName;
                sFeatureNameAttr += _("'");
                MgXmlElementEmitter oXmlFeatureClass(oTheWholeEnchilada,_("FeatureClass"),sFeatureNameAttr.c_str());
                AddDefinition(oTheWholeEnchilada,_("Feature.Name"),sName.c_str());
                AddDefinition(oTheWholeEnchilada,_("Feature.Title"),sName.c_str());

                STRING sDescription = pClass->GetDescription();
                AddDefinition(oTheWholeEnchilada,_("Feature.Description"),sDescription.c_str());

                // TODO: evaluate what, of interest, can be found here.
                //Ptr<MgPropertyDefinitionCollection> pProperties = pClass->GetProperties();

                // Also include the definitions that are common to all classes in this source.
                STRING sDefinitions = oDefinitions.Contents();
                oXmlFeatureClass.Write(sDefinitions.c_str(),sDefinitions.length());
            }
        }
    } // while not end of ResourceList

    // TODO: rename this m_s... it's a misnomer.
    m_sSourcesAndClasses = oTheWholeEnchilada.Contents();

    // Note that the string m_sSourcesAndClasses is really just an xml fragment;
    // there's no one root item, just a bunch of <Class> elements.
    m_pXmlInput = new MgXmlParser(m_sSourcesAndClasses.c_str());
    // Skip anything out of the ordinary.
    m_pXmlInput->SetOptions(keSkipWhitespace|keSkipComments|keSkipProcessingInstructions);
    // Advance to the first thing on our list.
    m_bOk = m_pXmlInput->Next();
}


MgWfsFeatureDefinitions::~MgWfsFeatureDefinitions()
{
    if(m_pXmlInput != NULL)
        delete(m_pXmlInput);
}


void MgWfsFeatureDefinitions::AddDefinition(CStream& oStream,CPSZ pszPropertyName,CPSZ pszPropertyValue)
{
    STRING sItem(_("item='"));
    sItem+=pszPropertyName;
    sItem+=_("'");
    MgXmlElementEmitter oProperty(oStream,_("Define"),sItem.c_str());
    oProperty.Write(pszPropertyValue);
}


bool MgWfsFeatureDefinitions::Next()
{
    // Let's only do work if we enter with
    // any expectation of success.
    if(m_bOk) {
        while (true) {
            // End of stream, for some reason? No "next".
            if(m_pXmlInput->AtEnd()) {
                m_bOk = false;
                break; // while
            }

            // Are we over a begin element?
            if(m_pXmlInput->Current().Type() != keBeginElement) {
                m_bOk = false;
                break; // while
            }

            // Okay; a good sign... let's start poking around.
            MgXmlBeginElement& Begin = (MgXmlBeginElement&)m_pXmlInput->Current();

            // Are we on a well-formed FeatureClass item?
            STRING sId;
            if(Begin.Name() != _("FeatureClass") || !Begin.GetAttribute(_("id"),sId)) {
                m_bOk = false; // internal error.
                break; // while
            }

            // Is the feature class one we want?
            // This is good!  We like this condition.
            if(IsWantedSubset(sId.c_str())) {
                break; // while
            }

            // Unwanted feature.  Let's skip it.
            MgXmlSynchronizeOnElement SkipThis(*m_pXmlInput,_("FeatureClass"));
            // Declare our intent to go into the element.
            SkipThis.AtBegin();
            // Now, SkipThis will automatically advance to the end.
            // This might potentially have put us AtEnd of the stream,
            // but we check for that at loop top.
        }
    }

    return m_bOk;
}

void MgWfsFeatureDefinitions::GenerateDefinitions(MgUtilDictionary& Dictionary)
{
    STRING sClass;
    STRING sTitle;

    MgXmlSynchronizeOnElement oFeatureClass(*m_pXmlInput,_("FeatureClass"));
    if(oFeatureClass.AtBegin()) {
        // Spin through all of the things in our feature class...
        while(!oFeatureClass.AtEnd()) {
            // ... which should just be a sequence of <Define> elements
            MgXmlSynchronizeOnElement oDefine(*m_pXmlInput,_("Define"));
            MgXmlBeginElement* pBegin;
            if(oDefine.AtBegin(&pBegin)) {
                STRING sName;
                pBegin->GetAttribute(_("item"),sName);

                // Done with the begin element; advance into its contents.
                m_pXmlInput->Next(); pBegin = NULL;

                // Spin through and slurp out the entire contents of the
                // definition.
                STRING sValue;
                while(!oDefine.AtEnd()) {
                    sValue += m_pXmlInput->Current().Contents();
                    m_pXmlInput->Next();
                }

                // Whatever we find, let's add it to that great dictionary out there.
                Dictionary.AddDefinition(sName,sValue);
            }
        }
    }
}

bool MgWfsFeatureDefinitions::GetElementContents(MgXmlParser& Input,CPSZ pszElementName,STRING& sValue)
{
    MgXmlSynchronizeOnElement Element(Input,pszElementName);
    if(!Element.AtBegin())
        return false;

    if(Input.Current().Type() == keText) {
        sValue = Input.Current().Contents();
        return true;
    }

    return false;
}


bool MgWfsFeatureDefinitions::GetMetadataDefinitions(MgXmlParser& Input,CStream& oStream)
{
    STRING sDebug = Input.Current().Contents();
    MgXmlSynchronizeOnElement ElementResourceDocumentHeader(Input,_("ResourceDocumentHeader"));
    if(!ElementResourceDocumentHeader.AtBegin())
        return false;

    sDebug = Input.Current().Contents();
    MgXmlSynchronizeOnElement ElementMetadata(Input,_("Metadata"));
    if(!ElementMetadata.AtBegin())
        return false;

    sDebug = Input.Current().Contents();
    MgXmlSynchronizeOnElement ElementSimple(Input,_("Simple"));
    if(!ElementSimple.AtBegin())
        return false;

    while(!ElementSimple.AtEnd()) {
        MgXmlSynchronizeOnElement ElementProperty(Input,_("Property"));
        if(ElementProperty.AtBegin()) {
            STRING sName;
            STRING sValue;
            if(GetElementContents(Input,_("Name"),sName) && GetElementContents(Input,_("Value"),sValue)) {
                STRING sDefinitionName =  _("Feature.") + sName;
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
                  while((iPos = sValue.find(_("\x201d"))) >= 0) // gendered quote, as has been encountered.
                    sValue = sValue.substr(0,iPos) + _("\"") + sValue.substr(iPos+1);
                }
                //----------------------------------------------------------------------
                AddDefinition(oStream,sDefinitionName.c_str(),sValue.c_str());
            }
        }
    }

    return true;
}


bool MgWfsFeatureDefinitions::SkipElement(MgXmlParser& Input,CPSZ pszElementName)
{
    STRING sName;
    if(pszElementName == NULL && Input.Current().Type() == keBeginElement) {
        MgXmlBeginElement& Begin = (MgXmlBeginElement&)Input.Current();
        sName = Begin.Name();
        pszElementName = sName.c_str();
    }

    MgXmlSynchronizeOnElement Whatever(Input,pszElementName);

    return Whatever.AtBegin();
}


void MgWfsFeatureDefinitions::Initialize()
{
    /* a debuggery cheat, for when the server isn't there...
    VPSZ p = MgOgcServer::LoadFile(_("c:\\FeatureDefinitions.xml"));
    sLayers = p;
    free(p);
    //*/
}


// Does the resource contain the feature named?
bool MgWfsFeatureDefinitions::HasFeature(CPSZ pszFeatureName)
{
    STRING sKey;
    sKey = _("<Define item='Feature.Name'>"); // NOXLATE
    sKey += pszFeatureName;
    sKey +=_("<");// NOXLATE

    int iPos = this->m_sSourcesAndClasses.find(sKey);
    return iPos >= 0;
}


// Request that the otherwise complete enumeration be restricted to this subset.
// Returns true if successful; false indicates a mismatch (like asking for a
// feature that isn't part of the complete set.)
// String is a comma-separated list of feature names.
bool MgWfsFeatureDefinitions::SubsetFeatureList(CPSZ pszFeatureNames)
{
    if(pszFeatureNames == NULL || szlen(pszFeatureNames) == 0) {
        return true;
    }

    // We're destructive to this string, so let's make a local copy.
    STRING sTypeNames(pszFeatureNames);

    int iPos;
    while( (iPos = sTypeNames.find(_(","))) >= 0 ) {
        STRING sType = sTypeNames.substr(0,iPos);
        sTypeNames = sTypeNames.substr(iPos +/* length(",")*/1);
        if(!AddSubset(sType.c_str()))
            return false;
    }

    return AddSubset(sTypeNames.c_str());
}

bool MgWfsFeatureDefinitions::AddSubset(CPSZ pszTypeName)
{
    if(!HasFeature(pszTypeName))
        return false;
    else {
        m_sSubsetOfTypes += pszTypeName;
        m_sSubsetOfTypes += _("\n");
        return true;
    }
}

bool MgWfsFeatureDefinitions::IsWantedSubset(CPSZ pszTypeName)
{
    // special case: no subset is in effect.
    if(m_sSubsetOfTypes.length() <= 1)
        return true;

    STRING sTypeName(_("\n"));
    sTypeName += pszTypeName;
    sTypeName += _("\n");
    int iPos = m_sSubsetOfTypes.find(sTypeName);
    return  iPos >= 0;
}


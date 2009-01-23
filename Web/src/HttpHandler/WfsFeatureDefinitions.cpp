//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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


/*

  Before:
    A FeatureType's name was the composition of an abbreviation of the
    Resource name (sans "Library://" and ".FeatureSource") concatenated
    with the class's definition name, so you'd end up with something like
    this:  "Samples/Sheboygan/Data/HydrographicLines:HydrographicLines"
  Now:
    A FeatureType's name now conforms to XML Qualified Name (QName) syntax,
    so the same feature above would now be:
       "ns191970179:HydrographicLines"
    with the following support:
       xmlns:ns191970179="Library://Samples/Sheboygan/Data/HydrographicLines.FeatureSource"
    WFS certification seems to require QName syntax.  The algorithm used here allows us
    to shorten class names, while still being able to detect data resource name
    via namespace, (or absent that -- as might be the case for HTTP GET forms of
    GetFeature or DescribeFeatureType -- the ability to infer via the namespace prefix
    "hash" what the likely feature source is.)

    The following definitions are thus created:

    Feature.Name:     "HydrographicLines"
    Feature.Prefix:   "ns191970179"
    Feature.FullName: "ns191970179:HydrographicLines"
    Feature.Resource: "Library://Samples/Sheboygan/Data/HydrographicLines.FeatureSource"
*/

#include "OgcFramework.h"
#include "WfsFeatureDefinitions.h"

MgWfsFeatureDefinitions::MgWfsFeatureDefinitions(MgResourceService* pResourceService,
                                                 MgFeatureService* pFeatureService)
:   m_pResourceService(pResourceService)
,   m_pFeatureService(pFeatureService)
,   m_pFeatureTypes(NULL)
,   m_pXmlInput(NULL)
,   m_sSubsetOfTypes(_("\n"))
,   m_bOk(false)
{
    Initialize();
}

MgWfsFeatureDefinitions::MgWfsFeatureDefinitions(MgResourceService* pResourceService,
                                                 MgFeatureService* pFeatureService,
                                                 MgStringCollection* pFeatureTypes)
:   m_pResourceService(pResourceService)
,   m_pFeatureService(pFeatureService)
,   m_pFeatureTypes(pFeatureTypes)
,   m_pXmlInput(NULL)
,   m_sSubsetOfTypes(_("\n"))
,   m_bOk(false)
{
    Initialize();
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


bool MgWfsFeatureDefinitions::GetMetadataDefinitions(MgXmlParser& Input,CStream& oStream, bool& isPublished)
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
                STRING sDefinitionName =  _("Feature.");
                // Present the names slightly differently than internal representation.
                // System-defined metadata is published with an underscore prefix.  We
                // publish this without the underscore: "_Bounds" -> "Layer.Bounds".
                // User-defined metadata will not have the underscore, and we present
                // this for consumption as "Layer.user.Whatever" -- just to make sure
                // that the user and system namespaces remain distinct.
                if(sName[0] == '_')
                    sDefinitionName += sName.substr(1);
                else
                    sDefinitionName += _("user.") + sName;
                //----------------------------------------------------------------------
                // If it starts and ends with escaped angled brackets, let's assume it's
                // "corrupted" XML that simply needs unescaping.
                //
                // TODO: This is not meant to be a long-term solution; it just overcomes
                // a current schema restriction on metadata consisting of mixed content.
                while(sValue.length() > 0 && isspace(sValue[sValue.length()-1]))
                  sValue = sValue.substr(0,sValue.length()-1);
                STRING::size_type iLt =sValue.find(_("&lt;"));
                STRING::size_type iGt = sValue.rfind(_("&gt;"));
                STRING::size_type iLen = sValue.length();
                if(sValue.find(_("&lt;")) == 0 && sValue.rfind(_("&gt;")) == sValue.length() - 4) {
                  STRING::size_type iPos;
                  while((iPos = sValue.find(_("&lt;"))) != STRING::npos)
                    sValue = sValue.substr(0,iPos) + _("<") + sValue.substr(iPos+4);
                  while((iPos = sValue.find(_("&gt;"))) != STRING::npos)
                    sValue = sValue.substr(0,iPos) + _(">") + sValue.substr(iPos+4);
                  while((iPos = sValue.find(_("\x201d"))) != STRING::npos) // gendered quote, as has been encountered.
                    sValue = sValue.substr(0,iPos) + _("\"") + sValue.substr(iPos+1);
                }
                //----------------------------------------------------------------------
                AddDefinition(oStream,sDefinitionName.c_str(),sValue.c_str());
                if(SZ_EQ(sDefinitionName.c_str(), _("Feature.IsPublished")) && SZ_EQ(sValue.c_str(), _("1")))
                {
                    isPublished = true;
                }
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
    /*
    // NOTE: It would be cool if this logic could exist on the server, allowing only
    // one round-trip between webtier and server for this entire aggregated bit of info.
    */

    // Run API command to retrieve all the available WFS feature sources.
    STRING sLayers = m_pResourceService->EnumerateResourceDocuments(NULL,
        MgResourceType::FeatureSource, MgResourceHeaderProperties::Metadata);
    
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
    // The end result is some "pseudo-XML" (no single root element)
    // that is a collection of feature classes, as follows:
    //
    //  <FeatureClass id='ns180401301:BuildingOutlines' xmlns:ns180401301='Library://Samples/Sheboygan/Data/BuildingOutlines.FeatureSource'>
    //  <Define item='Feature.Name'>BuildingOutlines</Define>
    //  <Define item='Feature.Title'>BuildingOutlines</Define>
    //  <Define item='Feature.FullName'>ns180401301:BuildingOutlines</Define>
    //  <Define item='Feature.Description'></Define>
    //  <Define item='Feature.Prefix'>ns180401301</Define>
    //  <Define item='Feature.Resource'>Library://Samples/Sheboygan/Data/BuildingOutlines.FeatureSource</Define>
    //  <Define item='Feature.PrimarySRS'>EPSG:4326</Define>
    //  <Define item='Feature.Bounds'><Bounds SRS="EPSG:4326" west="-87.74" south="43.68" east="-87.69" north="43.815"></Define>
    //  <Define item='Feature.Abstract'>Building Outlines.</Define>
    //  <Define item='Feature.Keywords'>Buildings, Outlines</Define>
    //  <Define item='Feature.Title'>Building Outlines</Define>
    //  <Define item='Feature.IsPublished'>1</Define>
    //  </FeatureClass>

    CStringStream oTheWholeEnchilada;

    // Optimization: Determine the prefix for each required feature type.
    // This allows us to filter out any feature sources with a prefix not
    // in the list.
    Ptr<MgStringCollection> requiredPrefixList;
    if(m_pFeatureTypes != NULL)
    {
        requiredPrefixList= new MgStringCollection();
        for(int i = 0; i< m_pFeatureTypes->GetCount(); i++)
        {
            STRING typeName = m_pFeatureTypes->GetItem(i);
            STRING::size_type separatorPos = typeName.find(_(":"));
            if(separatorPos != STRING::npos)
            {
                STRING requiredPrefix = typeName.substr(0, separatorPos);
                if(!requiredPrefix.empty())
                {
                    requiredPrefixList->Add(requiredPrefix);
                }
                else
                {
                    // No prefix. We can't filter this way.
                    requiredPrefixList = NULL;
                    break;
                }
            }
            else
            {
                // No prefix. We can't filter this way.
                requiredPrefixList = NULL;
                break;
            }
        }
    }
    else
    {
        // No typenames were specified. No filtering required.
        requiredPrefixList = NULL;
    }

    while(!ResourceList.AtEnd())
    {
        // And just inside of that, is it a ResourceDocument element?
        MgXmlSynchronizeOnElement ResourceDocument(Input,_("ResourceDocument"));
        if(!ResourceDocument.AtBegin())
            return; // Something is wrong.  We leave.

        STRING sResource; // "Library://....FeatureSource"
        STRING sPrefix;   // "ns######" based on hash of sResource.

        CStringStream oDefinitions;
        bool required = false;

        // We're looking specifically for ResourceId elements.
        while(!ResourceDocument.AtEnd())
        {
            if(GetElementContents(Input,_("ResourceId"),sResource))
            {
                // We get the canonical xmlns:prefix associated with the resource.
                FeatureSourceToPrefix(sResource,sPrefix);

                // Optimization: Stop processing if this feature source isn't published
                if(requiredPrefixList != NULL)
                {
                    if(!requiredPrefixList->Contains(sPrefix))
                    {
                        bool elemFound = false;
                        STRING expPrefix = sPrefix + _("sn"); //NOXLATE
                        for (int idx = 0; idx < requiredPrefixList->GetCount(); idx++)
                        {
                            STRING reqPrefix = requiredPrefixList->GetItem(idx);
                            if (reqPrefix.find(expPrefix) != STRING::npos)
                            {
                                elemFound = true;
                                break;
                            }
                        }
                        if (!elemFound)
                        {
                            required = false;
                            break;
                        }
                    }
                }

                AddDefinition(oDefinitions,_("Feature.Prefix"),sPrefix.c_str());
                AddDefinition(oDefinitions,_("Feature.Resource"),sResource.c_str());
            }
            else if(GetMetadataDefinitions(Input,oDefinitions,required))
            {
                // Optimization: Stop processing if this feature source isn't published
                if(!required)
                {
                    break;
                }
            }
            else
            {
              SkipElement(Input,NULL);
            }
        } // while not at end of ResourceDocument

        // We are only interested in feature sources that are published via WFS
        if(required)
        {
            MgResourceIdentifier idResource(sResource);

            // Now, given the feature source, we need to find out stuff about
            // it; there are one or more Feature *Classes* defined within it.
            // And what classes we can expect to find.
            // That's done indirectly by enumerating the schemas the source reports...
            STRING sDontCare(_(""));
            Ptr<MgFeatureSchemaCollection> pSchemas = this->m_pFeatureService->DescribeSchema(&idResource, sDontCare, NULL);

            for(INT32 iSchema=0; iSchema<pSchemas->GetCount(); iSchema++) {
                Ptr<MgFeatureSchema> pSchema = pSchemas->GetItem(iSchema);
                Ptr<MgClassDefinitionCollection> pClasses = pSchema->GetClasses();
                // ... and by the classes each schema reports.
                for(INT32 iClass=0; iClass<pClasses->GetCount(); iClass++) {
                    Ptr<MgClassDefinition> pClass = pClasses->GetItem(iClass);

                    // TODO: STALE?
                    // What internally is known as the name is really
                    // the human-readable thing, which in OGC parlance is the title.
                    STRING sName = pClass->GetName();
                    STRING sSchemaName = pSchema->GetName();

                    STRING sHashSchemaName;
                    if (pSchemas->GetCount() > 1)
                    {
                        // in case we have multiple schema add the hash string for schema name to be able to identify
                        // the class definition since we may have same class name on different schemas
                        MgUtil::Int32ToString(StringHasher(sSchemaName.c_str()),sHashSchemaName);
                        sHashSchemaName = _("sn")+ sHashSchemaName;
                    }

                    STRING newPrefix = sPrefix + sHashSchemaName;
                    // TODO: STALE?
                    // And what OGC wants to call a Name is really the internal
                    // cookie.  We use the stripped down resource name (source)
                    // plus the human-readable name
                    STRING sFullName = newPrefix + _(":") + sName;

                    // Now, spew out everything we know about this class.
                    STRING sFeatureNameAttr(_("id='"));
                    sFeatureNameAttr += sFullName;
                    sFeatureNameAttr += _("' xmlns:");
                    sFeatureNameAttr += newPrefix;
                    sFeatureNameAttr += _("='");
                    sFeatureNameAttr += sResource;
                    sFeatureNameAttr += _("'");

                    MgXmlElementEmitter oXmlFeatureClass(oTheWholeEnchilada,_("FeatureClass"),sFeatureNameAttr.c_str());
                    AddDefinition(oTheWholeEnchilada,_("Feature.Name"),sName.c_str());
                    AddDefinition(oTheWholeEnchilada,_("Feature.Title"),sName.c_str());
                    AddDefinition(oTheWholeEnchilada,_("Feature.FullName"),sFullName.c_str());
                    // Stuff like Feature.Prefix and Feature.Resource are in oDefinitions, and
                    // will be appended below.

                    // This is strange; it's called the "Description", but ends up being
                    // the type's name.
                    STRING sDescription = pClass->GetDescription();
                    AddDefinition(oTheWholeEnchilada,_("Feature.Description"),sDescription.c_str());

                    // TODO: evaluate what, of interest, can be found here.
                    //Ptr<MgPropertyDefinitionCollection> pProperties = pClass->GetProperties();

                    // Also tack on the definitions that are common to all classes in this source.
                    STRING sDefinitions = oDefinitions.Contents();
                    oXmlFeatureClass.Write(sDefinitions.c_str(),sDefinitions.length());
                }
            }
        } // while not end of ResourceList
    }

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


// Does the resource contain the feature named?
bool MgWfsFeatureDefinitions::HasFeature(CPSZ pszFeatureName)
{
    STRING sKey;
    sKey = _("<Define item='Feature.FullName'>"); // NOXLATE
    sKey += pszFeatureName;
    sKey +=_("<");// NOXLATE

    STRING::size_type iPos = this->m_sSourcesAndClasses.find(sKey);
    return iPos != STRING::npos;
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

    STRING::size_type iPos;
    while( (iPos = sTypeNames.find(_(","))) != STRING::npos ) {
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
    STRING::size_type iPos = m_sSubsetOfTypes.find(sTypeName);
    return  iPos != STRING::npos;
}


bool MgWfsFeatureDefinitions::FeatureSourceToPrefix(CREFSTRING sFeatureSource,
                                                    REFSTRING sPrefix)
{
    STRING sHash;
    MgUtil::Int32ToString(StringHasher(sFeatureSource.c_str()),sHash);
    sPrefix = _("ns")+ sHash; //NOXLATE

    return true;
}


bool MgWfsFeatureDefinitions::PrefixToFeatureSource(STRING sPrefix, REFSTRING sFeatureSource, REFSTRING sSchemaName)
{
    sSchemaName = _(""); // NOXLATE
    STRING sKey;
    sKey = _("xmlns:"); // NOXLATE
    sKey += sPrefix;
    sKey +=_("='");// NOXLATE

    STRING::size_type iPosNs = sPrefix.find(_("ns")); //NOXLATE
    if (iPosNs == 0) // prefix needs to strt with "ns"
    {
        iPosNs = sPrefix.find(_("sn")); // look for schema hash name
        if(iPosNs != STRING::npos)
            sSchemaName = sPrefix.substr(iPosNs); // get the value in case we have it
    }
    STRING::size_type iPos = m_sSourcesAndClasses.find(sKey);
    if(iPos == STRING::npos && iPosNs != STRING::npos)
    {
        sKey = _("xmlns:"); // NOXLATE
        sKey += sPrefix.substr(0, iPosNs);
        sKey +=_("='");// NOXLATE
        iPos = m_sSourcesAndClasses.find(sKey);
    }
    if(iPos != STRING::npos) {
        iPos += sKey.length(); // advance us past the key, we're pointing to the FeatureSource.
        STRING::size_type iEnd = m_sSourcesAndClasses.find(_("'"),iPos); // NOXLATE
        if(iEnd != STRING::npos) {
            sFeatureSource = m_sSourcesAndClasses.substr(iPos,iEnd-iPos);
            return true;
        }
    }
    return false;


    // TODO: for convenience, truncate sPrefix (allow qname.)
    return false; // TODO: FINISH
}


// Just a simple hashing algorithm
unsigned MgWfsFeatureDefinitions::StringHasher(CPSZ pszString)
{
  size_t i = 0;
  size_t iLen = szlen(pszString);
  unsigned x = 0;
  unsigned uRet = 0;

  for(i = 0; i<iLen; i++) {
    uRet = (uRet << 4) + pszString[i];
    x = (uRet & 0xF0000000);
    if(x != 0)
      uRet = uRet ^ (x >> 24);
      uRet = uRet & (~ x);
  }
  return uRet;
}






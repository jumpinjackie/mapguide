#include "ResourceDefs.h"
#include "ResourcePackageManifestParser.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgdResourcePackageManifestParser::MgdResourcePackageManifestParser()
{
    m_operations.reserve(256);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgdResourcePackageManifestParser::~MgdResourcePackageManifestParser()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Parses the specified resource package manifest.
///
void MgdResourcePackageManifestParser::Parse(const string& manifest)
{
    assert(!manifest.empty());

    MG_RESOURCE_SERVICE_TRY()

    m_description.clear();
    m_operations.clear();

    m_xmlUtil.reset(new MgXmlUtil(manifest));

    DOMElement* rootNode = m_xmlUtil->GetRootNode();

    m_xmlUtil->GetElementValue(rootNode, "Description", m_description, false);

    DOMNode* opsNode = m_xmlUtil->GetElementNode(rootNode, "Operations");
    DOMNodeList* opNodeList = m_xmlUtil->GetNodeList(opsNode, "Operation");

    if (NULL != opNodeList)
    {
        for (XMLSize_t i = 0; i < opNodeList->getLength(); ++i)
        {
            MgOperationInfo opInfo;
            wstring name, version;
            DOMNode* opNode = opNodeList->item(i);

            m_xmlUtil->GetElementValue(opNode, "Name", name);
            m_xmlUtil->GetElementValue(opNode, "Version", version);

            opInfo.SetName(name);
            opInfo.SetVersion(version);

            DOMNodeList* paramNodeList = m_xmlUtil->GetNodeList(opNode, "Parameter");

            if (NULL != paramNodeList)
            {
                for (XMLSize_t j = 0; j < paramNodeList->getLength(); ++j)
                {
                    MgdOperationParameter opParam;
                    wstring name, value, contentType;
                    DOMNode* paramNode = paramNodeList->item(j);

                    m_xmlUtil->GetElementValue(paramNode, "Name", name);
                    m_xmlUtil->GetElementValue(paramNode, "Value", value);
                    m_xmlUtil->GetElementValue(paramNode, "ContentType", contentType, false);

                    opParam.SetValue(value);
                    opParam.SetContentType(contentType);
                    opInfo.AddParameter(name, opParam);
                }
            }

            m_operations.push_back(opInfo);
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourcePackageManifestParser.Parse")
}

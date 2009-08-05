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

#include "PlatformBase.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgPrintLayoutServiceBase object.
///
MgPrintLayoutServiceBase::MgPrintLayoutServiceBase() : MgService()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destroys an MgPrintLayoutServiceBase object.
///
MgPrintLayoutServiceBase::~MgPrintLayoutServiceBase()
{
    for (MgPrintLayoutElementFactoryMap::iterator i = m_printLayoutElementFactories.begin();
        i != m_printLayoutElementFactories.end(); ++i)
    {
        SAFE_RELEASE((*i).second);
    }
    m_printLayoutElementFactories.clear();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the class ID.
///
INT32 MgPrintLayoutServiceBase::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes this object.
///
void MgPrintLayoutServiceBase::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Realizes a print layout element from its XML definition string.
///
MgPrintLayoutElementBase* MgPrintLayoutServiceBase::RealizePrintLayoutElement(
    MgResourceService* resourceService,
    CREFSTRING resourceXml)
{
    assert(NULL != resourceService && !resourceXml.empty());
    Ptr<MgPrintLayoutElementBase> printLayoutElement;
    
    // Parse the resource XML.
    string strXml = MgUtil::WideCharToMultiByte(resourceXml);
    MdfParser::SAX2Parser parser;
    parser.ParseString(strXml.c_str(), strXml.length());

    if (parser.GetSucceeded())
    {
        // Attempt to get a pointer to the print layout element definition
        std::auto_ptr<PrintLayoutElementDefinition> elementDef(parser.DetachPrintLayoutElementDefinition());
        if (elementDef.get() != NULL)
        {
            STRING defType = elementDef->GetType();
            MgPrintLayoutElementFactoryMap::iterator pIter = m_printLayoutElementFactories.find(defType);
            if (m_printLayoutElementFactories.end() != pIter)
            {
                MgPrintLayoutElementFactoryBase* pElementFactory = pIter->second;
                printLayoutElement = pElementFactory->CreateObject();
                printLayoutElement->PopulateFromResource(elementDef.get());
            }
        }
    }

    return printLayoutElement.Detach();
}

MgPrintLayoutBase* MgPrintLayoutServiceBase::CreatePrintLayout(
        MgResourceService* resourceService,
        MgResourceIdentifier* resId)
{
    if (NULL == resourceService || NULL == resId)
    {
        throw new MgNullArgumentException(L"MgPrintLayoutServiceBase.CreatePrintLayout",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else if (!resId->IsResourceTypeOf(MgResourceType::PrintLayoutDefinition))
    {
        throw new MgInvalidResourceTypeException(L"MgPrintLayoutServiceBase.CreatePrintLayout",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgByteReader> reader = resourceService->GetResourceContent(resId);
    STRING strXml = reader->ToString();

    return RealizePrintLayout(resourceService, strXml);
}

MgPrintLayoutElementBase* MgPrintLayoutServiceBase::CreatePrintLayoutElement(
        MgResourceService* resourceService,
        MgResourceIdentifier* resId)
{
    if (NULL == resourceService || NULL == resId)
    {
        throw new MgNullArgumentException(L"MgPrintLayoutServiceBase.CreatePrintLayoutElement",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else if (!resId->IsResourceTypeOf(MgResourceType::PrintLayoutElementDefinition))
    {
        throw new MgInvalidResourceTypeException(L"MgPrintLayoutServiceBase.CreatePrintLayoutElement",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgByteReader> reader = resourceService->GetResourceContent(resId);
    STRING strXml = reader->ToString();

    return RealizePrintLayoutElement(resourceService, strXml);
}


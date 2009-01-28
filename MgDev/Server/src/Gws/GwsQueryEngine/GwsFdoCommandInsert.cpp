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

///////////////////////////////////////////////////////////////////////////////
//
// Includes
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GwsQueryEngineImp.h"


//////////////////////////////////////////////////////////////////////////////
//
// class CGwsFdoInsertCommand
//
///////////////////////////////////////////////////////////////////////////////

CGwsFdoInsertCommand::CGwsFdoInsertCommand (
    FdoIConnection           *session,
    const GWSQualifiedName   &classname
)
:CGwsFdoCommand (session, classname)
{
}


///////////////////////////////////////////////////////////////////////////////
CGwsFdoInsertCommand::~CGwsFdoInsertCommand ()
{
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus  CGwsFdoInsertCommand::Init (const wchar_t* pFDOCommandClass /*NULL*/)
{
    EGwsStatus fdoes = eGwsOk;

    try {
        eGwsOkThrow (CGwsFdoCommand::Init ());

        m_pCommand = m_connection->CreateCommand(FdoCommandType_Insert);
        assert (m_pCommand != NULL);

        // Set class name
        WSTR name;
        if(pFDOCommandClass)
            name = pFDOCommandClass;
        else
            name = QualifiedClassName();
        ((FdoIInsert *)m_pCommand.p)->SetFeatureClassName(name.c_str());

    } catch(FdoException *e) {
        PushFdoException (eGwsFailedToPrepareCommand, e);
        e->Release();
        fdoes = eGwsFailedToPrepareCommand;

    } catch(EGwsStatus es) {
        PushStatus (es);
        fdoes = es;
    }
    return fdoes;
}


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoInsertCommand::PrepareInternal ()
{
    PrepareNonKeyProperties ();
    PrepareObjectProperties ();
}


///////////////////////////////////////////////////////////////////////////////
FdoPropertyValueCollection * CGwsFdoInsertCommand::GetPropertyValues ()
{
    if (m_pProperties == NULL) {
        m_pProperties = ((FdoIInsert*)m_pCommand.p)->GetPropertyValues();
    }

    return m_pProperties;
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoInsertCommand::Execute (CGwsMutableFeature & feat)
{
    Clear ();

    EGwsStatus fdoes = eGwsOk;
    if(!m_bIsPrepared) {
        PushStatus (eGwsFdoCommandIsNotPrepared);
        return eGwsFdoCommandIsNotPrepared;
    }

    try
    {
        GetPropertyValues (); // initialize property value collection
        eGwsOkThrow(SetProperties (feat, true)) ;

        FdoPtr<FdoIFeatureReader> pReader = ((FdoIInsert*)m_pCommand.p)->Execute();

        //!FDO may return null pointer even if the command succeed. So remove the following assert.
        //assert (pReader != NULL);

        // Read assigned Id
        if( pReader !=  NULL )
        {
            if (pReader->ReadNext()) {

                // Read the key of just inserted record
                //
                CGwsDataValueCollection  *  keyvals = NULL;

                for (FdoInt32 idx = 0; m_identity != NULL && idx < m_identity->GetCount (); idx ++) {
                    FdoPtr<FdoDataPropertyDefinition>  keyprop =
                        m_identity->GetItem (idx);
                    FdoPtr<FdoDataValue>               val =
                        GwsQueryUtils::GetDataPropertyValue (pReader,
                        keyprop->GetDataType (),
                        keyprop->GetName ());
                    if (val == NULL)
                        continue;
                    if (keyvals == NULL)
                        keyvals = (CGwsDataValueCollection *) CGwsDataValueCollection::Create ();
                    keyvals->Add (val);
                    feat.SetValue (keyprop->GetName (),val);
                }

#ifdef OBJECT_PROPERTIES
                //insert object property values that were cached in SetProperties()
                if(!m_objPropNames.empty())
                {
                    //Get object property class instance array that was cached in SetProperties()
                    int numProps = m_objPropNames.size();
                    for( int i = 0; i<numProps; i++)
                    {
                        const std::wstring strPropName = m_objPropNames[i];
                        const std::wstring objectClassName =
                            acmapFdoUtils::constructObjectClassName(aTOw(m_classname.Class()),strPropName.c_str());

                        //get the object property values
                        AcMapFdoFeatureIterator* featIter = NULL;
                        eGwsOkThrow(feature.GetFeature(wTOa(strPropName.c_str()),featIter));
                        assert(featIter != NULL);
                        //insert the primary keys of the parent feature and the object property values
                        eGwsOkThrow(InsertObjectProperties(objectClassName.c_str(), valList, featIter));
                    }
                }
#endif

                // long ver;
                // Read sequence number
                if (!m_revisionprop.empty()) {
                    FdoInt32 revnum = pReader->GetInt32 (m_revisionprop.c_str ());
                    feat.SetRevisionNumber (revnum);
                }
                feat.SetFeatureId (GWSFeatureId (keyvals));
            }
            pReader->Close();
        }

    } catch(FdoException *e) {
        PushFdoException (eGwsFailedToExecuteCommand, e);
        e->Release();
        fdoes = eGwsFailedToExecuteCommand;

    } catch(EGwsStatus es) {
        PushStatus (es);
        fdoes = es;
    }
    ReleasePropertyValues (); // releases property value collection

    return fdoes;
}


///////////////////////////////////////////////////////////////////////////////
#ifdef OBJECTPROPERTY
EGwsStatus CGwsFdoInsertCommand::InsertObjectProperties (const wchar_t* qObjectPropName,
                                                                         AcMapFdoKeyValues& primaryKey,
                                                                         AcMapFdoFeatureIterator* featIter)
{
    EGwsStatus fdoes = eGwsOk;

    MAPFDOTRY
    {
        //Get cached insert command for object property class instances
        ObjectPropCommands::iterator it = m_objPropCommands.find(qObjectPropName);
        //should be there
        assert(it != m_objPropCommands.end());
        CGwsFdoInsertCommand* pInsCommand = dynamic_cast<CGwsFdoInsertCommand*>((*it).second);
        assert(pInsCommand != NULL);
        //iterate and insert each feature
        AcMapFdoFeature* pFeature = NULL;
        while(featIter->GetNextFeature
            (pFeature))
        {
            GisPtr<FdoPropertyValueCollection> pProps = pInsCommand->GetPropertyValues();
            //loop over the identity properties to get the FK name and type
            assert(m_identity->GetCount () == primaryKey.length());
            for (GisInt32 idx = 0; idx < m_identity->GetCount (); idx ++) {
                GisPtr<FdoDataPropertyDefinition>  keyprop;
                keyprop = m_identity->GetItem (idx);
                //Construct hack foreign key name per Enterpise spec (to be fixed in FDO2)
                const std::wstring hackFKName =
                    acmapFdoUtils::constructObjectPropertyFK(aTOw(m_classname.Class()), keyprop->GetName());
                //get the special FK
                GisPtr<FdoPropertyValue> propertyValue = pProps->GetItem(hackFKName.c_str());
                //set it's value with the identity of the newly inserted feature
                GisPtr<FdoDataValue> dataValue = acmapFdoDataConversionUtils::CreateFdoValue(keyprop->GetDataType(), primaryKey[idx]);
                propertyValue->SetValue(dataValue);
            }
            //insert the class
            eGwsOkThrow(pInsCommand->Execute(*pFeature));
        }
    }
    MAPFDOCATCH_ALL(true)
    return fdoes;
}
#endif

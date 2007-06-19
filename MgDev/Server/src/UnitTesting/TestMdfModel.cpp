//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "TestMdfModel.h"
#include "ServiceManager.h"
#include "SAX2Parser.h"

const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestMdfModel, "TestMdfModel");


TestMdfModel::TestMdfModel()
{
    // Initialize service objects.
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();

    m_svcResource = dynamic_cast<MgResourceService*>(
        serviceManager->RequestService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);
}


TestMdfModel::~TestMdfModel()
{
}


void TestMdfModel::setUp()
{
}


void TestMdfModel::tearDown()
{
}


void TestMdfModel::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning MdfModel tests.\n")));

    try
    {
        // set user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        // add symbols / layer definitions for testing MdfModel / MdfParser
        Ptr<MgResourceIdentifier> sdres1 = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestSimpleSymbol.SymbolDefinition");
        Ptr<MgByteSource> sdsrc1 = new MgByteSource(L"../UnitTestFiles/MdfTestSimpleSymbol.sd", false);
        Ptr<MgByteReader> sdrdr1 = sdsrc1->GetReader();
        m_svcResource->SetResource(sdres1, sdrdr1, NULL);

        Ptr<MgResourceIdentifier> sdres2 = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestCompoundSymbol.SymbolDefinition");
        Ptr<MgByteSource> sdsrc2 = new MgByteSource(L"../UnitTestFiles/MdfTestCompoundSymbol.sd", false);
        Ptr<MgByteReader> sdrdr2 = sdsrc2->GetReader();
        m_svcResource->SetResource(sdres2, sdrdr2, NULL);

        Ptr<MgResourceIdentifier> ldfres3 = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestCompTypeStyle.LayerDefinition");
        Ptr<MgByteSource> ldfsrc3 = new MgByteSource(L"../UnitTestFiles/MdfTestCompTypeStyle.ldf", false);
        Ptr<MgByteReader> ldfrdr3 = ldfsrc3->GetReader();
        m_svcResource->SetResource(ldfres3, ldfrdr3, NULL);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestMdfModel::TestEnd()
{
    try
    {
        // set user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        // delete symbols / layer definitions for testing MdfModel / MdfParser
        Ptr<MgResourceIdentifier> sdres1 = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestSimpleSymbol.SymbolDefinition");
        m_svcResource->DeleteResource(sdres1);
        Ptr<MgResourceIdentifier> sdres2 = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestCompoundSymbol.SymbolDefinition");
        m_svcResource->DeleteResource(sdres2);
        Ptr<MgResourceIdentifier> ldfres3 = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestCompTypeStyle.LayerDefinition");
        m_svcResource->DeleteResource(ldfres3);
    }
    catch(MgFileIoException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nMgFileIoException - Possible file permission error.\nError: %W\n"), message.c_str()));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        message += e->GetStackTrace(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }

    ACE_DEBUG((LM_INFO, ACE_TEXT("\nMdfModel tests completed.\n\n")));
}


void TestMdfModel::TestCase_Symbology()
{
    try
    {
        Version symbolDefVersion(1, 0, 0);
        Version layerDefVersionOld(1, 0, 0);
        Version layerDefVersionNew(1, 1, 0);
        MdfParser::SAX2Parser parser;

        // ------------------------------------------------------
        // process symbol #1 - a simple symbol definition
        // ------------------------------------------------------

        // create a scope so temp files are unlocked
        {
            Ptr<MgResourceIdentifier> sdres = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestSimpleSymbol.SymbolDefinition");

            // parse the symbol - this exercises MdfParser deserialization
            Ptr<MgByteReader> rdr = m_svcResource->GetResourceContent(sdres);
            Ptr<MgByteSink> sink = new MgByteSink(rdr);
            Ptr<MgByte> bytes = sink->ToBuffer();
            CPPUNIT_ASSERT(bytes->GetLength() > 0);

            parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());
            CPPUNIT_ASSERT(parser.GetSucceeded());

            // write the file - this exercises MdfParser serialization
            auto_ptr<SymbolDefinition> symbolDef1(parser.DetachSymbolDefinition());
            CPPUNIT_ASSERT(symbolDef1.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestSimpleSymbolCopy1.sd", symbolDef1.get(), &symbolDefVersion);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestSimpleSymbolCopy1.sd"));

            // parse and resave the newly written file
            Ptr<MgByteSource> src1 = new MgByteSource(L"../UnitTestFiles/MdfTestSimpleSymbolCopy1.sd");
            Ptr<MgByteReader> rdr1 = src1->GetReader();
            Ptr<MgByteSink> sink1 = new MgByteSink(rdr1);
            Ptr<MgByte> bytes1 = sink1->ToBuffer();
            parser.ParseString((const char*)bytes1->Bytes(), bytes1->GetLength());
            auto_ptr<SymbolDefinition> symbolDef2(parser.DetachSymbolDefinition());
            parser.WriteToFile("../UnitTestFiles/MdfTestSimpleSymbolCopy2.sd", symbolDef2.get(), &symbolDefVersion);

            // compare the two files
            Ptr<MgByteSource> src2 = new MgByteSource(L"../UnitTestFiles/MdfTestSimpleSymbolCopy2.sd");
            Ptr<MgByteReader> rdr2 = src2->GetReader();
            Ptr<MgByteSink> sink2 = new MgByteSink(rdr2);
            Ptr<MgByte> bytes2 = sink2->ToBuffer();
            CPPUNIT_ASSERT(bytes1->GetLength() == bytes2->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1->Bytes(), bytes2->Bytes(), bytes1->GetLength()) == 0);

            // currently no need for a versioning test for symbol definitions
        }

        // delete the files
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestSimpleSymbolCopy1.sd", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestSimpleSymbolCopy2.sd", true);

        // ------------------------------------------------------
        // process symbol #2 - a compound symbol definition
        // ------------------------------------------------------

        // create a scope so temp files are unlocked
        {
            Ptr<MgResourceIdentifier> sdres = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestCompoundSymbol.SymbolDefinition");

            // parse the symbol - this exercises MdfParser deserialization
            Ptr<MgByteReader> rdr = m_svcResource->GetResourceContent(sdres);
            Ptr<MgByteSink> sink = new MgByteSink(rdr);
            Ptr<MgByte> bytes = sink->ToBuffer();
            CPPUNIT_ASSERT(bytes->GetLength() > 0);

            parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());
            CPPUNIT_ASSERT(parser.GetSucceeded());

            // write the file - this exercises MdfParser serialization
            auto_ptr<SymbolDefinition> symbolDef1(parser.DetachSymbolDefinition());
            CPPUNIT_ASSERT(symbolDef1.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestCompoundSymbolCopy1.sd", symbolDef1.get(), &symbolDefVersion);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestCompoundSymbolCopy1.sd"));

            // parse and resave the newly written file
            Ptr<MgByteSource> src1 = new MgByteSource(L"../UnitTestFiles/MdfTestCompoundSymbolCopy1.sd");
            Ptr<MgByteReader> rdr1 = src1->GetReader();
            Ptr<MgByteSink> sink1 = new MgByteSink(rdr1);
            Ptr<MgByte> bytes1 = sink1->ToBuffer();
            parser.ParseString((const char*)bytes1->Bytes(), bytes1->GetLength());
            auto_ptr<SymbolDefinition> symbolDef2(parser.DetachSymbolDefinition());
            parser.WriteToFile("../UnitTestFiles/MdfTestCompoundSymbolCopy2.sd", symbolDef2.get(), &symbolDefVersion);

            // compare the two files
            Ptr<MgByteSource> src2 = new MgByteSource(L"../UnitTestFiles/MdfTestCompoundSymbolCopy2.sd");
            Ptr<MgByteReader> rdr2 = src2->GetReader();
            Ptr<MgByteSink> sink2 = new MgByteSink(rdr2);
            Ptr<MgByte> bytes2 = sink2->ToBuffer();
            CPPUNIT_ASSERT(bytes1->GetLength() == bytes2->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1->Bytes(), bytes2->Bytes(), bytes1->GetLength()) == 0);

            // currently no need for a versioning test for symbol definitions
        }

        // delete the files
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestCompoundSymbolCopy1.sd", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestCompoundSymbolCopy2.sd", true);

        // ------------------------------------------------------
        // process layer definition with composite type style
        // ------------------------------------------------------

        // create a scope so temp files are unlocked
        {
            Ptr<MgResourceIdentifier> ldfres = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestCompTypeStyle.LayerDefinition");

            // parse the LDF - this exercises MdfParser deserialization
            Ptr<MgByteReader> rdr = m_svcResource->GetResourceContent(ldfres);
            Ptr<MgByteSink> sink = new MgByteSink(rdr);
            Ptr<MgByte> bytes = sink->ToBuffer();
            CPPUNIT_ASSERT(bytes->GetLength() > 0);

            parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());
            CPPUNIT_ASSERT(parser.GetSucceeded());

            // write the file - this exercises MdfParser serialization
            auto_ptr<VectorLayerDefinition> layerDef1(parser.DetachVectorLayerDefinition());
            CPPUNIT_ASSERT(layerDef1.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestCompTypeStyleCopy1.ldf", NULL, layerDef1.get(), NULL, NULL, &layerDefVersionNew);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestCompTypeStyleCopy1.ldf"));

            // parse and resave the newly written file
            Ptr<MgByteSource> src1 = new MgByteSource(L"../UnitTestFiles/MdfTestCompTypeStyleCopy1.ldf");
            Ptr<MgByteReader> rdr1 = src1->GetReader();
            Ptr<MgByteSink> sink1 = new MgByteSink(rdr1);
            Ptr<MgByte> bytes1 = sink1->ToBuffer();
            parser.ParseString((const char*)bytes1->Bytes(), bytes1->GetLength());
            auto_ptr<VectorLayerDefinition> layerDef2(parser.DetachVectorLayerDefinition());
            parser.WriteToFile("../UnitTestFiles/MdfTestCompTypeStyleCopy2.ldf", NULL, layerDef2.get(), NULL, NULL, &layerDefVersionNew);

            // compare the two files
            Ptr<MgByteSource> src2 = new MgByteSource(L"../UnitTestFiles/MdfTestCompTypeStyleCopy2.ldf");
            Ptr<MgByteReader> rdr2 = src2->GetReader();
            Ptr<MgByteSink> sink2 = new MgByteSink(rdr2);
            Ptr<MgByte> bytes2 = sink2->ToBuffer();
            CPPUNIT_ASSERT(bytes1->GetLength() == bytes2->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1->Bytes(), bytes2->Bytes(), bytes1->GetLength()) == 0);

            // versioning test - save the layer using version 1.0.0
            parser.WriteToFile("../UnitTestFiles/MdfTestCompTypeStyleCopy3.ldf", NULL, layerDef2.get(), NULL, NULL, &layerDefVersionOld);
        }

        // delete the files
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestCompTypeStyleCopy1.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestCompTypeStyleCopy2.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestCompTypeStyleCopy3.ldf", true);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestMdfModel::TestCase_ExtendedData()
{
}

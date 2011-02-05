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

        Ptr<MgResourceIdentifier> ldfres3 = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestTypeStyles.LayerDefinition");
        Ptr<MgByteSource> ldfsrc3 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles.ldf", false);
        Ptr<MgByteReader> ldfrdr3 = ldfsrc3->GetReader();
        m_svcResource->SetResource(ldfres3, ldfrdr3, NULL);

        Ptr<MgResourceIdentifier> mdfres4 = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestMap.MapDefinition");
        Ptr<MgByteSource> mdfsrc4 = new MgByteSource(L"../UnitTestFiles/MdfTestMap.mdf", false);
        Ptr<MgByteReader> mdfrdr4 = mdfsrc4->GetReader();
        m_svcResource->SetResource(mdfres4, mdfrdr4, NULL);
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
        Ptr<MgResourceIdentifier> ldfres3 = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestTypeStyles.LayerDefinition");
        m_svcResource->DeleteResource(ldfres3);
        Ptr<MgResourceIdentifier> mdfres4 = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestMap.MapDefinition");
        m_svcResource->DeleteResource(mdfres4);
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


void TestMdfModel::TestCase_Serialization()
{
    try
    {
        MdfParser::SAX2Parser parser;

        Ptr<MgResourceIdentifier> sdresV = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestDummy.SymbolDefinition");
        Ptr<MgResourceIdentifier> ldfresV = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestDummy.LayerDefinition");
        Ptr<MgResourceIdentifier> mdfresV = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestDummy.MapDefinition");

        // ------------------------------------------------------
        // process symbol #1 - a simple symbol definition
        // ------------------------------------------------------

        // iterate over the symbol definition schema versions
        for (int minorVersion=1; minorVersion>=0; --minorVersion)
        {
            Version symbolDefVersion(1, minorVersion, 0);

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

            parser.WriteToFile("../UnitTestFiles/MdfTestSimpleSymbol_Copy1.sd", symbolDef1.get(), &symbolDefVersion);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestSimpleSymbol_Copy1.sd"));

            // parse and resave the newly written file
            Ptr<MgByteSource> src1 = new MgByteSource(L"../UnitTestFiles/MdfTestSimpleSymbol_Copy1.sd");
            Ptr<MgByteReader> rdr1 = src1->GetReader();
            Ptr<MgByteSink> sink1 = new MgByteSink(rdr1);
            Ptr<MgByte> bytes1 = sink1->ToBuffer();
            parser.ParseString((const char*)bytes1->Bytes(), bytes1->GetLength());
            auto_ptr<SymbolDefinition> symbolDef2(parser.DetachSymbolDefinition());
            CPPUNIT_ASSERT(symbolDef2.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestSimpleSymbol_Copy2.sd", symbolDef2.get(), &symbolDefVersion);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestSimpleSymbol_Copy2.sd"));

            // compare the two files
            Ptr<MgByteSource> src2 = new MgByteSource(L"../UnitTestFiles/MdfTestSimpleSymbol_Copy2.sd");
            Ptr<MgByteReader> rdr2 = src2->GetReader();
            Ptr<MgByteSink> sink2 = new MgByteSink(rdr2);
            Ptr<MgByte> bytes2 = sink2->ToBuffer();
            CPPUNIT_ASSERT(bytes1->GetLength() == bytes2->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1->Bytes(), bytes2->Bytes(), bytes1->GetLength()) == 0);

            // save the new resource to the repository to validate the XML
            m_svcResource->SetResource(sdresV, rdr1, NULL);
            m_svcResource->DeleteResource(sdresV);
        }

        // delete the files
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestSimpleSymbol_Copy1.sd", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestSimpleSymbol_Copy2.sd", true);

        // ------------------------------------------------------
        // process symbol #2 - a compound symbol definition
        // ------------------------------------------------------

        // iterate over the symbol definition schema versions
        for (int minorVersion=1; minorVersion>=0; --minorVersion)
        {
            Version symbolDefVersion(1, minorVersion, 0);

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

            parser.WriteToFile("../UnitTestFiles/MdfTestCompoundSymbol_Copy1.sd", symbolDef1.get(), &symbolDefVersion);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestCompoundSymbol_Copy1.sd"));

            // parse and resave the newly written file
            Ptr<MgByteSource> src1 = new MgByteSource(L"../UnitTestFiles/MdfTestCompoundSymbol_Copy1.sd");
            Ptr<MgByteReader> rdr1 = src1->GetReader();
            Ptr<MgByteSink> sink1 = new MgByteSink(rdr1);
            Ptr<MgByte> bytes1 = sink1->ToBuffer();
            parser.ParseString((const char*)bytes1->Bytes(), bytes1->GetLength());
            auto_ptr<SymbolDefinition> symbolDef2(parser.DetachSymbolDefinition());
            CPPUNIT_ASSERT(symbolDef2.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestCompoundSymbol_Copy2.sd", symbolDef2.get(), &symbolDefVersion);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestCompoundSymbol_Copy2.sd"));

            // compare the two files
            Ptr<MgByteSource> src2 = new MgByteSource(L"../UnitTestFiles/MdfTestCompoundSymbol_Copy2.sd");
            Ptr<MgByteReader> rdr2 = src2->GetReader();
            Ptr<MgByteSink> sink2 = new MgByteSink(rdr2);
            Ptr<MgByte> bytes2 = sink2->ToBuffer();
            CPPUNIT_ASSERT(bytes1->GetLength() == bytes2->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1->Bytes(), bytes2->Bytes(), bytes1->GetLength()) == 0);

            // save the new resource to the repository to validate the XML
            m_svcResource->SetResource(sdresV, rdr1, NULL);
            m_svcResource->DeleteResource(sdresV);
        }

        // delete the files
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestCompoundSymbol_Copy1.sd", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestCompoundSymbol_Copy2.sd", true);

        // ------------------------------------------------------
        // process layer definition with type styles
        // ------------------------------------------------------

        // iterate over the layer definition schema versions
        const int LAYER_SCHEMA_COUNT = 4 + 1;      //4 for 1.x.x and 1 for 2.x.x
        std::vector<Version> layerVersions(LAYER_SCHEMA_COUNT);
        //layer 1.0.0 - MapGuide 2007
        layerVersions[0].SetMajor(1);
        layerVersions[0].SetMinor(0);
        layerVersions[0].SetRevision(0);
        //layer 1.1.0 - MapGuide 2008
        layerVersions[1].SetMajor(1);
        layerVersions[1].SetMinor(1);
        layerVersions[1].SetRevision(0);
        //layer 1.2.0 - MapGuide 2009
        layerVersions[2].SetMajor(1);
        layerVersions[2].SetMinor(2);
        layerVersions[2].SetRevision(0);
        //layer 1.3.0 - MapGuide 2010
        layerVersions[3].SetMajor(1);
        layerVersions[3].SetMinor(3);
        layerVersions[3].SetRevision(0);
        //layer 2.3.0 - MapGuide 2012
        layerVersions[4].SetMajor(2);
        layerVersions[4].SetMinor(3);
        layerVersions[4].SetRevision(0);

        for (vector<Version>::size_type layerVersionIndex = 0; layerVersionIndex != layerVersions.size(); ++layerVersionIndex)
        {
            Version &layerDefVersion = layerVersions[layerVersionIndex];

            Ptr<MgResourceIdentifier> ldfres = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestTypeStyles.LayerDefinition");

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

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_Copy1.ldf", NULL, layerDef1.get(), NULL, NULL, NULL, NULL, &layerDefVersion);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_Copy1.ldf"));

            // parse and resave the newly written file
            Ptr<MgByteSource> src1 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_Copy1.ldf");
            Ptr<MgByteReader> rdr1 = src1->GetReader();
            Ptr<MgByteSink> sink1 = new MgByteSink(rdr1);
            Ptr<MgByte> bytes1 = sink1->ToBuffer();
            parser.ParseString((const char*)bytes1->Bytes(), bytes1->GetLength());
            auto_ptr<VectorLayerDefinition> layerDef2(parser.DetachVectorLayerDefinition());
            CPPUNIT_ASSERT(layerDef2.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_Copy2.ldf", NULL, layerDef2.get(), NULL, NULL, NULL, NULL, &layerDefVersion);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_Copy2.ldf"));

            // compare the two files
            Ptr<MgByteSource> src2 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_Copy2.ldf");
            Ptr<MgByteReader> rdr2 = src2->GetReader();
            Ptr<MgByteSink> sink2 = new MgByteSink(rdr2);
            Ptr<MgByte> bytes2 = sink2->ToBuffer();

            CPPUNIT_ASSERT(bytes1->GetLength() == bytes2->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1->Bytes(), bytes2->Bytes(), bytes1->GetLength()) == 0);

            // save the new resource to the repository to validate the XML
            m_svcResource->SetResource(ldfresV, rdr1, NULL);
            m_svcResource->DeleteResource(ldfresV);
        }

        // delete the files
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_Copy1.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_Copy2.ldf", true);

        
        // ------------------------------------------------------
        // process map definition with type styles
        // ------------------------------------------------------

        // iterate over the map definition schema versions
        const int MAP_SCHEMA_COUNT = 1 + 1;      //1 for 1.x.x and 1 for 2.x.x
        std::vector<Version> mapVersions(MAP_SCHEMA_COUNT);
        //map 1.0.0 - MapGuide before 2012
        mapVersions[0].SetMajor(1);
        mapVersions[0].SetMinor(0);
        mapVersions[0].SetRevision(0);
        //map 2.3.0 - MapGuide 2012
        mapVersions[1].SetMajor(2);
        mapVersions[1].SetMinor(3);
        mapVersions[1].SetRevision(0);

        for (vector<Version>::size_type mapVersionIndex = 0; mapVersionIndex != mapVersions.size(); ++mapVersionIndex)
        {
            Version &mapDefVersion = mapVersions[mapVersionIndex];

            Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestMap.MapDefinition");

            // parse the MDF - this exercises MdfParser deserialization
            Ptr<MgByteReader> rdr = m_svcResource->GetResourceContent(mdfres);
            Ptr<MgByteSink> sink = new MgByteSink(rdr);
            Ptr<MgByte> bytes = sink->ToBuffer();
            CPPUNIT_ASSERT(bytes->GetLength() > 0);

            parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());
            CPPUNIT_ASSERT(parser.GetSucceeded());

            // write the file - this exercises MdfParser serialization
            auto_ptr<MapDefinition> mapDef1(parser.DetachMapDefinition());
            CPPUNIT_ASSERT(mapDef1.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestMap_Copy1.mdf", mapDef1.get(), NULL, NULL, NULL, NULL, NULL, &mapDefVersion);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestMap_Copy1.mdf"));

            // parse and resave the newly written file
            Ptr<MgByteSource> src1 = new MgByteSource(L"../UnitTestFiles/MdfTestMap_Copy1.mdf");
            Ptr<MgByteReader> rdr1 = src1->GetReader();
            Ptr<MgByteSink> sink1 = new MgByteSink(rdr1);
            Ptr<MgByte> bytes1 = sink1->ToBuffer();
            parser.ParseString((const char*)bytes1->Bytes(), bytes1->GetLength());
            auto_ptr<MapDefinition> mapDef2(parser.DetachMapDefinition());
            CPPUNIT_ASSERT(mapDef2.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestMap_Copy2.mdf", mapDef2.get(), NULL, NULL, NULL, NULL, NULL, &mapDefVersion);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestMap_Copy2.mdf"));

            // compare the two files
            Ptr<MgByteSource> src2 = new MgByteSource(L"../UnitTestFiles/MdfTestMap_Copy2.mdf");
            Ptr<MgByteReader> rdr2 = src2->GetReader();
            Ptr<MgByteSink> sink2 = new MgByteSink(rdr2);
            Ptr<MgByte> bytes2 = sink2->ToBuffer();

            CPPUNIT_ASSERT(bytes1->GetLength() == bytes2->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1->Bytes(), bytes2->Bytes(), bytes1->GetLength()) == 0);

            // save the new resource to the repository to validate the XML
            m_svcResource->SetResource(mdfresV, rdr1, NULL);
            m_svcResource->DeleteResource(mdfresV);
        }

        // delete the files
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestMap_Copy1.mdf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestMap_Copy2.mdf", true);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestMdfModel::TestCase_Versioning()
{
    try
    {
        Version layerDefVersion09(0, 9, 0); // MapGuide 2006
        Version layerDefVersion10(1, 0, 0); // MapGuide 2007
        Version layerDefVersion11(1, 1, 0); // MapGuide 2008
        Version layerDefVersion12(1, 2, 0); // MapGuide 2009
        Version layerDefVersion13(1, 3, 0); // MapGuide 2010
        Version layerDefVersion23(2, 3, 0); // MapGuide 2012
        Version symbolDefVersion10(1, 0, 0); // MapGuide 2008
        Version symbolDefVersion11(1, 1, 0); // MapGuide 2009
        MdfParser::SAX2Parser parser;

        Ptr<MgResourceIdentifier> sdresV = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestDummy.SymbolDefinition");
        Ptr<MgResourceIdentifier> ldfresV = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestDummy.LayerDefinition");

        // ------------------------------------------------------
        // process symbol - a compound symbol definition
        // ------------------------------------------------------

        // create a scope so temp files are unlocked
        {
            Ptr<MgResourceIdentifier> sdres = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestCompoundSymbol.SymbolDefinition");

            // parse the symbol definition - this exercises MdfParser deserialization
            Ptr<MgByteReader> rdr = m_svcResource->GetResourceContent(sdres);
            Ptr<MgByteSink> sink = new MgByteSink(rdr);
            Ptr<MgByte> bytes = sink->ToBuffer();
            CPPUNIT_ASSERT(bytes->GetLength() > 0);

            parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());
            CPPUNIT_ASSERT(parser.GetSucceeded());

            // write the file using each version - this exercises MdfParser serialization
            auto_ptr<SymbolDefinition> symbolDef1(parser.DetachSymbolDefinition());
            CPPUNIT_ASSERT(symbolDef1.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestCompoundSymbol_v10_Copy1.sd", symbolDef1.get(), &symbolDefVersion10);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestCompoundSymbol_v10_Copy1.sd"));

            parser.WriteToFile("../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy1.sd", symbolDef1.get(), &symbolDefVersion11);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy1.sd"));

            // parse and resave the newly written files
            Ptr<MgByteSource> src1_10 = new MgByteSource(L"../UnitTestFiles/MdfTestCompoundSymbol_v10_Copy1.sd");
            Ptr<MgByteReader> rdr1_10 = src1_10->GetReader();
            Ptr<MgByteSink> sink1_10 = new MgByteSink(rdr1_10);
            Ptr<MgByte> bytes1_10 = sink1_10->ToBuffer();
            parser.ParseString((const char*)bytes1_10->Bytes(), bytes1_10->GetLength());
            auto_ptr<SymbolDefinition> symbolDef2_10(parser.DetachSymbolDefinition());
            CPPUNIT_ASSERT(symbolDef2_10.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestCompoundSymbol_v10_Copy2.sd", symbolDef2_10.get(), &symbolDefVersion10);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestCompoundSymbol_v10_Copy2.sd"));

            Ptr<MgByteSource> src1_11 = new MgByteSource(L"../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy1.sd");
            Ptr<MgByteReader> rdr1_11 = src1_11->GetReader();
            Ptr<MgByteSink> sink1_11 = new MgByteSink(rdr1_11);
            Ptr<MgByte> bytes1_11 = sink1_11->ToBuffer();
            parser.ParseString((const char*)bytes1_11->Bytes(), bytes1_11->GetLength());
            auto_ptr<SymbolDefinition> symbolDef2_11(parser.DetachSymbolDefinition());
            CPPUNIT_ASSERT(symbolDef2_11.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy2.sd", symbolDef2_11.get(), &symbolDefVersion11);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy2.sd"));

            // compare the files
            Ptr<MgByteSource> src2_10 = new MgByteSource(L"../UnitTestFiles/MdfTestCompoundSymbol_v10_Copy2.sd");
            Ptr<MgByteReader> rdr2_10 = src2_10->GetReader();
            Ptr<MgByteSink> sink2_10 = new MgByteSink(rdr2_10);
            Ptr<MgByte> bytes2_10 = sink2_10->ToBuffer();
            CPPUNIT_ASSERT(bytes1_10->GetLength() == bytes2_10->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1_10->Bytes(), bytes2_10->Bytes(), bytes1_10->GetLength()) == 0);

            Ptr<MgByteSource> src2_11 = new MgByteSource(L"../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy2.sd");
            Ptr<MgByteReader> rdr2_11 = src2_11->GetReader();
            Ptr<MgByteSink> sink2_11 = new MgByteSink(rdr2_11);
            Ptr<MgByte> bytes2_11 = sink2_11->ToBuffer();
            CPPUNIT_ASSERT(bytes1_11->GetLength() == bytes2_11->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1_11->Bytes(), bytes2_11->Bytes(), bytes1_11->GetLength()) == 0);

            // verify extended data support is working...
            //   - symbolDef2_10 was loaded from XML containing extended data
            //   - symbolDef2_11 was loaded from XML containing no extended data
            // the data in these object models should be the same

            parser.WriteToFile("../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy3a.sd", symbolDef2_10.get(), &symbolDefVersion11);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy3a.sd"));

            parser.WriteToFile("../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy3b.sd", symbolDef2_11.get(), &symbolDefVersion11);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy3b.sd"));

            Ptr<MgByteSource> src3a_11 = new MgByteSource(L"../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy3a.sd");
            Ptr<MgByteReader> rdr3a_11 = src3a_11->GetReader();
            Ptr<MgByteSink> sink3a_11 = new MgByteSink(rdr3a_11);
            Ptr<MgByte> bytes3a_11 = sink3a_11->ToBuffer();

            Ptr<MgByteSource> src3b_11 = new MgByteSource(L"../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy3b.sd");
            Ptr<MgByteReader> rdr3b_11 = src3b_11->GetReader();
            Ptr<MgByteSink> sink3b_11 = new MgByteSink(rdr3b_11);
            Ptr<MgByte> bytes3b_11 = sink3b_11->ToBuffer();

            CPPUNIT_ASSERT(bytes3a_11->GetLength() == bytes3b_11->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes3a_11->Bytes(), bytes3b_11->Bytes(), bytes3a_11->GetLength()) == 0);

            // save the new resources to the repository to validate the XML
            m_svcResource->SetResource(sdresV, rdr1_10, NULL);
            m_svcResource->DeleteResource(sdresV);
            m_svcResource->SetResource(sdresV, rdr1_11, NULL);
            m_svcResource->DeleteResource(sdresV);
            m_svcResource->SetResource(sdresV, rdr3a_11, NULL);
            m_svcResource->DeleteResource(sdresV);
        }

        // delete the files
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestCompoundSymbol_v10_Copy1.sd", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestCompoundSymbol_v10_Copy2.sd", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy1.sd", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy2.sd", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy3a.sd", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestCompoundSymbol_v11_Copy3b.sd", true);

        // ------------------------------------------------------
        // process layer definition with type styles
        // ------------------------------------------------------

        // create a scope so temp files are unlocked
        {
            Ptr<MgResourceIdentifier> ldfres = new MgResourceIdentifier(L"Library://UnitTests/MdfModel/MdfTestTypeStyles.LayerDefinition");

            // parse the LDF - this exercises MdfParser deserialization
            Ptr<MgByteReader> rdr = m_svcResource->GetResourceContent(ldfres);
            Ptr<MgByteSink> sink = new MgByteSink(rdr);
            Ptr<MgByte> bytes = sink->ToBuffer();
            CPPUNIT_ASSERT(bytes->GetLength() > 0);

            parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());
            CPPUNIT_ASSERT(parser.GetSucceeded());

            // write the file using each version - this exercises MdfParser serialization
            auto_ptr<VectorLayerDefinition> layerDef1(parser.DetachVectorLayerDefinition());
            CPPUNIT_ASSERT(layerDef1.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v09_Copy1.ldf", NULL, layerDef1.get(), NULL, NULL, NULL, NULL, &layerDefVersion09);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v09_Copy1.ldf"));

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v10_Copy1.ldf", NULL, layerDef1.get(), NULL, NULL, NULL, NULL, &layerDefVersion10);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v10_Copy1.ldf"));

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v11_Copy1.ldf", NULL, layerDef1.get(), NULL, NULL, NULL, NULL, &layerDefVersion11);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v11_Copy1.ldf"));

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v12_Copy1.ldf", NULL, layerDef1.get(), NULL, NULL, NULL, NULL, &layerDefVersion12);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v12_Copy1.ldf"));

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v13_Copy1.ldf", NULL, layerDef1.get(), NULL, NULL, NULL, NULL, &layerDefVersion13);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v13_Copy1.ldf"));

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v23_Copy1.ldf", NULL, layerDef1.get(), NULL, NULL, NULL, NULL, &layerDefVersion23);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy1.ldf"));

            // parse and resave the newly written files
            Ptr<MgByteSource> src1_09 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v09_Copy1.ldf");
            Ptr<MgByteReader> rdr1_09 = src1_09->GetReader();
            Ptr<MgByteSink> sink1_09 = new MgByteSink(rdr1_09);
            Ptr<MgByte> bytes1_09 = sink1_09->ToBuffer();
            parser.ParseString((const char*)bytes1_09->Bytes(), bytes1_09->GetLength());
            auto_ptr<VectorLayerDefinition> layerDef2_09(parser.DetachVectorLayerDefinition());
            CPPUNIT_ASSERT(layerDef2_09.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v09_Copy2.ldf", NULL, layerDef2_09.get(), NULL, NULL, NULL, NULL, &layerDefVersion09);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v09_Copy2.ldf"));

            Ptr<MgByteSource> src1_10 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v10_Copy1.ldf");
            Ptr<MgByteReader> rdr1_10 = src1_10->GetReader();
            Ptr<MgByteSink> sink1_10 = new MgByteSink(rdr1_10);
            Ptr<MgByte> bytes1_10 = sink1_10->ToBuffer();
            parser.ParseString((const char*)bytes1_10->Bytes(), bytes1_10->GetLength());
            auto_ptr<VectorLayerDefinition> layerDef2_10(parser.DetachVectorLayerDefinition());
            CPPUNIT_ASSERT(layerDef2_10.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v10_Copy2.ldf", NULL, layerDef2_10.get(), NULL, NULL, NULL, NULL, &layerDefVersion10);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v10_Copy2.ldf"));

            Ptr<MgByteSource> src1_11 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v11_Copy1.ldf");
            Ptr<MgByteReader> rdr1_11 = src1_11->GetReader();
            Ptr<MgByteSink> sink1_11 = new MgByteSink(rdr1_11);
            Ptr<MgByte> bytes1_11 = sink1_11->ToBuffer();
            parser.ParseString((const char*)bytes1_11->Bytes(), bytes1_11->GetLength());
            auto_ptr<VectorLayerDefinition> layerDef2_11(parser.DetachVectorLayerDefinition());
            CPPUNIT_ASSERT(layerDef2_11.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v11_Copy2.ldf", NULL, layerDef2_11.get(), NULL, NULL, NULL, NULL, &layerDefVersion11);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v11_Copy2.ldf"));

            Ptr<MgByteSource> src1_12 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v12_Copy1.ldf");
            Ptr<MgByteReader> rdr1_12 = src1_12->GetReader();
            Ptr<MgByteSink> sink1_12 = new MgByteSink(rdr1_12);
            Ptr<MgByte> bytes1_12 = sink1_12->ToBuffer();
            parser.ParseString((const char*)bytes1_12->Bytes(), bytes1_12->GetLength());
            auto_ptr<VectorLayerDefinition> layerDef2_12(parser.DetachVectorLayerDefinition());
            CPPUNIT_ASSERT(layerDef2_12.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v12_Copy2.ldf", NULL, layerDef2_12.get(), NULL, NULL, NULL, NULL, &layerDefVersion12);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v12_Copy2.ldf"));

            Ptr<MgByteSource> src1_13 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v13_Copy1.ldf");
            Ptr<MgByteReader> rdr1_13 = src1_13->GetReader();
            Ptr<MgByteSink> sink1_13 = new MgByteSink(rdr1_13);
            Ptr<MgByte> bytes1_13 = sink1_13->ToBuffer();
            parser.ParseString((const char*)bytes1_13->Bytes(), bytes1_13->GetLength());
            auto_ptr<VectorLayerDefinition> layerDef2_13(parser.DetachVectorLayerDefinition());
            CPPUNIT_ASSERT(layerDef2_13.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v13_Copy2.ldf", NULL, layerDef2_13.get(), NULL, NULL, NULL, NULL, &layerDefVersion13);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v13_Copy2.ldf"));

            Ptr<MgByteSource> src1_23 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy1.ldf");
            Ptr<MgByteReader> rdr1_23 = src1_23->GetReader();
            Ptr<MgByteSink> sink1_23 = new MgByteSink(rdr1_23);
            Ptr<MgByte> bytes1_23 = sink1_23->ToBuffer();
            parser.ParseString((const char*)bytes1_23->Bytes(), bytes1_23->GetLength());
            auto_ptr<VectorLayerDefinition> layerDef2_23(parser.DetachVectorLayerDefinition());
            CPPUNIT_ASSERT(layerDef2_23.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v23_Copy2.ldf", NULL, layerDef2_23.get(), NULL, NULL, NULL, NULL, &layerDefVersion23);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy2.ldf"));

            // compare the files
            Ptr<MgByteSource> src2_09 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v09_Copy2.ldf");
            Ptr<MgByteReader> rdr2_09 = src2_09->GetReader();
            Ptr<MgByteSink> sink2_09 = new MgByteSink(rdr2_09);
            Ptr<MgByte> bytes2_09 = sink2_09->ToBuffer();
            CPPUNIT_ASSERT(bytes1_09->GetLength() == bytes2_09->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1_09->Bytes(), bytes2_09->Bytes(), bytes1_09->GetLength()) == 0);

            Ptr<MgByteSource> src2_10 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v10_Copy2.ldf");
            Ptr<MgByteReader> rdr2_10 = src2_10->GetReader();
            Ptr<MgByteSink> sink2_10 = new MgByteSink(rdr2_10);
            Ptr<MgByte> bytes2_10 = sink2_10->ToBuffer();
            CPPUNIT_ASSERT(bytes1_10->GetLength() == bytes2_10->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1_10->Bytes(), bytes2_10->Bytes(), bytes1_10->GetLength()) == 0);

            Ptr<MgByteSource> src2_11 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v11_Copy2.ldf");
            Ptr<MgByteReader> rdr2_11 = src2_11->GetReader();
            Ptr<MgByteSink> sink2_11 = new MgByteSink(rdr2_11);
            Ptr<MgByte> bytes2_11 = sink2_11->ToBuffer();
            CPPUNIT_ASSERT(bytes1_11->GetLength() == bytes2_11->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1_11->Bytes(), bytes2_11->Bytes(), bytes1_11->GetLength()) == 0);

            Ptr<MgByteSource> src2_12 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v12_Copy2.ldf");
            Ptr<MgByteReader> rdr2_12 = src2_12->GetReader();
            Ptr<MgByteSink> sink2_12 = new MgByteSink(rdr2_12);
            Ptr<MgByte> bytes2_12 = sink2_12->ToBuffer();
            CPPUNIT_ASSERT(bytes1_12->GetLength() == bytes2_12->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1_12->Bytes(), bytes2_12->Bytes(), bytes1_12->GetLength()) == 0);

            Ptr<MgByteSource> src2_13 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v13_Copy2.ldf");
            Ptr<MgByteReader> rdr2_13 = src2_13->GetReader();
            Ptr<MgByteSink> sink2_13 = new MgByteSink(rdr2_13);
            Ptr<MgByte> bytes2_13 = sink2_13->ToBuffer();
            CPPUNIT_ASSERT(bytes1_13->GetLength() == bytes2_13->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1_13->Bytes(), bytes2_13->Bytes(), bytes1_13->GetLength()) == 0);

            Ptr<MgByteSource> src2_23 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy2.ldf");
            Ptr<MgByteReader> rdr2_23 = src2_23->GetReader();
            Ptr<MgByteSink> sink2_23 = new MgByteSink(rdr2_23);
            Ptr<MgByte> bytes2_23 = sink2_23->ToBuffer();
            CPPUNIT_ASSERT(bytes1_23->GetLength() == bytes2_23->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1_23->Bytes(), bytes2_23->Bytes(), bytes1_23->GetLength()) == 0);

            // verify extended data support is working...
            //   - layerDef2_10 was loaded from XML containing extended data
            //   - layerDef2_11 was loaded from XML containing extended data
            //   - layerDef2_12 was loaded from XML containing no extended data
            //   - layerDef2_13 was loaded from XML containing no extended data
            //   - layerDef2_23 was loaded from XML containing no extended data
            // the data in these object models should be the same

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v23_Copy3a.ldf", NULL, layerDef2_10.get(), NULL, NULL, NULL, NULL, &layerDefVersion23);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy3a.ldf"));

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v23_Copy3b.ldf", NULL, layerDef2_11.get(), NULL, NULL, NULL, NULL, &layerDefVersion23);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy3b.ldf"));

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v23_Copy3c.ldf", NULL, layerDef2_12.get(), NULL, NULL, NULL, NULL, &layerDefVersion23);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy3c.ldf"));

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v23_Copy3d.ldf", NULL, layerDef2_13.get(), NULL, NULL, NULL, NULL, &layerDefVersion23);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy3d.ldf"));

            parser.WriteToFile("../UnitTestFiles/MdfTestTypeStyles_v23_Copy3e.ldf", NULL, layerDef2_23.get(), NULL, NULL, NULL, NULL, &layerDefVersion23);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy3e.ldf"));

            Ptr<MgByteSource> src3a_23 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy3a.ldf");
            Ptr<MgByteReader> rdr3a_23 = src3a_23->GetReader();
            Ptr<MgByteSink> sink3a_23 = new MgByteSink(rdr3a_23);
            Ptr<MgByte> bytes3a_23 = sink3a_23->ToBuffer();

            Ptr<MgByteSource> src3b_23 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy3b.ldf");
            Ptr<MgByteReader> rdr3b_23 = src3b_23->GetReader();
            Ptr<MgByteSink> sink3b_23 = new MgByteSink(rdr3b_23);
            Ptr<MgByte> bytes3b_23 = sink3b_23->ToBuffer();

            Ptr<MgByteSource> src3c_23 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy3c.ldf");
            Ptr<MgByteReader> rdr3c_23 = src3c_23->GetReader();
            Ptr<MgByteSink> sink3c_23 = new MgByteSink(rdr3c_23);
            Ptr<MgByte> bytes3c_23 = sink3c_23->ToBuffer();

            Ptr<MgByteSource> src3d_23 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy3d.ldf");
            Ptr<MgByteReader> rdr3d_23 = src3d_23->GetReader();
            Ptr<MgByteSink> sink3d_23 = new MgByteSink(rdr3d_23);
            Ptr<MgByte> bytes3d_23 = sink3d_23->ToBuffer();

            Ptr<MgByteSource> src3e_23 = new MgByteSource(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy3e.ldf");
            Ptr<MgByteReader> rdr3e_23 = src3e_23->GetReader();
            Ptr<MgByteSink> sink3e_23 = new MgByteSink(rdr3e_23);
            Ptr<MgByte> bytes3e_23 = sink3e_23->ToBuffer();

            CPPUNIT_ASSERT(bytes3a_23->GetLength() == bytes3b_23->GetLength());
            CPPUNIT_ASSERT(bytes3b_23->GetLength() == bytes3c_23->GetLength());
            CPPUNIT_ASSERT(bytes3c_23->GetLength() == bytes3d_23->GetLength());
            CPPUNIT_ASSERT(bytes3d_23->GetLength() == bytes3e_23->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes3a_23->Bytes(), bytes3b_23->Bytes(), bytes3a_23->GetLength()) == 0);
            CPPUNIT_ASSERT(memcmp(bytes3b_23->Bytes(), bytes3c_23->Bytes(), bytes3b_23->GetLength()) == 0);
            CPPUNIT_ASSERT(memcmp(bytes3c_23->Bytes(), bytes3d_23->Bytes(), bytes3c_23->GetLength()) == 0);
            CPPUNIT_ASSERT(memcmp(bytes3d_23->Bytes(), bytes3e_23->Bytes(), bytes3d_23->GetLength()) == 0);

            // save the new resources to the repository to validate the XML
            m_svcResource->SetResource(ldfresV, rdr1_09, NULL);
            m_svcResource->DeleteResource(ldfresV);
            m_svcResource->SetResource(ldfresV, rdr1_10, NULL);
            m_svcResource->DeleteResource(ldfresV);
            m_svcResource->SetResource(ldfresV, rdr1_11, NULL);
            m_svcResource->DeleteResource(ldfresV);
            m_svcResource->SetResource(ldfresV, rdr1_12, NULL);
            m_svcResource->DeleteResource(ldfresV);
            m_svcResource->SetResource(ldfresV, rdr1_13, NULL);
            m_svcResource->DeleteResource(ldfresV);
            m_svcResource->SetResource(ldfresV, rdr1_23, NULL);
            m_svcResource->DeleteResource(ldfresV);
            m_svcResource->SetResource(ldfresV, rdr3a_23, NULL);
            m_svcResource->DeleteResource(ldfresV);
        }

        // delete the files
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v09_Copy1.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v10_Copy1.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v11_Copy1.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v12_Copy1.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v13_Copy1.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy1.ldf", true);

        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v09_Copy2.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v10_Copy2.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v11_Copy2.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v12_Copy2.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v13_Copy2.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy2.ldf", true);

        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy3a.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy3b.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy3c.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy3d.ldf", true);
        MgFileUtil::DeleteFile(L"../UnitTestFiles/MdfTestTypeStyles_v23_Copy3e.ldf", true);

        // As MapDefinition 1.0.0 doesn't take extension into consideration, if a MDF 2.3 is
        // stored into 1.0.0, there will be some information missing. So no test code about
        // version.
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

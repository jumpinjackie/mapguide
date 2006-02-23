// -*- C++ -*-

// This file is part of the OPeNDAP/GDAL driver.

// Copyright (c) 2003 OPeNDAP, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// Author: James Gallagher <jgallagher@opendap.org>
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>

#include "dods-datatypes.h"

#include "dodsdataset.h"

CPL_CVSID("$Id: dodsdataset_test.cpp,v 1.4 2004/01/29 23:19:24 jimg Exp $");

using namespace CppUnit;

class DODSDatasetTest : public TestFixture {
private:
    DODSDataset *poDataset;
    DODSDataset *poDS;
    string url_1, url_2;

public:
    DODSDatasetTest() : 
	url_1("http://localhost/dods-test/nph-dods/data/nc/fnoc1.nc?u[1][lat][lon]"),
	url_2("http://localhost/dods-test/nph-dods/data/nc/fnoc1.nc?v[1-16][lat][lon]")
    {
	// Register all the GDAL drivers. If you only want one driver
	// registered, look at gdalallregister.cpp. 
	GDALAllRegister();
    }

    ~DODSDatasetTest() {}
    
    void setUp () {
	poDS = new DODSDataset;
    }

    void tearDown() {
	delete poDS;
    }

    CPPUNIT_TEST_SUITE( DODSDatasetTest );

    CPPUNIT_TEST(connect_to_server_test);
    CPPUNIT_TEST(parse_input_test);
    CPPUNIT_TEST(verify_layer_spec_test);
    CPPUNIT_TEST(get_var_info_test);
    CPPUNIT_TEST(build_constraint_test);
#if 0
    CPPUNIT_TEST(are_same_type_test);
#endif
    CPPUNIT_TEST(contiguous_bands_test);
    CPPUNIT_TEST(gdal_open_test);
    CPPUNIT_TEST(get_raster_test);
    CPPUNIT_TEST(irasteriohelper_test);
    CPPUNIT_TEST(irasterio_test);
    CPPUNIT_TEST(get_geo_info_test);
    CPPUNIT_TEST(get_projection_ref_test);

    CPPUNIT_TEST_SUITE_END();

    void connect_to_server_test() {
	poDS->d_oURL = "http://localhost/dods-test/nph-dods/data/nc/fnoc1.nc";
	try {
	    AISConnect *poAISConn = poDS->connect_to_server();
	    CPPUNIT_ASSERT(poAISConn);
	}
	catch (Error &e) { 
	    cerr << "Error: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"This should not throw and error");
	}

	poDS->d_oURL = "ftp://localhost/dods-test/nph-dods/data/nc/fnoc1.nc";
	try {
	    AISConnect *poAISConn = poDS->connect_to_server();
	    CPPUNIT_ASSERT(!poAISConn);
	}
	catch (Error &e) { 
	    DBG(cerr << e.get_error_message() << endl);
	    CPPUNIT_ASSERT("Threw  error");
	}

	poDS->d_oURL = "http://localhost/index.html";
	try {
	    AISConnect *poAISConn = poDS->connect_to_server();
	    CPPUNIT_ASSERT(!poAISConn);
	}
	catch (Error &e) { 
	    DBG(cerr << e.get_error_message() << endl);
	    CPPUNIT_ASSERT("Threw  error");
	}
    }

    void parse_input_test() {
	poDS->parse_input(url_1);
	CPPUNIT_ASSERT(poDS->d_oURL == "http://localhost/dods-test/nph-dods/data/nc/fnoc1.nc");
	CPPUNIT_ASSERT(poDS->d_oVarName == "u");
	CPPUNIT_ASSERT(poDS->d_oBandExpr == "[1][lat][lon]");

	poDS->parse_input(string("http://localhost/fnoc1.nc?bogus[lat][lon]"));
	CPPUNIT_ASSERT(poDS->d_oURL == "http://localhost/fnoc1.nc");
	CPPUNIT_ASSERT(poDS->d_oVarName == "bogus");
	CPPUNIT_ASSERT(poDS->d_oBandExpr == "[lat][lon]");

	poDS->parse_input(string("http://localhost/fnoc1.nc?bogus[1][11-21][lat][lon]"));
	CPPUNIT_ASSERT(poDS->d_oURL == "http://localhost/fnoc1.nc");
	CPPUNIT_ASSERT(poDS->d_oVarName == "bogus");
	CPPUNIT_ASSERT(poDS->d_oBandExpr == "[1][11-21][lat][lon]");

	// Try invalid input. All should throw Error.
	try {
	    poDS->parse_input(string("http://localhost/nph-dods/fnoc1.ncu[0]"));
	    CPPUNIT_ASSERT(!"Failed to throw Error!");
	}
	catch (Error &e) { CPPUNIT_ASSERT("Threw Error"); }

	try {
	    poDS->parse_input(string("http://localhost/nph-dods/fnoc1.nc?u"));
	    CPPUNIT_ASSERT(!"Failed to throw Error!");
	}
	catch (Error &e) { CPPUNIT_ASSERT("Threw Error"); }

	try {
	    poDS->parse_input(string("http://localhost/nph-dods/fnoc1.nc?[0]"));
	    CPPUNIT_ASSERT(!"Failed to throw Error!");
	}
	catch (Error &e) { CPPUNIT_ASSERT("Threw Error"); }

	try {
	    poDS->parse_input(string("?u[0]"));
	    CPPUNIT_ASSERT(!"Failed to throw Error!");
	}
	catch (Error &e) { CPPUNIT_ASSERT("Threw Error"); }
    }

    void verify_layer_spec_test() {
	DODSDataset::dim_spec ds;
	ds.type = DODSDataset::dim_spec::lat;
	poDS->d_oBandSpec.push_back(ds);
	ds.type = DODSDataset::dim_spec::lon;
	poDS->d_oBandSpec.push_back(ds);
	ds.type = DODSDataset::dim_spec::index; ds.start = 1;
	poDS->d_oBandSpec.push_back(ds);
	poDS->d_iVarRank = 3;
	try {
	    poDS->verify_layer_spec();
	    CPPUNIT_ASSERT("Worked!");
	}
	catch (Error &e) {CPPUNIT_ASSERT(!"Should not throw Error"); }

	// Missing dimension
	poDS->d_iVarRank = 4;
	try {
	    poDS->verify_layer_spec();
	    CPPUNIT_ASSERT(!"Should have thrown Error!");
	}
	catch (Error &e) {
	    DBG(cerr << "Message: " << e.get_error_message() << endl);
	    CPPUNIT_ASSERT("Should throw Error");
	}

	// Add a fourth dim spec; should work
	ds.type = DODSDataset::dim_spec::range; ds.start = 1; ds.stop = 8;
	poDS->d_oBandSpec.push_back(ds);
	try {
	    poDS->verify_layer_spec();
	    CPPUNIT_ASSERT("Worked!");
	}
	catch (Error &e) {CPPUNIT_ASSERT(!"Should not throw Error"); }

	// Only one range spec allowed; should fail
	ds.type = DODSDataset::dim_spec::range; ds.start = 1; ds.stop = 9;
	poDS->d_iVarRank = 5;
	poDS->d_oBandSpec.push_back(ds);
	try {
	    poDS->verify_layer_spec();
	    CPPUNIT_ASSERT(!"Should have failed!");
	}
	catch (Error &e) {
	    DBG(cerr << "Message: " << e.get_error_message() << endl);
	    CPPUNIT_ASSERT("Should throw Error");
	}

	// We are allowed any number of index specs; should work
	ds.type = DODSDataset::dim_spec::index; ds.start = 1; 
	poDS->d_oBandSpec[4]= ds;
	try {
	    poDS->verify_layer_spec();
	    CPPUNIT_ASSERT("Should work!");
	}
	catch (Error &e) {CPPUNIT_ASSERT(!"Should not throw Error"); }
    }

    void get_var_info_test() {
	AISConnect ac("http://localhost/dods-test/nph-dods/data/nc/fnoc1.nc");
	DAS das;
	DDS dds;
	ac.request_das(das);
	ac.request_dds(dds);
	poDS->d_oVarName = "u";
	DODSDataset::dim_spec ds;
	ds.type = DODSDataset::dim_spec::range; ds.start = 2; ds.stop = 8;
	poDS->d_oBandSpec.push_back(ds);
	ds.type = DODSDataset::dim_spec::lat;
	poDS->d_oBandSpec.push_back(ds);
	ds.type = DODSDataset::dim_spec::lon;
	poDS->d_oBandSpec.push_back(ds);
	poDS->d_iVarRank = 3;

	try {
	    poDS->get_var_info(das, dds);
	    CPPUNIT_ASSERT(poDS->d_iVarRank == 3);
	    CPPUNIT_ASSERT(poDS->d_iNumBands == 7);
	    CPPUNIT_ASSERT(poDS->nRasterYSize == 17);
	    CPPUNIT_ASSERT(poDS->nRasterXSize == 21);
	    CPPUNIT_ASSERT(poDS->d_eDatatype == GDT_Int16);
	}
	catch (Error &e) {
	    cerr << "Error: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"Should not throw Error");
	}
    }

#if 0
    void are_same_type_test() {
	// I don't have easy access to data sources where there are things
	// with bands of different types... 01/27/04 jhrg
	poDataset = static_cast<DODSDataset *>(GDALOpen(url_2.c_str(), 
							GA_ReadOnly));
	int *panBandMap = new int[5];
	panBandMap[0] = 1; panBandMap[1] = 1; panBandMap[2] = 1; 
	panBandMap[3] = 1; panBandMap[4] = 1; 
	CPPUNIT_ASSERT(poDataset->are_same_type(5, panBandMap));
	
	panBandMap[0] = 1; panBandMap[1] = 2; panBandMap[2] = 3;
	panBandMap[3] = 4; panBandMap[4] = 5; 
	CPPUNIT_ASSERT(poDataset->are_same_type(5, panBandMap));
	
	panBandMap[0] = 1; panBandMap[1] = 3; panBandMap[2] = 5;
	panBandMap[3] = 7; panBandMap[4] = 9; 
	CPPUNIT_ASSERT(poDataset->are_same_type(5, panBandMap));
    }
#endif

    void contiguous_bands_test() {
	poDataset = static_cast<DODSDataset *>(GDALOpen(url_2.c_str(), 
							GA_ReadOnly));

	int *panBandMap = new int[5];
	panBandMap[0] = 1; panBandMap[1] = 2; panBandMap[2] = 3;
	panBandMap[3] = 4; panBandMap[4] = 5; 

	DBG(cerr << "contiguous_bands(0, 5, panBandMap): "
	    << poDataset->contiguous_bands(0, 5, panBandMap) << endl);
	CPPUNIT_ASSERT(poDataset->contiguous_bands(0, 5, panBandMap) == 4);
	
	panBandMap[0] = 1; panBandMap[1] = 2; panBandMap[2] = 3;
	panBandMap[3] = 5; panBandMap[4] = 6; 

	DBG(cerr << "contiguous_bands(0, 5, panBandMap): "
	    << poDataset->contiguous_bands(0, 5, panBandMap) << endl);
	CPPUNIT_ASSERT(poDataset->contiguous_bands(0, 5, panBandMap) == 2);

	DBG(cerr << "contiguous_bands(3, 5, panBandMap): "
	     << poDataset->contiguous_bands(3, 5, panBandMap) << endl);
	CPPUNIT_ASSERT(poDataset->contiguous_bands(3, 5, panBandMap) == 4);
    }

    void gdal_open_test() {
	poDataset = static_cast<DODSDataset *>(GDALOpen(url_1.c_str(), 
							GA_ReadOnly));

	CPPUNIT_ASSERT(poDataset->GetVarName() == "u");
	CPPUNIT_ASSERT(poDataset->GetNumBands() == 1);
	CPPUNIT_ASSERT(poDataset->GetDatatype() == GDT_Int16);

	poDataset = static_cast<DODSDataset *>(GDALOpen(url_2.c_str(), 
							GA_ReadOnly));

	CPPUNIT_ASSERT(poDataset->GetVarName() == "v");
	CPPUNIT_ASSERT(poDataset->GetNumBands() == 16);
	CPPUNIT_ASSERT(poDataset->GetDatatype() == GDT_Int16);
    }

    void build_constraint_test() {
	poDS->d_oVarName = "u";
	poDS->nRasterXSize = 21;
	poDS->nRasterYSize = 17;
	DODSDataset::dim_spec ds;
	ds.type = DODSDataset::dim_spec::range; ds.start = 2; ds.stop = 8;
	poDS->d_oBandSpec.push_back(ds);
	ds.type = DODSDataset::dim_spec::lat;
	poDS->d_oBandSpec.push_back(ds);
	ds.type = DODSDataset::dim_spec::lon;
	poDS->d_oBandSpec.push_back(ds);
	poDS->d_iVarRank = 3;

	string c1 = poDS->build_constraint(0, 0, 10, 10, 1, 1);
	DBG(cerr << "C1: " << c1 <<endl);
	CPPUNIT_ASSERT(c1 == "u[1][0:9][0:9]");

	// Band four, given that the band spec is [2-8] means GDAL band 5
	// which is DAP index 4.
	string c2 = poDS->build_constraint(4, 5, 4, 6, 4, 4);
	DBG(cerr << "C2: " << c2 <<endl);
	CPPUNIT_ASSERT(c2 == "u[4][5:10][4:7]");

	string c3 = poDS->build_constraint(4, 5, 4, 6, 4, 6);
	DBG(cerr << "C3: " << c3 <<endl);
	CPPUNIT_ASSERT(c3 == "u[4:6][5:10][4:7]");
    }

    void get_raster_test() {
	try {
	    poDataset = static_cast<DODSDataset *>(GDALOpen(url_1.c_str(), 
							    GA_ReadOnly));
	    dods_int16 *pImage = new dods_int16[21 * 17];
	    // x size == 21, y size == 17 (0..20, 0..16)
	    poDataset->get_raster(0, 0, 21, 17, 1, 1, static_cast<void*>(pImage));
	    // I got these values (-1728, -303) by looking at the ASCII
	    // output for the variable 'u': http://localhost/dods-test/nph-dods/data/nc/fnoc1.nc.ascii?u[0:1:0][0:1:16][0:1:20]
	    // NOTE: Offsets are zero-based while band numbers are ones-based
	    // to match GDAL .
	    DBG(cerr << "Element 20,16: " << pImage[20+(16*21)] << endl);
	    CPPUNIT_ASSERT(pImage[0+(0*21)] == -1728);
	    CPPUNIT_ASSERT(pImage[20+(16*21)] == -303);
	    delete[] pImage;

	    poDataset = static_cast<DODSDataset *>(GDALOpen(url_2.c_str(), 
							    GA_ReadOnly));
	    pImage = new dods_int16[21 * 17];
	    // x size == 21, y size == 17 (0..20, 0..16)
	    poDataset->get_raster(0, 0, 21, 17, 10, 10, static_cast<void*>(pImage));
	    // I got these values (3121, 109) by looking at the ASCII
	    // output for the variable 'v': http://localhost/dods-test/nph-dods/data/nc/fnoc1.nc.ascii?v[9:1:9][0:1:16][0:1:20]
	    DBG(cerr << "Element 20,16: " << pImage[20+(16*21)] << endl);
	    CPPUNIT_ASSERT(pImage[0+(0*21)] == 2023);
	    CPPUNIT_ASSERT(pImage[20+(16*21)] == -41);
	    delete[] pImage;

	    pImage = new dods_int16[5 * 5];
	    // x size == 5, y size == 5 (0..4, 0..4)
	    poDataset->get_raster(9, 9, 5, 5, 10, 10, static_cast<void*>(pImage));
	    // I got these values (-957, -1281) by looking at the ASCII
	    // output for the variable 'v': http://localhost/dods-test/nph-dods/data/nc/fnoc1.nc.ascii?v[9:1:9][9:1:13][9:1:13]
	    DBG(cerr << "Element 4,4: " << pImage[4+(4*5)] << endl);
	    CPPUNIT_ASSERT(pImage[0+(0*5)] == -360);
	    CPPUNIT_ASSERT(pImage[4+(4*5)] == -987);
	    delete[] pImage;

	    // Grab two bands (10 and 11)
	    pImage = new dods_int16[5 * 5 * 2];
	    // x size == 5, y size == 5, z size = 2 (0..4, 0..4, 0..1)
	    poDataset->get_raster(9, 9, 5, 5, 10, 11, static_cast<void*>(pImage));
	    // output for the variable 'v': http://localhost/dods-test/nph-dods/data/nc/fnoc1.nc.ascii?v[9:1:10][9:1:13][9:1:13]
	    DBG(cerr << "Element 4,4: " << pImage[4+(4*5)+(0*5*5)] << endl);
	    CPPUNIT_ASSERT(pImage[0+(0*5)+(0*5*5)] == -360);
	    CPPUNIT_ASSERT(pImage[4+(4*5)+(0*5*5)] == -987);
	    CPPUNIT_ASSERT(pImage[0+(0*5)+(1*5*5)] == -334);
	    CPPUNIT_ASSERT(pImage[4+(4*5)+(1*5*5)] == -827);
	    delete[] pImage;
	}
	catch (Error &e) {
	    cerr << "Error: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"Caught Error while testing GetRaster!");
	}
    }

    // Test DODSRasterBand::IRasterIO()
    void irasteriohelper_test() {
	try {
	    // Get the whole raster U
	    poDataset = static_cast<DODSDataset *>(GDALOpen(url_1.c_str(), 
							    GA_ReadOnly));
	    DODSRasterBand *poBand = dynamic_cast<DODSRasterBand*>(poDataset->GetRasterBand(1));

	    // I know the size and type because I looked at the URL. See above.
	    dods_int16 *pasData = new dods_int16[21 * 17];

	    // Compute these as Dataset::RasterIO() would if they were
	    // defaulted. 
	    int nPixelSpace = GDALGetDataTypeSize( GDT_Int16 ) / 8;
	    int nLineSpace = nPixelSpace * 21; // nBufXSize == 21

	    poDataset->irasterio_helper(GF_Read, 
				       0, 0, 21, 17,
				       poBand->GetRasterDataType(),
				       pasData, 21, 17, 
				       GDT_Int16,
				       1, 1,
				       nPixelSpace, nLineSpace);

	    DBG(cerr << "Element 0,0: " << pasData[0+(0*21)] << endl);
	    DBG(cerr << "Element 20,16: " << pasData[20+(16*21)] << endl);

	    CPPUNIT_ASSERT(pasData[0+(0*21)] == -1728);
	    CPPUNIT_ASSERT(pasData[20+(16*21)] == -303);

	    delete[] pasData; pasData = 0;

	    // Get part of V
	    poDataset = static_cast<DODSDataset *>(GDALOpen(url_2.c_str(), 
							    GA_ReadOnly));
	    poBand = dynamic_cast<DODSRasterBand*>(poDataset->GetRasterBand(10));
	    // I know the size and type because I looked at the URL. See above.
	    pasData = new dods_int16[5 * 5];
	    nLineSpace = nPixelSpace * 5; // nBufXSize == 5!

	    poDataset->irasterio_helper(GF_Read,
				       9, 9, 5, 5, 
				       poBand->GetRasterDataType(),
				       pasData, 5, 5, 
				       GDT_Int16,
				       10, 10,
				       nPixelSpace, nLineSpace);

	    DBG(cerr << "Element 0,0: " << pasData[0+(0*5)] << endl);
	    DBG(cerr << "Element 4,4: " << pasData[4+(4*5)] << endl);

	    CPPUNIT_ASSERT(pasData[0+(0*5)] == -360);
	    CPPUNIT_ASSERT(pasData[4+(4*5)] == -987);

	    delete[] pasData; pasData = 0;

	    // Now Get V using a buffer of a different type
	    // I know the size and type because I looked at the URL. See above.
	    dods_float32 *pafData = new dods_float32[5 * 5];
	    nPixelSpace = GDALGetDataTypeSize( GDT_Float32 ) / 8;
	    nLineSpace = nPixelSpace * 5; // nBufXSize == 5!

	    poDataset->irasterio_helper(GF_Read,
				       9, 9, 5, 5,
				       poBand->GetRasterDataType(),
				       pafData, 5, 5, 
				       GDT_Float32,
				       10, 10,
				       nPixelSpace, nLineSpace);

	    DBG(cerr << "Element 0,0: " << pafData[0+(0*5)] << endl);
	    DBG(cerr << "Element 4,4: " << pafData[4+(4*5)] << endl);

	    CPPUNIT_ASSERT(pafData[0+(0*5)] == -360.0);
	    CPPUNIT_ASSERT(pafData[4+(4*5)] == -987.0);

	    delete[] pafData; pafData = 0;

	    // Now Get V using a buffer of a different size
	    // I know the size and type because I looked at the URL. See above.
	    pasData = new dods_int16[2 * 2];
	    nPixelSpace = GDALGetDataTypeSize( GDT_Int16 ) / 8;
	    nLineSpace = nPixelSpace * 2; // nBufXSize == 2!

	    poDataset->irasterio_helper(GF_Read,
				       9, 9, 5, 5, 
				       poBand->GetRasterDataType(),
				       pasData, 2, 2, 
				       GDT_Int16,
				       10, 10,
				       nPixelSpace, nLineSpace);

	    DBG(cerr << "Element 0,0: " << pasData[0+(0*2)] << endl);
	    DBG(cerr << "Element 1,1: " << pasData[1+(1*2)] << endl);

	    CPPUNIT_ASSERT(pasData[0+(0*2)] == -680);
	    CPPUNIT_ASSERT(pasData[1+(1*2)] == -1522);

	    delete[] pasData; pasData = 0;

	    // Now Get V using a buffer of a different size. Slightly
	    // different test from above.
	    // I know the size and type because I looked at the URL. See above.
	    pasData = new dods_int16[2 * 2];
	    nPixelSpace = GDALGetDataTypeSize( GDT_Int16 ) / 8;
	    nLineSpace = nPixelSpace * 2; // nBufXSize == 2!

	    poDataset->irasterio_helper(GF_Read,
				       9, 9, 4, 4, 
				       poBand->GetRasterDataType(),
				       pasData, 2, 2,
				       GDT_Int16,
				       10, 10,
				       nPixelSpace, nLineSpace);

	    DBG(cerr << "Element 0,0: " << pasData[0+(0*2)] << endl);
	    DBG(cerr << "Element 1,1: " << pasData[1+(1*2)] << endl);

	    CPPUNIT_ASSERT(pasData[0+(0*2)] == -680);
	    CPPUNIT_ASSERT(pasData[1+(1*2)] == -1522);

	    delete[] pasData; pasData = 0;

	    // Finally, get V into a buffer of both different size and type.
	    pafData = new dods_float32[2 * 2];
	    nPixelSpace = GDALGetDataTypeSize( GDT_Float32 ) / 8;
	    nLineSpace = nPixelSpace * 2; // nBufXSize == 2!

	    poDataset->irasterio_helper(GF_Read,
				       9, 9, 5, 5, 
				       poBand->GetRasterDataType(),
				       pafData, 2, 2, 
				       GDT_Float32,
				       10, 10,
				       nPixelSpace, nLineSpace);

	    DBG(cerr << "Element 0,0: " << pafData[0+(0*2)] << endl);
	    DBG(cerr << "Element 1,1: " << pafData[1+(1*2)] << endl);

	    CPPUNIT_ASSERT(pafData[0+(0*2)] == -680.0);
	    CPPUNIT_ASSERT(pafData[1+(1*2)] == -1522.0);

	    delete[] pafData; pafData = 0;

	    // Grab two bands at once
	    pasData = new dods_int16[5 * 5 * 2];
	    nPixelSpace = GDALGetDataTypeSize( GDT_Int16 ) / 8;
	    nLineSpace = nPixelSpace * 5; // nBufXSize == 5!

	    poDataset->irasterio_helper(GF_Read,
				       9, 9, 5, 5, 
				       poBand->GetRasterDataType(),
				       pasData, 5, 5, 
				       GDT_Int16,
				       10, 11,
				       nPixelSpace, nLineSpace);

	    DBG(cerr << "Band 1, 0,0: " << pasData[0+(0*5)+(0*5*5)] << endl);
	    DBG(cerr << "Band 1, 4,4: " << pasData[4+(4*5)+(0*5*5)] << endl);
	    DBG(cerr << "Band 2, 0,0: " << pasData[0+(0*5)+(1*5*5)] << endl);
	    DBG(cerr << "Band 2, 4,4: " << pasData[4+(4*5)+(1*5*5)] << endl);

	    CPPUNIT_ASSERT(pasData[0+(0*5)+(0*5*5)] == -360);
	    CPPUNIT_ASSERT(pasData[4+(4*5)+(0*5*5)] == -987);
	    CPPUNIT_ASSERT(pasData[0+(0*5)+(1*5*5)] == -334);
	    CPPUNIT_ASSERT(pasData[4+(4*5)+(1*5*5)] == -827);

	    delete[] pasData; pasData = 0;

	}
	catch (Error &e) {
	    cerr << "Error: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"Caught Error while testing IRasterIOHelper!");
	}
    }

    void irasterio_test() {
	// Get the whole raster U
	poDataset = static_cast<DODSDataset *>(GDALOpen(url_1.c_str(), 
							GA_ReadOnly));

	// I know the size and type because I looked at the URL. See above.
	dods_int16 *pasData = new dods_int16[21 * 17];

	// Compute these as Dataset::RasterIO() would if they were
	// defaulted. 
	int nPixelSpace = GDALGetDataTypeSize( GDT_Int16 ) / 8;
	int nLineSpace = nPixelSpace * 21; // nBufXSize == 21
	int nBandSpace = nLineSpace * 17; // nBufYSize == 17
	int nBandCount = 1;
	int *panBandMap = new int[1];
	panBandMap[0]=1;

	CPLErr e = poDataset->IRasterIO(GF_Read, 
					0, 0, 21, 17,
					pasData, 21, 17, 
					GDT_Int16,
					nBandCount, panBandMap,
					nPixelSpace, nLineSpace, nBandSpace);

	CPPUNIT_ASSERT(e == CE_None);

	DBG(cerr << "Element 0,0: " << pasData[0+(0*21)] << endl);
	DBG(cerr << "Element 20,16: " << pasData[20+(16*21)] << endl);

	CPPUNIT_ASSERT(pasData[0+(0*21)] == -1728);
	CPPUNIT_ASSERT(pasData[20+(16*21)] == -303);

	delete[] pasData; pasData = 0;
	delete[] panBandMap; panBandMap = 0;

	// Test multi band reads using V: Bands 10 and 11
	poDataset = static_cast<DODSDataset *>(GDALOpen(url_2.c_str(), 
							GA_ReadOnly));
	pasData = new dods_int16[5 * 5 * 2];
	nPixelSpace = GDALGetDataTypeSize( GDT_Int16 ) / 8;
	nLineSpace = nPixelSpace * 5; // nBufXSize == 5!
	nBandSpace = nLineSpace * 5; // nBufYSize == 5

	nBandCount = 2;
	panBandMap = new int[nBandCount];
	panBandMap[0]=10; panBandMap[1]=11;

	e = poDataset->IRasterIO(GF_Read, 
				 9, 9, 5, 5,
				 pasData, 5, 5,
				 GDT_Int16,
				 nBandCount, panBandMap,
				 nPixelSpace, nLineSpace, nBandSpace);

	CPPUNIT_ASSERT(e == CE_None);

	DBG(cerr << "Band 1, 0,0: " << pasData[0+(0*5)+(0*5*5)] << endl);
	DBG(cerr << "Band 1, 4,4: " << pasData[4+(4*5)+(0*5*5)] << endl);
	DBG(cerr << "Band 2, 0,0: " << pasData[0+(0*5)+(1*5*5)] << endl);
	DBG(cerr << "Band 2, 4,4: " << pasData[4+(4*5)+(1*5*5)] << endl);

	CPPUNIT_ASSERT(pasData[0+(0*5)+(0*5*5)] == -360);
	CPPUNIT_ASSERT(pasData[4+(4*5)+(0*5*5)] == -987);
	CPPUNIT_ASSERT(pasData[0+(0*5)+(1*5*5)] == -334);
	CPPUNIT_ASSERT(pasData[4+(4*5)+(1*5*5)] == -827);

	delete[] pasData; pasData = 0;
	delete[] panBandMap; panBandMap = 0;

	// Band 10 and 12
	pasData = new dods_int16[5 * 5 * 2];
	nPixelSpace = GDALGetDataTypeSize( GDT_Int16 ) / 8;
	nLineSpace = nPixelSpace * 5; // nBufXSize == 5!
	nBandSpace = nLineSpace * 5; // nBufYSize == 5

	nBandCount = 2;
	panBandMap = new int[nBandCount];
	panBandMap[0]=10; panBandMap[1]=12;

	e = poDataset->IRasterIO(GF_Read, 
				 9, 9, 5, 5,
				 pasData, 5, 5,
				 GDT_Int16,
				 nBandCount, panBandMap,
				 nPixelSpace, nLineSpace, nBandSpace);

	CPPUNIT_ASSERT(e == CE_None);

	DBG(cerr << "Band 1, 0,0: " << pasData[0+(0*5)+(0*5*5)] << endl);
	DBG(cerr << "Band 1, 4,4: " << pasData[4+(4*5)+(0*5*5)] << endl);
	DBG(cerr << "Band 2, 0,0: " << pasData[0+(0*5)+(1*5*5)] << endl);
	DBG(cerr << "Band 2, 4,4: " << pasData[4+(4*5)+(1*5*5)] << endl);

	CPPUNIT_ASSERT(pasData[0+(0*5)+(0*5*5)] == -360);
	CPPUNIT_ASSERT(pasData[4+(4*5)+(0*5*5)] == -987);
	CPPUNIT_ASSERT(pasData[0+(0*5)+(1*5*5)] == -349);
	CPPUNIT_ASSERT(pasData[4+(4*5)+(1*5*5)] == -1952);

	delete[] pasData; pasData = 0;
	delete[] panBandMap; panBandMap = 0;

	// Bands 9,10, 12,13
	pasData = new dods_int16[5 * 5 * 4];
	nPixelSpace = GDALGetDataTypeSize( GDT_Int16 ) / 8;
	nLineSpace = nPixelSpace * 5; // nBufXSize == 5!
	nBandSpace = nLineSpace * 5; // nBufYSize == 5

	nBandCount = 4;
	panBandMap = new int[nBandCount];
	panBandMap[0]=9; panBandMap[1]=10; panBandMap[2]=12; panBandMap[3]=13;

	e = poDataset->IRasterIO(GF_Read, 
				 9, 9, 5, 5,
				 pasData, 5, 5,
				 GDT_Int16,
				 nBandCount, panBandMap,
				 nPixelSpace, nLineSpace, nBandSpace);

	CPPUNIT_ASSERT(e == CE_None);

	DBG(cerr << "Band 1, 0,0: " << pasData[0+(0*5)+(0*5*5)] << endl);
	DBG(cerr << "Band 1, 4,4: " << pasData[4+(4*5)+(0*5*5)] << endl);
	DBG(cerr << "Band 2, 0,0: " << pasData[0+(0*5)+(1*5*5)] << endl);
	DBG(cerr << "Band 2, 4,4: " << pasData[4+(4*5)+(1*5*5)] << endl);
	DBG(cerr << "Band 3, 0,0: " << pasData[0+(0*5)+(2*5*5)] << endl);
	DBG(cerr << "Band 3, 4,4: " << pasData[4+(4*5)+(2*5*5)] << endl);
	DBG(cerr << "Band 4, 0,0: " << pasData[0+(0*5)+(3*5*5)] << endl);
	DBG(cerr << "Band 4, 4,4: " << pasData[4+(4*5)+(3*5*5)] << endl);

	CPPUNIT_ASSERT(pasData[0+(0*5)+(0*5*5)] == -78);
	CPPUNIT_ASSERT(pasData[4+(4*5)+(0*5*5)] == -1034);
	CPPUNIT_ASSERT(pasData[0+(0*5)+(1*5*5)] == -360);
	CPPUNIT_ASSERT(pasData[4+(4*5)+(1*5*5)] == -987);

	CPPUNIT_ASSERT(pasData[0+(0*5)+(2*5*5)] == -349);
	CPPUNIT_ASSERT(pasData[4+(4*5)+(2*5*5)] == -1952);
	CPPUNIT_ASSERT(pasData[0+(0*5)+(3*5*5)] == -487);
	CPPUNIT_ASSERT(pasData[4+(4*5)+(3*5*5)] == -1859);

	delete[] pasData; pasData = 0;
	delete[] panBandMap; panBandMap = 0;
    }

    void get_geo_info_test() {
	try {
	    cerr << endl 
		 << "These tests can take several minutes. They will fail without a network connection or if either the GSO or GSFC servers off line." << endl;

	    AISConnect ac("http://dods.gso.uri.edu/cgi-bin/nph-dods/DODS/pathfdr/1986/12/k86336175759.hdf", "example_conf.d/ais_database.xml");
	    DAS das;
	    DDS dds;
	    ac.request_das(das);
	    ac.request_dds(dds);
	    poDS->d_oVarName = "dsp_band_1";

	    poDS->get_geo_info(das, dds);
	    // sp_nav_earth_toplat: 71.1722
	    // dsp_nav_earth_botlat: 4.8278
	    // dsp_nav_earth_leflon: -112.11
	    // dsp_nav_earth_ritlon: -27.8897
	    CPPUNIT_ASSERT(poDS->d_dfLLLat == 4.8278);
	    CPPUNIT_ASSERT(poDS->d_dfLLLon == -112.11);
	    CPPUNIT_ASSERT(poDS->d_dfURLat == 71.1722);
	    CPPUNIT_ASSERT(poDS->d_dfURLon == -27.8897);

	    // MODIS level 3...
	    AISConnect ac2("http://g0dug03u.ecs.nasa.gov/daac-bin/dods/nph-dods/datapool/MOAA/MYD08_E3.003/2003.07.28/MYD08_E3.A2003209.003.2003234054010.hdf", "example_conf.d/ais_database.xml");
	    DAS das2;
	    DDS dds2;
	    ac2.request_das(das2);
	    ac2.request_dds(dds2);
	    poDS->d_oURL = "http://g0dug03u.ecs.nasa.gov/daac-bin/dods/nph-dods/datapool/MOAA/MYD08_E3.003/2003.07.28/MYD08_E3.A2003209.003.2003234054010.hdf";
	    poDS->d_oVarName = "mod08.Data%20Fields.Scattering_Angle_Mean";
	    poDS->get_geo_info(das2, dds2);

	    CPPUNIT_ASSERT(poDS->d_dfLLLat == -89.5);
	    CPPUNIT_ASSERT(poDS->d_dfLLLon == -179.5);
	    CPPUNIT_ASSERT(poDS->d_dfURLat == 89.5);
	    CPPUNIT_ASSERT(poDS->d_dfURLon == 179.5);
	}
	catch (Error &e) {
	    cerr << "Message: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"Should not throw Error");
	}
    }

    void get_projection_ref_test() {
	try {
	    AISConnect ac("http://dods.gso.uri.edu/cgi-bin/nph-dods/DODS/pathfdr/1986/12/k86336175759.hdf", "example_conf.d/ais_database.xml");
	    DAS das;
	    DDS dds;
	    ac.request_das(das);
	    ac.request_dds(dds);
	    poDS->d_oVarName = "dsp_band_1";

	    poDS->get_geo_info(das, dds);

	    cerr << endl << "The projection string is: " << endl
		 << poDS->GetProjectionRef() << endl;
	}
	catch (Error &e) {
	    cerr << "Message: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"Should not throw, check AIS stuff.");
	}
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(DODSDatasetTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;

    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}

// $Log: dodsdataset_test.cpp,v $
// Revision 1.4  2004/01/29 23:19:24  jimg
// Added tests for the new methods.
//
// Revision 1.3  2004/01/21 21:52:16  jimg
// Removed the unused method build_constraint(int). GDAL uses ones indexing
// for Bands while this driver was using zero-based indexing. I changed
// the code so the driver now also uses ones-based indexing for bands.
//
// Revision 1.2  2004/01/20 16:37:07  jimg
// Added tests for the IRasterIO() implementation.
//
// Revision 1.1  2003/12/12 23:28:17  jimg
// Added.
//
// Revision 1.1  2003/12/12 22:52:20  jimg
// Added.
//

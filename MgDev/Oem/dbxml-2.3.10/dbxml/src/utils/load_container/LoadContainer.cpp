//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: LoadContainer.cpp,v 1.27 2006/10/30 17:46:05 bostic Exp $
//
#include "dbxml/DbXml.hpp"

#include <string>
#include <vector>
#include <fstream>
#include "common_utils.h" // for signal handling

#ifdef HAVE_GETOPT
#include <unistd.h>
#else
extern "C" int getopt(int argc, char * const argv[], const char *optstring);
extern "C" char *optarg;
extern "C" int optind;
#endif

#include "LoadContainer.hpp"

using namespace DbXml;

LoadContainer::LoadContainer(XmlManager &mgr,
			     const std::string &containerName,
			     bool verbose)
	: _mgr(mgr), _conName(containerName), _verbose(verbose), _count(0)
{
	// nothing else to do
}

std::string LoadContainer::readFile(const std::string &theFile) const {

	std::string xmlString;

	/* Open as binary to preserve whitespace */
	std::ifstream inFile(theFile.c_str(), std::ios::in | std::ios::binary);
	if(!inFile) {
		std::cerr << "Could not open file '" <<
			theFile << "'. Giving up." << std::endl;
		throw std::exception();
	}

	while(!inFile.eof()) {
		std::string stringBuf;
		std::getline( inFile, stringBuf );
		// use \xA for the eol given that these are XML files.
		stringBuf += '\xA';
		xmlString += stringBuf;
	}

	inFile.close();
	return xmlString;
}

std::string LoadContainer::getNameFromPath(const std::string &theFile) const {
	std::string tmpName = theFile;
	std::string theName;
	unsigned int pos = tmpName.rfind("/");
	if(pos == std::string::npos ) {
		pos = tmpName.rfind("\\");
	}
	if(pos == std::string::npos) {
		theName = tmpName;
	} else {
		theName.assign( tmpName, pos+1, tmpName.length() );
	}

	return theName;
}

int LoadContainer::addFiles(const std::vector<std::string > &fileList,
			    const std::string &pathPrepend) const
{
	try {
		XmlContainer container =
			_mgr.openContainer(_conName, DB_CREATE);

		if (_verbose) {
			std::cout << "Created the container \"" <<
				container.getName() << "\""
				  << " ("
				  << (container.getContainerType() ==
				      XmlContainer::WholedocContainer ?
				      "whole document" : "node level")
				  << " storage)" << std::endl;
		}

		// Get an update context. Helpful for performance.
		XmlUpdateContext updateContext =
			_mgr.createUpdateContext();

		std::vector<std::string >::const_iterator theFile,
			end = fileList.end();
		for (theFile = fileList.begin(); theFile != end; ++theFile) {
			std::string totalPath = pathPrepend + *theFile;
			std::string xmlString = readFile(totalPath);
			XmlDocument xmlDoc = _mgr.createDocument();
			xmlDoc.setContent(xmlString);
			xmlDoc.setName(getNameFromPath(*theFile));

			container.putDocument(xmlDoc, updateContext,
					      DBXML_GEN_NAME);
			if (_verbose) {
				std::cout << "\tPut document \""
					  << xmlDoc.getName()
					  << "\"" << std::endl;
			}
			_count++;
			// if signal happened, bail out
			if (dbxml_util_interrupted())
				return 1;
		}
	}
	catch (XmlException &e) {
		std::cerr << "Error adding XML data to container: " <<
			_conName << std::endl;
		std::cerr << e.what() << std::endl;
		return -1;
	}
	return 0;
}

int LoadContainer::addFileList(const std::string &pathPrepend,
			       const std::string &fileName) const
{
	if (fileName.empty())
		return 0;
	std::ifstream inFile(fileName.c_str() ,std::ios::in);
	if(!inFile) {
		std::cerr << "Could not open file " << fileName
			  << ". Giving up." << std::endl;
		return -1;
	}

	std::vector<std::string > fileList;
	while(!inFile.eof()) {
		std::string stringBuf;
		std::getline( inFile, stringBuf );
		if(!stringBuf.empty()) {
			fileList.push_back(stringBuf);
		}
	}
	return addFiles(fileList, pathPrepend);
}

void usage()
{
        std::cerr << "dbxml_load_container -c container [options] [file1 file2 ...]\n"
		  << "Loads XML data into a the specified container.\n"
                  << "Options:\n"
                  << "\t-c - container name\n"
                  << "\t-f - filelist\n"
                  << "\t-h - database environment directory \n"
		  << "\t-P - specify environment password\n"
                  << "\t-p - path to prepend to each of the items in filelist\n"
                  << "\t-s - storage type [\"wholedoc\" (*) | \"node\"]\n"
		  << "\t-V - print software version\n"
		  << "\t-v - verbose output\n";
        exit(-1);
}

int main(int argc, char **argv)
{
	int ret = 0;
	std::string path2DbEnv;
	std::string theContainer;
	std::string storageType("wholedoc");
	std::string filePath;
	std::string fileList;
	std::vector<std::string> files2add;
	int ch;
	bool verbose = false;
	const char *password = 0;

	while ((ch = getopt(argc, argv, "?h:c:s:f:p:vVP:")) != -1) {
		switch (ch) {
		case 'h':
			path2DbEnv = optarg;
			break;
		case 'c':
			theContainer = optarg;
			break;
		case 's':
			storageType = optarg;
			break;
		case 'v':
			verbose = true;
			break;
		case 'V':
			printf("%s\n", DbXml::dbxml_version(NULL, NULL, NULL));
			printf("%s\n", DbEnv::version(NULL, NULL, NULL));
			exit(0);
		case 'P':
			password = optarg;
			break;
		case 'p':
		{
			// ensure we have a trailing slash
			std::string tmp(optarg);
			unsigned int pos = (int) tmp.rfind("/");
			if(pos != tmp.length()-1) {
				pos = (int) tmp.rfind("\\");  // windows
			}
			if (pos != tmp.length()-1) {
				tmp.append("/");
			}
			filePath.assign(tmp);
		}
		break;
		case 'f':
			fileList = optarg;
			break;
		case '?':
		default:
			usage();
			break;
		}
	}

	argc -= optind;
	argv += optind;
	for (int i = 0; i < argc; i++) {
		std::cout << "adding: " << argv[i] << std::endl;
		files2add.push_back(argv[i]);
	}

	if (path2DbEnv.empty())
		path2DbEnv = ".";

	// container is required
	if (theContainer.empty()) {
		std::cerr << "Container name must be specified" << std::endl;
		usage();
	}

	XmlContainer::ContainerType type = XmlContainer::NodeContainer;
	if(!storageType.compare("wholedoc")) {
		type = XmlContainer::WholedocContainer;
	} else if(storageType.compare("node")) {
		usage();
	}

	
	u_int32_t envFlags = (DB_CREATE|DB_PRIVATE|DB_INIT_MPOOL);
	bool transactional = false;
	DbEnv *dbenv = new DbEnv(0);
	if (password)
		dbenv->set_encrypt(password, DB_ENCRYPT_AES);
	dbenv->set_error_stream(&std::cerr);
	dbenv->set_cachesize(0, 64 * 1024 * 1024, 1);
	try {
		dbenv->open(path2DbEnv.c_str(), DB_JOINENV, 0);
		u_int32_t eflags = 0;
		dbenv->get_open_flags(&eflags);
		if (eflags & DB_INIT_TXN)
			transactional = true;
	} catch (DbException &e) {
		if (e.get_errno() == DB_VERSION_MISMATCH) {
			std::cerr << "Error opening environment " <<
				path2DbEnv.c_str() << ": " <<
				e.what() << std::endl;
			exit(-1);
		}
		if (verbose)
			std::cerr << "Unable to join environment " <<
				path2DbEnv.c_str() <<
				", creating one" << std::endl;
	}
	// try again, with new flags (no join), let it throw...
	dbenv->open(path2DbEnv.c_str(), envFlags, 0);

	XmlManager mgr(dbenv, DBXML_ADOPT_DBENV);
	mgr.setDefaultContainerType(type);
	if (transactional) {
		if (verbose)
			std::cout << "Environment is transactional" << std::endl;
		mgr.setDefaultContainerFlags(DBXML_TRANSACTIONAL);
	}

	SigBlock sb; // block signals
	LoadContainer lc(mgr, theContainer, verbose);

	ret = lc.addFileList(filePath, fileList);
	if ((ret == 0) && files2add.size())
		(void) lc.addFiles(files2add, filePath);

	if (verbose)
		std::cout << "\nPut " << lc.count() << " documents"
				  << std::endl;
	return ret;
}


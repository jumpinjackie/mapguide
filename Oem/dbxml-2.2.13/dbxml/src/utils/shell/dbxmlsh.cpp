//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: dbxmlsh.cpp,v 1.24 2005/11/12 02:33:55 gmf Exp $
//

#include <fstream>
#include <string>

#include "dbxml/DbXml.hpp"
#include "Environment.hpp"
#include "DefaultShell.hpp"
#include "common_utils.h"
#ifdef HAVE_GETOPT
#include <unistd.h>
#else
extern "C" int getopt(int argc, char * const argv[], const char *optstring);
extern "C" char *optarg;
extern "C" int optind;
#endif

using namespace DbXml;
using namespace std;

static void usage(const string &progname, int exitCode)
{
	string::size_type pos = progname.rfind('/');
	if(pos == string::npos) {
		pos = progname.rfind('\\');
	}
	if(pos == string::npos) {
		pos = 0;
	}
	else {
		++pos;
	}

	cerr << "Usage: " << progname.substr(pos) << " [OPTIONS]" << endl << endl;
	cerr << "Options:" << endl;
	cerr << "-c\t\tcreate a new environment in the directory specified by the -h option." << endl;
	cerr << "-h <home>\tuse specified directory as a Berkeley DB environment. (default: .)" << endl;
	cerr << "-P <password>\tspecify database password" << endl;
	cerr << "-s <script>\trun the script in non-interactive mode.\n\t\t Multiple scripts are allowed" << endl;
	cerr << "-t\t\ttransaction mode -- transactions can be used, and are required \n\t\tfor writes" << endl;
	cerr << "-V\t\tprint software version" << endl;
	cerr << "-v\t\tverbose. Using the switch a second time increases verbosity" << endl;
	cerr << "-x\t\tsecure mode -- disallow access to the local file system and \n\t\tthe internet" << endl;
	cerr << "-?\t\thelp -- print this message" << endl;
	cerr << endl;
	exit(exitCode);
}

int main(int argc, char **argv)
{
	// Deal with command line arguments
	string path2DbEnv = ".";
	u_int32_t envFlags = (DB_CREATE|DB_PRIVATE|DB_INIT_MPOOL);
	u_int32_t txnEnvFlags =	(DB_INIT_TXN|DB_INIT_LOCK|DB_INIT_LOG);
	u_int32_t dbxmlFlags = DBXML_ALLOW_EXTERNAL_ACCESS;
	vector<string> scripts;
	int verbose = 0;
	bool transactionMode = false;
	bool dbPrivate = false;
	bool envCreate = false;
	const char *progName = argv[0];
	const char *password = 0;
	int ch;
	int ret = 0;

	while ((ch = getopt(argc, argv, "?h:hs:tvxVP:c")) != -1) {
		switch (ch) {
		case 'h': {
			path2DbEnv = optarg;
			break;
		}
		case 'c': {
			envFlags &= ~DB_PRIVATE;
			envCreate = true;
			break;
		}
		case 'x': {
			dbxmlFlags &= ~DBXML_ALLOW_EXTERNAL_ACCESS;
			break;
		}
		case 't': {
			transactionMode = true;
			envFlags |= txnEnvFlags;
			break;
		}
		case 's': {
			scripts.push_back(optarg);
			break;
		}
		case 'v': {
			++verbose;
			break;
		}
		case 'V': {
			printf("%s\n", DbXml::dbxml_version(NULL, NULL, NULL));
			printf("%s\n", DbEnv::version(NULL, NULL, NULL));
			exit(0);
		}
		case 'P': {
			password = optarg;
			break;
		}
		case '?':
		default: {
			usage(progName, 0);
			break;
		}
		}
	}

	// Turn on logging if extra verbose is specified
	if(verbose > 1) {
		setLogLevel(LEVEL_ALL, true);
		setLogCategory(CATEGORY_ALL, true);
		setLogCategory(CATEGORY_NODESTORE, verbose > 2);
	}

	SigBlock sb; // block signals, resend at end of scope
	try {
		// Create a DB environment, and XmlManager
		DbEnv *dbenv = new DbEnv(0);
		if (password)
			dbenv->set_encrypt(password, DB_ENCRYPT_AES);
		dbenv->set_error_stream(&cerr);
		dbenv->set_cachesize(0, 64 * 1024 * 1024, 1);
		dbenv->set_lk_max_lockers(10000);
		dbenv->set_lk_max_locks(10000);
		dbenv->set_lk_max_objects(10000);
		int dberr = 0;
		if (!dbPrivate) {
			dbenv->set_lk_detect(DB_LOCK_DEFAULT);
			try {
				if (verbose && !envCreate) {
					cout <<
						"Attempting to join environment: "
					     << path2DbEnv << endl;
				}
				dberr = dbenv->open(path2DbEnv.c_str(),
						    DB_JOINENV, 0);
				if (dberr == 0) {
					if(envCreate) {
						cerr << "An environment already exists at " <<
							path2DbEnv.c_str()  << endl;
						exit(-1);
					}

					u_int32_t eflags = 0;
					dbenv->get_open_flags(&eflags);
					if (eflags & DB_INIT_TXN)
						transactionMode = true;
					else {
						if (verbose && (transactionMode == true))
							cout << "Joined a non-transactional environment, turning off transaction mode" << endl;
						transactionMode = false;
					}
				}
			} catch (DbException &e) {
				if (e.get_errno() == DB_VERSION_MISMATCH) {
					cerr << "Error opening environment " <<
						path2DbEnv.c_str() << ": " <<
						e.what() << endl;
					exit(-1);
				}
				if (verbose) {
					if(envCreate) {
						cerr << "Creating environment: " <<
							path2DbEnv.c_str() << endl;
					} else {
						cerr << "Unable to join environment " <<
							path2DbEnv.c_str() << ", creating a DB_PRIVATE environment" << endl;
					}
				}
				dberr = dbenv->open(path2DbEnv.c_str(),
						    envFlags, 0);
			}
		} else {
			dberr = dbenv->open(path2DbEnv.c_str(),
					    envFlags, 0);
		}
		if (dberr != 0) {
			cerr << "Error opening environment " <<
				path2DbEnv.c_str() << ", error is " <<
				dberr << endl;
			exit(-1);
		}
		XmlManager db(dbenv, dbxmlFlags|DBXML_ADOPT_DBENV);

		// Create the environment
		Environment env(db);
		env.transactions() = transactionMode;

		// Create the Shell object
		DefaultShell shell;

		// Run scripts, if specified
		if(!scripts.empty()) {
			env.interactive() = false;
			env.verbose() = verbose != 0;

			for(vector<string>::iterator i = scripts.begin();
			    i != scripts.end() && !env.quit(); ++i) {
				ifstream scriptFile(i->c_str(), ios::in);
				if(!scriptFile) {
					cerr << progName << ": cannot open script file: " << *i << endl;
				} else {
					env.streamName() = *i;
					env.lineNo() = 0;
					shell.mainLoop(scriptFile, env);
					scriptFile.close();
				}
			}
		}

		// Perform the queries
		if(!env.quit()) {
			env.interactive() = true;
			env.verbose() = true;
			env.streamName() = "stdin";
			env.lineNo() = 0;
			shell.mainLoop(cin, env);
		}
	}
	catch(exception &e) {
		cerr << progName << ": error at lowest level: " << e.what() << endl;
		ret = 1;
	}
	catch(...) {
		cerr << progName << ": error at lowest level: " << endl;
		ret = 1;
	}
	return ret;
}

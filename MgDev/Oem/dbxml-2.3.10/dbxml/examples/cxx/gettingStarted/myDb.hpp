//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
// $Id: myDb.hpp,v 1.10 2006/10/30 17:45:44 bostic Exp $
//

#include "dbxml/DbXml.hpp"
#include "db_cxx.h"

//Forward declare
class myDbEnv;

//these examples never want to open a database without an
//environment, so this class does not provide support for
//the simple case.
class myDb
{
    public:
        //Constructor that accepts a reference to a myDbEnv object.
        myDb( const std::string &dbName, DbXml::XmlManager &myDbEnvObj )
                : myDbEnvPtr_( &myDbEnvObj ),
		  database_( myDbEnvObj.getDbEnv(), 0 ),
		  dbName_( dbName ),
		  cFlags_(DB_CREATE),
		  dbIsOpen_( false )
        {
            if ( dbName_.empty() )
            {
                std::cerr << "Error opening Berkeley DB database. Name is null." << std::endl;
            }

            DbTxn *txn;
            //open the database
            try
            {

                //redirect debugging information to std::cerr
                database_.set_error_stream(&std::cerr);

                //get a transaction and open the database
                myDbEnvPtr_->getDbEnv()->txn_begin(0, &txn, 0);
                database_.set_error_stream( &std::cerr );
                database_.open(txn, dbName_.c_str(), 0, DB_BTREE, cFlags_,0);
                txn->commit( 0 );
                dbIsOpen_ = true;
            }
            catch(DbException &e)
            {
                std::cerr << "Error opening database: " << dbName_ << std::endl;
                std::cerr << e.what() << std::endl;

                //Abort the transaction. All changes made to the database since the start
                //of this transaction are discarded.
                txn->abort();

                exit( -1 );
            }
            catch(std::exception &e)
            {
                std::cerr << "Error opening container: " << dbName_ << std::endl;
                std::cerr << e.what() << std::endl;

                //Abort the transaction. All changes made to the database since the start
                //of this transaction are discarded.
                txn->abort();

                exit(-1);

            }
        }

        inline Db &getDatabase() { return database_; }

        inline DbEnv &getDbEnv() { return *(myDbEnvPtr_->getDbEnv()); }

        inline const std::string &getDatabaseName() { return dbName_; }

        ~myDb()
        {
            if ( dbIsOpen_ )
            {
                try
                {
                    database_.close( 0 );
                }
                catch(DbException &e)
                {
                    std::cerr << "Error closing database: " << dbName_ << std::endl;
                    std::cerr << e.what() << std::endl;
                }
                catch(std::exception &e)
                {
                    std::cerr << "Error closing database: " << dbName_ << std::endl;
                    std::cerr << e.what() << std::endl;
                }
            }
        }

    private:
        DbXml::XmlManager *myDbEnvPtr_;
        Db database_;
        std::string dbName_;
        u_int32_t cFlags_;
        bool dbIsOpen_;
};

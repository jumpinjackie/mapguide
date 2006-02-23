#include "gdal.h"
#include "gdal_alg.h"
#include "cpl_multiproc.h"

static int nThreadCount = 4, nIterations = 1, bLockOnOpen = TRUE;
static volatile int nPendingThreads = 0;
static const char *pszFilename = NULL;
static int nChecksum = 0;

static void *pGlobalMutex = NULL;

static void WorkerFunc( void * );

/************************************************************************/
/*                               Usage()                                */
/************************************************************************/

static void Usage()
{
    printf( "multireadtest [-nlo] [-t <thread#>] [-i <iterations>] filename\n" );
    exit( 1 );
}


/************************************************************************/
/*                                main()                                */
/************************************************************************/

int main( int argc, char ** argv )

{
    int iArg;

/* -------------------------------------------------------------------- */
/*      Process arguments.                                              */
/* -------------------------------------------------------------------- */
    argc = GDALGeneralCmdLineProcessor( argc, &argv, 0 );
    if( argc < 1 )
        exit( -argc );

    for( iArg = 1; iArg < argc; iArg++ )
    {
        if( EQUAL(argv[iArg],"-i") && iArg < argc-1 )
            nIterations = atoi(argv[++iArg]);
        else if( EQUAL(argv[iArg],"-t") && iArg < argc-1 )
            nThreadCount = atoi(argv[++iArg]);
        else if( EQUAL(argv[iArg],"-nlo") )
            bLockOnOpen = FALSE;
        else if( pszFilename == NULL )
            pszFilename = argv[iArg];
        else
        {
            printf( "Unrecognised argument: %s\n", argv[iArg] );
            Usage();
        }
    }

    if( pszFilename == NULL )
    {
        printf( "Need a file to operate on.\n" );
        Usage();
        exit( 1 );
    }

/* -------------------------------------------------------------------- */
/*      Get the checksum of band1.                                      */
/* -------------------------------------------------------------------- */
    GDALDatasetH hDS;

    GDALAllRegister();
    hDS = GDALOpen( pszFilename, GA_ReadOnly );
    if( hDS == NULL )
        exit( 1 );

    nChecksum = GDALChecksumImage( GDALGetRasterBand( hDS, 1 ), 
                                   0, 0, 
                                   GDALGetRasterXSize( hDS ), 
                                   GDALGetRasterYSize( hDS ) );
    
    GDALClose( hDS );

    printf( "Got checksum %d, launching %d worker threads on %s, %d iterations.\n", 
            nChecksum, nThreadCount, pszFilename, nIterations );

/* -------------------------------------------------------------------- */
/*      Fire off worker threads.                                        */
/* -------------------------------------------------------------------- */
    int iThread;

    pGlobalMutex = CPLCreateMutex();
    CPLReleaseMutex( pGlobalMutex );

    nPendingThreads = nThreadCount;

    for( iThread = 0; iThread < nThreadCount; iThread++ )
    {
        if( CPLCreateThread( WorkerFunc, NULL ) == -1 )
        {
            printf( "CPLCreateThread() failed.\n" );
            exit( 1 );
        }
    }

    while( nPendingThreads > 0 )
        CPLSleep( 0.5 );

    CPLReleaseMutex( pGlobalMutex );

    printf( "All threads complete.\n" );
}


/************************************************************************/
/*                             WorkerFunc()                             */
/************************************************************************/

static void WorkerFunc( void * )

{
    GDALDatasetH hDS;
    int iIter;

    if( bLockOnOpen )
        CPLAcquireMutex( pGlobalMutex, 100.0 );

    hDS = GDALOpen( pszFilename, GA_ReadOnly );

    if( bLockOnOpen )
        CPLReleaseMutex( pGlobalMutex );

    for( iIter = 0; iIter < nIterations && hDS != NULL; iIter++ )
    {
        int nMyChecksum;
        
        nMyChecksum = GDALChecksumImage( GDALGetRasterBand( hDS, 1 ), 
                                         0, 0, 
                                         GDALGetRasterXSize( hDS ), 
                                         GDALGetRasterYSize( hDS ) );

        if( nMyChecksum != nChecksum )
        {
            printf( "Checksum error in worker thread.\n" );
            break;
        }
    }

    if( hDS )
    {
        if( bLockOnOpen )
            CPLAcquireMutex( pGlobalMutex, 100.0 );
        GDALClose( hDS );
        if( bLockOnOpen )
            CPLReleaseMutex( pGlobalMutex );
    }

    CPLAcquireMutex( pGlobalMutex, 100.0 );
    nPendingThreads--;
    CPLReleaseMutex( pGlobalMutex );
}

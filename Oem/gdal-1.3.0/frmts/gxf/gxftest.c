#include "gxfopen.h"

int main( int argc, char ** argv )

{
    GXFHandle	hGXF;
    int		nXSize, nYSize, nSense, iScanline;
    CPLErr	eErr = CE_None;
    double	*padfLineBuf;
    char	*pszProjection;
    double	dfXOrigin, dfYOrigin, dfXSize, dfYSize, dfRotation;

    if( argc < 2 )
    {
        printf( "Usage: gxftest target.gxf\n" );
        exit( 1 );
    }
    
    hGXF = GXFOpen( argv[1] );

    if( hGXF == NULL )
        exit( 10 );
    
    GXFGetRawInfo( hGXF, &nXSize, &nYSize, &nSense, NULL, NULL, NULL );

    printf( "nXSize = %d, nYSize = %d, nSense = %d\n",
            nXSize, nYSize, nSense );

    pszProjection = GXFGetMapProjectionAsPROJ4( hGXF );
    
    printf( "Projection (PROJ.4): %s\n", pszProjection );

    CPLFree( pszProjection );

    pszProjection = GXFGetMapProjectionAsOGCWKT( hGXF );
    
    printf( "Projection (OGC WKT): %s\n", pszProjection );

    CPLFree( pszProjection );

    GXFGetRawPosition( hGXF, &dfXOrigin, &dfYOrigin, &dfXSize, &dfYSize,
                       &dfRotation );

    printf( "Raw Position: Origin=(%g,%g) PixelSize=(%g,%g) Rot=%g\n",
            dfXOrigin, dfYOrigin, dfXSize, dfYSize, dfRotation );
    
    
    GXFGetPROJ4Position( hGXF, &dfXOrigin, &dfYOrigin, &dfXSize, &dfYSize,
                       &dfRotation );

    printf( "PROJ4 Position: Origin=(%g,%g) PixelSize=(%g,%g) Rot=%g\n",
            dfXOrigin, dfYOrigin, dfXSize, dfYSize, dfRotation );
    
    
    padfLineBuf = (double *) CPLMalloc(sizeof(double)*nXSize);
    
    for( iScanline = 0; iScanline < nYSize && eErr == CE_None; iScanline++ )
    {
#ifdef DBMALLOC
        malloc_chain_check(1);
#endif    
        eErr = GXFGetScanline( hGXF, iScanline, padfLineBuf );

        if( eErr != CE_None )
            printf( "Error %d\n", eErr );

        printf( "Scanline %d = %g, %g, ... %g, %g\n",
                iScanline,
                padfLineBuf[0], padfLineBuf[1],
                padfLineBuf[nXSize-2], padfLineBuf[nXSize-1] );
    }

    CPLFree( padfLineBuf );

    GXFClose( hGXF );

#ifdef DBMALLOC
    malloc_dump(1);
#endif

    exit( 0 );
}

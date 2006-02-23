/******************************************************************************
 * $Id: ColorTable.i,v 1.3 2005/08/04 19:16:35 kruland Exp $
 *
 * Name:     ColorTable.i
 * Project:  GDAL Python Interface
 * Purpose:  GDAL Core SWIG Interface declarations.
 * Author:   Kevin Ruland, kruland@ku.edu
 *

 *
 * $Log: ColorTable.i,v $
 * Revision 1.3  2005/08/04 19:16:35  kruland
 * Clone() returns a newobject.  And changed some whitespace.
 *
 * Revision 1.2  2005/02/22 23:33:07  kruland
 * Implement GetCount, and GetColorTableEntry correctly.
 *
 * Revision 1.1  2005/02/15 21:37:43  kruland
 * Interface definition for ColorTable object.  Cut&Paste from gdal_priv.h.
 *
 *
*/

//************************************************************************
//
// Define the extensions for ColorTable (nee GDALColorTable)
//
//************************************************************************
%rename (ColorTable) GDALColorTable;

typedef int GDALPaletteInterp;

class GDALColorTable
{
public:
    GDALColorTable( GDALPaletteInterp = GPI_RGB );
    ~GDALColorTable();

%newobject Clone();
    GDALColorTable *Clone() const;

    GDALPaletteInterp GetPaletteInterpretation() const;

%rename (GetCount) GetColorEntryCount;
    int           GetColorEntryCount() const;

    GDALColorEntry* GetColorEntry(int);
    int           GetColorEntryAsRGB( int, GDALColorEntry * ) const;
    void          SetColorEntry( int, const GDALColorEntry * );

/* NEEDED 
 *
 * __str__;
 * serialize();
 */
};

#ifndef DESKTOP_TILE_DEFS_H
#define DESKTOP_TILE_DEFS_H

#include "MgDesktop.h"

class MG_DESKTOP_API MgdTileParameters
{
INTERNAL_API:
    static INT32 tileWidth;         // width for all tiles
    static INT32 tileHeight;        // height for all tiles
    static const INT32 tileDPI;     // assumed DPI for all tiles
    static STRING tileFormat;       // image format for all tiles
};

#endif
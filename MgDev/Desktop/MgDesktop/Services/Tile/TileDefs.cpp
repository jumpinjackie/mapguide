#include "TileDefs.h"

///////////////////////////////////////////////////////////////////////////////
/// Tile parameters.
///
/// NOTE: Changes to tileDPI must be accounted for in the HTML/AJAX viewer.
///       tileWidth and tileHeight are requested by the viewer during initialization.
///       The AJAX viewer supports both JPG and PNG tiles.
///
INT32  MgTileParameters::tileWidth  = 300;                    // width for all tiles
INT32  MgTileParameters::tileHeight = 300;                    // height for all tiles
const INT32  MgTileParameters::tileDPI    =  96;              // assumed DPI for all tiles
STRING MgTileParameters::tileFormat = L"PNG";    // image format for all tiles

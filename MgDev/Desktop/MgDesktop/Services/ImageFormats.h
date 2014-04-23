#ifndef DESKTOP_IMAGE_FORMATS_H
#define DESKTOP_IMAGE_FORMATS_H

#include "MgDesktop.h"
/// \ingroup Desktop_Misc_Module
/// \{
class MG_DESKTOP_API MgdImageFormats
{
PUBLISHED_API:
    /// \internal
    /// The "value(xxx)" comments are used by SWIG to build constants.php.  Do not alter them.

    /// Gif format
    static const STRING Gif;    /// \if INTERNAL value("GIF") \endif

    /// Jpeg format
    static const STRING Jpeg;   /// \if INTERNAL value("JPG") \endif

    /// Png format
    static const STRING Png;    /// \if INTERNAL value("PNG") \endif

    /// Png format
    static const STRING Png8;   /// \if INTERNAL value("PNG8") \endif

    /// Tiff format
    static const STRING Tiff;   /// \if INTERNAL value("TIF") \endif

    /// Raw format
    static const STRING Raw;    /// \if INTERNAL value("RAW") \endif
};
/// \}
#endif
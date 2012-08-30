#ifndef DESKTOP_UNMANAGED_DATA_TYPE_H
#define DESKTOP_UNMANAGED_DATA_TYPE_H
/// \ingroup Desktop_Misc_Module
/// \{
class MG_DESKTOP_API MgdUnmanagedDataType 
{
PUBLISHED_API:
    /// \internal
    /// The "value(xxx)" comments are used by SWIG to build constants.php.  Do not alter them.

    /// Folders
    static const STRING Folders;    /// \if INTERNAL value("Folders") \endif

    /// Files
    static const STRING Files;   /// \if INTERNAL value("Files") \endif

    /// Both Folders and Files
    static const STRING Both;    /// \if INTERNAL value("Both") \endif
};
/// \}
#endif
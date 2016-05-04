//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef _MGCOORDINATESYSTEMCATALOG_H_
#define _MGCOORDINATESYSTEMCATALOG_H_

class MgCoordinateSystemDictionary;
class MgCoordinateSystemDatumDictionary;
class MgCoordinateSystemEllipsoidDictionary;
class MgCoordinateSystemCategoryDictionary;
class MgCoordinateSystemGeodeticTransformation;
class MgCoordinateSystemGeodeticPathDictionary;
class MgCoordinateSystemGeodeticTransformDefDictionary;
class MgCoordinateSystemMathComparator;
class MgCoordinateSystemFormatConverter;
class MgCoordinateSystemProjectionInformation;
class MgCoordinateSystemUnitInformation;
class MgCoordinateSystemTransform;

enum LibraryStatus
{
    lsInitialized    = 0,
    lsInitializationFailed,
    lsLoadFailed
};

///////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Defines and manages the physical location of coordinate system data files
///
class MgCoordinateSystemCatalog : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemCatalog)

PUBLISHED_API:

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the default dictionary path and file names
    ///
    virtual void SetDefaultDictionaryDirAndFileNames()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the default dictionary path
    ///
    /// \return
    /// The default dictionary path
    ///
    virtual STRING GetDefaultDictionaryDir()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the path where the dictionary files are located
    ///
    /// \remarks
    /// If set to a valid path, this path is used to read dictionary files instead of the default dictionary path
    /// and the one specified by the MENTOR_DICTIONARY_PATH environment variable
    ///
    virtual void SetDictionaryDir(CREFSTRING sDirPath)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the current dictionary path
    ///
    /// \return
    /// The current dictionary path
    ///
    virtual STRING GetDictionaryDir()=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the default directory path where user defined coordinate system
    /// definitions can be found or can be written into, respectively.
    /// If set, this method returns the value of the MENTOR_USER_DICTIONARY_PATH
    /// environment variable. Returns 'CSIDL_LOCAL_APPDATA\Autodesk\User Geospatial Coordinate Systems'
    /// on Windows systems or an empty string on non-Windows systems, otherwise.
    ///
    /// \remarks
    /// The directory returned by this function is not guaranteed to exist. If it does, however,
    /// and is writable, too, the value returned by this function will be used to initialize this catalog of definitions
    /// on the first initialization.
    ///
    virtual STRING GetDefaultUserDictionaryDir()=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the actual directory where user defined coordinate system
    /// definitions can be found or can be written into. The directory set through this method must exist
    /// and writeable. If set to an empty string, the catalog's 'user defined coordinate system' directory
    /// will be unset and any updates will again happen only inside the directory
    /// as returned by GetDictionaryDir().
    ///
    virtual void SetUserDictionaryDir(CREFSTRING sDirPath)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the current directory where user defined coordinate system
    /// definitions is obtained from or is written into. Returns an empty string
    /// if no such directory is currently configured for this catalog.
    ///
    virtual STRING GetUserDictionaryDir()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the protection mode
    ///
    /// \param nMode (short/int)
    /// The protection mode
    ///
    virtual void SetProtectionMode(INT16 nMode)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the protection mode of this catalog
    ///
    /// \return
    /// The protection mode of this catalog
    ///
    virtual INT16 GetProtectionMode()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system category dictionary
    ///
    /// \return
    /// The coordinate system category dictionary
    ///
    virtual MgCoordinateSystemCategoryDictionary* GetCategoryDictionary()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system dictionary
    ///
    /// \return
    /// The coordinate system dictionary
    ///
    virtual MgCoordinateSystemDictionary* GetCoordinateSystemDictionary()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system datum dictionary
    ///
    /// \return
    /// The coordinate system datum dictionary
    ///
    virtual MgCoordinateSystemDatumDictionary* GetDatumDictionary()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system ellipsoid dictionary
    ///
    /// \return
    /// The coordinate system ellipsoid dictionary
    ///
    virtual MgCoordinateSystemEllipsoidDictionary* GetEllipsoidDictionary()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system geodetic path dictionary
    ///
    /// \return
    /// The coordinate system geodetic path dictionary
    ///
    virtual MgCoordinateSystemGeodeticPathDictionary* GetGeodeticPathDictionary()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system geodetic transform definition dictionary
    ///
    /// \return
    /// The coordinate system geodetic transform definition dictionary
    ///
    virtual MgCoordinateSystemGeodeticTransformDefDictionary* GetGeodeticTransformDefDictionary()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the geodetic transformations for the source and target coordinate system datums
    ///
    /// \param pSource (MgCoordinateSystemDatum)
    /// The source coordinate system datum
    /// \param pTarget (MgCoordinateSystemDatum)
    /// The target coordinate system datum
    ///
    /// \return
    /// The collection of MgCoordinateSystemGeodeticTransformation objects
    ///
    virtual MgDisposableCollection* GetGeodeticTransformations(MgCoordinateSystemDatum* pSource, MgCoordinateSystemDatum *pTarget)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system math comparator
    ///
    /// \return
    /// The coordinate system math comparator
    ///
    virtual MgCoordinateSystemMathComparator* GetMathComparator()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system format converter
    ///
    /// \return
    /// The coordinate system format converter
    ///
    virtual MgCoordinateSystemFormatConverter* GetFormatConverter()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system projection information
    ///
    /// \return
    /// The coordinate system projection information
    ///
    virtual MgCoordinateSystemProjectionInformation* GetProjectionInformation()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system unit information
    ///
    /// \return
    /// The coordinate system unit information
    ///
    virtual MgCoordinateSystemUnitInformation* GetUnitInformation()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system dictionary utility
    ///
    /// \return
    /// The coordinate system dictionary utility
    ///
    virtual MgCoordinateSystemDictionaryUtility* GetDictionaryUtility()=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether any kind of definition supported by this catalog can be updated in the dictionary files currently in use.
    ///
    /// Depending on whether this catalog uses a specific directory to store user-provided definitions in, this method
    /// will perform the following checks:
    /// \li <b>No specific directory supplied:</b> The target directory is verified to contain all required CSD files.
    /// If all have been found, they are verified to be writable. If not all CSD files have been found or
    /// if at least one CSD files cannot be written to, this method will return false.
    /// \li <b>Specific directory supplied:</b> The target directory is verified to exist and that it is writable, i.e.
    /// new files can be created in it. Any existing CSD files are assumed to be writeable then, too.
    ///
    /// \return Returns true, if definitions can be updated, incl. adding and deletion, as per the definition above.
    /// Otherwise false.
    /// \remarks
    /// Note, that even if this method returns true, the caller must be prepared that the actual
    /// update request of a definition might still fail.
    virtual bool AreDictionaryFilesWritable()=0;

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the library status.
    ///
    virtual LibraryStatus GetLibraryStatus()=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Prepares the catalog for disposal.
    ///
    virtual void PrepareForDispose()=0;

protected:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId(){return m_cls_id;};

CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemCatalog;
};

#endif //_MGCOORDINATESYSTEMCATALOG_H_

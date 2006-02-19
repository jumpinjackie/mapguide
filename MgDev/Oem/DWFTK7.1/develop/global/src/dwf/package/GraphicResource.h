//
//  Copyright (c) 1996-2005 by Autodesk, Inc.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE CORRECTNESS
//  OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE IT. AUTODESK
//  PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY DISCLAIMS ANY
//  LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL DAMAGES FOR ERRORS,
//  OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject to
//  restrictions set forth in FAR 52.227-19 (Commercial Computer Software
//  Restricted Rights) and DFAR 252.227-7013(c)(1)(ii) (Rights in Technical
//  Data and Computer Software), as applicable.
//


#ifndef _DWFTK_GRAPHIC_RESOURCE_H
#define _DWFTK_GRAPHIC_RESOURCE_H

///
///\file        dwf/package/GraphicResource.h
///\brief       This file contains the DWFGraphicResource class declaration.
///


#include "dwf/Toolkit.h"
#include "dwf/package/Resource.h"

namespace DWFToolkit
{

///
///\ingroup     dwfpackage
///
///\class       DWFGraphicResource   dwf/package/GraphicResource.h     "dwf/package/GraphicResource.h"
///\brief       This class represents a persistent stream of visualization data.
///\since       7.0.1
///
///             A graphic resource encapsulates visual vector or raster data.
///
class DWFGraphicResource : public DWFResource
{

public:

    ///
    ///         ???
    ///
    typedef enum
    {
        eNotSpecified,

        eAlwaysInSync,
        eAlwaysDifferent,
        eDecoupled

    } teOrientation;

public:

    ///
    ///         Constructor
    ///
    ///         This constructor is generally only used by the 
    ///         parsing process when the DWFPackageReader associated with 
    ///         (and providing read access to) the DWF package file is available.
    ///         The subsequent binding makes it possible to read resource content
    ///         data from the DWF package.
    ///
    ///\param   pPackageReader  Provides access to resource content in the DWF package.
    ///\throw   None
    ///
    _DWFTK_API
    DWFGraphicResource( DWFPackageReader* pPackageReader )
        throw();

    ///
    ///         Constructor
    ///
    ///         This constructor is generally only used by the 
    ///         parsing process when the DWFPackageReader associated with 
    ///         (and providing read access to) the DWF package file is available.
    ///         The subsequent binding makes it possible to read resource content
    ///         data from the DWF package.
    ///
    ///\param   zTitle              The friendly/display name of the resource.
    ///\param   zRole               Describes how the resource participates in the visualization.
    ///\param   zMIME               The data MIME type of the resource content.
    ///\param   zAuthor             An optional field for the individual or application that created the original graphics data set.    ///\param   zDescription        An optional field for detailed information about the graphics.    ///\param   zCreationTime       An optional field for indicating the origin of the graphics data.
    ///\param   zModificationTime   An optional field for indicating the last time the graphics data was changed (should be in GMT with timezone format.)
    ///                             prior to its inclusion in the DWF package (should be in GMT with timezone format.)
    ///\throw   None
    ///
    _DWFTK_API
    DWFGraphicResource( const DWFString& zTitle,
                        const DWFString& zRole,
                        const DWFString& zMIME,
                        const DWFString& zAuthor = L"",
                        const DWFString& zDescription = L"",
                        const DWFString& zCreationTime = L"",
                        const DWFString& zModificationTime = L"" )
        throw();

    ///
    ///         Destructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    virtual ~DWFGraphicResource()
        throw();

    ///
    ///         Returns a flag indicating whether or not this graphic data should
    ///         be displayed or rendered by default.
    ///
    ///\return  If \e true, the graphical content should be displayed by default.
    ///\throw   None
    /// 
    _DWFTK_API
    bool show() const
        throw()
    {
        return _bShow;
    }

    ///
    ///         Returns the relative z-ordering (display priority) for the graphic data.
    ///
    ///\return  The z-order index.
    ///\throw   None
    ///
    _DWFTK_API
    int zOrder() const
        throw()
    {
        return _nZOrder;
    }

    ///
    ///         Returns the path by which the graphic data should be clipped.
    ///
    ///\return  The clipping path. This pointer should not be deleted by the caller.
    ///\throw   None
    ///
    _DWFTK_API
    const double* const clip() const
        throw()
    {
        return (const double* const)_anClip;
    }

    ///
    ///         Returns the geometric bounds of the graphics data.
    ///
    ///\return  The bounding geometry.  This pointer should not be deleted by the caller.
    ///\throw   None
    ///
    _DWFTK_API
    const double* const extents() const
        throw()
    {
        return (const double* const)_anExtents;
    }

    ///
    ///         Returns the 4x4 matrix used to translate the data into
    ///         a section-specific target space (paper, units, etc.)
    ///
    ///\return  The 4x4 transformation matrix.  This pointer should not be deleted by the caller.
    ///\throw   None
    ///
    _DWFTK_API
    const double* const transform() const
        throw()
    {
        return (const double* const)_anTransform;
    }

    ///
    ///         ???
    ///
    _DWFTK_API
    int effectiveResolution() const
        throw()
    {
        return _nEffectiveResolution;
    }

    ///
    ///         Returns the optional field for the individual or application that created the original graphics data set.
    ///
    ///\return  The author.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& author() const
        throw()
    {
        return _zAuthor;
    }

    ///
    ///         Returns the optional field detailed information about the graphics.
    ///
    ///\return  The description.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& description() const
        throw()
    {
        return _zDescription;
    }

    ///
    ///         Returns the optional field indicating the origin of the graphics data.
    ///
    ///\return  The creation time.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& creationTime() const
        throw()
    {
        return _zCreationTime;
    }

    ///
    ///         Returns the optional field indicating the last modification of the graphics data.
    ///
    ///\return  The last modification time.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& modificationTime() const
        throw()
    {
        return _zModificationTime;
    }

    ///
    ///         Returns an enumeration indicating the desired orientation of the graphic on the target.
    ///
    ///\return  The orientation enumeration.
    ///\throw   None
    ///
    _DWFTK_API
    teOrientation orientation() const
        throw()
    {
        return _eOrientation;
    }

    ///
    ///         Sets the visualization parameters for the graphic data.
    ///
    ///\param   anTransform             The 4x4 matrix used to translate the data into
    ///                                 a section-specific target space (paper, units, etc.)
    ///                                 These data will be copied.
    ///\param   anExtents               The geometric bounds of the graphics data.
    ///                                 These data will be copied.
    ///\param   anClip                  The path by which the graphic data should be clipped.
    ///                                 These data will be copied.
    ///\param   bShow                   A flag indicating whether or not this graphic data should
    ///                                 be displayed or rendered by default.
    ///\param   nZOrder                 The relative z-ordering (display priority) for the graphic data.
    ///\param   nEffectiveResolution    ???
    ///\param   eOrientation            An enumeration indicating the desired orientation of the graphic on the target.
    ///\throw   DWFException
    ///
    _DWFTK_API
    void configureGraphic( const double*    anTransform,
                           const double*    anExtents = NULL,
                           const double*    anClip = NULL,
                           bool             bShow = true,
                           int              nZOrder = 0,
                           int              nEffectiveResolution = 0,
                           teOrientation    eOrientation = eNotSpecified )
        throw( DWFException );

    ///
    ///\copydoc DWFXMLBuildable::parseAttributeList()
    ///
    _DWFTK_API
    virtual void parseAttributeList( const char** ppAttributeList )
        throw( DWFException );

#ifndef DWFTK_READ_ONLY

    ///
    ///\copydoc DWFXMLSerializable::serializeXML()
    ///
    _DWFTK_API
    virtual void serializeXML( DWFXMLSerializer& rSerializer, unsigned int nFlags )
        throw( DWFException );

#endif


protected:

    bool    _bShow;
    int     _nZOrder;
    double  _anClip[4];
    double  _anExtents[4];
    double  _anTransform[4][4];
    int     _nEffectiveResolution;
    
    DWFString       _zAuthor;
    DWFString       _zDescription;
    DWFString       _zCreationTime;
    DWFString       _zModificationTime;
    teOrientation   _eOrientation;

private:

    DWFGraphicResource( const DWFGraphicResource& );
    DWFGraphicResource& operator=( const DWFGraphicResource& );
};

///
///\ingroup     dwfpackage
///
///\class       DWFImageResource   dwf/package/GraphicResource.h     "dwf/package/GraphicResource.h"
///\brief       This class represents a persistent stream of image data.
///\since       7.0.1
///
class DWFImageResource : public DWFGraphicResource
{

public:

    ///
    ///         Constructor
    ///
    ///         This constructor is generally only used by the 
    ///         parsing process when the DWFPackageReader associated with 
    ///         (and providing read access to) the DWF package file is available.
    ///         The subsequent binding makes it possible to read resource content
    ///         data from the DWF package.
    ///
    ///\param   pPackageReader  Provides access to resource content in the DWF package.
    ///\throw   None
    ///
    _DWFTK_API
    DWFImageResource( DWFPackageReader* pPackageReader )
        throw();

    ///
    ///         Constructor
    ///
    ///         This constructor is generally only used by the 
    ///         parsing process when the DWFPackageReader associated with 
    ///         (and providing read access to) the DWF package file is available.
    ///         The subsequent binding makes it possible to read resource content
    ///         data from the DWF package.
    ///
    ///\param   zTitle              The friendly/display name of the resource.
    ///\param   zRole               Describes how the resource participates in the visualization.
    ///\param   zMIME               The data MIME type of the resource content.
    ///\param   zAuthor             An optional field for the individual or application that created the original graphics data set.  \param   zDescription        An optional field for detailed information about the graphics.    \param   zCreationTime       An optional field for indicating the origin of the graphics data.
    ///\param   zModificationTime   An optional field for indicating the last time the graphics data was changed (should be in GMT with timezone format.)
    ///                             prior to its inclusion in the DWF package (should be in GMT with timezone format.)
    ///\throw   None
    ///
    _DWFTK_API
    DWFImageResource( const DWFString& zTitle,
                      const DWFString& zRole,
                      const DWFString& zMIME,
                      const DWFString& zAuthor = L"",
                      const DWFString& zDescription = L"",
                      const DWFString& zCreationTime = L"",
                      const DWFString& zModificationTime = L"" )
        throw();

    ///
    ///         Destructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    virtual ~DWFImageResource()
        throw();

    ///
    ///         Returns a flag indicating that this image data was acquired from a scanner.
    ///
    ///\return  If \e true, this is scanned image data.
    ///\throw   None
    ///
    _DWFTK_API
    bool scanned() const
        throw()
    {
        return _bScanned;
    }

    ///
    ///         Returns a flag indicating that the color data should be inverted
    ///         as appropriate for the image type.
    ///
    ///         RGB data should be flipped to BGR.
    ///         Bitonal data should be inverted Black to White or non-Black (depending on the imaging flags.)
    ///\throw   None
    ///
    _DWFTK_API
    bool invertColors() const
        throw()
    {
        return _bInvertColors;
    }

    ///
    ///         The number of bits per image pixel.
    ///
    ///\return  The bits per pixel.
    ///\throw   None
    ///
    _DWFTK_API
    unsigned char colorDepth() const
        throw()
    {
        return _nColorDepth;
    }

    ///
    ///         If applicable, this method returns the resolution
    ///         (in DPI) at which the original document was scanned
    ///         in order to create this image resource.
    ///
    ///\return  The scanner DPI.
    ///\throw   None
    ///
    _DWFTK_API
    int scannedResolution() const
        throw()
    {
        return _nScannedResolution;
    }

    ///
    ///         If applicable, this method returns the extents
    ///         of the original document.
    ///
    ///\return  An array of four (4) values.  This pointer must not be deleted by the caller.
    ///\throw   None
    ///
    _DWFTK_API
    const double* const originalExtents() const
        throw()
    {
        return (const double* const)_anOriginalExtents;
    }

    ///
    ///         Sets the visualization parameters for the image data.
    ///
    ///         This method should be called <b>in addition to</b> \a configureGraphic().
    ///
    ///\param   nColorDepth             The number of bits per image pixel.
    ///\param   bInvertColors           Indicates that the color data should be inverted
    ///                                 as appropriate for the image type.
    ///\param   bScannedImage           Indicates that this image data was acquired from a scanner.
    ///\param   nScannedResolution      The DPI at which the original document was scanned.
    ///\param   anOriginalExtents       The extents of the original scanned document.
    ///                                 These data will be copied.
    ///\throw   DWFException
    ///
    _DWFTK_API
    void configureImage( unsigned char  nColorDepth,
                         bool           bInvertColors = false,
                         bool           bScannedImage = false,
                         int            nScannedResolution = 0,
                         const double*  anOriginalExtents = NULL )
        throw( DWFException );

    ///
    ///\copydoc DWFXMLBuildable::parseAttributeList()
    ///
    _DWFTK_API
    virtual void parseAttributeList( const char** ppAttributeList )
        throw( DWFException );

#ifndef DWFTK_READ_ONLY

    ///
    ///\copydoc DWFXMLSerializable::serializeXML()
    ///
   _DWFTK_API
     void serializeXML( DWFXMLSerializer& rSerializer, unsigned int nFlags )
        throw( DWFException );

#endif


private:

    bool            _bScanned;
    bool            _bInvertColors;
    unsigned char   _nColorDepth;
    int             _nScannedResolution;
    double          _anOriginalExtents[4];

private:

    DWFImageResource( const DWFImageResource& );
    DWFImageResource& operator=( const DWFImageResource& );
};

}

#endif

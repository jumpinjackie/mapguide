#ifndef DESKTOP_RENDERING_OPTIONS_H
#define DESKTOP_RENDERING_OPTIONS_H

#include "MgDesktop.h"

/// \ingroup Desktop_Misc_Module
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Options for Rendering Service.
///
class MG_DESKTOP_API MgdRenderingOptions: public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgdRenderingOptions)

PUBLISHED_API:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Rendering behavior values.
    /// \n
    /// \c RenderSelection - Renders the selected feature(s).
    /// \n
    /// \c RenderLayers - Renders the dynamic layers on the map.
    /// \n
    /// \c KeepSelection - Renders the selected feature(s) even if they are outside the current scale.
    /// \n
    /// \c RenderLayers - Renders the base layers on the map
    enum
    {
        RenderSelection = 1,
        RenderLayers = 2,
        KeepSelection = 4,
        RenderBaseLayers = 8,
    };

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgdRenderingOptions object
    ///
    /// \param format
    /// Input
    /// Image format. Defines the format of the resulting image
    /// \param behavior
    /// Input
    /// A bitmask with the rendering behavior values
    /// \param selectionColor
    /// Input
    /// Selection color, including filling color and boundary color
    ///
    MgdRenderingOptions(CREFSTRING format, INT32 behavior, MgColor* selectionColor);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the image format
    ///
    /// \return
    /// The string value
    ///
    STRING GetImageFormat();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the rendering behavior
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetBehavior();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the selection color
    ///
    /// \return
    /// The MgColor object
    ///
    MgColor* GetSelectionColor();

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Default constructor
    ///
    MgdRenderingOptions();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Virtual destructor
    ///
    virtual ~MgdRenderingOptions();

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Dispose this object
    ///
    /// \return
    /// Nothing
    ///
    virtual void Dispose();

private:
    STRING m_format;
    INT32 m_behavior;
    Ptr<MgColor> m_selectionColor;
CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_RenderingService_RenderingOptions;
};
/// \}

#endif

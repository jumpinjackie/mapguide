#ifndef DESKTOP_RENDERING_OPTIONS_H
#define DESKTOP_RENDERING_OPTIONS_H

#include "MgDesktop.h"

/// \defgroup MgRenderingOptions MgRenderingOptions
/// \ingroup Rendering_Service_Module
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Options for Rendering Service.
///
class MG_DESKTOP_API MgRenderingOptions: public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgRenderingOptions)

PUBLISHED_API:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Rendering behavior values.
    /// \n
    /// \c RenderSelection - Renders the selected feature(s).
    /// \n
    /// \c RenderLayers - Renders the features on the map.
    /// \n
    /// \c KeepSelection - Renders the selected feature(s) even if they are outside the current scale.
    ///
    enum
    {
        RenderSelection = 1,
        RenderLayers = 2,
        KeepSelection = 4
    };

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgRenderingOptions object
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
    MgRenderingOptions(CREFSTRING format, INT32 behavior, MgColor* selectionColor);

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
    MgRenderingOptions();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Virtual destructor
    ///
    virtual ~MgRenderingOptions();

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

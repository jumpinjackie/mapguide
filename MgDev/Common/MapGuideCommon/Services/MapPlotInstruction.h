//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef MG_MAPPLOT_INSTRUCTION_H
#define MG_MAPPLOT_INSTRUCTION_H

/// \defgroup MgMapPlotInstruction MgMapPlotInstruction
/// \ingroup Mapping_Service_Module
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// MgMapPlotInstruction defines enumerated values used to indicate whether the plot
/// should be based on the center and scale of the map,
/// an overridden center and scale, or an overridden extent.
class MG_MAPGUIDE_API MgMapPlotInstruction
{
    DECLARE_CLASSNAME(MgMapPlotInstruction)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies the plot should use the center and scale of the map
    ///
    static const INT32 UseMapCenterAndScale = 0;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies the plot should use the overridden center and scale of the map
    ///
    static const INT32 UseOverriddenCenterAndScale = 1;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies the plot should use the overridden extent
    ///
    static const INT32 UseOverriddenExtent = 2;
};
/// \}

#endif

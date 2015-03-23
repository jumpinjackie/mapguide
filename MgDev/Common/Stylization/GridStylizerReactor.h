//
//  Copyright (C) 2005-2011 by Autodesk, Inc.
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

//#pragma once
#ifndef GRIDSTYLIZERREACTOR_H_
#define GRIDSTYLIZERREACTOR_H_

#include "Stylization.h"

///<summary>
/// Class for outer users to derive from.
/// Then they could customize the behavior when receiving the events.
/// For example, the outer users can create a progress control to show
/// the progress of the stylization transaction.
///</summary>
class GridStylizerReactor
{
public:
    STYLIZATION_API              GridStylizerReactor();

    STYLIZATION_API virtual      ~GridStylizerReactor();

    ///<summary>
    /// This function is called when stylizer begins to apply styles.
    ///</summary>
    STYLIZATION_API virtual void OnBeginApply(int begin, int end, int step);

    ///<summary>
    /// This function is called when stylizer is applying styles.
    /// When stylizer finishes one step, it calls this function to
    /// report the status to the outer users.
    ///</summary>
    ///<returns>
    /// Returns true to indicate the reactor handles this event successfully.
    /// If retruns false, means the stylization process encounters some unknown exceptions.
    /// such as users cancel it, etc. Then the stylizer will stop applying and call
    /// the reactor's OnCancelApply() function.
    ///</returns>
    STYLIZATION_API virtual bool OnStepApply(int position);

    ///<summary>
    /// This function is called when stylizer finishes to apply styles.
    ///</summary>
    STYLIZATION_API virtual void OnEndApply();

    ///<summary>
    /// This function is called when stylizer cancells applying styles successfully.
    ///</summary>
    STYLIZATION_API virtual void OnCancelApply();
};

#endif

//  $Header: //GBU/Metropolis/Main/Server/Common/Stylization/GridStylizerReactor.h#1 $
//
//  Copyright (C) 2005-2008 by Autodesk, Inc. All Rights Reserved.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE
//  CORRECTNESS OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE
//  IT. AUTODESK PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY
//  DISCLAIMS ANY LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL
//  DAMAGES FOR ERRORS, OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject
//  to restrictions set forth in FAR 52.227-19 (Commercial Computer
//  Software Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
//  (Rights in Technical Data and Computer Software), as applicable.
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

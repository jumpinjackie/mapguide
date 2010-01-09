//  $Header: //GBU/Metropolis/Main/Server/Common/Stylization/GridStylizerReactor.cpp#1 $
//
//  Copyright (C) 2005-2010 by Autodesk, Inc. All Rights Reserved.
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

#include "stdafx.h"
#include "GridStylizerReactor.h"


//
// GridStylizerReactor
//

GridStylizerReactor::GridStylizerReactor()
{
}

GridStylizerReactor::~GridStylizerReactor()
{
}

void GridStylizerReactor::OnBeginApply(int begin, int end, int step)
{
}

bool GridStylizerReactor::OnStepApply(int position)
{
    // The default behavior is returning true directly.
    // That indicates the reactor handles this event successfully.
    return true;
}

void GridStylizerReactor::OnEndApply()
{
}

void GridStylizerReactor::OnCancelApply()
{
}


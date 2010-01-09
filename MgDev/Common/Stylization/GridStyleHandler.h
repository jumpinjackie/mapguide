//  $Header: //GBU/Metropolis/Main/Server/Common/Stylization/GridStyleHandler.h#1 $
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

#ifndef GRIDSTYLEHANDLER_H_
#define GRIDSTYLEHANDLER_H_

// Forward declaration.
class GridData;
class Band;
class GridStatusReporter;

///<summary>
/// Class GridStyleHandler is the base class of all the grid style handlers.
/// This class provides the common interface to visit each pixel of the band and do
/// the different operations, such as to stylize the band.
///</summary>
class GridStyleHandler
{
public:
    ///<summary>
    /// Constructor.
    ///</summary>
    GridStyleHandler() {}

    ///<summary>
    /// Destructor.
    ///</summary>
    virtual ~GridStyleHandler() {}

    ///<summary>
    /// Function to clear the data info of the handler.
    ///</summary>
    virtual void Clear() = 0;

    ///<summary>
    /// Function to visit each pixel.
    ///</summary>
    virtual void Visit(unsigned int x, unsigned int y) = 0;

    ///<summary>
    /// Finished visiting all the pixels
    ///</summary>
    virtual void Finished(bool bSuccessful) {bSuccessful;}

    ///<summary>
    /// Set GridStatusReporter
    ///</summary>
    virtual void SetStatusReporter(GridStatusReporter *pReporter) = 0;

    ///<summary>
    /// Function to visit all pixels.
    ///</summary>
    virtual bool Visit() = 0;
};

#endif

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

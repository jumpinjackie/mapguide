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

#ifndef GRIDSTATUSREPORTER_H_
#define GRIDSTATUSREPORTER_H_

///<summary>
/// Class for the transaction to report its status.
///</summary>
class GridStatusReporter
{
public:
    ///<summary>
    /// This is a base class, and the constructor does nothing.
    /// It is just null implementation.
    ///</summary>

                 GridStatusReporter() {}

    ///<summary>
    /// This is a base class, and the destructor does nothing.
    /// It is just null implementation.
    ///</summary>
    virtual      ~GridStatusReporter() {}

    ///<summary>
    // Indicates the transaction has been started.
    ///</summary>
    virtual void Begin() = 0;

    ///<summary>
    // Indicates the transaction has been finished successfully.
    ///</summary>
    virtual void Commit() = 0;

    ///<summary>
    // Indicates the transaction encountered some error and has been terminated.
    ///</summary>
    virtual void Rollback() = 0;

    ///<summary>
    // Indicates the transaction has been finished one step.
    // And checks the returned value.
    // If the returned value is true, that means the step() function has been handled successfully.
    // Otherwise, false means encountering some exceptions.
    ///</summary>
    virtual bool Step(int stepSize = 1) = 0;
};

#endif

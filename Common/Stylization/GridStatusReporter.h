//  $Header: //GBU/Metropolis/Main/Server/Common/Stylization/GridStatusReporter.h#1 $
//
//  Copyright (C) 2005-2009 by Autodesk, Inc. All Rights Reserved.
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

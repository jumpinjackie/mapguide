//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef DATAVALUEPOOL_H_
#define DATAVALUEPOOL_H_

#include "Stylization.h"
#include "DataValueStack.h"

//forward declare
class DataValue;
class BooleanValue;
class DateTimeValue;
class DoubleValue;
class Int64Value;
class StringValue;


//Keeps around pools of DataValue object instances
//as quicker alternative to memory allocation/object creation
//during query execution, since we use and then throw away a lot
//of DataValue objects when executing queries for every single feature.
//In theory we could overload the new/delete of DataValues to use this pool
//but why not keep things simpler?
//Very rudimentary implementation, the user can shoot themselves in
//the foot if not careful when releasing (or not!) pooled objects:
//
//================================================================================
//How to Shoot Yourself In the Foot
//Developer's Insight, December 1991 (approx version)
//The proliferation of modern programming languages (all of which seem to have stolen
//countless features from one another) sometimes makes it difficult to remember what
//language you're currently using. This guide is offered as a public service to help
//programmers who find themselves in such dilemmas.
//
//C
//You shoot yourself in the foot.
//
//C++
//You accidently create a dozen instances of yourself and shoot them all in the foot.
//Providing emergency medical assistance is impossible since you can't tell which are
//bitwise copies and which are just pointing at others and saying "That's me, over there."
//
//FORTRAN
//You shoot yourself in each toe, iteratively, until you run out of toes, then you read
//in the next foot and repeat. If you run out of bullets, you continue anyway because you
//have no exception-handling facility.
//
//Modula-2
//After realizing that you can't actually accomplish anything in this language, you shoot
//yourself in the head.
//
//COBOL
//USEing a COLT 45 HANDGUN, AIM gun at LEG.FOOT, THEN place ARM.HAND.FINGER on HANDGUN.
//TRIGGER and SQUEEZE. THEN return HANDGUN to HOLSTER. CHECK whether shoelace needs to be
//retied.
//
//Lisp
//You shoot yourself in the appendage which holds the gun with which you shoot yourself
//in the appendage which holds the gun with which you shoot yourself in the appendage
//which holds...
//
//BASIC
//Shoot yourself in the foot with a water pistol. On big systems, continue until entire
//lower body is waterlogged.
//
//Forth
//Foot yourself in the shoot.
//
//APL
//You shoot yourself in the foot; then spend all day figuring out how to do it in fewer
//characters.
//
//Pascal
//The compiler won't let you shoot yourself in the foot.
//
//Snobol
//If you succeed, shoot yourself in the left foot. If you fail, shoot yourself in the right foot.
//
//HyperTalk
//Put the first bullet of the gun into foot left of leg of you. Answer the result.
//
//Prolog
//You tell your program you want to be shot in the foot. The program figures out how to do it,
//but the syntax doesn't allow it to explain.
//
//370 JCL
//You send your foot down to MIS with a 4000-page document explaining how you want it to be
//shot. Three years later, your foot comes back deep-fried.
//===========================================================================================

class DataValuePool
{
public:
    DataValuePool();
    virtual ~DataValuePool();

    Int64Value* ObtainInt64Value(long long val);
    DoubleValue* ObtainDoubleValue(double val);
    StringValue* ObtainStringValue(wchar_t* val, bool dispose = false);
    BooleanValue* ObtainBooleanValue(bool val);
    DateTimeValue* ObtainDateTimeValue(FdoDateTime val);
    void RelinquishDataValue(DataValue* dv);

private:
    void RelinquishBooleanValue(BooleanValue* dv);
    void RelinquishStringValue(StringValue* dv);
    void RelinquishDoubleValue(DoubleValue* dv);
    void RelinquishInt64Value(Int64Value* dv);
    void RelinquishDateTimeValue(DateTimeValue* dv);

    DataValueStack<Int64Value> m_int64Pool;
    DataValueStack<DoubleValue> m_doublePool;
    DataValueStack<StringValue> m_stringPool;
    DataValueStack<BooleanValue> m_boolPool;
    DataValueStack<DateTimeValue> m_datePool;
};

#endif

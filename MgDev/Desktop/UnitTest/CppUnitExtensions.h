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

#ifndef _CPPUNIT_EXTENSIONS_H
#define _CPPUNIT_EXTENSIONS_H

/** Asserts that the given expression throws an exception of the specified type.
 * \ingroup Assertions
 * Example of usage:
 * \code
 *   std::out_of_range exception;
 *   std::vector<int> v;
 *  CPPUNIT_ASSERT_THROW_EX( v.at( 50 ), std::out_of_range, exception );
 * \endcode
 */
# define CPPUNIT_ASSERT_THROW_EX( expression, ExceptionType, exception )    \
   do {                                                                     \
      bool cpputExceptionThrown_ = false;                                   \
      try {                                                                 \
         expression;                                                        \
      } catch ( ExceptionType & ex_) {                                      \
         cpputExceptionThrown_ = true;                                      \
         exception = ex_;                                                   \
      }                                                                     \
                                                                            \
      if ( cpputExceptionThrown_ )                                          \
         break;                                                             \
                                                                            \
      CPPUNIT_NS::Asserter::fail(                                           \
                     "Expected exception: " #ExceptionType                  \
                     " not thrown.",                                        \
                     CPPUNIT_SOURCELINE() );                                \
   } while ( false )


/** Asserts that the given expression throws an MgException of the specified type.
 * \ingroup Assertions
 * Example of usage:
 * \code
 *   MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
 *   CPPUNIT_ASSERT_THROW_MG(mgr->DeleteLog(L""), MgNullArgumentException*);
 * \endcode
 */
# define CPPUNIT_ASSERT_THROW_MG( expression, MgExceptionType )             \
   do {                                                                     \
      Ptr<MgException> exception;                                           \
      bool cpputExceptionThrown_ = false;                                   \
      try {                                                                 \
         expression;                                                        \
      } catch ( MgExceptionType & ex_) {                                    \
         cpputExceptionThrown_ = true;                                      \
         exception = ex_;                                                   \
      }                                                                     \
                                                                            \
      if ( cpputExceptionThrown_ )                                          \
         break;                                                             \
                                                                            \
      CPPUNIT_NS::Asserter::fail(                                           \
                     "Expected exception: " #MgExceptionType                \
                     " not thrown.",                                        \
                     CPPUNIT_SOURCELINE() );                                \
   } while ( false )

#endif // _CPPUNIT_EXTENSIONS_H

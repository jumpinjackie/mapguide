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

// Generated by perl script fill_bits.pl. Ask Eric Haines.

//     Call by something like this:
//         EMapFillPatternFactory fillFac; // Try to do this class creation once and reuse it.
//         ...
//         WT_User_Fill_Pattern fsfp(0);    // NOTE: the ID *will* be changed by the next call
//         fillFac.request_pattern( EMapFillPatternFactory::Shapefill51, fsfp );
//         pwtf->desired_rendition().set_fill_pattern() = fsfp;

#ifndef WHIP_FILL_LIBRARY_H
#define WHIP_FILL_LIBRARY_H

class EMapFillPatternFactory {
    public:
        enum Enum {
            nonexistent = -1, // return value for find_index
            Shapefill12,      //  0
            Shapefill13,      //  1
            Shapefill14,      //  2
            Shapefill15,      //  3
            Shapefill16,      //  4
            Shapefill17,      //  5
            Shapefill18,      //  6
            Shapefill19,      //  7
            Shapefill20,      //  8
            Shapefill21,      //  9
            Shapefill22,      // 10
            Shapefill23,      // 11
            Shapefill24,      // 12
            Shapefill25,      // 13
            Shapefill26,      // 14
            Shapefill27,      // 15
            Shapefill28,      // 16
            Shapefill29,      // 17
            Shapefill30,      // 18
            Shapefill31,      // 19
            Shapefill32,      // 20
            Shapefill33,      // 21
            Shapefill34,      // 22
            Shapefill35,      // 23
            Shapefill36,      // 24
            Shapefill37,      // 25
            Shapefill38,      // 26
            Shapefill39,      // 27
            Shapefill40,      // 28
            Shapefill41,      // 29
            Shapefill42,      // 30
            Shapefill43,      // 31
            Shapefill44,      // 32
            Shapefill45,      // 33
            Shapefill46,      // 34
            Shapefill47,      // 35
            Shapefill48,      // 36
            Shapefill49,      // 37
            Shapefill50,      // 38
            Shapefill51,      // 39
            Shapefill52,      // 40
            Shapefill53,      // 41
            Shapefill54,      // 42
            Shapefill55,      // 43
            Shapefill56,      // 44
            Shapefill57,      // 45
            Shapefill58,      // 46
            Shapefill59,      // 47
            Shapefill60,      // 48
            Shapefill61,      // 49
            Shapefill62,      // 50
            Shapefill63,      // 51
            Shapefill64,      // 52
            Shapefill65,      // 53
            Shapefill66,      // 54
            Shapefill67,      // 55
            Shapefill68,      // 56
            Shapefill69,      // 57
            Shapefill70,      // 58
            Shapefill71,      // 59
            count
        };

    WT_User_Fill_Pattern **m_patterns;

    EMapFillPatternFactory()
    {
        m_patterns = new WT_User_Fill_Pattern *[EMapFillPatternFactory::count];
        for ( int i=0; i<EMapFillPatternFactory::count; i++ )
        {
            m_patterns[i] = NULL;
        }
    };
    ~EMapFillPatternFactory()
    {
        for ( int i=0; i<EMapFillPatternFactory::count; i++ )
        {
            if ( m_patterns[i] != NULL ) {
                delete m_patterns[i];
            }
        }
        delete [] m_patterns;
    };

    void request_pattern( EMapFillPatternFactory::Enum pattern_id, WT_User_Fill_Pattern & fill_pattern )
    {
        switch ( pattern_id )
        {
        case Shapefill12:
            {
                if ( m_patterns[Shapefill12] == NULL ) {
                    WT_Byte const bd[8] = {0x7F, 0xFF, 0xF7, 0xFF, 0x7F, 0xFF, 0xF7, 0xFF};
                    m_patterns[Shapefill12] = new WT_User_Fill_Pattern(
                        Shapefill12, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill12];
            }
            break;

        case Shapefill13:
            {
                if ( m_patterns[Shapefill13] == NULL ) {
                    WT_Byte const bd[8] = {0xDD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF, 0x77, 0xFF};
                    m_patterns[Shapefill13] = new WT_User_Fill_Pattern(
                        Shapefill13, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill13];
            }
            break;

        case Shapefill14:
            {
                if ( m_patterns[Shapefill14] == NULL ) {
                    WT_Byte const bd[8] = {0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77};
                    m_patterns[Shapefill14] = new WT_User_Fill_Pattern(
                        Shapefill14, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill14];
            }
            break;

        case Shapefill15:
            {
                if ( m_patterns[Shapefill15] == NULL ) {
                    WT_Byte const bd[8] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
                    m_patterns[Shapefill15] = new WT_User_Fill_Pattern(
                        Shapefill15, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill15];
            }
            break;

        case Shapefill16:
            {
                if ( m_patterns[Shapefill16] == NULL ) {
                    WT_Byte const bd[8] = {0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22};
                    m_patterns[Shapefill16] = new WT_User_Fill_Pattern(
                        Shapefill16, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill16];
            }
            break;

        case Shapefill17:
            {
                if ( m_patterns[Shapefill17] == NULL ) {
                    WT_Byte const bd[8] = {0x88, 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00};
                    m_patterns[Shapefill17] = new WT_User_Fill_Pattern(
                        Shapefill17, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill17];
            }
            break;

        case Shapefill18:
            {
                if ( m_patterns[Shapefill18] == NULL ) {
                    WT_Byte const bd[8] = {0x80, 0x00, 0x08, 0x00, 0x80, 0x00, 0x08, 0x00};
                    m_patterns[Shapefill18] = new WT_User_Fill_Pattern(
                        Shapefill18, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill18];
            }
            break;

        case Shapefill19:
            {
                if ( m_patterns[Shapefill19] == NULL ) {
                    WT_Byte const bd[8] = {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                    m_patterns[Shapefill19] = new WT_User_Fill_Pattern(
                        Shapefill19, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill19];
            }
            break;

        case Shapefill20:
            {
                if ( m_patterns[Shapefill20] == NULL ) {
                    WT_Byte const bd[8] = {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                    m_patterns[Shapefill20] = new WT_User_Fill_Pattern(
                        Shapefill20, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill20];
            }
            break;

        case Shapefill21:
            {
                if ( m_patterns[Shapefill21] == NULL ) {
                    WT_Byte const bd[8] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00};
                    m_patterns[Shapefill21] = new WT_User_Fill_Pattern(
                        Shapefill21, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill21];
            }
            break;

        case Shapefill22:
            {
                if ( m_patterns[Shapefill22] == NULL ) {
                    WT_Byte const bd[8] = {0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00};
                    m_patterns[Shapefill22] = new WT_User_Fill_Pattern(
                        Shapefill22, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill22];
            }
            break;

        case Shapefill23:
            {
                if ( m_patterns[Shapefill23] == NULL ) {
                    WT_Byte const bd[8] = {0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00};
                    m_patterns[Shapefill23] = new WT_User_Fill_Pattern(
                        Shapefill23, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill23];
            }
            break;

        case Shapefill24:
            {
                if ( m_patterns[Shapefill24] == NULL ) {
                    WT_Byte const bd[8] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
                    m_patterns[Shapefill24] = new WT_User_Fill_Pattern(
                        Shapefill24, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill24];
            }
            break;

        case Shapefill25:
            {
                if ( m_patterns[Shapefill25] == NULL ) {
                    WT_Byte const bd[8] = {0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0};
                    m_patterns[Shapefill25] = new WT_User_Fill_Pattern(
                        Shapefill25, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill25];
            }
            break;

        case Shapefill26:
            {
                if ( m_patterns[Shapefill26] == NULL ) {
                    WT_Byte const bd[8] = {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88};
                    m_patterns[Shapefill26] = new WT_User_Fill_Pattern(
                        Shapefill26, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill26];
            }
            break;

        case Shapefill27:
            {
                if ( m_patterns[Shapefill27] == NULL ) {
                    WT_Byte const bd[8] = {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC};
                    m_patterns[Shapefill27] = new WT_User_Fill_Pattern(
                        Shapefill27, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill27];
            }
            break;

        case Shapefill28:
            {
                if ( m_patterns[Shapefill28] == NULL ) {
                    WT_Byte const bd[8] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
                    m_patterns[Shapefill28] = new WT_User_Fill_Pattern(
                        Shapefill28, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill28];
            }
            break;

        case Shapefill29:
            {
                if ( m_patterns[Shapefill29] == NULL ) {
                    WT_Byte const bd[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
                    m_patterns[Shapefill29] = new WT_User_Fill_Pattern(
                        Shapefill29, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill29];
            }
            break;

        case Shapefill30:
            {
                if ( m_patterns[Shapefill30] == NULL ) {
                    WT_Byte const bd[8] = {0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x81};
                    m_patterns[Shapefill30] = new WT_User_Fill_Pattern(
                        Shapefill30, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill30];
            }
            break;

        case Shapefill31:
            {
                if ( m_patterns[Shapefill31] == NULL ) {
                    WT_Byte const bd[8] = {0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88};
                    m_patterns[Shapefill31] = new WT_User_Fill_Pattern(
                        Shapefill31, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill31];
            }
            break;

        case Shapefill32:
            {
                if ( m_patterns[Shapefill32] == NULL ) {
                    WT_Byte const bd[8] = {0x33, 0x66, 0xCC, 0x99, 0x33, 0x66, 0xCC, 0x99};
                    m_patterns[Shapefill32] = new WT_User_Fill_Pattern(
                        Shapefill32, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill32];
            }
            break;

        case Shapefill33:
            {
                if ( m_patterns[Shapefill33] == NULL ) {
                    WT_Byte const bd[8] = {0xEE, 0xDD, 0xBB, 0x77, 0xEE, 0xDD, 0xBB, 0x77};
                    m_patterns[Shapefill33] = new WT_User_Fill_Pattern(
                        Shapefill33, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill33];
            }
            break;

        case Shapefill34:
            {
                if ( m_patterns[Shapefill34] == NULL ) {
                    WT_Byte const bd[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
                    m_patterns[Shapefill34] = new WT_User_Fill_Pattern(
                        Shapefill34, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill34];
            }
            break;

        case Shapefill35:
            {
                if ( m_patterns[Shapefill35] == NULL ) {
                    WT_Byte const bd[8] = {0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x81};
                    m_patterns[Shapefill35] = new WT_User_Fill_Pattern(
                        Shapefill35, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill35];
            }
            break;

        case Shapefill36:
            {
                if ( m_patterns[Shapefill36] == NULL ) {
                    WT_Byte const bd[8] = {0x88, 0x44, 0x22, 0x11, 0x88, 0x44, 0x22, 0x11};
                    m_patterns[Shapefill36] = new WT_User_Fill_Pattern(
                        Shapefill36, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill36];
            }
            break;

        case Shapefill37:
            {
                if ( m_patterns[Shapefill37] == NULL ) {
                    WT_Byte const bd[8] = {0xCC, 0x66, 0x33, 0x99, 0xCC, 0x66, 0x33, 0x99};
                    m_patterns[Shapefill37] = new WT_User_Fill_Pattern(
                        Shapefill37, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill37];
            }
            break;

        case Shapefill38:
            {
                if ( m_patterns[Shapefill38] == NULL ) {
                    WT_Byte const bd[8] = {0x77, 0xBB, 0xDD, 0xEE, 0x77, 0xBB, 0xDD, 0xEE};
                    m_patterns[Shapefill38] = new WT_User_Fill_Pattern(
                        Shapefill38, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill38];
            }
            break;

        case Shapefill39:
            {
                if ( m_patterns[Shapefill39] == NULL ) {
                    WT_Byte const bd[8] = {0xFF, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
                    m_patterns[Shapefill39] = new WT_User_Fill_Pattern(
                        Shapefill39, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill39];
            }
            break;

        case Shapefill40:
            {
                if ( m_patterns[Shapefill40] == NULL ) {
                    WT_Byte const bd[8] = {0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0};
                    m_patterns[Shapefill40] = new WT_User_Fill_Pattern(
                        Shapefill40, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill40];
            }
            break;

        case Shapefill41:
            {
                if ( m_patterns[Shapefill41] == NULL ) {
                    WT_Byte const bd[8] = {0xFF, 0x88, 0x88, 0x88, 0xFF, 0x88, 0x88, 0x88};
                    m_patterns[Shapefill41] = new WT_User_Fill_Pattern(
                        Shapefill41, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill41];
            }
            break;

        case Shapefill42:
            {
                if ( m_patterns[Shapefill42] == NULL ) {
                    WT_Byte const bd[8] = {0xFF, 0xFF, 0xCC, 0xCC, 0xFF, 0xFF, 0xCC, 0xCC};
                    m_patterns[Shapefill42] = new WT_User_Fill_Pattern(
                        Shapefill42, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill42];
            }
            break;

        case Shapefill43:
            {
                if ( m_patterns[Shapefill43] == NULL ) {
                    WT_Byte const bd[8] = {0xFF, 0xAA, 0xFF, 0xAA, 0xFF, 0xAA, 0xFF, 0xAA};
                    m_patterns[Shapefill43] = new WT_User_Fill_Pattern(
                        Shapefill43, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill43];
            }
            break;

        case Shapefill44:
            {
                if ( m_patterns[Shapefill44] == NULL ) {
                    WT_Byte const bd[8] = {0x82, 0x44, 0x28, 0x10, 0x28, 0x44, 0x82, 0x01};
                    m_patterns[Shapefill44] = new WT_User_Fill_Pattern(
                        Shapefill44, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill44];
            }
            break;

        case Shapefill45:
            {
                if ( m_patterns[Shapefill45] == NULL ) {
                    WT_Byte const bd[8] = {0xC3, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0xC3, 0x81};
                    m_patterns[Shapefill45] = new WT_User_Fill_Pattern(
                        Shapefill45, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill45];
            }
            break;

        case Shapefill46:
            {
                if ( m_patterns[Shapefill46] == NULL ) {
                    WT_Byte const bd[8] = {0xAA, 0x44, 0xAA, 0x11, 0xAA, 0x44, 0xAA, 0x11};
                    m_patterns[Shapefill46] = new WT_User_Fill_Pattern(
                        Shapefill46, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill46];
            }
            break;

        case Shapefill47:
            {
                if ( m_patterns[Shapefill47] == NULL ) {
                    WT_Byte const bd[8] = {0xCC, 0xCC, 0x33, 0x33, 0xCC, 0xCC, 0x33, 0x33};
                    m_patterns[Shapefill47] = new WT_User_Fill_Pattern(
                        Shapefill47, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill47];
            }
            break;

        case Shapefill48:
            {
                if ( m_patterns[Shapefill48] == NULL ) {
                    WT_Byte const bd[8] = {0xB1, 0x30, 0x03, 0x1B, 0xD8, 0xC0, 0x0C, 0x8D};
                    m_patterns[Shapefill48] = new WT_User_Fill_Pattern(
                        Shapefill48, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill48];
            }
            break;

        case Shapefill49:
            {
                if ( m_patterns[Shapefill49] == NULL ) {
                    WT_Byte const bd[8] = {0x80, 0x10, 0x02, 0x20, 0x01, 0x08, 0x40, 0x04};
                    m_patterns[Shapefill49] = new WT_User_Fill_Pattern(
                        Shapefill49, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill49];
            }
            break;

        case Shapefill50:
            {
                if ( m_patterns[Shapefill50] == NULL ) {
                    WT_Byte const bd[8] = {0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00};
                    m_patterns[Shapefill50] = new WT_User_Fill_Pattern(
                        Shapefill50, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill50];
            }
            break;

        case Shapefill51:
            {
                if ( m_patterns[Shapefill51] == NULL ) {
                    WT_Byte const bd[8] = {0x80, 0x40, 0x20, 0x00, 0x02, 0x04, 0x08, 0x00};
                    m_patterns[Shapefill51] = new WT_User_Fill_Pattern(
                        Shapefill51, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill51];
            }
            break;

        case Shapefill52:
            {
                if ( m_patterns[Shapefill52] == NULL ) {
                    WT_Byte const bd[8] = {0x80, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00};
                    m_patterns[Shapefill52] = new WT_User_Fill_Pattern(
                        Shapefill52, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill52];
            }
            break;

        case Shapefill53:
            {
                if ( m_patterns[Shapefill53] == NULL ) {
                    WT_Byte const bd[8] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                    m_patterns[Shapefill53] = new WT_User_Fill_Pattern(
                        Shapefill53, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill53];
            }
            break;

        case Shapefill54:
            {
                if ( m_patterns[Shapefill54] == NULL ) {
                    WT_Byte const bd[8] = {0x82, 0x44, 0x39, 0x44, 0x82, 0x01, 0x01, 0x01};
                    m_patterns[Shapefill54] = new WT_User_Fill_Pattern(
                        Shapefill54, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill54];
            }
            break;

        case Shapefill55:
            {
                if ( m_patterns[Shapefill55] == NULL ) {
                    WT_Byte const bd[8] = {0xF8, 0x74, 0x22, 0x47, 0x8F, 0x17, 0x22, 0x71};
                    m_patterns[Shapefill55] = new WT_User_Fill_Pattern(
                        Shapefill55, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill55];
            }
            break;

        case Shapefill56:
            {
                if ( m_patterns[Shapefill56] == NULL ) {
                    WT_Byte const bd[8] = {0x55, 0xA0, 0x40, 0x40, 0x55, 0x0A, 0x04, 0x04};
                    m_patterns[Shapefill56] = new WT_User_Fill_Pattern(
                        Shapefill56, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill56];
            }
            break;

        case Shapefill57:
            {
                if ( m_patterns[Shapefill57] == NULL ) {
                    WT_Byte const bd[8] = {0x20, 0x50, 0x88, 0x88, 0x88, 0x88, 0x05, 0x02};
                    m_patterns[Shapefill57] = new WT_User_Fill_Pattern(
                        Shapefill57, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill57];
            }
            break;

        case Shapefill58:
            {
                if ( m_patterns[Shapefill58] == NULL ) {
                    WT_Byte const bd[8] = {0xBF, 0x00, 0xBF, 0xBF, 0xB0, 0xB0, 0xB0, 0xB0};
                    m_patterns[Shapefill58] = new WT_User_Fill_Pattern(
                        Shapefill58, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill58];
            }
            break;

        case Shapefill59:
            {
                if ( m_patterns[Shapefill59] == NULL ) {
                    WT_Byte const bd[8] = {0xFF, 0x80, 0x80, 0x80, 0xFF, 0x08, 0x08, 0x08};
                    m_patterns[Shapefill59] = new WT_User_Fill_Pattern(
                        Shapefill59, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill59];
            }
            break;

        case Shapefill60:
            {
                if ( m_patterns[Shapefill60] == NULL ) {
                    WT_Byte const bd[8] = {0xAA, 0x00, 0x80, 0x00, 0x88, 0x00, 0x80, 0x00};
                    m_patterns[Shapefill60] = new WT_User_Fill_Pattern(
                        Shapefill60, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill60];
            }
            break;

        case Shapefill61:
            {
                if ( m_patterns[Shapefill61] == NULL ) {
                    WT_Byte const bd[8] = {0x08, 0x1C, 0x22, 0xC1, 0x80, 0x01, 0x02, 0x04};
                    m_patterns[Shapefill61] = new WT_User_Fill_Pattern(
                        Shapefill61, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill61];
            }
            break;

        case Shapefill62:
            {
                if ( m_patterns[Shapefill62] == NULL ) {
                    WT_Byte const bd[8] = {0x88, 0x14, 0x22, 0x41, 0x88, 0x00, 0xAA, 0x00};
                    m_patterns[Shapefill62] = new WT_User_Fill_Pattern(
                        Shapefill62, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill62];
            }
            break;

        case Shapefill63:
            {
                if ( m_patterns[Shapefill63] == NULL ) {
                    WT_Byte const bd[8] = {0x40, 0xA0, 0x00, 0x00, 0x04, 0x0A, 0x00, 0x00};
                    m_patterns[Shapefill63] = new WT_User_Fill_Pattern(
                        Shapefill63, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill63];
            }
            break;

        case Shapefill64:
            {
                if ( m_patterns[Shapefill64] == NULL ) {
                    WT_Byte const bd[8] = {0x03, 0x84, 0x48, 0x30, 0x0C, 0x02, 0x01, 0x01};
                    m_patterns[Shapefill64] = new WT_User_Fill_Pattern(
                        Shapefill64, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill64];
            }
            break;

        case Shapefill65:
            {
                if ( m_patterns[Shapefill65] == NULL ) {
                    WT_Byte const bd[8] = {0x80, 0x80, 0x41, 0x3E, 0x08, 0x08, 0x14, 0xE3};
                    m_patterns[Shapefill65] = new WT_User_Fill_Pattern(
                        Shapefill65, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill65];
            }
            break;

        case Shapefill66:
            {
                if ( m_patterns[Shapefill66] == NULL ) {
                    WT_Byte const bd[8] = {0x10, 0x20, 0x54, 0xAA, 0xFF, 0x02, 0x04, 0x08};
                    m_patterns[Shapefill66] = new WT_User_Fill_Pattern(
                        Shapefill66, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill66];
            }
            break;

        case Shapefill67:
            {
                if ( m_patterns[Shapefill67] == NULL ) {
                    WT_Byte const bd[8] = {0x77, 0x89, 0x8F, 0x8F, 0x77, 0x98, 0xF8, 0xF8};
                    m_patterns[Shapefill67] = new WT_User_Fill_Pattern(
                        Shapefill67, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill67];
            }
            break;

        case Shapefill68:
            {
                if ( m_patterns[Shapefill68] == NULL ) {
                    WT_Byte const bd[8] = {0x00, 0x08, 0x14, 0x2A, 0x55, 0x2A, 0x14, 0x08};
                    m_patterns[Shapefill68] = new WT_User_Fill_Pattern(
                        Shapefill68, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill68];
            }
            break;

        case Shapefill69:
            {
                if ( m_patterns[Shapefill69] == NULL ) {
                    WT_Byte const bd[8] = {0x28, 0x14, 0x22, 0x49, 0x9C, 0x39, 0x92, 0x44};
                    m_patterns[Shapefill69] = new WT_User_Fill_Pattern(
                        Shapefill69, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill69];
            }
            break;

        case Shapefill70:
            {
                if ( m_patterns[Shapefill70] == NULL ) {
                    WT_Byte const bd[8] = {0x06, 0x06, 0x06, 0x00, 0x60, 0x60, 0x60, 0x00};
                    m_patterns[Shapefill70] = new WT_User_Fill_Pattern(
                        Shapefill70, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill70];
            }
            break;

        case Shapefill71:
            {
                if ( m_patterns[Shapefill71] == NULL ) {
                    WT_Byte const bd[8] = {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11, 0xF1};
                    m_patterns[Shapefill71] = new WT_User_Fill_Pattern(
                        Shapefill71, 8, 8, 8, bd );
                }
                fill_pattern = *m_patterns[Shapefill71];
            }
            break;

        default:
            _ASSERT(0);
            break;
        }
    }

    // get index value given name
    Enum find_index( const wchar_t * name )
    {
        if ( _wcsicmp( name,L"shapefill12" ) == 0 )
        {
            return Shapefill12;
        }
        if ( _wcsicmp( name,L"shapefill13" ) == 0 )
        {
            return Shapefill13;
        }
        if ( _wcsicmp( name,L"shapefill14" ) == 0 )
        {
            return Shapefill14;
        }
        if ( _wcsicmp( name,L"shapefill15" ) == 0 )
        {
            return Shapefill15;
        }
        if ( _wcsicmp( name,L"shapefill16" ) == 0 )
        {
            return Shapefill16;
        }
        if ( _wcsicmp( name,L"shapefill17" ) == 0 )
        {
            return Shapefill17;
        }
        if ( _wcsicmp( name,L"shapefill18" ) == 0 )
        {
            return Shapefill18;
        }
        if ( _wcsicmp( name,L"shapefill19" ) == 0 )
        {
            return Shapefill19;
        }
        if ( _wcsicmp( name,L"shapefill20" ) == 0 )
        {
            return Shapefill20;
        }
        if ( _wcsicmp( name,L"shapefill21" ) == 0 )
        {
            return Shapefill21;
        }
        if ( _wcsicmp( name,L"shapefill22" ) == 0 )
        {
            return Shapefill22;
        }
        if ( _wcsicmp( name,L"shapefill23" ) == 0 )
        {
            return Shapefill23;
        }
        if ( _wcsicmp( name,L"shapefill24" ) == 0 )
        {
            return Shapefill24;
        }
        if ( _wcsicmp( name,L"shapefill25" ) == 0 )
        {
            return Shapefill25;
        }
        if ( _wcsicmp( name,L"shapefill26" ) == 0 )
        {
            return Shapefill26;
        }
        if ( _wcsicmp( name,L"shapefill27" ) == 0 )
        {
            return Shapefill27;
        }
        if ( _wcsicmp( name,L"shapefill28" ) == 0 )
        {
            return Shapefill28;
        }
        if ( _wcsicmp( name,L"shapefill29" ) == 0 )
        {
            return Shapefill29;
        }
        if ( _wcsicmp( name,L"shapefill30" ) == 0 )
        {
            return Shapefill30;
        }
        if ( _wcsicmp( name,L"shapefill31" ) == 0 )
        {
            return Shapefill31;
        }
        if ( _wcsicmp( name,L"shapefill32" ) == 0 )
        {
            return Shapefill32;
        }
        if ( _wcsicmp( name,L"shapefill33" ) == 0 )
        {
            return Shapefill33;
        }
        if ( _wcsicmp( name,L"shapefill34" ) == 0 )
        {
            return Shapefill34;
        }
        if ( _wcsicmp( name,L"shapefill35" ) == 0 )
        {
            return Shapefill35;
        }
        if ( _wcsicmp( name,L"shapefill36" ) == 0 )
        {
            return Shapefill36;
        }
        if ( _wcsicmp( name,L"shapefill37" ) == 0 )
        {
            return Shapefill37;
        }
        if ( _wcsicmp( name,L"shapefill38" ) == 0 )
        {
            return Shapefill38;
        }
        if ( _wcsicmp( name,L"shapefill39" ) == 0 )
        {
            return Shapefill39;
        }
        if ( _wcsicmp( name,L"shapefill40" ) == 0 )
        {
            return Shapefill40;
        }
        if ( _wcsicmp( name,L"shapefill41" ) == 0 )
        {
            return Shapefill41;
        }
        if ( _wcsicmp( name,L"shapefill42" ) == 0 )
        {
            return Shapefill42;
        }
        if ( _wcsicmp( name,L"shapefill43" ) == 0 )
        {
            return Shapefill43;
        }
        if ( _wcsicmp( name,L"shapefill44" ) == 0 )
        {
            return Shapefill44;
        }
        if ( _wcsicmp( name,L"shapefill45" ) == 0 )
        {
            return Shapefill45;
        }
        if ( _wcsicmp( name,L"shapefill46" ) == 0 )
        {
            return Shapefill46;
        }
        if ( _wcsicmp( name,L"shapefill47" ) == 0 )
        {
            return Shapefill47;
        }
        if ( _wcsicmp( name,L"shapefill48" ) == 0 )
        {
            return Shapefill48;
        }
        if ( _wcsicmp( name,L"shapefill49" ) == 0 )
        {
            return Shapefill49;
        }
        if ( _wcsicmp( name,L"shapefill50" ) == 0 )
        {
            return Shapefill50;
        }
        if ( _wcsicmp( name,L"shapefill51" ) == 0 )
        {
            return Shapefill51;
        }
        if ( _wcsicmp( name,L"shapefill52" ) == 0 )
        {
            return Shapefill52;
        }
        if ( _wcsicmp( name,L"shapefill53" ) == 0 )
        {
            return Shapefill53;
        }
        if ( _wcsicmp( name,L"shapefill54" ) == 0 )
        {
            return Shapefill54;
        }
        if ( _wcsicmp( name,L"shapefill55" ) == 0 )
        {
            return Shapefill55;
        }
        if ( _wcsicmp( name,L"shapefill56" ) == 0 )
        {
            return Shapefill56;
        }
        if ( _wcsicmp( name,L"shapefill57" ) == 0 )
        {
            return Shapefill57;
        }
        if ( _wcsicmp( name,L"shapefill58" ) == 0 )
        {
            return Shapefill58;
        }
        if ( _wcsicmp( name,L"shapefill59" ) == 0 )
        {
            return Shapefill59;
        }
        if ( _wcsicmp( name,L"shapefill60" ) == 0 )
        {
            return Shapefill60;
        }
        if ( _wcsicmp( name,L"shapefill61" ) == 0 )
        {
            return Shapefill61;
        }
        if ( _wcsicmp( name,L"shapefill62" ) == 0 )
        {
            return Shapefill62;
        }
        if ( _wcsicmp( name,L"shapefill63" ) == 0 )
        {
            return Shapefill63;
        }
        if ( _wcsicmp( name,L"shapefill64" ) == 0 )
        {
            return Shapefill64;
        }
        if ( _wcsicmp( name,L"shapefill65" ) == 0 )
        {
            return Shapefill65;
        }
        if ( _wcsicmp( name,L"shapefill66" ) == 0 )
        {
            return Shapefill66;
        }
        if ( _wcsicmp( name,L"shapefill67" ) == 0 )
        {
            return Shapefill67;
        }
        if ( _wcsicmp( name,L"shapefill68" ) == 0 )
        {
            return Shapefill68;
        }
        if ( _wcsicmp( name,L"shapefill69" ) == 0 )
        {
            return Shapefill69;
        }
        if ( _wcsicmp( name,L"shapefill70" ) == 0 )
        {
            return Shapefill70;
        }
        if ( _wcsicmp( name,L"shapefill71" ) == 0 )
        {
            return Shapefill71;
        }
        return nonexistent;
    }
};

#endif

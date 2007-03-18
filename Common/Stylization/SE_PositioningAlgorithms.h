//
//  Copyright (C) 2007 by Autodesk, Inc.
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

#ifndef SE_POSITIONINGALGORITHMS_H
#define SE_POSITIONINGALGORITHMS_H

class LineBuffer;
class SE_Renderer;
struct SE_Matrix;
struct SE_Style;
struct SE_RenderStyle;


class SE_PositioningAlgorithms
{
public:
    static void EightSurrounding(SE_Renderer*    renderer, 
                          LineBuffer*     geometry, 
                          SE_Matrix&      xform, 
                          SE_Style*       style, 
                          SE_RenderStyle* rstyle, 
                          double          mm2px
                          );

    static void PathLabels(SE_Renderer*    renderer, 
                          LineBuffer*     geometry, 
                          SE_Matrix&      xform, 
                          SE_Style*       style, 
                          SE_RenderStyle* rstyle, 
                          double          mm2px
                          );

    static void MultipleHighwaysShields(SE_Renderer*    renderer, 
                                        LineBuffer*     geometry, 
                                        SE_Matrix&      xform, 
                                        SE_Style*       style, 
                                        SE_RenderStyle* rstyle, 
                                        double          mm2px
                                        );
};

#endif // SE_POSITIONINGALGORITHMS_H

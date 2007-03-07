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

#ifndef GEOMETRYADAPTER_H
#define GEOMETRYADAPTER_H

#include "RendererStyles.h"
#include "RS_FeatureReader.h"

class RS_FilterExecutor;
class LineBuffer;
class LineBufferPool;

//-----------------------------------------------------------------------------
// Base class for helper classes which know how to stylize a particular
// type of geometry.
//
// *** IMPORTANT ***
// This class implements a common caching scheme for theming filters
// and theming expressions. The caching is done using pointers to strings
// in the incoming layer definition objects. This relies on these strings
// not changing during stylization of the layer. This should be the case
// since we instantiate a copy of the whole map definition during each
// map request.
//
//-----------------------------------------------------------------------------
class GeometryAdapter
{

public:
    GeometryAdapter(LineBufferPool*);
    virtual ~GeometryAdapter();


    virtual void Stylize(Renderer*                      /*renderer*/,
                         RS_FeatureReader*              /*features*/,
                         RS_FilterExecutor*             /*exec*/,
                         LineBuffer*                    /*lb*/,
                         MdfModel::FeatureTypeStyle*    /*style*/,
                         const MdfModel::MdfString*     /*tooltip = NULL*/,
                         const MdfModel::MdfString*     /*url = NULL*/,
                         RS_ElevationSettings*          /*elevSettings = NULL*/
                         )
    {
    };

    virtual void AddLabel(double           x,
                          double           y,
                          double           slope,
                          bool             useSlope,
                          MdfModel::Label* label,
                          RS_OverpostType  type,
                          bool             exclude,
                          Renderer*        renderer,
                          LineBuffer*      lb
                          );

    bool EvalColor              (const MdfModel::MdfString&     expr,
                                 RS_Color&                      color);

    bool ConvertLineThickness   (const MdfModel::MdfString&     expr,
                                 double&                        thickness);

    bool ConvertStroke          (MdfModel::Stroke*              stroke,
                                 RS_LineStroke&                 rstroke);

    bool ConvertStroke          (MdfModel::LineSymbolization2D*   lsym,
                                 RS_LineStroke&                 rstroke);

    bool ConvertFill            (MdfModel::AreaSymbolization2D*   fill,
                                 RS_FillStyle&                  rsfill);

    bool ConvertFill            (MdfModel::Fill*                fill,
                                 RS_FillStyle&                  rsfill);

    bool ConvertMarkerDef       (MdfModel::Symbol*              marker,
                                 RS_MarkerDef&                  mdef);

    bool ConvertTextDef         (MdfModel::TextSymbol*          text,
                                 RS_TextDef&                    tdef);

    bool ConvertTextHAlign      (const MdfModel::MdfString&     halign,
                                 RS_HAlignment&                 rshalign);

    bool ConvertTextVAlign      (const MdfModel::MdfString&     valign,
                                 RS_VAlignment&                 rsvalign);

    bool ExecFdoFilter(const MdfModel::MdfString* pExpression);

    FdoExpression* ObtainFdoExpression(const MdfModel::MdfString* pExpression);

protected:

    bool EvalDouble(const MdfModel::MdfString& expr, double& res);
    bool EvalBoolean(const MdfModel::MdfString& expr, bool& res);
    bool EvalString(const MdfModel::MdfString& exprstr, RS_String& res);
    bool GetElevationParams(RS_ElevationSettings* elevationSettings, 
        double& zOffset, double& zExtrusion, RS_ElevationType& elevType);

    RS_FilterExecutor* m_exec;
    LineBufferPool* m_lbPool;

    //BOGUS:
    //TODO: we use pointer to the MDF strings for caching --
    //this may be risky but all caching in the stylizer is
    //based on the MDF pointer staying the same throughout the
    //stylization process

    //FdoFilter cache
    std::map<const void*, FdoFilter*> m_hFilterCache;

    //FdoExpression cache
    std::map<const void*, FdoExpression*> m_hExpressionCache;
};

#endif

//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#ifndef GEOMETRYADAPTER_H_
#define GEOMETRYADAPTER_H_

#include "Stylization.h"
#include "RendererStyles.h"
#include "RS_FeatureReader.h"

class LineBuffer;
class LineBufferPool;

//-----------------------------------------------------------------------------
// Base class for helper classes which know how to stylize a particular
// type of geometry.
//
// *** IMPORTANT ***
// This class implements a common caching scheme for theming filters and
// theming expressions.  The caching is done using pointers to strings
// in the incoming layer definition objects.  This relies on these strings
// not changing during stylization of the layer.  This should be the case
// since we instantiate a copy of the whole map definition during each
// map request.
//
//-----------------------------------------------------------------------------
class GeometryAdapter
{
public:
    STYLIZATION_API GeometryAdapter(LineBufferPool* lbp);
    STYLIZATION_API virtual ~GeometryAdapter();

    STYLIZATION_API bool EvalBoolean      (const MdfModel::MdfString&  exprstr, bool&          res);
    STYLIZATION_API bool EvalDouble       (const MdfModel::MdfString&  exprstr, double&        res);
    STYLIZATION_API bool EvalString       (const MdfModel::MdfString&  exprstr, RS_String&     res);
    STYLIZATION_API bool EvalColor        (const MdfModel::MdfString&  exprstr, RS_Color&      color);
    STYLIZATION_API bool ConvertStroke    (MdfModel::Stroke*            stroke, RS_LineStroke& rstroke);
    STYLIZATION_API bool ConvertStroke    (MdfModel::LineSymbolization2D* lsym, RS_LineStroke& rstroke);
    STYLIZATION_API bool ConvertFill      (MdfModel::Fill*                fill, RS_FillStyle&  rsfill);
    STYLIZATION_API bool ConvertFill      (MdfModel::AreaSymbolization2D* fill, RS_FillStyle&  rsfill);
    STYLIZATION_API bool ConvertTextHAlign(const MdfModel::MdfString&   halign, RS_HAlignment& rshalign);
    STYLIZATION_API bool ConvertTextVAlign(const MdfModel::MdfString&   valign, RS_VAlignment& rsvalign);
    STYLIZATION_API bool ConvertSymbol    (MdfModel::Symbol*            symbol, RS_MarkerDef&  mdef);
    STYLIZATION_API bool ConvertTextDef   (MdfModel::TextSymbol*          text, RS_TextDef&    tdef);

    STYLIZATION_API virtual void Stylize(Renderer*                   renderer,
                                         RS_FeatureReader*           features,
                                         bool                        initialPass,
                                         FdoExpressionEngine*        exec,
                                         LineBuffer*                 lb,
                                         MdfModel::FeatureTypeStyle* style,
                                         const MdfModel::MdfString*  tooltip = NULL,
                                         const MdfModel::MdfString*  url = NULL,
                                         RS_ElevationSettings*       elevSettings = NULL,
                                         CSysTransformer* layer2mapxformer = NULL);

    STYLIZATION_API virtual void AddLabel(double           x,
                                          double           y,
                                          double           slope,
                                          bool             useSlope,
                                          MdfModel::Label* label,
                                          RS_OverpostType  type,
                                          bool             exclude,
                                          Renderer*        renderer,
                                          LineBuffer*      lb);

    STYLIZATION_API bool ExecFdoFilter(const MdfModel::MdfString* pExprstr);
    STYLIZATION_API FdoExpression* ObtainFdoExpression(const MdfModel::MdfString* pExprstr);

protected:
    STYLIZATION_API bool GetElevationParams(RS_ElevationSettings* elevationSettings,
                                            double& zOffset, double& zExtrusion,
                                            RS_ElevationType& elevType);

    double GetClipOffset(RS_LineStroke& lineStroke, double mapScale);

    FdoExpressionEngine* m_exec;
    LineBufferPool* m_lbPool;

    // BOGUS:
    // TODO: we use pointer to the MDF strings for caching --
    // this may be risky but all caching in the stylizer is
    // based on the MDF pointer staying the same throughout the
    // stylization process

    // FdoFilter cache
    std::map<const void*, FdoFilter*> m_hFilterCache;

    // FdoExpression cache
    std::map<const void*, FdoExpression*> m_hExpressionCache;
};

#endif

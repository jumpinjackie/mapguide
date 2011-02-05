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

#ifndef GDW2DREWRITER_H
#define GDW2DREWRITER_H

#include "whiptk/whip_toolkit.h"

// Standard action routines.
WT_Result gdr_open(WT_File & file);
WT_Result gdr_close(WT_File & file);
WT_Result gdr_read(WT_File & file, int desired_bytes, int &bytes_read, void * buffer);
WT_Result gdr_seek(WT_File & file, int distance, int &amount_seeked);

//WT_Result gdr_process_author(WT_Author & author, WT_File & file);
//WT_Result gdr_process_creator(WT_Creator & creator, WT_File & file);
//WT_Result gdr_process_created(WT_Creation_Time & created, WT_File & file);
//WT_Result gdr_process_modified(WT_Modification_Time & modified, WT_File & file);
//WT_Result gdr_process_sourceFilename(WT_Source_Filename & sourceFilename, WT_File & file);
//WT_Result gdr_process_sourceCreationTime(WT_Source_Creation_Time & sourceCreated, WT_File & file);
//WT_Result gdr_process_sourceModificationTime(WT_Source_Modification_Time & sourceModified, WT_File & file);
WT_Result gdr_process_units(WT_Units & units, WT_File & file);
//WT_Result gdr_process_embed(WT_Embed & embed, WT_File & file);
//WT_Result gdr_process_namedView(WT_Named_View & namedView, WT_File & file);
//WT_Result gdr_process_view(WT_View & view, WT_File & file);
WT_Result gdr_process_plotInfo(WT_Plot_Info & plotInfo, WT_File & file);
WT_Result gdr_process_background(WT_Background & background, WT_File & file);
WT_Result gdr_process_color(WT_Color & color, WT_File & file);
WT_Result gdr_process_lineWeight(WT_Line_Weight & lineWeight, WT_File & file);
WT_Result gdr_process_lineStyle(WT_Line_Style & lineStyle, WT_File & file);
WT_Result gdr_process_linePattern(WT_Line_Pattern & linePattern, WT_File & file);
WT_Result gdr_process_layer(WT_Layer & layer, WT_File & file);
WT_Result gdr_process_viewport(WT_Viewport & viewport, WT_File & file);
WT_Result gdr_process_visibility(WT_Visibility & visibility, WT_File & file);
WT_Result gdr_process_codePage(WT_Code_Page & codePage, WT_File & file);
WT_Result gdr_process_colorMap(WT_Color_Map & colorMap, WT_File & file);
//WT_Result gdr_process_comments(WT_Comments & comments, WT_File & file);
WT_Result gdr_process_contourSet(WT_Contour_Set & contourSet, WT_File & file);
//WT_Result gdr_process_copyright(WT_Copyright & copyright, WT_File & file);
WT_Result gdr_process_dashPattern(WT_Dash_Pattern & dashPattern, WT_File & file);
//WT_Result gdr_process_description(WT_Description & description, WT_File & file);
WT_Result gdr_process_fill(WT_Fill & fill, WT_File & file);
WT_Result gdr_process_filledEllipse(WT_Filled_Ellipse & filledEllipse, WT_File & file);
WT_Result gdr_process_font(WT_Font & font, WT_File & file);
WT_Result gdr_process_gouraudPolyline(WT_Gouraud_Polyline & gouraudPolyline, WT_File & file);
WT_Result gdr_process_gouraudPolytriangle(WT_Gouraud_Polytriangle & gouraudPolytriangle, WT_File & file);
WT_Result gdr_process_image(WT_Image & image, WT_File & file);
//WT_Result gdr_process_keywords(WT_Keywords & keywords, WT_File & file);
WT_Result gdr_process_markerSize(WT_Marker_Size & markerSize, WT_File & file);
WT_Result gdr_process_markerSymbol(WT_Marker_Symbol & markerSymbol, WT_File & file);
WT_Result gdr_process_mergeControl(WT_Merge_Control & mergeControl, WT_File & file);
//WT_Result gdr_process_namedViewList(WT_Named_View_List & namedViewList, WT_File & file);
WT_Result gdr_process_origin(WT_Origin & origin, WT_File & file);
WT_Result gdr_process_outlineEllipse(WT_Outline_Ellipse & outlineEllipse, WT_File & file);
WT_Result gdr_process_polygon(WT_Polygon & polygon, WT_File & file);
WT_Result gdr_process_polymarker(WT_Polymarker & polymarker, WT_File & file);
WT_Result gdr_process_polytriangle(WT_Polytriangle & polytriangle, WT_File & file);
WT_Result gdr_process_projection(WT_Projection & projection, WT_File & file);
//WT_Result gdr_process_subject(WT_Subject & subject, WT_File & file);
//WT_Result gdr_process_title(WT_Title & title, WT_File & file);
//WT_Result gdr_process_unknown(WT_Unknown & unknown, WT_File & file);
//WT_Result gdr_process_url(WT_URL & url, WT_File & file);
WT_Result gdr_process_pngGroup4Image(WT_PNG_Group4_Image & pngGroup4Image, WT_File & file);
WT_Result gdr_process_polyline(WT_Polyline & polyline, WT_File & file);
WT_Result gdr_process_text(WT_Text & text, WT_File & file);
WT_Result gdr_process_inkedArea(WT_Inked_Area & inkedArea, WT_File & file);
//WT_Result gdr_process_plotOptimized(WT_Plot_Optimized & plotOptimized, WT_File & file);
//WT_Result gdr_process_groupBegin(WT_Group_Begin & groupBegin, WT_File & file);
//WT_Result gdr_process_groupEnd(WT_Group_End & groupEnd, WT_File & file);
//WT_Result gdr_process_embeddedFont(WT_Embedded_Font & embeddedFont, WT_File & file);
//WT_Result gdr_process_trustedFontList(WT_Trusted_Font_List & tustedFontList, WT_File & file);
//WT_Result gdr_process_blockref(WT_BlockRef & blockref, WT_File & file);
//WT_Result gdr_process_blockMeaning(WT_Block_Meaning & blockMeaning, WT_File & file);
//WT_Result gdr_process_encryption(WT_Encryption & encryption, WT_File & file);
//WT_Result gdr_process_orientation(WT_Orientation & orientation, WT_File & file);
WT_Result gdr_process_alignment(WT_Alignment & alignment, WT_File & file);
//WT_Result gdr_process_password(WT_Password & password, WT_File & file);
//WT_Result gdr_process_guid(WT_Guid & guid, WT_File & file);
//WT_Result gdr_process_fileTime(WT_FileTime & fileTime, WT_File & file);
//WT_Result gdr_process_userData(WT_UserData & userData, WT_File & file);
WT_Result gdr_process_penPattern(WT_Pen_Pattern & penPattern, WT_File & file);
WT_Result gdr_process_fillPattern(WT_Fill_Pattern & fillPattern, WT_File & file);
//WT_Result gdr_process_DigitalSign(WT_SignData & digitalSing, WT_File & file);
//WT_Result gdr_process_dwf_header(WT_DWF_Header & dwf_header, WT_File & file);

#endif

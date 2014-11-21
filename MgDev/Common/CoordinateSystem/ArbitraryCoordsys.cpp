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

#include "GeometryCommon.h"
#include "CoordSysCommon.h"

#include "CoordSysUtil.h"                       //for Convert_Wide_To_UTF8, CsDictionaryOpenMode
#include "ArbitraryCoordsys.h"
#include "MentorUtil.h"            //for BuildCsprmFromArbitraryDef

extern "C" struct cs_Unittab_ cs_Unittab[]; // Mentor unit table

#ifndef _WIN32
#include <dlfcn.h>
#endif

using namespace CSLibrary;

// Arbitrary X-Y Coordinate Support
const int CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystemsCount = 39;
const ArbitraryCoordinateSystem CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystemsCount] =
{
    {L"XY-M",   L"Meter",            1.0},                        // NOXLATE
    {L"XY-FT",  L"Foot",             0.30480060960121920243},     // NOXLATE
    {L"XY-IN",  L"Inch",             0.0254000508001016002},      // NOXLATE
    {L"XY-IFT", L"IFoot",            0.3048},                     // NOXLATE
    {L"XY-CF",  L"ClarkeFoot",       0.3047972651151},            // NOXLATE
    {L"XY-IIN", L"IInch",            0.0254},                     // NOXLATE
    {L"XY-UI",  L"MicroInch",        0.0000254},                  // NOXLATE
    {L"XY-ML",  L"Mil",              0.0000000254},               // NOXLATE
    {L"XY-CM",  L"Centimeter",       0.01},                       // NOXLATE
    {L"XY-KM",  L"Kilometer",        1000.0},                     // NOXLATE
    {L"XY-YD",  L"Yard",             0.91440182880365760731},     // NOXLATE
    {L"XY-SY",  L"SearsYard",        0.914398414616029},          // NOXLATE
    {L"XY-MI",  L"Mile",             1609.34721869443738887477},  // NOXLATE
    {L"XY-IYD", L"IYard",            0.9144},                     // NOXLATE
    {L"XY-IMI", L"IMile",            1609.344},                   // NOXLATE
    {L"XY-KT",  L"Knot",             1852.0},                     // NOXLATE
    {L"XY-NM",  L"NautM",            1852.0},                     // NOXLATE
    {L"XY-DM",  L"Decimeter",        0.1},                        // NOXLATE
    {L"XY-MM",  L"Millimeter",       0.001},                      // NOXLATE
    {L"XY-DAM", L"Decameter",        10.0},                       // NOXLATE
    {L"XY-DK",  L"Dekameter",        10.0},                       // NOXLATE
    {L"XY-HM",  L"Hectometer",       100.0},                      // NOXLATE
    {L"XY-GM",  L"GermanMeter",      1.0000135965},               // NOXLATE
    {L"XY-CG",  L"CaGrid",           0.999738},                   // NOXLATE
    {L"XY-GC",  L"GunterChain",      20.11684023368047},          // NOXLATE
    {L"XY-CC",  L"ClarkeChain",      20.1166194976},              // NOXLATE
    {L"XY-BC",  L"BenoitChain",      20.11678249438},             // NOXLATE
    {L"XY-SC",  L"SearsChain",       20.11676512155},             // NOXLATE
    {L"XY-GL",  L"GunterLink",       0.2011684023368047},         // NOXLATE
    {L"XY-CL",  L"ClarkeLink",       0.201166194976},             // NOXLATE
    {L"XY-BL",  L"BenoitLink",       0.2011678249438},            // NOXLATE
    {L"XY-SL",  L"SearsLink",        0.2011676512155},            // NOXLATE
    {L"XY-RD",  L"Rod",              5.02921005842012},           // NOXLATE
    {L"XY-PE",  L"Perch",            5.02921005842012},           // NOXLATE
    {L"XY-PO",  L"Pole",             5.02921005842012},           // NOXLATE
    {L"XY-FU",  L"Furlong",          201.1684023368046},          // NOXLATE
    {L"XY-RO",  L"Rood",             3.778266898},                // NOXLATE
    {L"XY-CA",  L"CapeFoot",         0.3047972615},               // NOXLATE
    {L"XY-BR",  L"Brealey",          375.0}                       // NOXLATE
};

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified coordinate system code into the corresponding
/// wkt string.
///</summary>
///<param name="code">
/// The coordinate system code string to convert.
///</param>
///<returns>
/// String of the corresponding wkt
///</returns>
char* CCsArbitraryCoordinateSystemUtil::ConvertCoordinateSystemToWkt(struct cs_Csdef_ *pcsDefPtr)
{
     if (!pcsDefPtr)
    {
        return NULL;
    }
    if (!ProjectionIsNerthType (pcsDefPtr->prj_knm))
    {
        return NULL;
    }

    // This is an arbitrary XY system
    if(0==strlen(pcsDefPtr->unit))
    {
        return NULL;
    }

    // Arbitrary XY system
    string srsWkt;

    // Format the unit scale
    string sNumber;
    FormatNumber(sNumber, pcsDefPtr->unit_scl);

    // Assemble UNIT parameters
    string sLinearUnit;
    sLinearUnit  = "UNIT[\"";   // NOXLATE
    sLinearUnit += pcsDefPtr->unit;
    sLinearUnit += "\", ";
    sLinearUnit += sNumber;
    sLinearUnit += "]";

    string sLocalDatum;
    sLocalDatum = "LOCAL_DATUM[\"Local Datum\",0]";   // NOXLATE

    string sAxisX;
    sAxisX = "AXIS[\"X\",EAST]";    // NOXLATE

    string sAxisY;
    sAxisY = "AXIS[\"Y\",NORTH]";   // NOXLATE

    // Put the OGC WKT together
    srsWkt  = LOCAL_CS;
    srsWkt += "[\"Non-Earth (";
    srsWkt += pcsDefPtr->unit;
    srsWkt += ")\",";
    srsWkt += sLocalDatum;
    srsWkt += ",";
    srsWkt += sLinearUnit;
    srsWkt += ",";
    srsWkt += sAxisX;
    srsWkt += ",";
    srsWkt += sAxisY;
    srsWkt += "]";

#ifdef _WIN32
    return _strdup(srsWkt.c_str());
#else
    // linux_implementation_here
    char *str_dup = NULL;

    int len = srsWkt.length() + 1;
    str_dup = new char[len];
    if (str_dup != NULL)
    {
        memset(str_dup, '\0', len);
        memcpy(str_dup, srsWkt.c_str(), len);
    }

    return str_dup;

#endif
}

///////////////////////////////////////////////////////////////////////////////
bool CCsArbitraryCoordinateSystemUtil::IsArbitrary(const wchar_t* wszWktCoordinateSystem)
{
    if(wszWktCoordinateSystem)
    {
        char* tempSrsWkt = Convert_Wide_To_UTF8(wszWktCoordinateSystem);
        string srsWkt = tempSrsWkt;
        delete [] tempSrsWkt;
        tempSrsWkt = NULL;

        if (srsWkt.find(LOCALCS)==0 || srsWkt.find(LOCAL_CS)==0)
        {
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool CCsArbitraryCoordinateSystemUtil::WktToCsmapCoordSys(const wchar_t* wszCoordSys, struct cs_Csprm_* pCsprm, STRING& error)
{
    bool bResult = false;

    if(!wszCoordSys || !pCsprm)
    {
        return false;
    }
    char* tempSrsWkt = Convert_Wide_To_UTF8(wszCoordSys);
    string srsWkt = tempSrsWkt;
    delete [] tempSrsWkt;
    tempSrsWkt = NULL;

    // Check for LOCALCS and LOCAL_CS, ie: Arbitrary XY systems
    if((srsWkt.find(LOCALCS) == 0) || (srsWkt.find(LOCAL_CS) == 0))
    {
        // This is a local coordinate system
        string sObject;
        vector<string> vComponents;
        bool bSuccess = ParseWktString(srsWkt, sObject, vComponents);
        if (bSuccess)
        {
            //-- intialize CS
            cs_Csdef_ csdef;
            // Clear the Mentor structure
            memset(&csdef, 0, sizeof(cs_Csdef_));
            strcpy(csdef.desc_nm, ARBITRARY_XY);
            //-- copied into the projection, datum, and ellipse names and aliases
            //strcpy(csdef.dat_knm, ARBITRARY_XY);
            //strcpy(csdef.elp_knm, ARBITRARY_XY);
            strcpy(csdef.prj_knm, NERTH);   // NERTH - is a Mentor define projection for Non-georeferenced (aka non-earth) coordinate system
            csdef.scl_red = 1.0;
            csdef.map_scl = 1.0;

            //-- safely copy the local coord sys name into the coord sys name
            string sName = vComponents.at(0);

            size_t nStartingPosition = 0;
            size_t nLength = 0;

            // Check to see if the phrase "Non-Earth" exists
            size_t nPosition = sName.find(NON_EARTH);
            if(nPosition != -1)
            {
                // The names are too long for some of these so we will shorten them.
                // Example #1
                // Original: Non-Earth (feet)
                // Modified: feet
                //
                // Example #2
                // Original: Non-Earth (Chain (Benoit))
                // Modified: Chain (Benoit)

                // We want to start at the first character after "Non-Earth ("
                nStartingPosition = nPosition + strlen(NON_EARTH) + 2;

                // We don't want the ending ")"
                nLength = sName.size() - nStartingPosition - 1;
            }
            else
            {
                // Try looking for "Nonearth" instead
                nPosition = sName.find(NONEARTH);
                if(nPosition != -1)
                {
                    nStartingPosition = nPosition + strlen(NONEARTH);
                    nLength = sName.size() - nStartingPosition;
                }
            }

            string sNewName;
            if (0==nLength)
            {
                sNewName=sName;
            }
            else
            {
                sNewName = sName.substr(nStartingPosition, nLength);
            }

            // Trim any white space
            Trim(sNewName);

            bool bFoundMentorUnitName=false;

            // Check to see if the name will fit
            size_t nMaxSize = (sizeof(csdef.key_nm) / sizeof(csdef.key_nm[0]))-1;
            if(sNewName.length() <= nMaxSize)
            {
                strcpy(csdef.key_nm, sNewName.c_str());

                //-- get the units
                string sUnits = "";
                if (GetWktComponent(vComponents, UNIT, sUnits))
                {
                    bSuccess = ParseWktString(sUnits, sObject, vComponents);
                    if (bSuccess)
                    {
                        string sUnitWkt = vComponents.at(0);
                        if(sUnitWkt.length() > 0)
                        {
                            wchar_t* wsUnits = Convert_UTF8_To_Wide(sUnitWkt.c_str());

                            //-- ensure the scale is valid
                            string sScale = vComponents.at(1);
                            double dScale = atof(sScale.c_str());
                            if (dScale >= ZERO_SCALE)
                            {
                                csdef.unit_scl = dScale;

                                //-- calculate CSMap 'scale', assume map_scl was initialized to 1.0
                                csdef.scale = 1.0 / (csdef.map_scl * csdef.unit_scl);

                                bResult = true;
                            }

                            // Lookup units and find matching code to define the Mentor unit name
                            //trying to map the Unit name extracted from the WKT to unit names used in stock arbitrary systems
                            for (int i = 0; i < CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystemsCount; i++)
                            {
                                if(_wcsicmp(wsUnits, CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[i].unitsCode) == 0)
                                {
                                    char* tempname = Convert_Wide_To_UTF8(CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[i].code);
                                    char* tempunit = Convert_Wide_To_UTF8(CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[i].unitsCode);
                                    CS_stncp(csdef.key_nm, tempname, sizeof (csdef.key_nm));
                                    CS_stncp(csdef.unit, tempunit, sizeof (csdef.unit));
                                    delete [] tempname;
                                    delete [] tempunit;
                                    bFoundMentorUnitName=true;
                                    break;
                                }
                            }
                            delete [] wsUnits;

                            //sometimes the name won't make it because it comes from Oracle for instance and the name is different there
                            //but based on the unit scale that can be valid we need to find the equivalent system in Mentor
                            if (!bFoundMentorUnitName)
                            {
                                for (int i = 0; i < CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystemsCount; i++)
                                {
                                    if (bResult && fabs (CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[i].conversionToMeters - csdef.unit_scl) < 5.0E-8)
                                    {
                                        char* tempname = Convert_Wide_To_UTF8(CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[i].code);
                                        char* tempunit = Convert_Wide_To_UTF8(CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[i].unitsCode);
                                        CS_stncp(csdef.key_nm, tempname, sizeof (csdef.key_nm));
                                        CS_stncp(csdef.unit, tempunit, sizeof (csdef.unit));
                                        delete [] tempname;
                                        delete [] tempunit;
                                        bFoundMentorUnitName=true;
                                        break;
                                    }
                                }
                            }

                            //if we could not map it to one of the Mentor arbitrary stock systems
                            //lets look up the unit in the Mentor unit table
                            //At least the unit name and code and scale could match even though we do not have the actual
                            //definition in our dictionary
                            if (!bFoundMentorUnitName)
                            {
                                //Look it up in Mentor's table
                                for (int iUnit=0; cs_Unittab[iUnit].type != cs_UTYP_END; iUnit++)
                                {
                                    if (fabs(cs_Unittab[iUnit].factor - csdef.unit_scl) < 5.0E-8)
                                    {
                                        CS_stncp(csdef.unit, cs_Unittab[iUnit].name, sizeof(csdef.unit));
                                        bFoundMentorUnitName=true;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            //if we do not have a valid Mentor unit name we cannot succeed in setting the system definition
            if (bResult && bFoundMentorUnitName)
            {
                bool bResult=BuildCsprmFromArbitraryDef(csdef, *pCsprm);
                if (!bResult)
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }

    return bResult;
}

namespace CSLibrary
{
//=============================================================================
//
//  Parse a Wkt string into the name of the object and its comma-delimited,
//  enclosed in brackets components.  Eg, Wkt string:
//     PROJECTION["TM",PARAMETER["Lat",1.0],PARAMETER["Long",1.0]]
//  ... parses into:
//     sObject = "PROJECTION"
//     vComponents[0] = "TM"
//     vComponents[1] = "PARAMETER["Lat",1.0]"
//     vComponents[2] = "PARAMETER["Long",1.0]"
//

bool ParseWktString(string& sWktOriginal, string& sObject, vector<string>& vComponents)
{
   bool bOK = true;

   //-- make a working copy
   string sWkt = sWktOriginal;
   RemoveHiddenChars(sWkt);

   //-- use a char* string to build the component name because appending
   //-- single characters onto a STL string is very slow
   char* szComponent = new char[sWkt.length()+1];
   strcpy(szComponent, "");

   vComponents.clear();
   sObject = "";

   size_t nLen = sWkt.length();
   size_t n = string::npos;
   for (size_t i = 0; i<nLen; i++)
   {
       if ( MyIsDBCSLeadByte( (unsigned char)sWkt.at(i) ) )
       {
           i++;    // skip trail byte
       }
       else if ( sWkt.at(i) == '[' )
       {
           n = i;
           break;
       }
   }

   // check if last character is multi-byte
   int k = 0;
   bool bIsLastMB = false;
   while (k < static_cast<int>(sWkt.length()))
   {
       if ( MyIsDBCSLeadByte((unsigned char)sWkt.at(k)) )
       {
           bIsLastMB = true;
           k++;
       }
       else
       {
           bIsLastMB = false;
       }

       k++;
   }

   if ((n==string::npos) || (n<=1))
   {
      bOK = false;
   }
   else if (bIsLastMB || sWkt.at(sWkt.length()-1) != ']')
   {
      bOK = false; //-- missing closing bracket
   }
   else
   {
      //-- sObject = string before leading '['
      sObject = sWkt.substr(0,n);
      Trim(sObject);

      //-- strip leading and trailing brackets
      string s;
      s = sWkt.substr(n+1,sWkt.length()-n-2);

      //-- extract comman delimited strings not enclosed by '[]' or '""'
      int nBrackets = 0;
      bool bQuoted = false;
      size_t nLen = s.length();
      for( size_t i=0; (i<nLen) && (nBrackets>=0); i++)
      {
         char ch = s.at(i);

         if (ch == '[') nBrackets += 1;
         if (ch == ']') nBrackets -= 1;
         if (ch == '"') bQuoted = !bQuoted;
         if (ch == ',')
         {
            if ((nBrackets==0) && (bQuoted==false))
            {
               string sComponent = szComponent;
               RemoveHiddenChars(sComponent); //-- this also trims the string
               //if (sComponent != "") || (ch == '"')) //-- empty string is valid
               //{
                  vComponents.push_back(sComponent);
               //}
               strcpy(szComponent,"");
            }
            else
            {
               char sz[2];
               sz[0] = ch;
               sz[1] = 0;
               strcat(szComponent,sz);
            }
         }
         else if ((ch!='"') || (nBrackets>0)) //-- don't add '"' outside of []'s
         {
            if ( MyIsDBCSLeadByte( (unsigned char)ch ))
            {
                char sz[3];
                sz[0] = ch;
                if (i+1<nLen)
                {
                    sz[1] = s.at(i+1);
                }
                else
                {
                    sz[1] = 0;
                }
                sz[2] = 0;
                strcat(szComponent,sz);
                i++;     // skip trail byte
            }
            else
            {
                char sz[2];
                sz[0] = ch;
                sz[1] = 0;
                strcat(szComponent,sz);
            }
         }
      }

      //-- add the last string if not empty
      {
         string sComponent = szComponent;
         RemoveHiddenChars(sComponent); //-- this also trims the string
         if (sComponent != "")
         {
            vComponents.push_back(sComponent);
         }
      }

      //-- ensure brackets are all accounted for
      bOK = (nBrackets==0);
   }


   delete [] szComponent;

   return bOK;
}

bool GetWktComponent(vector<string>& vComponents, const char* szName, string& sComponent)
{
   bool bFound = false;

   for( size_t i=0; i<(vComponents.size()); i++)
   {
      string& s = vComponents.at(i);
      if (BeginsWith(s, szName))
      {
         sComponent = s;
         bFound = true;
      }
   }
   return bFound;
}

//=============================================================================
//  Remove hidden characters from a string: '\n' '\t' '\r' '\b'.  Leading and
//  trailing hidden chars are removed, all others are replaced with ' '.
void RemoveHiddenChars(string& s)
{
   size_t nLen = s.length();
   for( size_t i=0; i<nLen; i++)
   {
      char ch = s.at(i);
      if ((ch=='\n') || (ch=='\t') || (ch=='\r') || (ch=='\b'))
      {
         s.at(i) = ' ';
      }
   }

   Trim(s);
}

bool MyIsDBCSLeadByte(unsigned short c)
{
#ifdef _WIN32
    return IsDBCSLeadByte(static_cast<BYTE>(c)) ? true : false;
#else
    //BOGUS
    //No DBCS on Linux, so this should not matter
    return false;
#endif
}

//=============================================================================
//  Trim leading/trailing whitespace from a string
void Trim(string& s)
{
   int nBegin = 0;
   while ((nBegin < static_cast<int>(s.length())) && (s.at(nBegin)==' '))
   {
      nBegin++;
   }

   int nEnd = (int)(s.length()-1);
   while ((nEnd>=0) && (s.at(nEnd)==' '))
   {
      nEnd--;
   }

   if ((nBegin!=0) || (nEnd!=(s.length()-1)))
   {
      string sTemp = s.substr(nBegin, nEnd-nBegin+1);
      s = sTemp;
   }
}

//=============================================================================
//  Does string 's1' begin with 's2'?
//
bool BeginsWith(string& s1, const char* s2)
{
   return BeginsWith(s1.c_str(), s2);
}

bool BeginsWith(const char* s1, const char* s2)
{
   bool bOK = false;
   if ((strlen(s1) >= strlen(s2)) && (strlen(s2)>0))
   {
      if (_strnicmp(s1,s2,strlen(s2)) == 0)
      {
         bOK = true;
      }
   }
   return bOK;
}

//-- Format a floating point number as a string so that it has enough
//-- significat digits.  Use this routine to centralize number formatting
//-- throughout the component.
const char* FormatNumber(string& sNumber, double dNumber)
{
   const int SIG_DIGITS = 17;
   char sz[80] = "";

   if (fabs(dNumber) > 1e-4)
   {
      sprintf(sz, "%-.*lf", SIG_DIGITS, dNumber);

      //-- trim trailing zeros
      for( int i=(int)(strlen(sz)-1); (i>0) && (sz[i]=='0'); i--)
      {
         sz[i] = '\0';
      }

      //-- trim trailing '.'
      if ((strlen(sz)>0) && (sz[strlen(sz)-1] == '.'))
      {
         sz[strlen(sz)-1] = '\0';
      }
   }
   else if (fabs(dNumber) > ZERO_VALUE)
   {
      sprintf(sz, "%-.*lE", SIG_DIGITS, dNumber);
   }
   else
   {
      strcpy(sz, "0");
   }


   sNumber = sz;
   if (strstr(sNumber.c_str()," "))
   {
      Trim(sNumber);
   }

   return sNumber.c_str();
}

} // End namespace


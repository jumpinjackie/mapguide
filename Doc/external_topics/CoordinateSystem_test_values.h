/**
\addtogroup CSTestValues
<p>
  These values are used for testing the coordinate system API.
</p>
<h2>Geodetic Coordinate System </h2>
<h3>Used in the PHP Sample Code</h3>
<p>
  The following well-known text represents a coordinate system
  based on longitude and latitude whose name is "Longitude /
  Latitude (NAD 83)". The horizontal datum used to measure
  positions on the surface of the earth is named "NAD 83". The
  earth's surface is modeled as a squashed sphere. The name of
  the model is "GRS 80". The "GRS 80" model has a semi-major
  axis value of 6378137 and an inverse flattening value of
  298.257222101. The longitudinal measurement is taken from the
  Greenwich prime meridian. The Greenwich prime meridian is at
  an angle of 0.000000 degrees relative to the reference
  (Greenwich) prime meridian. The unit of measurement is an
  angular type whose name is "Decimal Degree", which can be
  converted to radians by applying the scalar factor of
  0.01745329251994330.
</p>
<pre>
SQL\> select cs_name,srid, wktext from mdsys.cs_srs where srid = 8265;
cs_name = Longitude / Latitude (NAD 83)
 srid=8265
wktext=
GEOGCS [
   "Longitude / Latitude (NAD 83)",
   DATUM
      "NAD 83",
      SPHEROID [
         "GRS 80",
         6378137,
         298.257222101
      ]
   ],
   PRIMEM [
      "Greenwich",
      0.000000
   ],
   UNIT [
      "Decimal Degree",
      0.01745329251994330
   ]
]
</pre>
<h3>Used in the C# Sample Code</h3>
<p>
  The following well-known text represents a coordinate system
  based on longitude and latitude whose name is "LL83".
  The horizontal datum used to measure
  positions on the surface of the earth is named "NAD83". The
  earth's surface is modeled as a squashed sphere. The name of
  the model is "GRS 80". The "GRS 80" model has a semi-major
  axis value of 6378137 and an inverse flattening value of
  298.257222101. The longitudinal measurement is taken from the
  Greenwich prime meridian. The Greenwich prime meridian is at
  an angle of 0.000000 degrees relative to the reference
  (Greenwich) prime meridian. The unit of measurement is an
  angular type whose name is "Decimal Degree", which can be
  converted to radians by applying the scalar factor of
  0.01745329251994330.

</p>
<pre>
GEOGCS [
   "LL83",
   DATUM [
      "NAD83",
      SPHEROID [
         "GRS1980",
	 6378137.000,
	 298.25722210
      ]
   ],
   PRIMEM [
      "Greenwich",
      0
   ],
   UNIT [
      "Degree",
      0.01745329251994
   ]
]
</pre>
<h2>Projected Coordinate System </h2>
<h3>Used in the PHP Sample Code</h3>
<p>
  The following well-known text represents a projected
  coordinate system whose name is "UTM Zone 18 (NAD 83)". It
  uses a parameterized transform called "Transverse Mercator"
  to transform geographic coordinates into the projected
  coordinates. The source coordinates are expressed in terms of
  a geographic coordinate system whose name is "NAD 83
  (Continental US)." based on longitude and latitude whose name
  is "Longitude / Latitude (NAD 83)". The "Transverse Mercator"
  transform takes three parameters. The name of the first
  parameter is "Scale_Factor", and its value is 0.999600. The
  name of the second parameter is "Central_Meridian", and its
  value is -117.000000. The name of the third parameter is
  "False_Easting", and its value is 500000.000000. The unit of
  measurement is a linear type whose name is "Meter", which can
  be converted to meters by applying the scalar factor of
  1.000000000000.
</p>
<pre>
SQL\> select cs_name,srid, wktext from mdsys.cs_srs where srid = 82217;
cs_name=UTM Zone 11 (NAD 83)
srid=82254
wktext=
PROJCS[
   "UTM Zone 18 (NAD 83)",
   GEOGCS [
      "NAD 83 (Continental US)",
      DATUM [
         "NAD 83 (Continental US)",
         SPHEROID [
            "GRS 80",
            6378137,
            298.257222101
         ]
      ],
      PRIMEM [
         "Greenwich", 0.000000
      ],
      UNIT [
         "Decimal Degree",
         0.01745329251994330
      ]
   ],
   PROJECTION [
      "Transverse Mercator"
   ],
   PARAMETER [
      "Scale_Factor",
      0.999600
   ],
   PARAMETER [
      "Central_Meridian",
      \-75.000000
   ],
   PARAMETER [
      "False_Easting",
      500000.000000
   ],
   UNIT [
      "Meter",
      1.000000000000
   ]
]
</pre>
<h3>Used in the C# Sample Code</h3>
<p>
  The following well-known text represents a projected
  coordinate system whose name is "UTM83-4". It
  uses a parameterized transform called "Transverse Mercator"
  to transform geographic coordinates into the projected
  coordinates. The source coordinates are expressed in terms of
  a geographic coordinate system whose name is "LL83".
  The "Transverse Mercator" transform takes five parameters.
  The name of the first parameter is "false_easting", and its value is 500000.000.
  The name of the second parameter is "false_northing", and its value is 0.000.
  The name of the third parameter is "central_meridian", and its value is -159.00000000000000.
  The name of the fourth parameter is "scale_factor", and its value is 0.9996.
  The name of the fifth parameter is "latitude_of_origin", and its value is 0.000.
  The unit of measurement is a linear type whose name is "Meter", which can
  be converted to meters by applying the scalar factor of 1.000000000000.
</p>
<pre>
PROJCS[
  "UTM83-4",
  GEOGCS[
    "LL83",
    DATUM[
      "NAD83",
      SPHEROID[
        "GRS1980",
	6378137.000,
	298.25722210
      ]
    ],
    PRIMEM[
      "Greenwich",
      0
    ],
    UNIT[
      "Degree",
      0.017453292519943295
    ]
  ],
  PROJECTION[
    "Transverse_Mercator"
  ],
  PARAMETER[
    "false_easting",
    500000.000
  ],
  PARAMETER[
    "false_northing",
    0.000
  ],
  PARAMETER[
    "central_meridian",
    -159.00000000000000
  ],
  PARAMETER[
    "scale_factor",
    0.9996
  ],
  PARAMETER[
    "latitude_of_origin",
    0.000
  ],
  UNIT[
    "Meter",
    1.00000000000000
  ]
]
</pre>
<h2> Arbitrary Coordinate System </h2>
<p>
  The following well-known text represents a local,
  ungeoreferenced coordinate system whose name is "Non-Earth
  (Mile)." The horizontal datum used to measure positions in
  the coordinate system is named "Local Datum". The type of the
  horizontal datum is 0. The unit of measurement is a linear
  type whose name is "Mile", which can be converted to meters
  by applying the scalar factor of 1.609344e+03. The system has
  two AXIS values indicating that it is two-dimensional. The
  directional value associated with the "X" AXIS signifies that
  "X" increases positively in value moving east from the
  origin. The directional value associated with the "Y" AXIS
  signifies that "Y" increases positively in value moving north
  from the origin.
</p>
<pre>
select cs_name, srid, wktext from mdsys.cs_srs where wktext like '%LOCAL_CS%';
cs_name=Non-Earth (Mile)
srid=262156
wktext=
LOCAL_CS [
   "Non-Earth (Mile)",
   LOCAL_DATUM [
      "Local Datum",
      0
   ]
    UNIT [
      "Mile",
      1.609344e+03
   ],
   AXIS [
      "X",
      EAST
   ],
   AXIS[
      "Y",
      NORTH
   ]
]
</pre>
**/

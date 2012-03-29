/**
\addtogroup WKTofSRS


<p>
This extended BNF grammar represents the syntax of a
Well-Known Text (WKT) specification of a Spatial Reference
System (SRS), also known as a coordinate system. The grammar
is taken verbatim from \link OGC01009 [OGC01-009] \endlink and is
reproduced here for convenience.
\code
<math transform> = <param mt> | <concat mt> | <inv mt> | <passthrough mt>
<param mt> = PARAM_MT["<classification name>" {,<parameter>}* ]
<parameter> = PARAMETER["<name>", <value>]
<value> = <number>
<concat mt> = CONCAT_MT[<math transform> {,<math transform>}* ]
<inv mt> = INVERSE_MT[<math transform>]
<passthrough mt> = PASSTHROUGH_MT[<integer>, <math transform>]
<coordinate system> = <horz cs> | <geocentric cs> | <vert cs> | <compd cs> | <fitted cs> | <local cs>
<horz cs> = <geographic cs> | <projected cs>
<projected cs> = PROJCS["<name>", <geographic cs>, <projection>, {<parameter>,}* <linear unit> {,<twin axes>}{,<authority>}]
<projection> = PROJECTION["<name>" {,<authority>}]
<geographic cs> = GEOGCS["<name>", <datum>, <prime meridian>, <angular unit> {,<twin axes>} {,<authority>}]
<datum> = DATUM["<name>", <spheroid> {,<to wgs84>} {,<authority>}]
<spheroid> = SPHEROID["<name>", <semi-major axis>, <inverse flattening> {,<authority>}]
<semi-major axis> = <number>
<inverse flattening> = <number>
<prime meridian> = PRIMEM["<name>", <longitude> {,<authority>}]
<longitude> = <number>
<angular unit> = <unit>
<linear unit> = <unit>
<unit> = UNIT["<name>", <conversion factor> {,<authority>}]
<conversion factor> = <number>
<geocentric cs> = GEOCCS["<name>", <datum>, <prime meridian>, <linear unit> {,<axis>, <axis>, <axis>} {,<authority>}]
<authority> = AUTHORITY["<name>", "<code>"]
<vert cs> = VERT_CS["<name>", <vert datum>, <linear unit>, {<axis>,} {,<authority>}]
<vert datum> = VERT_DATUM["<name>", <datum type> {,<authority>}]
<datum type> = <number>
<compd cs> = COMPD_CS["<name>", <head cs>, <tail cs> {,<authority>}]
<head cs> = <coordinate system>
<tail cs> = <coordinate system>
<twin axes> = <axis>, <axis>
<axis> = AXIS["<name>", NORTH | SOUTH | EAST | WEST | UP | DOWN | OTHER]
<to wgs84s> = TOWGS84[<seven param>]
<seven param> = <dx>, <dy>, <dz>, <ex>, <ey>, <ez>, <ppm>
<dx> = <number>
<dy> = <number>
<dz> = <number>
<ex> = <number>
<ey> = <number>
<ez> = <number>
<ppm> = <number>
<fitted cs> = FITTED_CS["<name>", <to base>, <base cs>]
<to base> = <math transform>
<base cs> = <coordinate system>
<local cs> = LOCAL_CS["<name>", <local datum>, <unit>, <axis>, {,<axis>}* {,<authority>}]
<local datum> = LOCAL_DATUM["<name>", <datum type> {,<authority>}]
\endcode

**/
/**
\addtogroup boundary

<p>
\link OGC99049 [OGC99-049] \endlink states that &quot;the boundary of a
geometry is a set of geometries of the next lower dimension.
The boundary of a Point or MultiPoint is the empty set. The
boundary of a non-closed Curve consists of itw two end
Points, the boundary of a closed Curve is empty. The boundary
of a MultiCurve consists of those Points that are in the
boundaries of an odd number of its element Curves. The
boundary of a Polygon consists of its set of Rings. The
boundary of a MultiPolygon consists of the set of Rings of
its Polygons. The boundary of an arbitrary collection of
geometries whose interiors are disjoint consists of
geometries drawn from the boundaries of the element
geometries by application of the 'mod 2' union rule.&quot;
</p>

**/

/** 
\addtogroup Curves
<h2>
  Summary
</h2>
<p>
  The curve geometry classes are MgCurveString, MgMultiCurveString, MgCurvePolygon, and MgMultiCurvePolygon.
</p>
<p>
  A provider may not support the construction of curve
  geometries or may not support their construction in all
  coordinate systems.
</p>
<p>
  The fundamental building block of the curve geometries is the
  curve segment. MgCurveSegment is the abstract base class, and MgLinearSegment and MgArcSegment are two concrete classes.
</p>
<p>
  These two classes are classified as geometry components; they
  are not geometries but are used to construct geometries. The
  coordinates defining an MgLinearSegment are interpolated
  linearly. The three coordinates (start, end, and control)
  defining an MgArcSegment belong to a circle.
</p>
<ul>
  <li>
    <p>
      An MgCurveString is composed of a collection of connected
      curve segments, which is not closed. The ending coordinate of
      one segment is the starting coordinate of the next segment
      in the sequence. The ending coordinate of the last segment is
      not the starting coordinate of the first segment. The curve
      string as a whole cannot be self-intersecting.
    </p>
  </li>
  <li>
    <p>
      An MgMultiCurveString is a geometry constructed from a
      collection of MgCurveString objects.
    </p>
  </li>
  <li>
    <p>
      An MgCurveRing is also a geometry component. It is used to
      construct an MgCurvePolygon geometry. Like the MgCurveString,
      it is composed of a collection of connected curve segments.
      But unlike the MgCurveString, it is closed. That is, the
      ending coordinate of the last segment is the starting
      coordinate of the first segment.
    </p>
    <p>
      An MgCurveRing can be constructed using a collection of curve
      segments containing only one member. An MgArcSegment can have
      a starting coordinate that is the same as the ending
      coordinate. This case places a special condition on the
      control point; it is taken to be the point diametrically
      opposed to the starting point.
    </p>
  </li>
  <li>
    <p>
      An MgCurvePolygon consists of an exterior boundary defined by
      an MgCurveRing and an optional collection of inner boundaries
      (&quot;holes&quot;).
    </p>
  </li>
  <li>
    <p>
      An MgMultiCurvePolygon is a geometry constructed from a
      collection of MgCurvePolygon objects.
    </p>
  </li>
</ul>
**/
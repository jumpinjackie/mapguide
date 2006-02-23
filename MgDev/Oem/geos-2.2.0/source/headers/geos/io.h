/**********************************************************************
 * $Id: io.h,v 1.5.2.1.2.1 2005/11/29 16:58:17 strk Exp $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 * Copyright (C) 2005 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#ifndef GEOS_IO_H
#define GEOS_IO_H

#include <memory>
#include <iostream>
#include <string>
#include <geos/platform.h>
#include <geos/geom.h>
#include <geos/util.h>
#include <iostream>
#include <iomanip>

using namespace std;

namespace geos {

/// An unsigned char
typedef unsigned char byte;

/**
 * \class ParseException io.h geos.h
 * \brief Notifies a parsing error
 */
class ParseException: public GEOSException {
public:
	ParseException();
	ParseException(string msg);
	ParseException(string msg, string var);
	ParseException(string msg, double num);
	~ParseException();
};

class StringTokenizer {
public:
	enum {
		TT_EOF,
		TT_EOL,
		TT_NUMBER,
		TT_WORD
	};
	StringTokenizer();
	StringTokenizer(string txt);
	~StringTokenizer();
	int nextToken();
	int peekNextToken();
	double getNVal();
	string getSVal();
private:
	string str;
	string stok;
	double ntok;
};

/**
 * \class WKTReader io.h geos.h
 * \brief WKT parser class; see also WKTWriter.
 */
class WKTReader {
public:
	//WKTReader();

	/**
	 * \brief Inizialize parser with given GeometryFactory.
	 *
	 * Note that all Geometry objects created by the
	 * parser will contain a pointer to the given factory
	 * so be sure you'll keep the factory alive for the
	 * whole WKTReader and created Geometry life.
	 */
	WKTReader(const GeometryFactory *gf);

	~WKTReader();

	/// Parse a WKT string returning a Geometry
	Geometry* read(string wellKnownText);

//	Geometry* read(Reader reader);	//Not implemented yet

protected:
	CoordinateSequence* getCoordinates(StringTokenizer *tokenizer);
	double getNextNumber(StringTokenizer *tokenizer);
	string getNextEmptyOrOpener(StringTokenizer *tokenizer);
	string getNextCloserOrComma(StringTokenizer *tokenizer);
	string getNextCloser(StringTokenizer *tokenizer);
	string getNextWord(StringTokenizer *tokenizer);
	Geometry* readGeometryTaggedText(StringTokenizer *tokenizer);
	Point* readPointText(StringTokenizer *tokenizer);
	LineString* readLineStringText(StringTokenizer *tokenizer);
	LinearRing* readLinearRingText(StringTokenizer *tokenizer);
	MultiPoint* readMultiPointText(StringTokenizer *tokenizer);
	Polygon* readPolygonText(StringTokenizer *tokenizer);
	MultiLineString* readMultiLineStringText(StringTokenizer *tokenizer);
	MultiPolygon* readMultiPolygonText(StringTokenizer *tokenizer);
	GeometryCollection* readGeometryCollectionText(StringTokenizer *tokenizer);
private:
	const GeometryFactory *geometryFactory;
	const PrecisionModel *precisionModel;
	Coordinate* getPreciseCoordinate(StringTokenizer *tokenizer);
	bool isNumberNext(StringTokenizer *tokenizer);
};

class Writer {
public:
	Writer();
	~Writer();
	void write(string txt);
	string toString();
private:
	string str;
};

/**
 * \class WKTWriter io.h geos.h
 *
 * \brief Outputs the textual representation of a Geometry.
 * See also WKTReader.
 * 
 * The WKTWriter outputs coordinates rounded to the precision
 * model. No more than the maximum number of necessary decimal places will be
 * output.
 * 
 * The Well-known Text format is defined in the <A
 * HREF="http://www.opengis.org/techno/specs.htm">OpenGIS Simple Features
 * Specification for SQL</A>.
 * 
 * A non-standard "LINEARRING" tag is used for LinearRings. The WKT spec does
 * not define a special tag for LinearRings. The standard tag to use is
 * "LINESTRING".
 *
 * See WKTReader for parsing.
 *
 */
class WKTWriter {
public:
	WKTWriter();
	~WKTWriter();

	//string(count, ch) can be used for this
	//static string stringOfChar(char ch, int count);

	/// Returns WKT string for the given Geometry
	string write(const Geometry *geometry);

	// Send Geometry's WKT to the given Writer
	void write(const Geometry *geometry, Writer *writer);

	string writeFormatted(const Geometry *geometry);

	void writeFormatted(const Geometry *geometry, Writer *writer);

protected:
	string formatter;
	void appendGeometryTaggedText(const Geometry *geometry, int level, Writer *writer);
	void appendPointTaggedText(const Coordinate* coordinate, int level, Writer *writer, const PrecisionModel* precisionModel);
	void appendLineStringTaggedText(const LineString *lineString, int level, Writer *writer);
	void appendLinearRingTaggedText(const LinearRing *lineString, int level, Writer *writer);
	void appendPolygonTaggedText(const Polygon *polygon, int level, Writer *writer);
	void appendMultiPointTaggedText(const MultiPoint *multipoint, int level, Writer *writer);
	void appendMultiLineStringTaggedText(const MultiLineString *multiLineString, int level,Writer *writer);
	void appendMultiPolygonTaggedText(const MultiPolygon *multiPolygon, int level, Writer *writer);
	void appendGeometryCollectionTaggedText(const GeometryCollection *geometryCollection, int level,Writer *writer);
	void appendPointText(const Coordinate* coordinate, int level, Writer *writer, const PrecisionModel* precisionModel);
	void appendCoordinate(const Coordinate* coordinate, Writer *writer, const PrecisionModel* precisionModel);
	string writeNumber(double d);
	void appendLineStringText(const LineString *lineString, int level, bool doIndent, Writer *writer);
	void appendPolygonText(const Polygon *polygon, int level, bool indentFirst, Writer *writer);
	void appendMultiPointText(const MultiPoint *multiPoint, int level, Writer *writer);
	void appendMultiLineStringText(const MultiLineString *multiLineString, int level, bool indentFirst,Writer *writer);
	void appendMultiPolygonText(const MultiPolygon *multiPolygon, int level, Writer *writer);
	void appendGeometryCollectionText(const GeometryCollection *geometryCollection, int level,Writer *writer);
private:
	enum {
		INDENT = 2
	};
//	static const int INDENT = 2;
	static string createFormatter(const PrecisionModel* precisionModel);
	bool isFormatted;
	int level;
	void writeFormatted(const Geometry *geometry, bool isFormatted, Writer *writer);
	void indent(int level, Writer *writer);
};

/*
 * \class ByteOrderValues io.h geos.h
 * 
 * Methods to read and write primitive datatypes from/to byte
 * sequences, allowing the byte order to be specified
 * 
 * Similar to the standard Java <code>ByteBuffer</code> class.
 */
class ByteOrderValues {

public:
	/*final*/ static int ENDIAN_BIG;
	/*final*/ static int ENDIAN_LITTLE;

	static int getInt(const byte *buf, int byteOrder);
	static void putInt(int intValue, byte *buf, int byteOrder);

	static int64 getLong(const byte *buf, int byteOrder);
	static void putLong(int64 longValue, byte *buf, int byteOrder);

	static double getDouble(const byte *buf, int byteOrder);
	static void putDouble(double doubleValue, byte *buf, int byteOrder);

};

/*
 * \class ByteOrderDataInStream io.h geos.h
 * 
 * Allows reading an stream of primitive datatypes from an underlying
 * istream, with the representation being in either common byte ordering.
 *
 */
class ByteOrderDataInStream {

public:

	ByteOrderDataInStream(istream *s=NULL):
		byteOrder(getMachineByteOrder()),
		stream(s) {}
	~ByteOrderDataInStream() {}

	/**
	 * Allows a single ByteOrderDataInStreamT to be reused
	 * on multiple istream.
	 */
	void setInStream(istream *s) { stream=s; }
	void setOrder(int order) { byteOrder=order; }

	byte readByte() // throws ParseException
	{
		stream->read(reinterpret_cast<char *>(buf), 1);
		if ( stream->eof() )
			throw new ParseException("Unespected EOF parsing WKB");
		return buf[0];
	}

	int readInt() // throws ParseException
	{
		stream->read(reinterpret_cast<char *>(buf), 4);
		if ( stream->eof() )
			throw new ParseException("Unespected EOF parsing WKB");
		return ByteOrderValues::getInt(buf, byteOrder);
	}

	long readLong() // throws ParseException
	{
		stream->read(reinterpret_cast<char *>(buf), 8);
		if ( stream->eof() )
			throw new ParseException("Unespected EOF parsing WKB");
		return ByteOrderValues::getLong(buf, byteOrder);
	}

	double readDouble() // throws ParseException
	{
		stream->read(reinterpret_cast<char *>(buf), 8);
		if ( stream->eof() )
			throw new ParseException("Unespected EOF parsing WKB");
		return ByteOrderValues::getDouble(buf, byteOrder);
	}

private:
	int byteOrder;
	istream *stream;

	// buffers to hold primitive datatypes
	byte buf[8];

};

/**
 * Constant values used by the WKB format
 */
namespace WKBConstants {
	const int wkbXDR = 0;
	const int wkbNDR = 1;
	const int wkbPoint = 1;
	const int wkbLineString = 2;
	const int wkbPolygon = 3;
	const int wkbMultiPoint = 4;
	const int wkbMultiLineString = 5;
	const int wkbMultiPolygon = 6;
	const int wkbGeometryCollection = 7;
}


/**
 * \class WKBReader io.h geos.h
 *
 * \brief Reads a Geometry from Well-Known Binary format.
 *
 * This class is designed to support reuse of a single instance to read
 * multiple geometries. This class is not thread-safe; each thread should
 * create its own instance.
 *
 * The Well-known Binary format is defined in the <A
 * HREF="http://www.opengis.org/techno/specs.htm">OpenGIS Simple Features
 * Specification for SQL</A>.
 * This implementation supports the extended WKB standard which allows
 * representing 3-dimensional coordinates.
 *
 */
class WKBReader {

public:

	WKBReader(const GeometryFactory &f): factory(f) {};

	/**
	 * \brief Reads a Geometry from an istream.
	 *
	 * @param is the stream to read from 
	 * @return the Geometry read
	 * @throws IOException
	 * @throws ParseException
	 */
	Geometry *read(istream &is);
		// throws IOException, ParseException

	/**
	 * \brief Print WKB in HEX form to out stream
	 *
	 * @param is is the stream to read from
	 * @param os is the stream to write to
	 */
	static ostream &printHEX(istream &is, ostream &os);
 
private:

	static string BAD_GEOM_TYPE_MSG;

	const GeometryFactory &factory;

	// for now support the WKB standard only - may be generalized later
	unsigned int inputDimension;

	ByteOrderDataInStream dis;

	vector<double> ordValues;

	Geometry *readGeometry();
		// throws IOException, ParseException

	Point *readPoint();
		// throws IOException

	LineString *readLineString();
		// throws IOException

	LinearRing *readLinearRing();
		// throws IOException

	Polygon *readPolygon();
		// throws IOException

	MultiPoint *readMultiPoint();
		// throws IOException, ParseException

	MultiLineString *readMultiLineString();
		// throws IOException, ParseException

	MultiPolygon *readMultiPolygon();
		// throws IOException, ParseException

	GeometryCollection *readGeometryCollection();
		// throws IOException, ParseException

	CoordinateSequence *readCoordinateSequence(int); // throws IOException

	void readCoordinate(); // throws IOException


};

/**
 *
 * \class WKBWriter io.h geos.h
 *
 * \brief Writes a Geometry into Well-Known Binary format.
 *
 * The WKB format is specified in the OGC Simple Features for SQL specification.
 * This implementation supports the extended WKB standard for representing
 * 3-dimensional coordinates.  The presence of 3D coordinates is signified
 * by setting the high bit of the wkbType word.
 *
 * Empty Points cannot be represented in WKB; an
 * IllegalArgumentException will be thrown if one is
 * written. The WKB specification does not support representing LinearRing
 * they will be written as LineString
 * 
 * This class is designed to support reuse of a single instance to read multiple
 * geometries. This class is not thread-safe; each thread should create its own
 * instance.
 *
 * @see WKBReader
 */
class WKBWriter {

public:

	WKBWriter(int dims=2, int bo=getMachineByteOrder());

	/**
	 * \brief Write a Geometry to an ostream.
	 *
	 * @param g the geometry to write
	 * @param os the output stream
	 * @throws IOException
	 */
	void write(const Geometry &g, ostream &os);
		// throws IOException, ParseException

private:

	int outputDimension;

	int byteOrder;

	ostream *outStream;

	byte buf[8];

	void writePoint(const Point &p);
		// throws IOException

	void writeLineString(const LineString &ls);
		// throws IOException

	void writePolygon(const Polygon &p);
		// throws IOException

	void writeGeometryCollection(const GeometryCollection &c, int wkbtype);
		// throws IOException, ParseException

	void writeCoordinateSequence(const CoordinateSequence &cs, bool sized);
		// throws IOException

	void writeCoordinate(const CoordinateSequence &cs, int idx, bool is3d);
		// throws IOException

	void writeGeometryType(int geometryType);
		// throws IOException

	void writeByteOrder();
		// throws IOException

	void writeInt(int intValue);
		// throws IOException

};

} // namespace geos

#endif

/**********************************************************************
 * $Log: io.h,v $
 * Revision 1.5.2.1.2.1  2005/11/29 16:58:17  strk
 * Back-ported WKB IO and C api.
 * Added required higher dimensional interfaces for CoordinateSequence
 *
 * Revision 1.5.2.1  2005/05/23 18:16:40  strk
 * more math.h to cmath conversions
 *
 * Revision 1.5  2004/07/19 10:33:12  strk
 * Class documentation changed to report geos.h as WKT writer/parser header file
 *
 * Revision 1.4  2004/07/08 19:34:49  strk
 * Mirrored JTS interface of CoordinateSequence, factory and
 * default implementations.
 * Added DefaultCoordinateSequenceFactory::instance() function.
 *
 * Revision 1.3  2004/07/07 10:29:54  strk
 * Adjusted exceptions documentation.
 *
 * Revision 1.2  2004/07/07 09:38:12  strk
 * Dropped WKTWriter::stringOfChars (implemented by std::string).
 * Dropped WKTWriter default constructor (internally created GeometryFactory).
 * Updated XMLTester to respect the changes.
 * Main documentation page made nicer.
 *
 * Revision 1.1  2004/07/02 13:20:42  strk
 * Header files moved under geos/ dir.
 *
 * Revision 1.15  2004/07/01 14:12:44  strk
 *
 * Geometry constructors come now in two flavors:
 * 	- deep-copy args (pass-by-reference)
 * 	- take-ownership of args (pass-by-pointer)
 * Same functionality is available through GeometryFactory,
 * including buildGeometry().
 *
 * Revision 1.14  2004/03/18 10:42:44  ybychkov
 * "IO" and "Util" upgraded to JTS 1.4
 * "Geometry" partially upgraded.
 *
 * Revision 1.13  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 *
 **********************************************************************/


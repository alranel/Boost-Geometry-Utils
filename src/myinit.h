#undef read
#undef bind
#undef times
#undef open
#undef seekdir
#undef setbuf
#undef abort
#undef close
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/ring.hpp>
#include <boost/geometry/multi/geometries/multi_linestring.hpp>
#include <boost/geometry/io/wkt/wkt.hpp>

typedef boost::geometry::model::d2::point_xy<double> point_xy;
typedef boost::geometry::model::polygon<point_xy,false,false> polygon;
typedef boost::geometry::model::linestring<point_xy> linestring;
typedef boost::geometry::model::multi_linestring<linestring> multi_linestring;
typedef boost::geometry::model::ring<point_xy,false,false> ring;

typedef boost::geometry::model::polygon<point_xy,false,false> opolygon;
typedef boost::geometry::model::multi_linestring<linestring> omultilinestring;

// IVSIZE is from perl/lib/CORE/config.h, defined as sizeof(IV)
#if 0 && IVSIZE >= 8

// if Perl integers are 64 bit:

// for av_fetch, use SvIV() 
// this library then supports 64 bit ints.
#define av_fetch_x(AV) SvIV(*av_fetch(AV, 0, 0))
#define av_fetch_y(AV) SvIV(*av_fetch(AV, 1, 0))

// for av_store, use newSViv()
#define av_store_point_xy(AV, X, Y)             \
  av_store(AV, 0, newSViv(X));                   \
  av_store(AV, 1, newSViv(Y))
#else
// otherwise:

// for av_fetch, coerce the Perl scalar to a double, with SvNV()
// Perl doubles commonly allow 53 bits for the mantissa.
// So in the common case, this library supports 53 bit integers,
// stored in doubles on the Perl side.
#define av_fetch_x(AV) SvNV(*av_fetch(AV, 0, 0))
#define av_fetch_y(AV) SvNV(*av_fetch(AV, 1, 0))

// for av_store, we expect Boost integers to fit in the
// 53 bit mantissa of a Perl double
#define av_store_point_xy(AV, X, Y)             \
  av_store(AV, 0, newSVnv(X));                  \
  av_store(AV, 1, newSVnv(Y))
#endif

#define av_fetch_point_xy(AV)                                   \
  make<point_xy>(av_fetch_x(innerav), av_fetch_y(innerav))

#include "poly2av.h"
#include "mline2av.h"

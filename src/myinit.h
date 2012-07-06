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
typedef boost::geometry::model::polygon<point_xy,false,false> opolygon;
typedef boost::geometry::model::linestring<point_xy> linestring;
typedef boost::geometry::model::multi_linestring<linestring> multilinestring;
typedef boost::geometry::model::ring<point_xy,false,false> ring;

#include "poly2av.h"

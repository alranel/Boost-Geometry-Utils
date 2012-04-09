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
#include <boost/geometry/multi/geometries/multi_linestring.hpp>
#include <boost/geometry/io/wkt/wkt.hpp>

typedef boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double>,false,false> polygon;
typedef boost::geometry::model::linestring<boost::geometry::model::d2::point_xy<double> > linestring;
typedef boost::geometry::model::multi_linestring<linestring> multilinestring;

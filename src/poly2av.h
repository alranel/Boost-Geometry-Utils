#ifndef bgu_poly2av_h_
#define bgu_poly2av_h_

#include "myinit.h"
#include <boost/geometry/algorithms/num_points.hpp>

SV*
polygon2perl(pTHX_ const polygon& poly)
{
  AV* av = newAV();
  AV* innerav;
  const unsigned int len = boost::geometry::num_points(poly);
  av_extend(av, len-1);
  for (unsigned int i = 0; i < len; i++) {
    innerav = newAV();
    av_store(av, i, newRV_noinc((SV*)innerav));
    av_fill(innerav, 1);
    // IVSIZE is from perl/lib/CORE/config.h, defined as sizeof(IV)
#if IVSIZE >= 8
    // if Perl integers are 64 bit, use newSViv()
    av_store(innerav, 0, newSViv(poly[i].X));
    av_store(innerav, 1, newSViv(poly[i].Y));
#else
    // otherwise we expect Clipper integers to fit in the
	// 53 bit mantissa of a Perl double
    av_store(innerav, 0, newSVnv(poly[i].X));
    av_store(innerav, 1, newSVnv(poly[i].Y));
#endif


  }
  return (SV*)newRV_noinc((SV*)av);
}



polygon*
perl2polygon(pTHX_ AV* theAv)
{
    using boost::geometry::model::d2::point_xy;
    using boost::geometry::append;
    using boost::geometry::make;
    
  const unsigned int len = av_len(theAv)+1;
  SV** elem;
  AV* innerav;
  polygon retval;
  for (unsigned int i = 0; i < len; i++) {
    elem = av_fetch(theAv, i, 0);
    if (!SvROK(*elem)
        || SvTYPE(SvRV(*elem)) != SVt_PVAV
        || av_len((AV*)SvRV(*elem)) < 1)
    {
      delete retval;
      return NULL;
    }
    innerav = (AV*)SvRV(*elem);
    // IVSIZE is from perl/lib/CORE/config.h, defined as sizeof(IV)
#if IVSIZE >= 8
    // if Perl integers are 64 bit, use SvIV()
    // this library then supports 64 bit ints.
    append(retval,make<point_xy<double> >(SvIV(*av_fetch(innerav, 0, 0)), SvIV(*av_fetch(innerav, 1, 0)));
#else
    // otherwise coerce the Perl scalar to a double, with SvNV()
    // Perl doubles commonly allow 53 bits for the mantissa.
    // So in the common case, this library supports 53 bit integers, stored in doubles on the Perl side.
    append(retval,make<point_xy<double> >(SvNV(*av_fetch(innerav, 0, 0)), SvNV(*av_fetch(innerav, 1, 0)));
#endif
  }
  return retval;
}


#endif

#ifndef bgu_poly2av_h_
#define bgu_poly2av_h_

#include <iostream>
#include "myinit.h"
#include <boost/geometry/algorithms/num_points.hpp>

SV*
polygon2perl(pTHX_ const polygon& poly)
{
  AV* av = newAV();
  AV* innerav;
  const unsigned int len = boost::geometry::num_points(poly.outer());
  av_extend(av, len-1);
fprintf(stderr, "Return: len=%d\n", len);
  
  for(unsigned int i = 0; i < len; i++) {
    innerav = newAV();
    av_store(av, i, newRV_noinc((SV*)innerav));
    av_fill(innerav, 1);
    // IVSIZE is from perl/lib/CORE/config.h, defined as sizeof(IV)
fprintf(stderr, "Returning: %lf %lf\n", poly.outer()[i].x(), poly.outer()[i].x());
#if 0 && IVSIZE >= 8
    // if Perl integers are 64 bit, use newSViv()
    av_store(innerav, 0, newSViv(poly.outer()[i].x()));
    av_store(innerav, 1, newSViv(poly.outer()[i].y()));
#else
    // otherwise we expect Clipper integers to fit in the
	// 53 bit mantissa of a Perl double
    av_store(innerav, 0, newSVnv(poly.outer()[i].x()));
    av_store(innerav, 1, newSVnv(poly.outer()[i].y()));
#endif

  }
  return (SV*)newRV_noinc((SV*)av);
}

void add_hole(AV* theAv, polygon* poly)
{
    using boost::geometry::append;
    using boost::geometry::make;

  const unsigned int len = av_len(theAv)+1;
  SV** elem;
  AV* innerav;
  poly->inners().resize(1);
  ring& inner = poly->inners().back();
  for (unsigned int i = 0; i < len; i++) {
    elem = av_fetch(theAv, i, 0);
    if (!SvROK(*elem)
        || SvTYPE(SvRV(*elem)) != SVt_PVAV
        || av_len((AV*)SvRV(*elem)) < 1)
    {
      return;
    }
    innerav = (AV*)SvRV(*elem);
    // IVSIZE is from perl/lib/CORE/config.h, defined as sizeof(IV)
#if 0 && IVSIZE >= 8
    // if Perl integers are 64 bit, use SvIV()
    // this library then supports 64 bit ints.
fprintf(stderr, "AH1: %lf %lf\n", SvIV(*av_fetch(innerav, 0, 0)), SvIV(*av_fetch(innerav, 1, 0)));
 append(inner,make<point_xy>(SvIV(*av_fetch(innerav, 0, 0)), SvIV(*av_fetch(innerav, 1, 0))));
#else
    // otherwise coerce the Perl scalar to a double, with SvNV()
    // Perl doubles commonly allow 53 bits for the mantissa.
    // So in the common case, this library supports 53 bit integers, stored in doubles on the Perl side.
fprintf(stderr, "AH2: %lf %lf\n", SvNV(*av_fetch(innerav, 0, 0)), SvNV(*av_fetch(innerav, 1, 0)));
    append(inner,make<point_xy>(SvNV(*av_fetch(innerav, 0, 0)), SvNV(*av_fetch(innerav, 1, 0))));
#endif
std::cerr << "inner : " << boost::geometry::dsv(inner) << std::endl;
  }
}

polygon* add_outer(AV* theAv)
{
    using boost::geometry::append;
    using boost::geometry::make;
    using boost::geometry::exterior_ring;

  const unsigned int len = av_len(theAv)+1;
  SV** elem;
  AV* innerav;
  polygon* retval = new polygon();
  ring& outer = retval->outer();
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
#if 0 && IVSIZE >= 8
    // if Perl integers are 64 bit, use SvIV()
    // this library then supports 64 bit ints.
fprintf(stderr, "Append1: %lf %lf\n", SvIV(*av_fetch(innerav, 0, 0)), SvIV(*av_fetch(innerav, 1, 0)));
 append(outer,make<point_xy>(SvIV(*av_fetch(innerav, 0, 0)), SvIV(*av_fetch(innerav, 1, 0))));
#else
    // otherwise coerce the Perl scalar to a double, with SvNV()
    // Perl doubles commonly allow 53 bits for the mantissa.
    // So in the common case, this library supports 53 bit integers, stored in doubles on the Perl side.
fprintf(stderr, "Append2: %lf %lf\n", SvNV(*av_fetch(innerav, 0, 0)), SvNV(*av_fetch(innerav, 1, 0)));
    append(outer,make<point_xy>(SvNV(*av_fetch(innerav, 0, 0)), SvNV(*av_fetch(innerav, 1, 0))));
#endif
  }
std::cerr << "outer : " << boost::geometry::dsv(*retval) << std::endl;
  return retval;
}

polygon*
perl2polygon(pTHX_ AV* theAv)
{
    using boost::geometry::correct;
    
  const unsigned int len = av_len(theAv)+1;
  SV** elem;
  AV* innerav;
  elem = av_fetch(theAv, 0, 0);
  if (!SvROK(*elem)
      || SvTYPE(SvRV(*elem)) != SVt_PVAV
      || av_len((AV*)SvRV(*elem)) < 1) {
    return NULL;
  }
  polygon* retval = add_outer((AV*)SvRV(*elem));
  if (retval == NULL)
    return NULL;
  
  for (unsigned int i = 1; i < len; i++) {
    elem = av_fetch(theAv, i, 0);
    if (!SvROK(*elem)
        || SvTYPE(SvRV(*elem)) != SVt_PVAV
        || av_len((AV*)SvRV(*elem)) < 1)
    {
      delete retval;
      return NULL;
    }
    innerav = (AV*)SvRV(*elem);
    add_hole(innerav, retval);
  }
fprintf(stderr, "Worked\n");
std::cerr << "poly : " << boost::geometry::dsv(*retval) << std::endl;
  correct(*retval);
std::cerr << "cpoly: " << boost::geometry::dsv(*retval) << std::endl;
  return retval;
}

#endif

#ifndef bgu_mline2av_h_
#define bgu_mline2av_h_

#include <iostream>
#include "myinit.h"
#include <boost/geometry/algorithms/num_points.hpp>

SV*
multi_linestring2perl(pTHX_ const multi_linestring& mls)
{
  AV* av = newAV();
  return (SV*)newRV_noinc((SV*)av);
}

void add_line(AV* theAv, multi_linestring* mls)
{
    using boost::geometry::append;
    using boost::geometry::make;

  const unsigned int len = av_len(theAv)+1;
  SV** elem;
  AV* innerav;
  linestring* ls = new linestring();
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
fprintf(stderr, "ALS1: %lf %lf\n", SvIV(*av_fetch(innerav, 0, 0)), SvIV(*av_fetch(innerav, 1, 0)));
    ls->push_back(make<point_xy>(SvIV(*av_fetch(innerav, 0, 0)), SvIV(*av_fetch(innerav, 1, 0))));
#else
    // otherwise coerce the Perl scalar to a double, with SvNV()
    // Perl doubles commonly allow 53 bits for the mantissa.
    // So in the common case, this library supports 53 bit integers, stored in doubles on the Perl side.
fprintf(stderr, "ALS2: %lf %lf\n", SvNV(*av_fetch(innerav, 0, 0)), SvNV(*av_fetch(innerav, 1, 0)));
    ls->push_back(make<point_xy>(SvNV(*av_fetch(innerav, 0, 0)), SvNV(*av_fetch(innerav, 1, 0))));
#endif
    std::cerr << "ls : " << boost::geometry::dsv(*ls) << std::endl;
  }
  mls->push_back(*ls);
}

multi_linestring*
perl2multi_linestring(pTHX_ AV* theAv)
{
    
  const unsigned int len = av_len(theAv)+1;
  SV** elem;
  AV* innerav;
  elem = av_fetch(theAv, 0, 0);
  if (!SvROK(*elem)
      || SvTYPE(SvRV(*elem)) != SVt_PVAV
      || av_len((AV*)SvRV(*elem)) < 1) {
    return NULL;
  }
  multi_linestring* retval = new multi_linestring();
  
  for (unsigned int i = 0; i < len; i++) {
    elem = av_fetch(theAv, i, 0);
    if (!SvROK(*elem)
        || SvTYPE(SvRV(*elem)) != SVt_PVAV
        || av_len((AV*)SvRV(*elem)) < 1)
    {
      delete retval;
      return NULL;
    }
    add_line((AV*)SvRV(*elem), retval);
  }
std::cerr << "mline : " << boost::geometry::dsv(*retval) << std::endl;
  return retval;
}

#endif

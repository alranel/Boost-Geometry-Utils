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
    av_store_point_xy(innerav, poly.outer()[i].x(), poly.outer()[i].y());
  }
  return (SV*)newRV_noinc((SV*)av);
}

int add_ring(AV* theAv, ring& theRing)
{
    using boost::geometry::append;
    using boost::geometry::make;

  const unsigned int len = av_len(theAv)+1;
  SV** elem;
  AV* innerav;
  for (unsigned int i = 0; i < len; i++) {
    elem = av_fetch(theAv, i, 0);
    if (!SvROK(*elem)
        || SvTYPE(SvRV(*elem)) != SVt_PVAV
        || av_len((AV*)SvRV(*elem)) < 1)
    {
      return 0;
    }
    innerav = (AV*)SvRV(*elem);
    append(theRing, av_fetch_point_xy(innerav));
  }
  return 1;
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
  polygon* retval = new polygon();
  ring& outer = retval->outer();
  if (!add_ring((AV*)SvRV(*elem), outer)) {
    delete retval;
    return NULL;
  }
  
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
    retval->inners().resize(1);
    ring& inner = retval->inners().back();
    if (!add_ring(innerav, inner)) {
      delete retval;
      return NULL;
    }
  }
  //correct(*retval);
  return retval;
}

#endif

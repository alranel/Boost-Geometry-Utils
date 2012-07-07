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
    append(inner, av_fetch_point_xy(innerav));
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
    append(outer, av_fetch_point_xy(innerav));
  }
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
  //correct(*retval);
  return retval;
}

#endif

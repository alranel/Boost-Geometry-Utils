#ifndef bgu_poly2av_h_
#define bgu_poly2av_h_

#include <iostream>
#include "myinit.h"
#include <boost/geometry/algorithms/num_points.hpp>

void add_ring_perl(AV* polygon_av, ring& theRing)
{
    AV* ring_av = newAV();
    AV* point_av;
    
    // number of points in the ring
    const unsigned int len = boost::geometry::num_points(theRing);
    av_extend(ring_av, len-1);
    //fprintf(stderr, "Points in ring: len=%d\n", len);
  
    for(unsigned int i = 0; i < len; i++) {
        point_av = newAV();
        av_store(ring_av, i, newRV_noinc((SV*)point_av));
        av_fill(point_av, 1);
        av_store_point_xy(point_av, theRing[i].x(), theRing[i].y());
    }
    
    av_push(polygon_av, newRV_noinc((SV*)ring_av));
}

SV*
polygon2perl(pTHX_ const polygon& poly)
{
    AV* av = newAV();
    
    ring my_ring = poly.outer();
    add_ring_perl(av, my_ring);
    
    std::vector<ring>::size_type sz = poly.inners().size();
    for (unsigned i = 0; i < sz; i++) {
        my_ring = poly.inners()[i];
        add_ring_perl(av, my_ring);
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

#ifndef bgu_mline2av_h_
#define bgu_mline2av_h_

#include <iostream>
#include "myinit.h"
#include <boost/geometry/algorithms/num_points.hpp>

SV*
multi_linestring2perl(pTHX_ const multi_linestring& mls)
{
  AV* av = newAV();
  std::cerr << "mls> " << boost::geometry::dsv(mls) << std::endl;
  const unsigned int size = mls.size();
  av_extend(av, size-1);
fprintf(stderr, "mls> size=%d\n", size);

  for (int i = 0; i < size; i++) {
    AV* lineav = newAV();
    linestring ls = mls[i];
    av_store(av, i, newRV_noinc((SV*)lineav));
    av_fill(lineav, 1);
    std::cerr << " ls> " << boost::geometry::dsv(ls) << std::endl;
    const unsigned int line_len = boost::geometry::num_points(ls);
    for (int j = 0; j < line_len; j++) {
      AV* pointav = newAV();
      av_store(lineav, j, newRV_noinc((SV*)pointav));
      av_fill(pointav, 1);
      std::cerr << "  p> " << ls[j].x() << ":" << ls[j].y() << std::endl;;
#if 0 && IVSIZE >= 8
    // if Perl integers are 64 bit, use newSViv()
    av_store(pointav, 0, newSViv(ls[j].x()));
    av_store(pointav, 1, newSViv(ls[j].y()));
#else
    // otherwise we expect Clipper integers to fit in the
	// 53 bit mantissa of a Perl double
    av_store(pointav, 0, newSVnv(ls[j].x()));
    av_store(pointav, 1, newSVnv(ls[j].y()));
#endif
    }
  }
    
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
    std::cerr << "ls: " << boost::geometry::dsv(*ls) << std::endl;
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

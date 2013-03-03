#ifndef bgu_voronoi2perl_h_
#define bgu_voronoi2perl_h_

#include "voronoi_visual_utils.hpp"
#include <cstdio>

using namespace boost::polygon;
typedef boost::polygon::voronoi_diagram<double> VD;
typedef boost::polygon::segment_data<double> bp_segment;
static const std::size_t EXTERNAL_COLOR = 1;

// helper functions addapted from boost::polygon voronoi example code
// http://www.boost.org/doc/libs/1_52_0/libs/polygon/example/voronoi_visualizer.cpp

point_xy retrieve_point(const VD::cell_type & cell, std::vector<bp_segment> & site_data) {
  VD::cell_type::source_index_type index = cell.source_index();
  VD::cell_type::source_category_type category = cell.source_category();
  if (category == SOURCE_CATEGORY_SEGMENT_START_POINT) {
    return low(site_data[index]);
  } else {
    return high(site_data[index]);
  }
}

bp_segment retrieve_segment(const VD::cell_type& cell, std::vector<bp_segment> & site_data) {
  VD::cell_type::source_index_type index = cell.source_index();
  return site_data[index];
}
  
void sample_curved_edge(
    const VD::edge_type& edge,
    std::vector<point_data<double> > * sampled_edge, double max_dist, std::vector<bp_segment> & site_data) {
  point_xy point = edge.cell()->contains_point() ?
      retrieve_point(*edge.cell(), site_data) :
      retrieve_point(*edge.twin()->cell(), site_data);
  bp_segment segment = edge.cell()->contains_point() ?
      retrieve_segment(*edge.twin()->cell(), site_data) :
      retrieve_segment(*edge.cell(), site_data);
  voronoi_visual_utils<double>::discretize(
      point, segment, max_dist, sampled_edge);
}

void color_exterior(const VD::edge_type* edge) {
  if (edge->color() == EXTERNAL_COLOR) {
    return;
  }
  edge->color(EXTERNAL_COLOR);
  edge->twin()->color(EXTERNAL_COLOR);
  const VD::vertex_type* v = edge->vertex1();
  if (v == NULL || !edge->is_primary()) {
    return;
  }
  v->color(EXTERNAL_COLOR);
  const VD::edge_type* e = v->incident_edge();
  do {
    color_exterior(e);
    e = e->rot_next();
  } while (e != v->incident_edge());
}

/* first attempt at getting medial axis-like geometry for the interior of a   */
/* polygon out of the voronoi_diagram - involves filtering out unwanted edges */
/* outside of the polygon and within holes                                    */
/* TODO: instead of linestrings, return richer data structure with MIC radius */
/*       and MIC tangent data                                                 */

void primary_voronoi_edges_polygon(const VD &vd, multi_linestring * mls, std::vector<bp_segment> &site_data, const int contour_start = 0) {
  // find and mark all edges outside of polygon->outer()
  for (VD::const_edge_iterator it = vd.edges().begin(); it != vd.edges().end(); ++it) {
    if (!it->is_finite()) { color_exterior(&(*it)); }
  }
  for (VD::const_edge_iterator it = vd.edges().begin();
       it != vd.edges().end(); ++it) {
    if (it->is_primary() && it->is_finite()
        // filter out edges inside holes
        && !(    it->cell()->source_index() < contour_start
              && it->twin()->cell()->source_index() < contour_start )
       // filter out edges outside of polygon->outer()
       && (      it->color() != EXTERNAL_COLOR
              && it->twin()->color() != EXTERNAL_COLOR )
       ) {

      // get the two edge end points
      std::vector<point_data<double> > samples;
      point_data<double> vertex0(it->vertex0()->x(), it->vertex0()->y());
      samples.push_back(vertex0);
      point_data<double> vertex1(it->vertex1()->x(), it->vertex1()->y());
      samples.push_back(vertex1);

      // approximate any parabolic arc edges
      if (it->is_curved()) { sample_curved_edge(*it, &samples, 1000.0, site_data); }

      mls->push_back(linestring(samples.begin(), samples.end()));
    }
  }
}

template <typename RingLike>
void ring2segments(const RingLike &my_ring, std::vector<bp_segment> &segments) {
    BOOST_AUTO(it, boost::begin(my_ring));
    BOOST_AUTO(end, boost::end(my_ring));
    BOOST_AUTO(previous, it);
    for (it++; it != end; ++previous, ++it) {
        segments.push_back( bp_segment( *previous, *it ) );
    }
    // If ring wasn't closed, add one more closing segment
    if (boost::size(my_ring) > 2) {
        if (boost::geometry::disjoint(*boost::begin(my_ring), *(boost::end(my_ring) - 1))) {
            segments.push_back( bp_segment( *(end - 1), *boost::begin(my_ring) ) );
        }
    }
}

#endif
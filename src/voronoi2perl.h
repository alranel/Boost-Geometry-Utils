#ifndef bgu_voronoi2perl_h_
#define bgu_voronoi2perl_h_

#include "voronoi_visual_utils.hpp"

using boost::polygon::voronoi_diagram;
using namespace boost::polygon;
  
/* THREE WAYS TO ITERATE OVER VORONOI GRAPH EDGES, FROM BOOST::POLYGON DOCS   */

/* simply iterating over the Voronoi edges (counts each edge twice):          */

int iterate_primary_edges1(const voronoi_diagram<double> &vd) {
  int result = 0;
  for (voronoi_diagram<double>::const_edge_iterator it = vd.edges().begin();
       it != vd.edges().end(); ++it) {
    if (it->is_primary()) {
      ++result;
    }
  }
  return result;
}
 
/* iterating over the Voronoi cells and then traversing edges around each     */
/* cell (counts each edge twice):                                             */

int iterate_primary_edges2(const voronoi_diagram<double> &vd) {
  int result = 0;
  for (voronoi_diagram<double>::const_cell_iterator it = vd.cells().begin();
       it != vd.cells().end(); ++it) {
    const voronoi_diagram<double>::cell_type &cell = *it;
    const voronoi_diagram<double>::edge_type *edge = cell.incident_edge();
    // This is convenient way to iterate edges around Voronoi cell.
    do {
      if (edge->is_primary())
        ++result;
      edge = edge->next();
    } while (edge != cell.incident_edge());
  }
  return result;
}


/* iterating over the Voronoi vertices and then traversing edges around each  */
/* vertex (the number of the iterations through each edge is equal to the     */
/* number of finite endpoints of the edge):                                   */

int iterate_primary_edges3(const voronoi_diagram<double> &vd) {
  int result = 0;
  for (voronoi_diagram<double>::const_vertex_iterator it = vd.vertices().begin();
       it != vd.vertices().end(); ++it) {
    const voronoi_diagram<double>::vertex_type &vertex = *it;
    const voronoi_diagram<double>::edge_type *edge = vertex.incident_edge();
    // This is convenient way to iterate edges around Voronoi vertex.
    do {
      if (edge->is_primary())
        ++result;
      edge = edge->rot_next();
    } while (edge != vertex.incident_edge());
  }
  return result;
}

/* from voronoi_visualizer.cpp example                                        */
/* use sample_curved_edge() and voronoi_visual_utils.hpp to turn curved       */
/* voronoi edges into approximating segments                                  */

/* tried to copy paste and hack sample_curved_edge() in here, but too buggy   */
/* need to follow that example a step at a time, not just copy and try to fix */


#endif
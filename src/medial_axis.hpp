// medial_axis.hpp header file
// derived from
// Boost.Polygon library voronoi_diagram.hpp header file
// which is
//          Copyright Andrii Sydorchuk 2010-2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org for updates, documentation, and revision history.

// Derivitive work by Michael E. Sheldrake, Copyright 2013, distributed
// under the same terms as the license above.

// This is essentially boost/polygon/voronoi_diagram.hpp adapted to further 
// process the Voronoi diagram graph structure to make it represent the 
// medial axis of a polygon (with holes) represented by segment input.

#ifndef BOOST_POLYGON_MEDIAL_AXIS
#define BOOST_POLYGON_MEDIAL_AXIS

#include <vector>
#include <utility>
#include <cstdio>

#include "boost/polygon/detail/voronoi_ctypes.hpp"
#include "boost/polygon/detail/voronoi_structures.hpp"

#include "boost/polygon/voronoi_geometry_type.hpp"

namespace boost {
namespace polygon {

// Forward declarations.
template <typename T>
class medial_axis_edge;
 
// Represents Voronoi cell.
// Data members:
//   1) index of the source within the initial input set
//   2) pointer to the incident edge
//   3) mutable color member
// Cell may contain point or segment site inside.
template <typename T>
class medial_axis_cell {
 public:
  typedef T coordinate_type;
  typedef std::size_t color_type;
  typedef medial_axis_edge<coordinate_type> medial_axis_edge_type;
  typedef std::size_t source_index_type;
  typedef SourceCategory source_category_type;

  medial_axis_cell(source_index_type source_index,
               source_category_type source_category) :
      source_index_(source_index),
      incident_edge_(NULL),
      color_(source_category) {}

  // Returns true if the cell contains point site, false else.
  bool contains_point() const {
    source_category_type source_category = this->source_category();
    return belongs(source_category, GEOMETRY_CATEGORY_POINT);
  }

  // Returns true if the cell contains segment site, false else.
  bool contains_segment() const {
    source_category_type source_category = this->source_category();
    return belongs(source_category, GEOMETRY_CATEGORY_SEGMENT);
  }

  source_index_type source_index() const {
    return source_index_;
  }

  source_category_type source_category() const {
    return static_cast<source_category_type>(color_ & SOURCE_CATEGORY_BITMASK);
  }

  // Degenerate cells don't have any incident edges.
  bool is_degenerate() const { return incident_edge_ == NULL; }

  medial_axis_edge_type* incident_edge() { return incident_edge_; }
  const medial_axis_edge_type* incident_edge() const { return incident_edge_; }
  void incident_edge(medial_axis_edge_type* e) { incident_edge_ = e; }

  color_type color() const { return color_ >> BITS_SHIFT; }
  void color(color_type color) const {
    color_ &= BITS_MASK;
    color_ |= color << BITS_SHIFT;
  }

 private:
  // 5 color bits are reserved.
  enum Bits {
    BITS_SHIFT = 0x5,
    BITS_MASK = 0x1F
  };

  source_index_type source_index_;
  medial_axis_edge_type* incident_edge_;
  mutable color_type color_;
};

// Represents Voronoi vertex.
// Data members:
//   1) vertex coordinates
//   2) pointer to the incident edge
//   3) mutable color member
template <typename T>
class medial_axis_vertex {
 public:
  typedef T coordinate_type;
  typedef std::size_t color_type;
  typedef medial_axis_edge<coordinate_type> medial_axis_edge_type;

  medial_axis_vertex(const coordinate_type& x, const coordinate_type& y,
                     const coordinate_type& r=0) :
      x_(x),
      y_(y),
      r_(r),
      incident_edge_(NULL),
      color_(0) {}

  const coordinate_type& x() const { return x_; }
  const coordinate_type& y() const { return y_; }
  const coordinate_type& r() const { return r_; }

  bool is_degenerate() const { return incident_edge_ == NULL; }

  medial_axis_edge_type* incident_edge() { return incident_edge_; }
  const medial_axis_edge_type* incident_edge() const { return incident_edge_; }
  void incident_edge(medial_axis_edge_type* e) { incident_edge_ = e; }

  color_type color() const { return color_ >> BITS_SHIFT; }
  void color(color_type color) const {
    color_ &= BITS_MASK;
    color_ |= color << BITS_SHIFT;
  }

 private:
  // 5 color bits are reserved.
  enum Bits {
    BITS_SHIFT = 0x5,
    BITS_MASK = 0x1F
  };

  coordinate_type x_;
  coordinate_type y_;
  coordinate_type r_;
  medial_axis_edge_type* incident_edge_;
  mutable color_type color_;
};

// Half-edge data structure. Represents Voronoi edge.
// Data members:
//   1) pointer to the corresponding cell
//   2) pointer to the vertex that is the starting
//      point of the half-edge
//   3) pointer to the twin edge
//   4) pointer to the CCW next edge
//   5) pointer to the CCW prev edge
//   6) mutable color member
template <typename T>
class medial_axis_edge {
 public:
  typedef T coordinate_type;
  typedef medial_axis_cell<coordinate_type> medial_axis_cell_type;
  typedef medial_axis_vertex<coordinate_type> medial_axis_vertex_type;
  typedef medial_axis_edge<coordinate_type> medial_axis_edge_type;
  typedef std::size_t color_type;

  medial_axis_edge(bool is_linear, bool is_primary) :
      cell_(NULL),
      vertex_(NULL),
      twin_(NULL),
      next_(NULL),
      prev_(NULL),
      color_(0) {
    if (is_linear)
      color_ |= BIT_IS_LINEAR;
    if (is_primary)
      color_ |= BIT_IS_PRIMARY;
  }

  medial_axis_cell_type* cell() { return cell_; }
  const medial_axis_cell_type* cell() const { return cell_; }
  void cell(medial_axis_cell_type* c) { cell_ = c; }

  medial_axis_vertex_type* vertex0() { return vertex_; }
  const medial_axis_vertex_type* vertex0() const { return vertex_; }
  void vertex0(medial_axis_vertex_type* v) { vertex_ = v; }

  medial_axis_vertex_type* vertex1() { return twin_->vertex0(); }
  const medial_axis_vertex_type* vertex1() const { return twin_->vertex0(); }

  medial_axis_edge_type* twin() { return twin_; }
  const medial_axis_edge_type* twin() const { return twin_; }
  void twin(medial_axis_edge_type* e) { twin_ = e; }

  medial_axis_edge_type* next() { return next_; }
  const medial_axis_edge_type* next() const { return next_; }
  void next(medial_axis_edge_type* e) { next_ = e; }

  medial_axis_edge_type* prev() { return prev_; }
  const medial_axis_edge_type* prev() const { return prev_; }
  void prev(medial_axis_edge_type* e) { prev_ = e; }

  // Returns a pointer to the rotation next edge
  // over the starting point of the half-edge.
  medial_axis_edge_type* rot_next() { return prev_->twin(); }
  const medial_axis_edge_type* rot_next() const { return prev_->twin(); }

  // Returns a pointer to the rotation prev edge
  // over the starting point of the half-edge.
  medial_axis_edge_type* rot_prev() { return twin_->next(); }
  const medial_axis_edge_type* rot_prev() const { return twin_->next(); }

  // Returns true if the edge is finite (segment, parabolic arc).
  // Returns false if the edge is infinite (ray, line).
  bool is_finite() const { return vertex0() && vertex1(); }

  // Returns true if the edge is infinite (ray, line).
  // Returns false if the edge is finite (segment, parabolic arc).
  bool is_infinite() const { return !vertex0() || !vertex1(); }

  // Returns true if the edge is linear (segment, ray, line).
  // Returns false if the edge is curved (parabolic arc).
  bool is_linear() const {
    return (color_ & BIT_IS_LINEAR) ? true : false;
  }

  // Returns true if the edge is curved (parabolic arc).
  // Returns false if the edge is linear (segment, ray, line).
  bool is_curved() const {
    return (color_ & BIT_IS_LINEAR) ? false : true;
  }

  // Returns false if edge goes through the endpoint of the segment.
  // Returns true else.
  bool is_primary() const {
    return (color_ & BIT_IS_PRIMARY) ? true : false;
  }

  // Returns true if edge goes through the endpoint of the segment.
  // Returns false else.
  bool is_secondary() const {
    return (color_ & BIT_IS_PRIMARY) ? false : true;
  }

  color_type color() const { return color_ >> BITS_SHIFT; }
  void color(color_type color) const {
    color_ &= BITS_MASK;
    color_ |= color << BITS_SHIFT;
  }
  
  // where radius from vertex0 hits source segment
  const detail::point_2d<default_voronoi_builder::int_type> foot() const { 
    return foot_;
  }
  void foot(coordinate_type x, coordinate_type y) const {
    foot_.x(x);
    foot_.y(y);
  }

 private:
  // 5 color bits are reserved.
  enum Bits {
    BIT_IS_LINEAR = 0x1,  // linear is opposite to curved
    BIT_IS_PRIMARY = 0x2,  // primary is opposite to secondary

    BITS_SHIFT = 0x5,
    BITS_MASK = 0x1F
  };

  medial_axis_cell_type* cell_;
  medial_axis_vertex_type* vertex_;
  medial_axis_edge_type* twin_;
  medial_axis_edge_type* next_;
  medial_axis_edge_type* prev_;
  mutable color_type color_;
  mutable detail::point_2d<default_voronoi_builder::int_type> foot_;

};

template <typename T>
struct medial_axis_traits {
  typedef T coordinate_type;
  typedef medial_axis_cell<coordinate_type> cell_type;
  typedef medial_axis_vertex<coordinate_type> vertex_type;
  typedef medial_axis_edge<coordinate_type> edge_type;
  typedef class {
   public:
    enum { ULPS = 128 };
    bool operator()(const vertex_type& v1, const vertex_type& v2) const {
      return (ulp_cmp(v1.x(), v2.x(), ULPS) ==
              detail::ulp_comparison<T>::EQUAL) &&
             (ulp_cmp(v1.y(), v2.y(), ULPS) ==
              detail::ulp_comparison<T>::EQUAL);
    }
   private:
    typename detail::ulp_comparison<T> ulp_cmp;
  } vertex_equality_predicate_type;
};

// Voronoi output data structure.
// CCW ordering is used on the faces perimeter and around the vertices.
template <typename T, typename TRAITS = medial_axis_traits<T> >
class medial_axis {
 public:
  typedef typename TRAITS::coordinate_type coordinate_type;
  typedef typename TRAITS::cell_type cell_type;
  typedef typename TRAITS::vertex_type vertex_type;
  typedef typename TRAITS::edge_type edge_type;

  typedef std::vector<cell_type> cell_container_type;
  typedef typename cell_container_type::const_iterator const_cell_iterator;

  typedef std::vector<vertex_type> vertex_container_type;
  typedef typename vertex_container_type::const_iterator const_vertex_iterator;

  typedef std::vector<edge_type> edge_container_type;
  typedef typename edge_container_type::const_iterator const_edge_iterator;

  medial_axis() {}

  void clear() {
    cells_.clear();
    vertices_.clear();
    edges_.clear();
  }

  const cell_container_type& cells() const {
    return cells_;
  }

  const vertex_container_type& vertices() const {
    return vertices_;
  }

  const edge_container_type& edges() const {
    return edges_;
  }

  std::size_t num_cells() const {
    return cells_.size();
  }

  std::size_t num_edges() const {
    return edges_.size();
  }

  std::size_t num_vertices() const {
    return vertices_.size();
  }

  void _reserve(int num_sites) {
    cells_.reserve(num_sites);
    vertices_.reserve(num_sites << 1);
    edges_.reserve((num_sites << 2) + (num_sites << 1));
  }

  template <typename CT>
  void _process_single_site(const detail::site_event<CT>& site) {
    cells_.push_back(cell_type(site.initial_index(), site.source_category()));
  }

  // Insert a new half-edge into the output data structure.
  // Takes as input left and right sites that form a new bisector.
  // Returns a pair of pointers to a new half-edges.
  template <typename CT>
  std::pair<void*, void*> _insert_new_edge(
      const detail::site_event<CT>& site1,
      const detail::site_event<CT>& site2) {
    // Get sites' indexes.
    int site_index1 = site1.sorted_index();
    int site_index2 = site2.sorted_index();

    bool is_linear = is_linear_edge(site1, site2);
    bool is_primary = is_primary_edge(site1, site2);

    // Create a new half-edge that belongs to the first site.
    edges_.push_back(edge_type(is_linear, is_primary));
    edge_type& edge1 = edges_.back();
 
    // Create a new half-edge that belongs to the second site.
    edges_.push_back(edge_type(is_linear, is_primary));
    edge_type& edge2 = edges_.back();

    // Add the initial cell during the first edge insertion.
    if (cells_.empty()) {
      cells_.push_back(cell_type(
          site1.initial_index(), site1.source_category()));
    }

    // The second site represents a new site during site event
    // processing. Add a new cell to the cell records.
    cells_.push_back(cell_type(
        site2.initial_index(), site2.source_category()));

    // Set up pointers to cells.
    edge1.cell(&cells_[site_index1]);
    edge2.cell(&cells_[site_index2]);

    // Set up twin pointers.
    edge1.twin(&edge2);
    edge2.twin(&edge1);

    // Return a pointer to the new half-edge.
    return std::make_pair(&edge1, &edge2);
  }

  // Insert a new half-edge into the output data structure with the
  // start at the point where two previously added half-edges intersect.
  // Takes as input two sites that create a new bisector, circle event
  // that corresponds to the intersection point of the two old half-edges,
  // pointers to those half-edges. Half-edges' direction goes out of the
  // new Voronoi vertex point. Returns a pair of pointers to a new half-edges.
  template <typename CT1, typename CT2>
  std::pair<void*, void*> _insert_new_edge(
      const detail::site_event<CT1>& site1,
      const detail::site_event<CT1>& site3,
      const detail::circle_event<CT2>& circle,
      void* data12, void* data23) {
    edge_type* edge12 = static_cast<edge_type*>(data12);
    edge_type* edge23 = static_cast<edge_type*>(data23);

    // Add a new Voronoi vertex.
    vertices_.push_back(vertex_type(circle.x(), circle.y(), 
                                    circle.lower_x() - circle.x()));
    vertex_type& new_vertex = vertices_.back();

    // Update vertex pointers of the old edges.
    edge12->vertex0(&new_vertex);
    edge23->vertex0(&new_vertex);

    bool is_linear = is_linear_edge(site1, site3);
    bool is_primary = is_primary_edge(site1, site3);

    // Add a new half-edge.
    edges_.push_back(edge_type(is_linear, is_primary));
    edge_type& new_edge1 = edges_.back();
    new_edge1.cell(&cells_[site1.sorted_index()]);

    // Add a new half-edge.
    edges_.push_back(edge_type(is_linear, is_primary));
    edge_type& new_edge2 = edges_.back();
    new_edge2.cell(&cells_[site3.sorted_index()]);

    // Update twin pointers.
    new_edge1.twin(&new_edge2);
    new_edge2.twin(&new_edge1);

    // Update vertex pointer.
    new_edge2.vertex0(&new_vertex);

    // Update Voronoi prev/next pointers.
    edge12->prev(&new_edge1);
    new_edge1.next(edge12);
    edge12->twin()->next(edge23);
    edge23->prev(edge12->twin());
    edge23->twin()->next(&new_edge2);
    new_edge2.prev(edge23->twin());

    //set foot provisionally
    new_edge2.foot((coordinate_type) site3.point0().x(), (coordinate_type) site3.point0().y());

    // Return a pointer to the new half-edge.
    return std::make_pair(&new_edge1, &new_edge2);
  }

  void _build() {
    // Remove degenerate edges.
    edge_iterator last_edge = edges_.begin();
    for (edge_iterator it = edges_.begin(); it != edges_.end(); it += 2) {
      const vertex_type* v1 = it->vertex0();
      const vertex_type* v2 = it->vertex1();
      if (v1 && v2 && vertex_equality_predicate_(*v1, *v2)) {
        remove_edge(&(*it));
      }
      else {
        if (it != last_edge) {
          edge_type* e1 = &(*last_edge = *it);
          edge_type* e2 = &(*(last_edge + 1) = *(it + 1));
          e1->twin(e2);
          e2->twin(e1);
          if (e1->prev()) {
            e1->prev()->next(e1);
            e2->next()->prev(e2);
          }
          if (e2->prev()) {
            e1->next()->prev(e1);
            e2->prev()->next(e2);
          }
        }
        last_edge += 2;
      }
    }
    edges_.erase(last_edge, edges_.end());

    // Set up incident edge pointers for cells and vertices.
    for (edge_iterator it = edges_.begin(); it != edges_.end(); ++it) {
      it->cell()->incident_edge(&(*it));
      if (it->vertex0()) {
        it->vertex0()->incident_edge(&(*it));
      }
    }

    // Remove degenerate vertices.
    vertex_iterator last_vertex = vertices_.begin();
    for (vertex_iterator it = vertices_.begin(); it != vertices_.end(); ++it) {
      if (it->incident_edge()) {
        if (it != last_vertex) {
          *last_vertex = *it;
          vertex_type* v = &(*last_vertex);
          edge_type* e = v->incident_edge();
          do {
            e->vertex0(v);
            e = e->rot_next();
          } while (e != v->incident_edge());
        }
        ++last_vertex;
      }
    }
    vertices_.erase(last_vertex, vertices_.end());

    // Set up next/prev pointers for infinite edges.
    if (vertices_.empty()) {
      if (!edges_.empty()) {
        // Update prev/next pointers for the line edges.
        edge_iterator edge_it = edges_.begin();
        edge_type* edge1 = &(*edge_it);
        edge1->next(edge1);
        edge1->prev(edge1);
        ++edge_it;
        edge1 = &(*edge_it);
        ++edge_it;

        while (edge_it != edges_.end()) {
          edge_type* edge2 = &(*edge_it);
          ++edge_it;

          edge1->next(edge2);
          edge1->prev(edge2);
          edge2->next(edge1);
          edge2->prev(edge1);

          edge1 = &(*edge_it);
          ++edge_it;
        }

        edge1->next(edge1);
        edge1->prev(edge1);
      }
    } else {
      // Update prev/next pointers for the ray edges.
      for (cell_iterator cell_it = cells_.begin();
         cell_it != cells_.end(); ++cell_it) {
        if (cell_it->is_degenerate())
          continue;
        // Move to the previous edge while
        // it is possible in the CW direction.
        edge_type* left_edge = cell_it->incident_edge();
        while (left_edge->prev() != NULL) {
          left_edge = left_edge->prev();
          // Terminate if this is not a boundary cell.
          if (left_edge == cell_it->incident_edge())
            break;
        }

        if (left_edge->prev() != NULL)
          continue;

        edge_type* right_edge = cell_it->incident_edge();
        while (right_edge->next() != NULL)
          right_edge = right_edge->next();
        left_edge->prev(right_edge);
        right_edge->next(left_edge);
      }
    }

    // The above gets us the complete Voronoi diagram.
    // Now we'll narrow that down to just the medial axis for the polygon.
    
    // Mark edges exterior to the polygon by setting color attribute to 1.
    // (Adjacent vertices and cells are also marked.)
    for (edge_iterator it = edges_.begin(); it != edges_.end(); ++it) {
      if (!it->is_finite()) { mark_exterior(&(*it)); }
    }

    // Now all the cells associated with the polygon's outer contour segments 
    // have color() == 1, while all cells associated with holes still have 
    // color() == 0. This isn't always enough information to label all edges
    // inside holes correctly. We'll go ahead and label edges not associated
    // with the outer cells as edges in holes, and then later correct
    // mislabeled edges.

    for (edge_iterator it = edges_.begin(); it != edges_.end(); ++it) {
      if (  it->cell()->color() == 0
         // cells with color 0 at this point are either holes
         // or regions within the polygon associated with concavites
         && it->twin()->cell()->color() == 0
         // this avoids labeling edges coming directly off
         // of the inner side of the medial axis that surrounds a hole
         && (  it->next()->twin()->cell()->color() != 1
            && it->prev()->twin()->cell()->color() != 1)
         ) {
        it->color(1);
      }
    }

    // Now we find cells with a mix of primary edges labeled as inside and 
    // outside. Adjacent primary edges can't have different inside-outside 
    // status. We're sure about the edges we've labled as within the polygon 
    // so far. So we recursively label adjacent primary edges as within if they
    // don't have that label already, and non-primary edges associated with
    // curved edges get their labels fixed too.

    for (cell_iterator it = cells_.begin(); it != cells_.end(); ++it) {
      edge_type* e = it->incident_edge();
      do {
        if (e->is_primary() && e->next()->is_primary()) {
          if (e->color() == 0 && e->next()->color() != 0) {
            mark_interior(e->next());
            } 
          if (e->color() != 0 && e->next()->color() == 0) {
            mark_interior(e, true);
            }
        }
        e = e->next();
      } while (e != it->incident_edge());
    }

    // Some non-primary edges of more complex cells don't get corrected
    // by the steps above. But now they're easy to identify and fix.
    
    for (edge_iterator it = edges_.begin(); it != edges_.end(); ++it) {
      if (!it->is_primary() &&
           (  it->rot_next()->is_primary() && it->rot_next()->color() == 0
           && it->rot_prev()->is_primary() && it->rot_prev()->color() == 0
           )
         ) {
        it->color(0);
      }
    }

    // Now all edges within the polygon have color() == 0 and all edges
    // outside of the polygon or inside holes in the polygon have 
    // color() == 1.

    /////////////
    // At this point we modify the half edge graph to better represent the 
    // the medial axis.
    // The main thing to do is update next() and prev() pointers to follow
    // along the primary edges that represent the medial axis, instead
    // of having them point just to the next/prev within each Voronoi cell.

    // Get the edge corresponding to the first polygon input segment
    // so the first loop we traverse is the outer polygon loop.
    // Currently it doesn't matter that we process that first, but we
    // may want to enhance the output data structure later to reflect 
    // inside vs outside/in-concavity/in-hole medial axis edge collections.

    edge_type * start_edge = NULL;
    for (edge_iterator it = edges_.begin(); it != edges_.end(); ++it) {
      if (it->cell()->source_index() == 0
          && it->color() == 0
          && it->is_primary()
         ) {
        start_edge = &(*it);
        break;
      }
    }

    // Walk the edge references and modify to represent medial axis.
    while (start_edge != NULL) {
      edge_type * edge = start_edge;
      //start_edge->color(2);
      do {
        // mark visited internal edges (will restore to 0 afterward)
        edge->color(2);
        // if next edge is within polygon
        if (edge->next()->color() == 0 || edge->next()->color() == 2) {
          if (edge->next()->is_primary()) { 
            // go to next edge within same cell 
            edge = edge->next();
          } else { 
            // skip over a non-primary edge to the primary edge that follows it
            edge_type * prev = edge;
            edge = edge->next()->twin()->next();
            // first make the clipped-out edges ahead link to themselves
            prev->next()->twin()->next(prev->next());
            prev->next()->prev(prev->next()->twin());
            // now link this to new next
            prev->next(edge);
            edge->prev(prev);
          }
        } else {
          // corner - end touches polygon, so turn around
          edge_type * prev = edge;
          edge = edge->twin();
          // first connect edges ahead to eachother
          prev->next()->prev(edge->prev());
          edge->prev()->next(prev->next());
          // now link the corner edges together
          prev->next(edge);
          edge->prev(prev);
        }
      } while (edge != start_edge && edge->color() != 2);

      // After the first run, any further runs are following internal hole 
      // loops. Find the first edge of the first/next hole.
      start_edge = NULL;
      for (edge_iterator it = edges_.begin(); it != edges_.end(); ++it) {
        if (it->color() == 0
            && it->is_primary()
           ) {
          start_edge = &(*it);
          break;
        }
      }
    }
    
    // Restore color() == 0 for internal edges.
    for (edge_iterator it = edges_.begin(); it != edges_.end(); ++it) {
      if (it->color() == 2) {
        it->color(0);
      }
    }

    // Debug reporting
    /*
    if (0) {
      printf("original edges\n");
      printf("srcInd isInf curved   color");
      printf("  this     twin       next       prev        point\n");
      for (edge_iterator it = edges_.begin(); it != edges_.end(); ++it) {
        printf("%3d   %5s  %7s  %2d  ",
          it->cell()->source_index(),
          (it->is_finite() ? "     ":" INF "),
          (it->is_curved() ? " curve ":" line  "),
          it->color()
        );
        printf("%llu, %llu , %llu, %llu ",
          (unsigned long long int) &(*it),
          (unsigned long long int) it->twin(),
          (unsigned long long int) it->next(),
          (unsigned long long int) it->prev()
        );
      if (it->vertex0()) {
        printf("[%f , %f , %f]",
          it->vertex0()->x(),
          it->vertex0()->y(),
          it->vertex0()->r()
        );
      }
      else {printf("no vert0");}
      printf("\n");
      }
    }
    */
  }

 private:
  typedef typename cell_container_type::iterator cell_iterator;
  typedef typename vertex_container_type::iterator vertex_iterator;
  typedef typename edge_container_type::iterator edge_iterator;
  typedef typename TRAITS::vertex_equality_predicate_type
    vertex_equality_predicate_type;

  template <typename SEvent>
  bool is_primary_edge(const SEvent& site1, const SEvent& site2) const {
    bool flag1 = site1.is_segment();
    bool flag2 = site2.is_segment();
    if (flag1 && !flag2) {
      return (site1.point0() != site2.point0()) &&
             (site1.point1() != site2.point0());
    }
    if (!flag1 && flag2) {
      return (site2.point0() != site1.point0()) &&
             (site2.point1() != site1.point0());
    }
    return true;
  }

  template <typename SEvent>
  bool is_linear_edge(const SEvent& site1, const SEvent& site2) const {
    if (!is_primary_edge(site1, site2)) {
      return true;
    }
    return !(site1.is_segment() ^ site2.is_segment());
  }

  // Remove degenerate edge.
  void remove_edge(edge_type* edge) {
    // Update the endpoints of the incident edges to the second vertex.
    vertex_type* vertex = edge->vertex0();
    edge_type* updated_edge = edge->twin()->rot_next();
    while (updated_edge != edge->twin()) {
      updated_edge->vertex0(vertex);
      updated_edge = updated_edge->rot_next();
    }

    edge_type* edge1 = edge;
    edge_type* edge2 = edge->twin();

    edge_type* edge1_rot_prev = edge1->rot_prev();
    edge_type* edge1_rot_next = edge1->rot_next();

    edge_type* edge2_rot_prev = edge2->rot_prev();
    edge_type* edge2_rot_next = edge2->rot_next();

    // Update prev/next pointers for the incident edges.
    edge1_rot_next->twin()->next(edge2_rot_prev);
    edge2_rot_prev->prev(edge1_rot_next->twin());
    edge1_rot_prev->prev(edge2_rot_next->twin());
    edge2_rot_next->twin()->next(edge1_rot_prev);
  }

  void mark_exterior(edge_type* edge) {
    if (edge->color() == 1) {
      return;
    }
    edge->color(1);
    edge->twin()->color(1);
    edge->cell()->color(1);
    edge->twin()->cell()->color(1);
    vertex_type* v = edge->vertex1();
    if (v == NULL || !edge->is_primary()) {
      return;
    }
    v->color(1);
    edge_type* e = v->incident_edge();
    do {
      if (! ( !e->is_primary() && 
              (!e->rot_next()->is_primary() || !e->rot_prev()->is_primary()) 
            )
        ) {
        mark_exterior(e);
      }
      e = e->rot_next();
    } while (e != v->incident_edge());
  }

  void mark_interior(edge_type* edge, bool backward = false) {
    //if (edge->color() == 0) {
    //  return;
    //}
    edge->color(0);
    edge->twin()->color(0);
    vertex_type* v = edge->vertex0();
    edge_type* e = v->incident_edge();

    if (edge->is_curved()) {
      edge_type* start_e = (edge->cell()->contains_point()) ? edge : edge->twin();
      e = start_e;
      do {
        if (!e->is_primary()) { 
          e->color(0);
          e->twin()->color(0);
        }
        e = e->next();
      } while (e != start_e);
    }

    if (!backward) {
      v = edge->vertex1();
    } 
    //else {
    //  //v = edge->prev()->vertex0();
    //  v = edge->vertex0();
    //}
    //if (!edge->is_primary()) {
    //  return;
    //}
    v->color(0);
    e = v->incident_edge();
    do {
      if (e->is_primary() && e->next()->is_primary()) {
        if (e->color() == 0 && e->next()->color() != 0) {
          mark_interior(e->next());
        }
        if (e->color() != 0 && e->next()->color() == 0) {
          mark_interior(e, true);
        }
      }
      if (e->is_primary() && e->prev()->is_primary()) {
        if (e->color() == 0 && e->prev()->color() != 0) {
          mark_interior(e->prev(), true);
        }
        if (e->color() != 0 && e->prev()->color() == 0) {
          mark_interior(e);
        }
      }
      e = e->rot_next();
    } while (e != v->incident_edge());
  }

  bool edge_source_sort(edge_type & a, edge_type & b) {
    return (a.cell()->source_index() < b.cell()->source_index());
  }

  bool is_exterior (const edge_type& e) { return (e->color() != 0); }

  cell_container_type cells_;
  vertex_container_type vertices_;
  edge_container_type edges_;
  vertex_equality_predicate_type vertex_equality_predicate_;

  // Disallow copy constructor and operator=
  medial_axis(const medial_axis&);
  void operator=(const medial_axis&);
};
}  // polygon
}  // boost

#endif  // BOOST_POLYGON_MEDIAL_AXIS

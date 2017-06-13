#include "edge.hpp"

Edge::Edge(int vertex_a_id, int vertex_b_id, int cost)
    : vertex_a_id_(vertex_a_id), vertex_b_id_(vertex_b_id), cost_(cost) {}

int Edge::vertex_a_id() const { return vertex_a_id_; }

int Edge::vertex_b_id() const { return vertex_b_id_; }

int Edge::cost() const { return cost_; }

int Edge::neighbor_vertex_id(int vertex_id) const {
  return vertex_id == vertex_a_id_ ? vertex_b_id_ : vertex_a_id_;
}

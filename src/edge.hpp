#ifndef EDGE_HPP
#define EDGE_HPP

class Edge {
 public:
  Edge(int vertex_a_id, int vertex_b_id, int cost);

  int vertex_a_id() const;
  int vertex_b_id() const;
  int cost() const;
  int neighbor_vertex_id(int vertex_id) const;

 private:
  int vertex_a_id_;
  int vertex_b_id_;

  int cost_;
};

#endif

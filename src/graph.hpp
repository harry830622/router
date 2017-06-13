#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "database.hpp"
#include "edge.hpp"
#include "vertex.hpp"

class Graph {
 public:
  Graph(int num_vertices);

  int num_vertices() const;
  int num_edges() const;

  const Vertex& vertex(int id) const;
  const Edge& edge(int id) const;

  void Draw(std::ostream& os, const Database& database) const;

  void AddEdge(int vertex_a_id, int vertex_b_id, int cost);

 private:
  std::vector<Vertex> vertices_;
  std::vector<Edge> edges_;
};

#endif

#include "graph.hpp"

using namespace std;

Graph::Graph(int num_vertices) : vertices_(num_vertices) {}

int Graph::num_vertices() const { return vertices_.size(); }

int Graph::num_edges() const { return edges_.size(); }

const Vertex& Graph::vertex(int id) const { return vertices_.at(id); }

const Edge& Graph::edge(int id) const { return edges_.at(id); }

void Graph::Draw(std::ostream& os, const Database& database) const {
  os << "set title 'Spanning Graph'" << endl;
  os << "set xrange[" << database.outline_lower_left().x() << ":"
     << database.outline_upper_right().x() << "]" << endl;
  os << "set yrange[" << database.outline_lower_left().y() << ":"
     << database.outline_upper_right().y() << "]" << endl;
  os << "set style line 1 lt 1 lc rgb 'black' lw 1 pt 1 ps 2" << endl;
  os << "unset key" << endl;

  os << "plot '-' w lp ls 1" << endl;
  for (const Edge& edge : edges_) {
    const int pin_a_id = edge.vertex_a_id();
    const int pin_b_id = edge.vertex_b_id();
    const Pin& pin_a = database.pin(pin_a_id);
    const Pin& pin_b = database.pin(pin_b_id);
    os << pin_a.x() << " " << pin_a.y() << endl;
    os << pin_b.x() << " " << pin_b.y() << endl;
    os << endl;
  }
  os << "e" << endl;
  os << "pause -1 'Press any key'" << endl;
}

void Graph::AddEdge(int vertex_a_id, int vertex_b_id, int cost) {
  const int new_edge_id = edges_.size();

  edges_.push_back(Edge(vertex_a_id, vertex_b_id, cost));

  vertices_.at(vertex_a_id).ConnectEdge(new_edge_id);
  vertices_.at(vertex_b_id).ConnectEdge(new_edge_id);
}

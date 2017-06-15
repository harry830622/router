#include "router.hpp"

#include <boost/pending/disjoint_sets.hpp>

#include <algorithm>
#include <map>

using namespace std;

using DisjointSet = boost::disjoint_sets<int*, int*>;

auto larger = [](int x, int y) { return x > y; };
using reverse_multimap = multimap<int, int, decltype(larger)>;

Router::Router(Database& database)
    : database_(database),
      spanning_graph_(database_.num_pins()),
      minimum_spanning_tree_(database_.num_pins()) {}

void Router::DrawSpanningGraph(ostream& os) const {
  spanning_graph_.Draw(os, database_);
}

void Router::DrawMinimumSpanningTree(ostream& os) const {
  minimum_spanning_tree_.Draw(os, database_);
}

void Router::Run() {
  ConstructSpanningGraph();
  ConstructMinimumSpanningTree();
}

// private

void Router::ConstructSpanningGraph() {
  const int num_pins = database_.num_pins();

  vector<int> sorted_pin_ids;
  sorted_pin_ids.reserve(num_pins);
  for (int i = 0; i < num_pins; ++i) {
    const int id = i;
    sorted_pin_ids.push_back(id);
  }

  // Sort pins by x + y
  sort(sorted_pin_ids.begin(), sorted_pin_ids.end(),
       [&](int pin_a_id, int pin_b_id) {
         const Pin& pin_a = database_.pin(pin_a_id);
         const Pin& pin_b = database_.pin(pin_b_id);
         return pin_a.x() + pin_a.y() < pin_b.x() + pin_b.y();
       });

  reverse_multimap R1(larger);
  reverse_multimap R2(larger);
  for (const int current_pin_id : sorted_pin_ids) {
    const Pin& current_pin = database_.pin(current_pin_id);
    const int current_pin_x = current_pin.x();
    const int current_pin_y = current_pin.y();

    R1.insert(make_pair(current_pin_x, current_pin_id));
    R2.insert(make_pair(current_pin_y, current_pin_id));

    for (auto it = R1.lower_bound(current_pin_x); it != R1.end();) {
      const int pin_id = it->second;
      if (pin_id != current_pin_id) {
        const Pin& pin = database_.pin(pin_id);
        const int pin_x = it->first;
        const int pin_y = pin.y();
        if (pin_x - pin_y > current_pin_x - current_pin_y) {
          spanning_graph_.AddEdge(
              pin_id, current_pin_id,
              Point::ManhattanDistance(pin.coordinates(),
                                       current_pin.coordinates()));
          it = R1.erase(it);
        } else {
          break;
        }
      } else {
        ++it;
      }
    }
    for (auto it = R2.upper_bound(current_pin_y); it != R2.end();) {
      const int pin_id = it->second;
      if (pin_id != current_pin_id) {
        const Pin& pin = database_.pin(pin_id);
        const int pin_x = pin.x();
        const int pin_y = it->first;
        if (pin_x - pin_y <= current_pin_x - current_pin_y) {
          spanning_graph_.AddEdge(
              pin_id, current_pin_id,
              Point::ManhattanDistance(pin.coordinates(),
                                       current_pin.coordinates()));
          it = R2.erase(it);
        } else {
          break;
        }
      } else {
        ++it;
      }
    }
  }

  // Sort pins by x - y
  sort(sorted_pin_ids.begin(), sorted_pin_ids.end(),
       [&](int pin_a_id, int pin_b_id) {
         const Pin& pin_a = database_.pin(pin_a_id);
         const Pin& pin_b = database_.pin(pin_b_id);
         return pin_a.x() - pin_a.y() < pin_b.x() - pin_b.y();
       });

  multimap<int, int> R3;
  reverse_multimap R4(larger);
  for (const int current_pin_id : sorted_pin_ids) {
    const Pin& current_pin = database_.pin(current_pin_id);
    const int current_pin_x = current_pin.x();
    const int current_pin_y = current_pin.y();

    R3.insert(make_pair(current_pin_y, current_pin_id));
    R4.insert(make_pair(current_pin_x, current_pin_id));

    for (auto it = R3.lower_bound(current_pin_y); it != R3.end();) {
      const int pin_id = it->second;
      if (pin_id != current_pin_id) {
        const Pin& pin = database_.pin(pin_id);
        const int pin_x = pin.x();
        const int pin_y = it->first;
        if (pin_x + pin_y < current_pin_x + current_pin_y) {
          spanning_graph_.AddEdge(
              pin_id, current_pin_id,
              Point::ManhattanDistance(pin.coordinates(),
                                       current_pin.coordinates()));
          it = R3.erase(it);
        } else {
          break;
        }
      } else {
        ++it;
      }
    }
    for (auto it = R4.upper_bound(current_pin_x); it != R4.end();) {
      const int pin_id = it->second;
      if (pin_id != current_pin_id) {
        const Pin& pin = database_.pin(pin_id);
        const int pin_x = it->first;
        const int pin_y = pin.y();
        if (pin_x + pin_y >= current_pin_x + current_pin_y) {
          spanning_graph_.AddEdge(
              pin_id, current_pin_id,
              Point::ManhattanDistance(pin.coordinates(),
                                       current_pin.coordinates()));
          it = R4.erase(it);
        } else {
          break;
        }
      } else {
        ++it;
      }
    }
  }
}

void Router::ConstructMinimumSpanningTree() {
  const int num_edges = spanning_graph_.num_edges();

  vector<int> sorted_edge_ids;
  sorted_edge_ids.reserve(num_edges);
  for (int i = 0; i < num_edges; ++i) {
    const int id = i;
    sorted_edge_ids.push_back(id);
  }

  sort(sorted_edge_ids.begin(), sorted_edge_ids.end(),
       [&](int edge_a_id, int edge_b_id) {
         const Edge& edge_a = spanning_graph_.edge(edge_a_id);
         const Edge& edge_b = spanning_graph_.edge(edge_b_id);
         return edge_a.cost() < edge_b.cost();
       });

  vector<int> ranks(num_edges);
  vector<int> parents(num_edges);
  DisjointSet disjoint_set(&ranks[0], &parents[0]);
  for (const int edge_id : sorted_edge_ids) {
    disjoint_set.make_set(edge_id);
  }

  for (const int current_edge_id : sorted_edge_ids) {
    const Edge& current_edge = spanning_graph_.edge(current_edge_id);
    const int vertex_a_id = current_edge.vertex_a_id();
    const int vertex_b_id = current_edge.vertex_b_id();
    const int set_a = disjoint_set.find_set(vertex_a_id);
    const int set_b = disjoint_set.find_set(vertex_b_id);
    if (set_a != set_b) {
      minimum_spanning_tree_.AddEdge(vertex_a_id, vertex_b_id,
                                     current_edge.cost());
      disjoint_set.link(vertex_a_id, vertex_b_id);
    }
  }
}

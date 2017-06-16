#include "router.hpp"

#include <boost/pending/disjoint_sets.hpp>

#include <algorithm>
#include <list>
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

void Router::DrawRectilinearMinimumSpanningTree(ostream& os) const {
  os << "set title 'RMST'" << endl;
  os << "set xrange[" << database_.outline_lower_left().x() << ":"
     << database_.outline_upper_right().x() << "]" << endl;
  os << "set yrange[" << database_.outline_lower_left().y() << ":"
     << database_.outline_upper_right().y() << "]" << endl;
  os << "set style line 1 lt 1 lc rgb 'black' lw 1 pt 2 ps 1" << endl;
  os << "unset key" << endl;

  os << "plot '-' w lp ls 1" << endl;
  for (const Line& line : horizontal_lines_) {
    os << line.end_point_a().x() << " " << line.end_point_a().y() << endl;
    os << line.end_point_b().x() << " " << line.end_point_b().y() << endl;
    os << endl;
  }
  for (const Line& line : vertical_lines_) {
    os << line.end_point_a().x() << " " << line.end_point_a().y() << endl;
    os << line.end_point_b().x() << " " << line.end_point_b().y() << endl;
    os << endl;
  }
  os << "e" << endl;
  os << "pause -1 'Press any key'" << endl;
}

void Router::Output(ostream& os) const {
  long long int wirelength = 0;
  /* for (const Line& line : horizontal_lines_) { */
  /*   wirelength += */
  /*       Point::ManhattanDistance(line.end_point_a(), line.end_point_b()); */
  /* } */
  /* for (const Line& line : vertical_lines_) { */
  /*   wirelength += */
  /*       Point::ManhattanDistance(line.end_point_a(), line.end_point_b()); */
  /* } */

  os << "NumRoutedPins = " << database_.num_pins() << endl;
  os << "WireLength = " << wirelength << endl;
  for (const Line& line : horizontal_lines_) {
    os << "H-line "
       << "(" << line.end_point_a().x() << "," << line.end_point_a().y() << ") "
       << "(" << line.end_point_b().x() << "," << line.end_point_b().y() << ")"
       << endl;
  }
  for (const Line& line : vertical_lines_) {
    os << "V-line "
       << "(" << line.end_point_a().x() << "," << line.end_point_a().y() << ") "
       << "(" << line.end_point_b().x() << "," << line.end_point_b().y() << ")"
       << endl;
  }
}

void Router::Run() {
  ConstructSpanningGraph();
  ConstructMinimumSpanningTree();
  RectilinearizeMinimumSpanningTree();
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

  for (const int edge_id : sorted_edge_ids) {
    const Edge& edge = spanning_graph_.edge(edge_id);
    const int vertex_a_id = edge.vertex_a_id();
    const int vertex_b_id = edge.vertex_b_id();
    const int set_a = disjoint_set.find_set(vertex_a_id);
    const int set_b = disjoint_set.find_set(vertex_b_id);
    if (set_a != set_b) {
      minimum_spanning_tree_.AddEdge(vertex_a_id, vertex_b_id, edge.cost());
      disjoint_set.link(vertex_a_id, vertex_b_id);
    }
  }
}

void Router::RectilinearizeMinimumSpanningTree() {
  const int num_edges = minimum_spanning_tree_.num_edges();

  vector<Line> horizontal_lines;
  vector<Line> vertical_lines;
  horizontal_lines.reserve(num_edges);
  vertical_lines.reserve(num_edges);

  for (int i = 0; i < num_edges; ++i) {
    const int id = i;
    const Edge& edge = minimum_spanning_tree_.edge(id);
    const int vertex_a_id = edge.vertex_a_id();
    const int vertex_b_id = edge.vertex_b_id();
    const int pin_a_id = vertex_a_id;
    const int pin_b_id = vertex_b_id;
    const Pin& pin_a = database_.pin(pin_a_id);
    const Pin& pin_b = database_.pin(pin_b_id);
    int x = pin_a.x();
    if (pin_a.x() < pin_b.x()) {
      horizontal_lines.push_back(
          Line(Point(pin_a.x(), pin_a.y()), Point(pin_b.x(), pin_a.y())));
      x = pin_b.x();
    }
    if (pin_a.x() > pin_b.x()) {
      horizontal_lines.push_back(
          Line(Point(pin_b.x(), pin_b.y()), Point(pin_a.x(), pin_b.y())));
      x = pin_a.x();
    }
    if (pin_a.y() < pin_b.y()) {
      vertical_lines.push_back(Line(Point(x, pin_a.y()), Point(x, pin_b.y())));
    }
    if (pin_a.y() > pin_b.y()) {
      vertical_lines.push_back(Line(Point(x, pin_b.y()), Point(x, pin_a.y())));
    }
  }

  horizontal_lines_ = horizontal_lines;
  vertical_lines_ = vertical_lines;

  /* sort(horizontal_lines.begin(), horizontal_lines.end(), */
  /*      [](const Line& line_a, const Line& line_b) { */
  /*        return line_a.end_point_a().x() < line_b.end_point_a().x(); */
  /*      }); */
  /* sort(vertical_lines.begin(), vertical_lines.end(), */
  /*      [](const Line& line_a, const Line& line_b) { */
  /*        return line_a.end_point_a().y() < line_b.end_point_a().y(); */
  /*      }); */

  /* list<Line> nonoverlap_horizontal_lines(horizontal_lines.begin(), */
  /*                                        horizontal_lines.end()); */
  /* list<Line> nonoverlap_vertical_lines(vertical_lines.begin(), */
  /*                                      vertical_lines.end()); */
  /* for (auto it = nonoverlap_horizontal_lines.begin(); */
  /*      it != nonoverlap_horizontal_lines.end();) { */
  /*   auto current_it = it; */
  /*   const int current_x_left = current_it->end_point_a().x(); */
  /*   const int current_x_right = current_it->end_point_b().x(); */
  /*   const int current_y = current_it->end_point_a().y(); */
  /*   ++it; */
  /*   const int next_x_left = it->end_point_a().x(); */
  /*   const int next_x_right = it->end_point_b().x(); */
  /*   const int next_y = it->end_point_a().y(); */
  /*   if (next_x_left <= current_x_right && next_y == current_y) { */
  /*     *current_it = Line( */
  /*         Point(current_x_left, current_y), */
  /*         Point(next_x_right > current_x_right ? next_x_right : current_x_right, */
  /*               current_y)); */
  /*     it = nonoverlap_horizontal_lines.erase(it); */
  /*     --it; */
  /*   } */
  /* } */
  /* for (auto it = nonoverlap_vertical_lines.begin(); */
  /*      it != nonoverlap_vertical_lines.end();) { */
  /*   auto current_it = it; */
  /*   const int current_y_lower = current_it->end_point_a().y(); */
  /*   const int current_y_upper = current_it->end_point_b().y(); */
  /*   const int current_x = current_it->end_point_a().x(); */
  /*   ++it; */
  /*   const int next_y_lower = it->end_point_a().y(); */
  /*   const int next_y_upper = it->end_point_b().y(); */
  /*   const int next_x = it->end_point_a().x(); */
  /*   if (next_y_lower <= current_y_upper && next_x == current_x) { */
  /*     *current_it = */
  /*         Line(Point(current_x, current_y_lower), */
  /*              Point(current_x, */
  /*                    next_y_upper > current_y_upper ? next_y_upper */
  /*                                                   : current_y_upper)); */
  /*     it = nonoverlap_vertical_lines.erase(it); */
  /*     --it; */
  /*   } */
  /* } */

  /* horizontal_lines_ = vector<Line>(nonoverlap_horizontal_lines.begin(), */
  /*                                  nonoverlap_horizontal_lines.end()); */
  /* vertical_lines_ = vector<Line>(nonoverlap_vertical_lines.begin(), */
  /*                                nonoverlap_vertical_lines.end()); */
}

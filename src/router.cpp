#include "router.hpp"

#include <algorithm>
#include <map>

using namespace std;

auto larger = [](int x, int y) { return x > y; };
using reverse_multimap = multimap<int, int, decltype(larger)>;

Router::Router(Database& database)
    : database_(database), spanning_graph_(database_.num_pins()) {}

void Router::DrawSpanningGraph(ostream& os) const {
  spanning_graph_.Draw(os, database_);
}

void Router::Run() { ConstructSpanningGraph(); }

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
  for (int current_pin_id : sorted_pin_ids) {
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
  for (int current_pin_id : sorted_pin_ids) {
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

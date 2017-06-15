#include "line.hpp"

using namespace std;

Line::Line(const Point& end_point_a, const Point& end_point_b)
    : end_point_a_(end_point_a), end_point_b_(end_point_b) {}

void Line::Print(std::ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Line:" << endl;
  ++indent_level;
  end_point_a_.Print(os, indent_level);
  end_point_b_.Print(os, indent_level);
}

const Point& Line::end_point_a() const { return end_point_a_; }

const Point& Line::end_point_b() const { return end_point_b_; }

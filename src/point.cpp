#include "./point.hpp"

#include <cmath>

using namespace std;

int Point::ManhattanDistance(const Point& point_a, const Point& point_b) {
  return abs(point_a.x() - point_b.x()) + abs(point_a.y() - point_b.y());
}

Point::Point(int x, int y) : x_(x), y_(y) {}

void Point::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Point:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "x_: " << x_ << endl;
  os << string(indent_level * indent_size, ' ') << "y_: " << y_ << endl;
}

int Point::x() const { return x_; }

int Point::y() const { return y_; }

#ifndef POINT_HPP
#define POINT_HPP

#include <iostream>

class Point {
 public:
  static int ManhattanDistance(const Point& point_a, const Point& point_b);

  Point(int x, int y);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  int x() const;
  int y() const;

 private:
  int x_;
  int y_;
};

#endif

#ifndef LINE_HPP
#define LINE_HPP

#include "point.hpp"

class Line {
 public:
  Line(const Point& end_point_a, const Point& end_point_b);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  const Point& end_point_a() const;
  const Point& end_point_b() const;

 private:
  Point end_point_a_;
  Point end_point_b_;
};

#endif

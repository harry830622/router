#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "pin.hpp"

#include <vector>

class Database {
 public:
  Database(std::istream& input);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  const Point& outline_lower_left() const;
  const Point& outline_upper_right() const;
  int num_pins() const;
  const Pin& pin(int id) const;

 private:
  void Parse(std::istream& input);

  Point outline_lower_left_;
  Point outline_upper_right_;
  std::vector<Pin> pins_;
};

#endif

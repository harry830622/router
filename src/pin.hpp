#ifndef PIN_HPP
#define PIN_HPP

#include "./point.hpp"

#include <string>

class Pin {
 public:
  Pin(const std::string& name, int x, int y);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  const std::string& name() const;
  const Point& coordinates() const;
  int x() const;
  int y() const;

 private:
  std::string name_;
  Point coordinates_;
};

#endif

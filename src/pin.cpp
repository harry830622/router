#include "./pin.hpp"

using namespace std;

Pin::Pin(const string& name, int x, int y) : name_(name), coordinates_(x, y) {}

void Pin::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Pin:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "name_: " << name_ << endl;
  os << string(indent_level * indent_size, ' ') << "coordinates_:" << endl;
  coordinates_.Print(os, indent_level + 1);
}

const string& Pin::name() const { return name_; }

const Point& Pin::coordinates() const { return coordinates_; }

int Pin::x() const { return coordinates_.x(); }

int Pin::y() const { return coordinates_.y(); }
